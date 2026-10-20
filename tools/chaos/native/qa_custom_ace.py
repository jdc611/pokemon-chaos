"""Compiled-ROM ace distribution, six-member boss strategies and legacy saves."""
from qa_custom_build import *
raw=(ROOT/'ace-layout.bin').read_bytes()
v=struct.unpack('<46I',raw[:46*4])
(VERSION,DS,S1,S2,S3,SHINY)=v[:6]
leaders=v[6:14];other_bosses=v[14:22]
(TSIZE,TFLAGS,REFLECT,LIGHTSCREEN,PROTECT,TAILWIND,SAND,RAIN,ELECTRIC,SUN,PSYCHIC,ORAN,SITRUS,OVERRIDE)=v[22:36]
(SPSIZE,*STATOFF)=v[36:43]
BEEDRILL,MEGABEEDRILL,ADAPTABILITY=v[43:]
REQUIRED,FORBIDDEN=struct.unpack('<2Q',raw[-16:])
assert (DS,S1,S2,S3)==(48,15872,3968,1616)
assert rd(rd('gSaveBlock3Ptr')+DRAFT+VERSION,1)==1
counts=[0,0,0,0];jackpots=0
mon=scratch+800;r=mon+MS
for seed in range(1,int(os.getenv("ACE_SEEDS","128"))+1):
 wr(rd('gSaveBlock3Ptr')+SEED,seed)
 for crate in range(6):
  call6('CustomBuildCandidate',mon,r,crate*3+1,0,5);power=call('CustomBuildSum',r)
  call6('CustomBuildCandidate',mon,r,crate*3+2,0,5);mega=call('CustomBuildSum',r)
  assert power>mega>=180,(seed,crate,power,mega)
 shiny=0
 for index in range(15,18):
  call6('CustomBuildCandidate',mon,r,index,0,5)
  endpoint=rd(r+16,2)+(rd(r+24,1) if index%3==2 else 0)
  assert 550<=endpoint<=800,(index,endpoint)
  jackpots+=endpoint>700
  first=bytesat(mon,MS);profile=bytesat(r,26)
  shiny+=bool(data(mon,SHINY))
  call6('CustomBuildCandidate',mon,r,index,0,5)
  assert bytesat(mon,MS)==first and bytesat(r,26)==profile
 assert shiny>=1
 counts[shiny]+=1
if int(os.getenv("ACE_SEEDS","128"))>=128:assert all(counts[1:]) and jackpots>0
print('PASS 768 crate pairs enforce B>C, 128 Golden crates guarantee shinies; exact 1/2/3 counts',counts[1:],'jackpots',jackpots,flush=True)
wr(rd('gSaveBlock3Ptr')+SEED,24680)
wr('gBattleTypeFlags',8)
for trainer in (*leaders,*other_bosses,RIVAL,ROUTE_RIVAL,CHAMP):
 call('CreateNPCTrainerParty',p+6*MS,trainer)
 count=sum(bool(data(p+(6+i)*MS,SP)) for i in range(6))
 assert count==6 if trainer in leaders else count>0,(trainer,count)
 original=bytesat(p+6*MS,count*MS)
 for i in range(count):
  m=p+(6+i)*MS;sp=data(m,SP)
  moves=[data(m,MOVE+j) for j in range(4)]
  assert len(set(moves))==4 and all(call('ChaosUsableMove',x) for x in moves),(trainer,i,moves)
  assert call('GetMonAbility',m)>0
  assert not call('ChaosRemovedItem',data(m,HELD))
  if trainer in leaders or trainer in other_bosses:
   bases=[call('CustomBuildBaseStat',m,j) for j in range(6)]
   native=[rd(symbols['gSpeciesInfo']+sp*SPSIZE+offset,1) for offset in STATOFF]
   assert sum(bases)==sum(native),(trainer,sp,bases,native)
 flags=call('GetAiFlags',trainer,1);flags|=lib.call_high()<<32
 assert flags&REQUIRED==REQUIRED and flags&FORBIDDEN==0,(trainer,hex(flags))
 call('CreateNPCTrainerParty',p+6*MS,trainer)
 again=bytesat(p+6*MS,count*MS)
 assert again==original,(trainer,[(j//MS,j%MS,a,b) for j,(a,b) in enumerate(zip(original,again)) if a!=b][:20])
 if trainer in leaders:
  lead=p+6*MS
  assert data(lead,MOVE+2)==REFLECT if trainer!=leaders[5] else True
  if trainer==leaders[5]:assert any(data(p+(6+i)*MS,MOVE+3)==TAILWIND for i in range(6))
 print('PASS boss team/AI/identity',trainer,count,flush=True)
print('PASS all eight Gym rosters, randomized ordinary BST totals, coherent tools, no hidden-information AI and stable repeat generation',flush=True)
# A Gym's actual Mega form keeps a random spread, its legitimate Mega budget
# and its signature ability, rather than falling back to optimized species stats.
call('CreateNPCTrainerParty',p+6*MS,leaders[5])
m=next(p+(6+i)*MS for i in range(6) if data(p+(6+i)*MS,SP)==BEEDRILL)
before=[call('CustomBuildBaseStat',m,j) for j in range(6)]
setdata(m,SP,MEGABEEDRILL);call('CalculateMonStats',m)
after=[call('CustomBuildBaseStat',m,j) for j in range(6)]
native=[rd(symbols['gSpeciesInfo']+MEGABEEDRILL*SPSIZE+offset,1) for offset in STATOFF]
assert sum(after)==sum(native) and after!=native and call('GetMonAbility',m)==ADAPTABILITY
setdata(m,SP,BEEDRILL);call('CalculateMonStats',m)
assert [call('CustomBuildBaseStat',m,j) for j in range(6)]==before
print('PASS Gym Mega form retains randomized weights, legitimate Mega BST and signature ability',flush=True)
# Load a real previous-version flash save, rather than converting its members.
state('ace-before-legacy')
legacy=repo.parent/'native-qa-custom/custom-build.sav'
assert lib.save_restore(str(legacy).encode())
assert call('LoadGameSave',0)==1;call('LoadPlayerParty')
assert call('IsCustomBuild') and not call('CustomBuildAceRules')
for i in range(rd('gPartiesCount',1)):
 m=p+i*MS;record=call('CustomBuildFind',data(m,PID));assert record
 assert sum(call('CustomBuildBaseStat',m,j) for j in range(6))==call('CustomBuildSum',record)
print('PASS actual generation-zero flash save loads with original individual allocations and legacy roll rules',flush=True)
state('ace-before-legacy',1)
