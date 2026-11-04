"""Native staff requests, v12 migration, permanent claims and return eligibility.

Contracts and quest prerequisites are explicit fixtures. The first request is
accepted through the actual office menus; all rewards use native settlement.
"""
import runpy,struct,re
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
blob=(ROOT/'bonds-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(blob)//4),blob)
ids={n:int(i) for n,i in re.findall(r'#define\s+(TRAINER_\w+)\s+(\d+)',(repo/'include/constants/opponents_frlg.h').read_text())}
catalog=[ids[n] for n in re.findall(r'\{(TRAINER_\w+),',(repo/'src/data/gym_leader_recruit_sources.h').read_text())]
assert len(catalog)<=256 and len(catalog)==len(set(catalog)) and c[0]==3156 and c[1]==3188 and c[2]==13
def record(i):return current()+c[3]+i*c[4]
def money():return call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
def result():return rd('gSpecialVar_Result',2)
def claimed(trainer):
 i=catalog.index(trainer);return bool(rd(current()+c[0]+i//8,1)&(1<<(i%8)))
def select(i):
 wr('gSpecialVar_LastTalked',230+i,2);call('GleStaffMemberSelect');assert result()==0
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
# Migrate a real earlier contract, retaining the entire v12 prefix.
assert call('GleRecruitTrainer',current(),c[10],3,40)==0
wr(record(0)+c[5],1,1);wr(current()+c[19],12,2)
before=bytearray(rd(current()+i,1) for i in range(c[0]));before[c[19]:c[19]+2]=struct.pack('<H',13)
for i in range(32):wr(current()+c[0]+i,255,1)
call('GymLeaderState')
assert bytes(rd(current()+i,1) for i in range(c[0]))==before and claimed(c[10])
assert sum(rd(current()+c[0]+i,1).bit_count() for i in range(32))==1
select(0);call('GleStaffDismiss');assert claimed(c[10]) and not rd(record(0),2)
wr(current()+v[2],8,1);wr(current()+c[17],100,2);wr(current()+c[18],0,2)
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],100000)
# Choose one willing, eligible trainer of each fixed story kind.
chosen={}
for trainer in catalog:
 if trainer==c[10]:continue
 kind=call('GleMix',rd(current()+4)^trainer^0xB04D51)%4
 if kind in chosen:continue
 call('SetTrainerFlag',trainer);call('GymLeaderRecruitResetOffer')
 if not call('GymLeaderRecruitOffer',trainer):continue
 call('GleRecruitInfo')
 if result()==1:chosen[kind]=trainer
 if len(chosen)==4:break
assert len(chosen)==4
for kind,trainer in sorted(chosen.items()):
 assert call('GleRecruitTrainer',current(),trainer,3,40)==0
 select(kind);call('GleStaffPersonalInfo');assert result()==0
 funds=money();call('GleStaffPersonalClaim');assert result()==3 and money()==funds
print('PASS native v12-prefix migration, durable dismissed bond and all four unmet personal-request gates',flush=True)
# Reach the personal request through the actual Gym office.
warp(v[23],2,13);press(64);press(1);until(lambda:menu_count()==5);choose(0);until(lambda:menu_count()==7)
choose(5);until(lambda:menu_count()==7);assert not rd(record(0)+c[5],1)
wr(record(0)+c[15],2,2);wr(record(0)+c[16],1,2)
choose(5);until(yesno);snap('staff-personal-complete-offer');funds=money();choose(1);until(lambda:menu_count()==7)
assert money()==funds and not claimed(chosen[0])
choose(5);until(yesno);choose(0);until(lambda:menu_count()==7)
assert money()==funds+1500 and rd(record(0)+c[5],1) and claimed(chosen[0])
snap('staff-personal-bond')
press(2);until(lambda:menu_count()==5);press(2);frames(80)
# Native construction fulfils the coaching and settlement requests.
for building in [c[12],c[13]]:
 plot=next(i for i in range(c[20]) if not rd(current()+v[52]+i*v[53],1) and rd(current()+v[52]+i*v[53]+2,1)>=2)
 scratch=symbols['gStringVar1'];wr(scratch,100000)
 assert call('GleConstruct',current(),plot,building,scratch)==0
wr(current()+v[54],12,2);wr(current()+c[11],1,2)
for kind in range(4):
 select(kind);call('GleStaffPersonalInfo');assert result()==(2 if kind==0 else 1)
 funds=money();call('GleStaffPersonalClaim');assert result()==(3 if kind==0 else 0)
 assert money()==funds+(0 if kind==0 else 1500)
 assert claimed(chosen[kind]) and rd(record(kind)+c[5],1) and rd(record(kind)+c[6],1)==65 and rd(record(kind)+c[8],1)==3
 funds=money();call('GleStaffPersonalClaim');assert result()==3 and money()==funds
# Dismissal and a paid native rehire restore loyalty without another award.
select(0);call('GleStaffDismiss');assert claimed(chosen[0])
call('GymLeaderRecruitResetOffer');assert call('GymLeaderRecruitOffer',chosen[0]);call('GleRecruitInfo');assert result()==1
funds=money();call('GleRecruitHire');assert result()==0 and money()<funds
assert rd(record(0),2)==chosen[0] and rd(record(0)+c[5],1) and rd(record(0)+c[6],1)>=80
select(0);funds=money();call('GleStaffPersonalClaim');assert result()==3 and money()==funds
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert all(claimed(t) for t in chosen.values()) and claimed(c[10])
print('PASS actual office request decline/accept, native buildings, all story rewards, unchanged potential, no repeat award, paid rehire and native flash',flush=True)
# A formerly unlucky return roll changes with progress and cannot block rank 8.
wr(current()+v[2],7,1)
trainer=next(t for t in catalog if t not in chosen.values() and t!=c[10] and call('GleMix',rd(current()+4)^t^(rd(current()+v[56])//80)^0x524554)%100>=86)
assert call('GleRecruitTrainer',current(),trainer,3,40)==0
wr(record(4)+c[9],2,1);select(4)
for _ in range(2):
 wr('gSpecialVar_Result',0,2);call('GleStaffAction');assert result()==3 and rd(record(4)+c[9],1)==2
wr(current()+v[2],8,1);wr('gSpecialVar_Result',0,2);call('GleStaffAction')
assert result()==0 and not rd(record(4)+c[9],1) and not rd(record(4)+c[14],1)
print('PASS fixed-window return retry and rank-8 recovery without an immutable random lock; personal bonds complete',flush=True)
