"""All 72 postgame allocations and a played non-native-type capture.

Runs the existing pre-ending exclusive suite first, including real Glastrier
capture. The ending flag and demotion are explicit progression fixtures.
"""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_exclusive.py');globals().update(ctx)
assert obtained(5,0)
# Selecting another type is refused until Oak's examination is complete.
call('VarSet',0x8004,0);wr('gSpecialVar_Result',0,2);call('GleExclusiveSelect');assert rd('gSpecialVar_Result',2)==0
wr(current()+v[114]+223//8,rd(current()+v[114]+223//8,1)|128,1)
wr(current()+v[2],1,1);assert call('GymLeaderFinalOakComplete')
for slot in range(4):
 warp(slot)
 for ty in range(18):
  call('VarSet',0x8004,slot);wr('gSpecialVar_Result',ty,2);call('GleExclusiveSelect')
  assert rd('gSpecialVar_Result',2)==(2 if obtained(ty,slot) else 1)
  if obtained(ty,slot):assert not call('GymLeaderPrepareExclusiveBattle');continue
  assert call('GymLeaderPrepareExclusiveBattle')==1
  assert call('GetMonData2',enemy,v[10])==call('GymLeaderExclusiveSpecies',ty,slot)
  assert call('GetMonData2',enemy,v[11])==call('GymLeaderLevelCap')==15
  wr('gBattleOutcome',3,1);call('GleExclusiveFinish');assert not obtained(ty,slot)
  call('GleExclusiveRefresh');assert rd('gSpecialVar_Result',2)==1
print('PASS postgame-only selection, all 72 native identities, retained own-type catch, demotion/current cap and independent retries',flush=True)
warp(0);press(64);press(1);until(lambda:menu_count()==18);snap('postgame-field-type-menu')
press(2)
for _ in range(25):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
assert not call('ScriptContext_IsEnabled')
press(1);until(lambda:menu_count()==18);choose(0);until(yesno);choose(1);until(yesno)
snap('postgame-other-type-confirm');assert not obtained(0,0)
assert call('AddBagItem',v[115],1);choose(0)
until(lambda:rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1,1200)
assert call('GetMonData2',enemy,v[10])==call('GymLeaderExclusiveSpecies',0,0)
snap('postgame-other-type-battle');press(16);press(1)
for _ in range(500):
 if task('Task_BagMenu_HandleInput')<16:break
 frames(1)
else:raise AssertionError('postgame battle Bag unavailable')
frames(100)
for _ in range(6):
 if rd(symbols['gBagPosition']+v[119],1)==v[120]:break
 press(16,50)
else:raise AssertionError('postgame Poke Ball pocket unavailable')
ptr=rd(symbols['gBagPockets']+v[118]*v[120])
target=next(i for i in range(v[124]) if rd(ptr+i*v[123],2)==v[115])
cursor=rd(symbols['gBagPosition']+v[121]+v[120]*2,2)+rd(symbols['gBagPosition']+v[122]+v[120]*2,2)
for _ in range(abs(target-cursor)):press(128 if target>cursor else 64)
press(1,25);press(1,25)
for _ in range(1000):
 if obtained(0,0):break
 if yesno():choose(1)
 else:press(1,20)
else:raise AssertionError('postgame actual capture not recorded')
for _ in range(25):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
assert not call('ScriptContext_IsEnabled') and obtained(5,0)
assert call('GetMonData2',symbols['gParties']+4*v[5],v[10])==call('GymLeaderExclusiveSpecies',0,0)
# The researcher stays present after success and the same allocation is quiet.
press(1);until(lambda:menu_count()==18);choose(0)
for _ in range(50):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
assert not call('ScriptContext_IsEnabled')
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and obtained(0,0) and obtained(5,0)
call('VarSet',0x8004,0);wr('gSpecialVar_Result',0,2);call('GleExclusiveSelect')
assert rd('gSpecialVar_Result',2)==2 and not call('GymLeaderPrepareExclusiveBattle')
print('PASS actual postgame type-menu cancel/selection, non-native-type wild battle and capture, resident researcher, no duplicate and flash persistence',flush=True)
