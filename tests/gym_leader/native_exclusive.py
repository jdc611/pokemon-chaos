"""All 72 linked encounters plus controller-driven Glastrier approach/capture.
Gym rank/type and the Master Ball are explicit test fixtures, not balance wins.
"""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
assert gym_type==5
state('exclusive-opening-base')
positions=[(9,3),(5,12),(9,7),(7,13)]
def mapid():
 a=rd('gSaveBlock1Ptr')+v[25]
 return rd(a+1,1)|(rd(a,1)<<8)
def warp(slot):
 map=v[110+slot];x,y=positions[slot]
 lib.call6(symbols['SetWarpDestination'],map>>8,map&255,-1,x,y,0)
 # Map setup fixture skips the audio fade while testing many ARM calls.
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def obtained(ty,slot):
 bit=ty*4+slot
 return bool(rd(current()+v[114]+bit//8,1)&(1<<(bit%8)))
enemy=symbols['gParties']+6*v[5]
for slot,required in enumerate([3,4,6,8]):
 wr(current()+v[2],1,1);warp(slot)
 call('VarSet',0x8004,slot);call('GleExclusiveInfo')
 assert rd('gSpecialVar_Result',2)==0
 assert call('GymLeaderPrepareExclusiveBattle')==0
 wr(current()+v[2],required,1)
 for ty in range(18):
  wr(current()+v[3],ty,1);call('VarSet',0x8004,slot);call('GleExclusiveInfo')
  assert rd('gSpecialVar_Result',2)==1,(slot,ty,rd('gSpecialVar_Result',2),hex(mapid()),hex(v[110+slot]),rd(current()+v[2],1))
  assert call('GymLeaderPrepareExclusiveBattle')==1
  assert call('GetMonData2',enemy,v[10])==call('GymLeaderExclusiveSpecies',ty,slot)
  assert call('GetMonData2',enemy,v[11])==call('GymLeaderLevelCap')
  assert any(call('GetMonData2',enemy,v[38]+i) for i in range(4))
  for outcome in [1,3]:
   wr('gBattleOutcome',outcome,1);call('GleExclusiveFinish')
   assert not obtained(ty,slot)
  assert call('GymLeaderPrepareExclusiveBattle')==1
 wr(current()+v[3],5,1)
print('PASS all 72 linked species/levels/moves, rank gates and retry after defeat/flee transaction fixtures',flush=True)
# Direct capture transaction once for a different type/slot, then refusal.
wr(current()+v[3],0,1);call('VarSet',0x8004,3);call('GleExclusiveInfo')
assert call('GymLeaderPrepareExclusiveBattle')==1
wr('gBattleOutcome',7,1);call('GleExclusiveFinish')
assert obtained(0,3) and call('GymLeaderPrepareExclusiveBattle')==0
# Restore the untouched emulator timing/context for the actual UI capture.
state('exclusive-opening-base',1)
# Actual Ice player encounter in Seafoam, with correct overworld species graphic.
wr(current()+v[3],5,1);wr(current()+v[2],3,1);warp(0)
gfx=symbols['sExclusiveObjects']+2*v[116]+v[117]
assert rd(gfx,1)|(rd(gfx+1,1)<<8)==(16384|v[6])
snap('exclusive-glastrier-overworld')
press(64);press(1);until(yesno);snap('exclusive-save-prompt');choose(1)
until(yesno);snap('exclusive-glastrier-confirm');choose(1)
for _ in range(12):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
assert not obtained(5,0)
# Supply a guaranteed-catch item so this tests the real capture and bookkeeping,
# not the catch-rate balance or a forced outcome variable.
assert call('AddBagItem',v[115],1)
press(1);until(yesno);choose(1);until(yesno);choose(0)
for _ in range(1200):
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
 press(1,12)
else:
 snap('exclusive-action-failure');raise AssertionError(('exclusive battle did not reach the native action menu',hex(rd('gBattlerControllerFuncs')),hex(symbols['HandleInputChooseAction']|1)))
assert call('GetMonData2',enemy,v[10])==v[6]
snap('exclusive-glastrier-battle')
press(16);press(1)
for _ in range(500):
 if task('Task_BagMenu_HandleInput')<16:break
 frames(1)
else:raise AssertionError('battle Bag did not open')
frames(100)
for _ in range(6):
 if rd(symbols['gBagPosition']+v[119],1)==v[120]:break
 press(16,50)
else:raise AssertionError('Poké Ball pocket unavailable')
ptr=rd(symbols['gBagPockets']+v[118]*v[120])
target=next(i for i in range(v[124]) if rd(ptr+i*v[123],2)==v[115])
cursor=rd(symbols['gBagPosition']+v[121]+v[120]*2,2)+rd(symbols['gBagPosition']+v[122]+v[120]*2,2)
for _ in range(abs(target-cursor)):press(128 if target>cursor else 64)
snap('exclusive-master-ball-fixture')
press(1,25);press(1,25)
for _ in range(1000):
 if obtained(5,0):break
 if yesno():choose(1)
 else:press(1,20)
else:
 snap('exclusive-capture-failure');raise AssertionError('native capture did not record unique encounter')
assert rd('gPartiesCount',1)==4
caught=symbols['gParties']+3*v[5]
assert call('GetMonData2',caught,v[10])==v[6]
assert call('GetMonData2',caught,v[11])==35
for _ in range(15):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
assert call('TrySavingData',0)==1
expected=bytes(rd(current()+i,1) for i in range(v[1]))
assert call('LoadGameSave',0)==1
assert bytes(rd(current()+i,1) for i in range(v[1]))==expected
call('VarSet',0x8004,0);call('GleExclusiveInfo')
assert rd('gSpecialVar_Result',2)==2 and not call('GymLeaderPrepareExclusiveBattle')
snap('exclusive-captured-and-saved')
print('PASS actual Glastrier sprite, save prompt, declined approach, native wild battle/Master Ball capture, unique record and flash persistence',flush=True)
