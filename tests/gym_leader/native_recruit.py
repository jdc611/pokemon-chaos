"""Native overworld victory/recruitment, staff office, Summary and practice.
Signing funds, player level and deterministic run seed are explicit fixtures.
"""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
def record(slot=0):return current()+v[167]+slot*v[168]
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(120):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('walk blocked',pos(),(x,y)))
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def objectids():
 e=rd(symbols['gMapHeader']+v[157]);p=rd(e+v[158])
 return [rd(p+i*v[116],1) for i in range(rd(e,1))]
def finish_text():
 for _ in range(100):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('native field script did not release control')
for boss in [v[184],v[185]]:
 call('SetTrainerFlag',boss);call('GymLeaderRecruitResetOffer')
 assert not call('GymLeaderRecruitOffer',boss)
 call('ClearTrainerFlag',boss)
# Select one reproducible test seed before the real battle. Repeated requests
# in the actual game cannot reroll willingness; the run seed never changes.
call('SetTrainerFlag',v[177])
for seed in range(100):
 wr(current()+4,seed)
 call('GymLeaderRecruitResetOffer')
 if not call('GymLeaderRecruitOffer',v[177]):continue
 call('GleRecruitInfo')
 if rd('gSpecialVar_Result',2)==1 and call('GleMix',seed^v[177]^0x524554)%100<89:break
else:raise AssertionError('no recruitment seed fixture')
# Recruitment follows the native A/B speaker selection after paired battles.
state('recruit-before-pair-fixture')
p=symbols['gTrainerBattleParameter'];wr(p+v[186],v[185],2);wr(p+v[187],v[177]&255,1);wr(p+v[187]+1,v[177]>>8,1)
call('SetTrainerFlag',v[185]);call('GymLeaderRecruitResetOffer');wr('gWhichTrainerToFaceAfterBattle',1,2)
assert call('GymLeaderRecruitPostBattleOffer')
call('GymLeaderRecruitResetOffer');wr('gWhichTrainerToFaceAfterBattle',0,2)
assert not call('GymLeaderRecruitPostBattleOffer')
wr(p+v[187],0,1);wr(p+v[187]+1,0,1);wr('gWhichTrainerToFaceAfterBattle',1,2);call('GymLeaderRecruitResetOffer')
assert not call('GymLeaderRecruitPostBattleOffer')
call('ClearTrainerFlag',v[185]);wr('gWhichTrainerToFaceAfterBattle',0,2)
state('recruit-before-pair-fixture',1)
call('ClearTrainerFlag',v[177]);call('GymLeaderRecruitResetOffer')
# A supplied level-cap Fire partner wins the actual ordinary trainer battle. No
# enemy HP, outcome, defeated flag or recruitment result is injected.
call('CreateRandomMonWithIVs',symbols['gParties'],v[188],15,31)
call('HealPlayerParty')
import re
move_ids={n:int(i) for n,i in re.findall(r'\b(MOVE_[A-Z0-9_]+) = (\d+)',(repo/'include/constants/moves.h').read_text())}
fire_moves={move_ids[n] for n in ['MOVE_EMBER','MOVE_FLAME_WHEEL','MOVE_FLAMETHROWER','MOVE_FLARE_BLITZ','MOVE_FIRE_FANG','MOVE_HEAT_WAVE','MOVE_BURN_UP','MOVE_FIRE_BLAST'] if n in move_ids}
owned_moves=[call('GetMonData2',symbols['gParties'],v[38]+j) for j in range(4)]
target=next((j for j,m in enumerate(owned_moves) if m in fire_moves),0)
warp(v[150],47,46);press(64);press(1)
for _ in range(2000):
 if yesno():break
 if task('Task_HandleChooseMonInput')<16:
  if rd(symbols['gParties']+v[13],2)>0:press(2,20)
  else:press(16,20);press(1,25);press(1,25)
  continue
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseMove']|1:
  cur=rd('gMoveSelectionCursor',1)
  if cur%2!=target%2:press(16 if target%2 else 32,8)
  cur=rd('gMoveSelectionCursor',1)
  if cur//2!=target//2:press(128 if target//2 else 64,8)
 press(1,15)
else:
 snap('recruit-battle-failure');raise AssertionError('ordinary battle did not reach the recruitment offer')
assert call('HasTrainerBeenFought',v[177])
assert rd('gBattleOutcome',1)==1
snap('native-recruitment-offer')
# A failed hire and an explicit decline retain the defeated trainer's offer.
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],0);choose(0);finish_text()
assert rd(record(),2)==0 and call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==0
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],5000)
press(1);until(yesno);choose(1);finish_text()
assert rd(record(),2)==0
press(1);until(yesno);before=call('GetMoney',rd('gSaveBlock1Ptr')+v[64]);choose(0)
until(lambda:rd(record(),2)==v[177]);finish_text()
potential=rd(record()+v[172],1)
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==before-500-250*potential
assert rd('gPartiesCount',1)==3 and not rd(record()+v[175],1)
assert call('TrySavingData',0)==1
expected=bytes(rd(record()+i,1) for i in range(v[168]))
wr(record(),0,2);assert call('LoadGameSave',0)==1
assert bytes(rd(record()+i,1) for i in range(v[168]))==expected
warp(v[150],47,46);assert 3 not in objectids()
print('PASS actual ordinary trainer victory, offer retry after failed funds/decline, paid hire, no Pokémon gift, flash record and original NPC relocation',flush=True)
warp(v[58+town],14,9);assert 210 in objectids()
press(64);press(1);until(lambda:menu_count()==7)
snap('staff-visible-reserve')
press(2);until(lambda:menu_count()==2);press(2);frames(50)
call('GleWarpHome');frames(240)
walkto(3,12);press(32);press(1);until(lambda:menu_count()==2)
choose(0);until(lambda:menu_count()==7)
snap('staff-office-record')
choose(0);until(lambda:menu_count()==7)
assert rd(record()+v[175],1)==1
choose(2);frames(180)
assert rd('gMapHeader')!=0
assert call('GetMonData2',symbols['gGlePreviewParty'],v[11])==15
assert any(call('GetMonData2',symbols['gGlePreviewParty'],v[38]+j) for j in range(4))
snap('staff-native-summary')
press(2,120);until(lambda:menu_count()==7)
press(2);until(lambda:menu_count()==2);press(2);frames(50)
call('GleWarpHome');frames(240);assert 210 in objectids()
snap('staff-active-gym')
# Register an existing third starter through the real Guide selector.
walkto(7,12);press(16);press(1);until(lambda:menu_count()==8)
choose(1);until(lambda:call('GymLeaderRegistering'));frames(160)
press(16,25);press(128,25);press(1,25);press(1,25);press(128);press(1,100)
until(lambda:menu_count()==8);assert rd(current()+v[26],1)==1
press(2);frames(50)
walkto(3,12);press(32);press(1);until(lambda:menu_count()==2)
choose(0);until(lambda:menu_count()==7)
xp=rd(current()+v[31]);q=rd(current()+v[32],1);wins=rd(current()+v[29],2);losses=rd(current()+v[30],2)
money=call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
choose(3);until(lambda:call('GymLeaderTrainingActive'));frames(300)
snap('staff-native-practice')
for _ in range(1400):
 press(1,18)
 if not call('GymLeaderBattleTrainer'):break
else:raise AssertionError('staff practice did not return')
until(lambda:menu_count()==7)
assert rd(current()+v[31])==xp and rd(current()+v[32],1)==q
assert rd(current()+v[29],2)==wins and rd(current()+v[30],2)==losses
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==money
for i in range(3):assert rd(symbols['gParties']+i*v[5]+v[13],2)==rd(symbols['gParties']+i*v[5]+v[14],2)
print('PASS native staff office assignment, actual roster Summary, visible active trainer and real practice with no official/money/queue farming',flush=True)
# Preserve the developed roster through a demotion, while a practice battle
# still follows the current cap. These are explicit progression fixtures.
state('staff-before-progression')
wr(current()+v[2],2,1);wr(current()+v[72],8,1)
a=call('Alloc_',v[101],0);b=call('Alloc_',1024,0)
assert call('GymLeaderPrepareStaffRoster',0,a,b,1)
enemy=symbols['gParties']+6*v[5]
assert call('GetMonData2',enemy,v[11])==100
assert call('GymLeaderPrepareStaffRoster',0,a,b,0)
assert call('GetMonData2',enemy,v[11])==25
call('Free',a);call('Free',b)
state('staff-before-progression',1)
# Move an existing record to the rival as a departure fixture. Original staff
# records and rosters remain attached to this identity, then can be persuaded.
press(2);until(lambda:menu_count()==2);press(2);frames(50)
wr(record()+v[176],2,1);wr(record()+v[175],0,1)
warp(v[181],6,7);assert 210 in objectids()
call('VarSet',0x800F,210);call('GleStaffMemberSelect');assert rd('gSpecialVar_Result',2)==0
call('VarSet',0x800D,0);call('GleStaffAction');assert rd('gSpecialVar_Result',2)==3
wr(current()+v[2],8,1);wr(current()+26,100,2)
old=bytes(rd(record()+i,1) for i in range(v[168]))
call('VarSet',0x800D,0);call('GleStaffAction');assert rd('gSpecialVar_Result',2)==0
assert not rd(record()+v[176],1) and not rd(record()+v[175],1)
assert bytes(rd(record()+i,1) for i in range(6))==old[:6]
snap('staff-rival-return')
print('PASS retained developed roster/current-cap practice fixtures, visible rival defector, low-rank refusal and improved-Gym return preserving identity/records',flush=True)
# Dismiss from the trainer's own native interaction and verify the actor does
# not return through viewport-based object spawning before leaving the map.
state('staff-before-progression',1)
press(2);until(lambda:menu_count()==2);press(2);frames(50)
call('GleWarpHome');frames(240)
walkto(2,6);press(64);press(1);until(lambda:menu_count()==7)
choose(4);until(yesno);choose(0);until(lambda:menu_count()==1)
assert rd(record(),2)==0
press(2);frames(50)
assert call('GetObjectEventIdByLocalIdAndMap',210,v[23]&255,v[23]>>8)==16
walkto(6,13);walkto(3,13);walkto(3,6);walkto(2,6);frames(80)
assert call('GetObjectEventIdByLocalIdAndMap',210,v[23]&255,v[23]>>8)==16
print('PASS native confirmed dismissal removes the record and actor without lazy respawn',flush=True)
