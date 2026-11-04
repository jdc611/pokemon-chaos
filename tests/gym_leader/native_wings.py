"""Universal six-bird research, native field prompts and actual Articuno capture."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(100):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('walk blocked',pos(),(x,y)))
walkto(6,9);walkto(5,9);walkto(5,8);press(32);press(1)
until(lambda:menu_count()==17)
choose(12);until(lambda:menu_count()==17)
assert not rd(current()+v[138]+12,1)
wr(current()+v[2],3,1)
choose(12);until(lambda:menu_count()==7)
assert rd(current()+v[138]+12,1)==1
choose(0);until(lambda:menu_count()==7)
snap('wings-notebook')
press(2);until(lambda:menu_count()==17);press(2);frames(50)
assert not call('ScriptContext_IsEnabled')
state('wings-opened')
spots=[(14,18),(41,24),(30,7),(39,27),(17,93),(43,7)]
approaches=[(14,19,64),(41,25,64),(30,8,64),(39,28,64),(17,94,64),(42,7,16)]
def warp(slot):
 m=v[145+slot];x,y,_=approaches[slot]
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def objects():
 events=rd(symbols['gMapHeader']+v[157]);p=rd(events+v[158])
 return [p+i*v[116] for i in range(rd(events,1))]
def bird():return next((p for p in objects() if rd(p,1)==250),None)
def captured(slot):
 bit=73+slot;return bool(rd(current()+v[114]+bit//8,1)&(1<<(bit%8)))
highest_rank=v[-1]
wr(current()+highest_rank,1,1)
enemy=symbols['gParties']+6*v[5]
for slot,rank in enumerate([3,4,5,6,7,8]):
 wr(current()+v[2],rank-1,1);warp(slot)
 call('GleWingsInfo');assert rd('gSpecialVar_Result',2)==0
 assert not bird() and call('GymLeaderPrepareWingsBattle')==0
 wr(current()+v[2],rank,1);warp(slot)
 b=bird();assert b
 assert rd(b+v[117],1)|(rd(b+v[117]+1,1)<<8)==(16384|v[151+slot])
 x,y=spots[slot]
 assert call('MapGridGetCollisionAt',x+7,y+7)==0
 px,py,_=approaches[slot];assert call('MapGridGetCollisionAt',px+7,py+7)==0
 call('GleWingsInfo');assert rd('gSpecialVar_Result',2)==1
 assert call('GymLeaderPrepareWingsBattle')==1
 assert call('GetMonData2',enemy,v[10])==v[151+slot]
 assert call('GetMonData2',enemy,v[11])==call('GymLeaderLevelCap')
 assert any(call('GetMonData2',enemy,v[38]+j) for j in range(4))
 for outcome in [1,3]:
  wr('gBattleOutcome',outcome,1);call('GleWingsFinish');assert not captured(slot)
 # The actual object script is accessible from an adjacent, walkable tile.
 press(approaches[slot][2]);press(1);until(yesno);choose(1);until(yesno);choose(1)
 for _ in range(15):
  if not call('ScriptContext_IsEnabled'):break
  press(1,20)
 assert not call('ScriptContext_IsEnabled')
 assert not captured(slot)
print('PASS six independent migration sites, ranks, actual native approach/decline, collision, species/levels/moves and defeat/flee retries',flush=True)
# Earned sightings remain available after demotion, at the current level cap.
for slot,rank in enumerate([3,4,5,6,7,8]):
 wr(current()+highest_rank,rank,1);wr(current()+v[2],1,1);warp(slot)
 assert bird(),('demotion removed unlocked roost',slot)
 call('GleWingsInfo');assert rd('gSpecialVar_Result',2)==1
 assert call('GymLeaderPrepareWingsBattle')==1
 assert call('GetMonData2',enemy,v[11])==call('GymLeaderLevelCap')==15
 if slot<5:
  warp(slot+1);call('GleWingsInfo')
  assert not bird() and rd('gSpecialVar_Result',2)==0
  assert call('GymLeaderPrepareWingsBattle')==0
assert call('TrySavingData',0)==1
assert call('LoadGameSave',0)==1
warp(5);call('GleWingsInfo')
assert bird() and rd('gSpecialVar_Result',2)==1
assert call('GymLeaderPrepareWingsBattle')==1
assert call('GetMonData2',enemy,v[11])==15
wr(current()+v[138]+12,0,1)
wr(current()+highest_rank,3,1)
call('GleWingsNotebook');assert rd('gSpecialVar_Result',2)==1
assert rd(current()+v[138]+12,1)==1
print('PASS permanent earned roost access after demotion/flash reload, current-cap levels and future-rank refusal',flush=True)
# Capture order is independent; no project enforces another bird's capture.
for slot in [5,1,4,0,3,2]:
 wr(current()+v[2],8,1);warp(slot);call('GleWingsInfo')
 assert call('GymLeaderPrepareWingsBattle')==1
 wr('gBattleOutcome',7,1);call('GleWingsFinish');assert captured(slot)
 assert call('GymLeaderPrepareWingsBattle')==0
 warp(slot);assert not bird()
assert rd(current()+v[137]+12,1)==6
print('PASS nonsequential six-capture ledger, no duplicate encounters and completion count',flush=True)
# Restore clean native timing for real capture, not an injected outcome.
state('wings-opened',1);wr(current()+v[2],3,1);warp(0)
assert call('AddBagItem',v[115],1)
snap('research-articuno-roost')
press(64);press(1);until(yesno);choose(1);until(yesno);choose(0)
for _ in range(1200):
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
 press(1,12)
else:raise AssertionError('Articuno action menu failed')
assert call('GetMonData2',enemy,v[10])==v[151]
snap('research-articuno-battle')
press(16);press(1)
for _ in range(500):
 if task('Task_BagMenu_HandleInput')<16:break
 frames(1)
else:raise AssertionError('battle Bag did not open')
frames(100)
for _ in range(6):
 if rd(symbols['gBagPosition']+v[119],1)==v[120]:break
 press(16,50)
p=rd(symbols['gBagPockets']+v[118]*v[120])
target=next(i for i in range(v[124]) if rd(p+i*v[123],2)==v[115])
cursor=rd(symbols['gBagPosition']+v[121]+v[120]*2,2)+rd(symbols['gBagPosition']+v[122]+v[120]*2,2)
for _ in range(abs(target-cursor)):press(128 if target>cursor else 64)
press(1,25);press(1,25)
for _ in range(1000):
 if captured(0):break
 if yesno():choose(1)
 else:press(1,20)
else:raise AssertionError('actual Articuno capture did not record')
assert rd('gPartiesCount',1)==4
assert call('GetMonData2',symbols['gParties']+3*v[5],v[10])==v[151]
assert rd(current()+v[137]+12,1)==1
# Its capture is separate from Ice's Glastrier allocation.
assert not rd(current()+v[114]+(5*4)//8,1)&(1<<((5*4)%8))
for _ in range(15):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
assert call('TrySavingData',0)==1
expected=bytes(rd(current()+i,1) for i in range(v[1]))
assert call('LoadGameSave',0)==1
assert bytes(rd(current()+i,1) for i in range(v[1]))==expected
warp(0);assert not bird()
snap('research-articuno-captured')
print('PASS native Articuno battle/Master Ball capture, research-only ledger, flash save and no respawn; Ice Glastrier untouched',flush=True)
