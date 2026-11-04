"""Native optional rival doubles and independent per-rank GLA settlement."""
import runpy,struct
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
cdata=(ROOT/'doubles-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(cdata)//4),cdata)
def bit(n):return bool(rd(current()+v[114]+n//8,1)&(1<<(n%8)))
def money():return call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
def records():return tuple(rd(current()+v[k],n) for k,n in [(29,2),(30,2),(31,4),(32,1),(56,4)])
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def settle():
 frames(100)
 for _ in range(140):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('double coordinator did not release')
warp(v[24],2,9);assert not call('MapGridGetCollisionAt',9,15) and not call('MapGridGetCollisionAt',9,16)
press(64);press(1);until(yesno);snap('gla-double-confirm');choose(1);settle()
assert not call('GymLeaderBattleTrainer') and not bit(224)
wr(current()+v[26],0,1);press(1);until(yesno);choose(0);settle();assert not bit(224) and not call('GymLeaderBattleTrainer')
# Singles and doubles have distinct victory eligibility.
wr(current()+c[3],1,1);assert call('GymLeaderPrepareRivalFriendly')==4 and call('GymLeaderDoubleAvailable')
wr(current()+c[12],2,1);wr(current()+c[11],0,2)
for i in range(3):
 mon=symbols['gParties']+i*v[5];call('CreateRandomMonWithIVs',mon,c[0],15,31);call('SetMonMoveSlot',mon,c[1],0)
 for j in [1,2,3]:call('SetMonMoveSlot',mon,0,j)
 wr(current()+v[27]+i*4,call('GetMonData2',mon,v[36]));wr(current()+v[28]+i*4,call('GetMonData2',mon,v[37]))
wr(current()+v[26],3,1);wr('gPartiesCount',3,1);call('HealPlayerParty')
opt=rd('gSaveBlock2Ptr')+0x14;wr(opt,(rd(opt,2)&~7)|2|0x600,2)
baseline=records();funds=money();wallet=rd(current()+v[66],1)
press(1);until(yesno);choose(0);until(lambda:call('GymLeaderBattleTrainer')!=0);frames(250)
assert rd('gBattleTypeFlags')&c[2] and rd('gBattlersCount',1)==4
for i in range(4):assert not call('CanDynamax',i)
assert not call('CanTerastallize',1) and not call('CanTerastallize',3)
snap('gla-real-double-battle')
for _ in range(2400):
 if task('Task_HandleChooseMonInput')<16:
  slot=rd(symbols['gPartyMenu']+9,1);active=set()
  for battler in [0,2]:
   who=rd(symbols['gBattlerPartyIndexes']+battler*c[13],c[13])
   if who<3 and rd(symbols['gParties']+who*v[5]+v[13],2)>0:active.add(who)
  if slot<3 and slot not in active and rd(symbols['gParties']+slot*v[5]+v[13],2)>0:press(1,25);press(1,25)
  else:press(16 if slot==0 else 128,20)
 else:press(1,18)
 if not call('GymLeaderBattleTrainer'):break
else:snap('double-battle-timeout');raise AssertionError('actual double battle did not finish')
assert rd('gBattleOutcome',1)==1
settle();assert bit(224) and not bit(232) and rd(current()+c[3],1)==1 and records()==baseline
assert money()==funds+2000 and rd(current()+v[66],1)==wallet+1
for i in range(3):
 mon=symbols['gParties']+i*v[5];assert call('GetMonData2',mon,v[10])==c[0] and rd(mon+v[13],2)==rd(mon+v[14],2)
 assert call('GetMonData2',mon,v[36])==rd(current()+v[27]+i*4)
assert call('GymLeaderPrepareRivalDouble')==4
print('PASS native GLA doubles decline/registration refusal, real four-battler victory, both-side gimmick limits, healed identities, rank/queue neutrality and separate single/double rewards',flush=True)
wr(current()+v[2],2,1);baseline=records();funds=money()
assert call('GymLeaderPrepareRivalDouble')==1;wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense')
assert not bit(225) and records()==baseline and money()==funds
assert call('GymLeaderPrepareRivalDouble')==1;wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense')
assert bit(225) and not(rd(current()+c[3],1)&2) and money()==funds+4000
assert call('GymLeaderPrepareRivalFriendly')==1;wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense')
assert not(rd(current()+c[3],1)&2)
# Full wallet keeps the earned rank/kind; a demoted leader may collect later.
wr(current()+v[2],7,1);wr(current()+v[66],24,1)
assert call('GymLeaderPrepareRivalDouble')==1;wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense')
assert bit(230) and bit(238) and rd(current()+v[66],1)==24
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and bit(238)
wr(current()+v[2],1,1);wr(current()+v[66],23,1);call('GleRivalDoubleInfo')
assert not bit(238) and rd(current()+v[66],1)==24
assert rd(current()+v[67]+23*v[68]+c[6],1)==7 and rd(current()+v[67]+23*v[68]+c[7],1)==2
call('GleRivalDoubleInfo');assert rd(current()+v[66],1)==24
assert call('GymLeaderPrepareRivalDouble')==4
print('PASS double loss/retry, independent normal-friendly eligibility, per-rank anti-refarm, pending Elite voucher/earned tier/demotion and native flash',flush=True)
