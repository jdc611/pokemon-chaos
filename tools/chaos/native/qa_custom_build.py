"""Individual custom builds and save/PC/growth checks on the compiled ROM."""
from qa_oaks_draft import *
CUSTOM=DRAFTMODE+1
lib.call6.restype=C.c_uint
def call6(fn,*args):
 result=lib.call6(symbols[fn],*(list(args)+[0]*6)[:6]);assert result!=0xdeadbeef,fn;return result
start(24680)
wr(s+START,CUSTOM,1);call('OaksDraftInitialize')
assert call('IsCustomBuild') and call('IsOaksDraft')
assert SIZE==1616
assert call('CanTerastallize',0)==0 and call('CanUseZMove',1)==0 and call('CanDynamax',0)==0
records=[]
for seed in range(1,9):
 wr(s+SEED,seed);call('CustomBuildGenerate',scratch)
 mons=[rd(scratch+i*2,2) for i in range(18)]
 assert len(set(mons))==18 and all(mons),mons
 for i,sp in enumerate(mons):
  depth=call('CustomBuildDepth',sp);mega=call('CustomBuildHasMega',sp)
  assert (1<=depth<=2) if i%3==0 else (depth==0 and bool(mega)==(i%3==2)),(i,sp,depth,mega)
print('PASS eight seeded drafts have valid, distinct A/B/C candidates',flush=True)
wr(s+SEED,24680)
for round in range(6):
 idx=round*3+round%3
 mon=p+round*MS;r=scratch+512;floor=r+64;abilities=r+96
 call6('CustomBuildCandidate',mon,r,idx,0,5)
 assert data(mon,LV)==5
 assert rd(r,4)
 # Candidate's level argument is fifth; a host trampoline supplies it safely.
 call('CustomBuildFloors',r,floor);call('CustomBuildAbilities',r,abilities,0)
 assert len({rd(abilities+i*2,2) for i in range(3)})==3
 bst=call('CustomBuildSum',r);assert bst>=180
 floors=[rd(floor+i*2,2) for i in range(6)];assert sum(floors)<=bst*3//4
 for i in range(6):wr(r+4+i*2,floors[i],2)
 wr(r+4,floors[0]+bst-sum(floors),2)
 record=bytesat(r,26)
 # Commit to party and the six-record save ledger; no species-wide mutation.
 assert call6('CustomBuildCommit',mon,r,round%3,255,0)
 assert call('CustomBuildBaseStat',mon,0)==rd(r+4,2)
 assert bytesat(call('CustomBuildFind',rd(r)),26)==record
 records.append(record)
 if round==2:
  call('SavePlayerParty');assert call('TrySavingData',0)==1
  call('ClearSav3');assert call('LoadGameSave',0)==1;call('LoadPlayerParty')
  s=rd('gSaveBlock3Ptr');assert rd(s+DRAFT+ROUND,1)==3
  for j,expected in enumerate(records):
   assert bytesat(call('CustomBuildFind',data(p+j*MS,PID)),26)==expected

assert rd(s+DRAFT+ROUND,1)==6 and rd('gPartiesCount',1)==6
call('SavePlayerParty');assert call('TrySavingData',0)==1
assert lib.save_dump(str(ROOT/'custom-build.sav').encode())
call('ClearSav3');assert call('LoadGameSave',0)==1;call('LoadPlayerParty')
s=rd('gSaveBlock3Ptr')
for i in range(6):assert bytesat(call('CustomBuildFind',data(p+i*MS,PID)),26)==records[i]
print('PASS six independent builds, uncapped HP allocation, ability options and actual flash save/reload',flush=True)
state('custom-after-draft')
