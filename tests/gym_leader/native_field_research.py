"""Five research projects: native clues, field seals, captures and flash records."""
import runpy,json
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
catalog=json.load(open('docs/gym-leader-edition/field-research-sites.json'))
sites=catalog[:15];species=v[-(len(catalog)+30):-30][:15]
assert len(species)==len(sites)==15
highest=v[-1]
def warp(slot):
 site=sites[slot];m=site['map_id'];lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,site['x'],site['y']+1,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def bit(n):return bool(rd(current()+v[114]+n//8,1)&(1<<(n%8)))
def objects():
 events=rd(symbols['gMapHeader']+v[157]);p=rd(events+v[158])
 return [p+i*v[116] for i in range(rd(events,1))]
def field(slot):return next((p for p in objects() if rd(p,1)==200+slot),None)
def info(slot):wr('gSpecialVar_LastTalked',200+slot,2);call('GleFieldInfo');return rd('gSpecialVar_Result',2)
def settle():
 for _ in range(35):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 assert not call('ScriptContext_IsEnabled')
def approach():press(64);press(1)
enemy=symbols['gParties']+6*v[5]
# Isolate interaction tests from ordinary random grass battles.
wr('sWildEncountersDisabled',1,1)
# Nearby original trainers are a completed-battle fixture, not removed from gameplay.
for trainer in range(768):call('SetTrainerFlag',trainer)
state('field-opening')
# Origin evidence gates each Loyal Three encounter even with rank eligibility.
wr(current()+v[2],8,1);wr(current()+highest,8,1);warp(11)
assert info(11)==4 and call('GymLeaderPrepareFieldBattle')==0
approach();settle()
state('field-before-discoveries')
# Each group remembers early discovery independently of current/earned rank.
wr(current()+v[2],1,1);wr(current()+highest,1,1)
for slot,site in enumerate(sites):
 warp(slot);p=field(slot);assert p,('missing object',slot)
 assert bit(130+slot) and not bit(95+slot)
 assert info(slot)==0 and call('GymLeaderPrepareFieldBattle')==0
 x,y=site['x']+7,site['y']+7
 assert call('MapGridGetCollisionAt',x,y)==0,('solid research site',slot)
 assert call('MapGridGetCollisionAt',x,y+1)==0,('solid approach',slot)
 assert not call('MetatileBehavior_IsSurfableWaterOrUnderwater',call('MapGridGetMetatileBehaviorAt',x,y))
 assert call('GymLeaderSpeciesReserved',species[slot])
for project,count in [(7,4),(9,4),(8,3),(13,3),(11,1)]:assert rd(current()+v[137]+project,1)==count
print('PASS 15 native placements/dry walkable approaches, early discoveries, rank refusal, reservation and three-site Toxic Pact evidence gate',flush=True)
# All fifteen inscriptions use actual controller-driven menus.
wr(current()+v[2],8,1);wr(current()+highest,8,1)
for slot,site in enumerate(sites):
 warp(slot);assert info(slot)==3
 assert call("GetObjectEventIdByLocalId",200+slot)<16,("inactive field actor",slot)

 wr('gSpecialVar_Result',127,2);call('GleFieldSolve');assert not bit(165+slot)
 approach();until(lambda:menu_count()==3)
 if slot==0:snap('field-treasure-inscription-menu')
 choose((site['answer']+1)%3);settle();assert not bit(165+slot)
 approach();until(lambda:menu_count()==3);choose(site['answer']);until(yesno)
 choose(1);until(yesno);choose(1);settle()
 assert bit(165+slot) and info(slot)==1
 assert call('GymLeaderPrepareFieldBattle')==1
 assert call('GetMonData2',enemy,v[10])==species[slot]
 assert call('GetMonData2',enemy,v[11])==100
 assert any(call('GetMonData2',enemy,v[38]+j) for j in range(4))
 for outcome in [1,3]:
  wr('gBattleOutcome',outcome,1);call('GleFieldFinish');assert not bit(95+slot)
print('PASS all 15 real native seal menus/wrong-answer retries, cancel transaction refusal, solved persistence, save/battle declines and native species/levels/moves/retries',flush=True)
# All three forest research objects coexist with unchanged original map objects.
wr(current()+v[138]+12,1,1);warp(0)
assert field(0) and field(14) and any(rd(p,1)==250 for p in objects())
assert len({rd(p,1) for p in objects()})==len(objects())
# Every earned encounter remains scaled to the demoted current cap.
wr(current()+v[2],1,1)
for slot in range(15):
 warp(slot);assert info(slot)==1 and call('GymLeaderPrepareFieldBattle')==1
 assert call('GetMonData2',enemy,v[11])==15
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
state('field-solved')
for slot in [14,11,3,8,1,12,5,10,6,0,13,4,9,7,2]:
 warp(slot);assert info(slot)==1 and call('GymLeaderPrepareFieldBattle')==1
 wr('gBattleOutcome',7,1);call('GleFieldFinish');assert bit(95+slot)
 warp(slot);assert not field(slot)
 assert call('GymLeaderPrepareFieldBattle')==0
print('PASS three coexisting forest research objects, distinct IDs, demotion/current cap, flash and all-15 independent no-respawn ledgers',flush=True)
# The actual GLA notebooks expose each project's native menus and report pages.
state('field-solved',1);call('GleWarpLeague');frames(240)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(100):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('field desk blocked',pos(),(x,y)))
walkto(6,9);walkto(5,9);walkto(5,8);press(32);press(1)
until(lambda:menu_count()==17)
for project,n in [(7,5),(8,4),(9,5),(11,2),(13,4)]:
 choose(project);until(lambda:menu_count()==n);choose(0);until(lambda:menu_count()==n)
 snap('field-notebook-'+str(project));press(2);until(lambda:menu_count()==17)
press(2);settle()
print('PASS five actual native GLA project notebooks, detailed reports and cancel/return flow',flush=True)
# Capture uses a real Uxie battle and test-supplied Master Ball, not forced HP/outcome.
state('field-solved',1);warp(8);assert call('AddBagItem',v[115],1)
approach();until(yesno);choose(1);until(yesno);choose(0)
for _ in range(1200):
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
 press(1,12)
else:raise AssertionError('Uxie action menu failed')
assert call('GetMonData2',enemy,v[10])==species[8]
snap('field-uxie-battle')
press(16);press(1)
for _ in range(500):
 if task('Task_BagMenu_HandleInput')<16:break
 frames(1)
else:raise AssertionError('battle Bag did not open')
frames(100)
for _ in range(6):
 if rd(symbols['gBagPosition']+v[119],1)==v[120]:break
 press(16,50)
p=rd(symbols['gBagPockets']+v[118]*v[120]);target=next(i for i in range(v[124]) if rd(p+i*v[123],2)==v[115])
cursor=rd(symbols['gBagPosition']+v[121]+v[120]*2,2)+rd(symbols['gBagPosition']+v[122]+v[120]*2,2)
for _ in range(abs(target-cursor)):press(128 if target>cursor else 64)
press(1,25);press(1,25)
for _ in range(1000):
 if bit(103):break
 if yesno():choose(1)
 else:press(1,20)
else:raise AssertionError('actual Uxie capture did not record')
settle();assert rd('gPartiesCount',1)==4
assert call('GetMonData2',symbols['gParties']+3*v[5],v[10])==species[8]
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
warp(8);assert not field(8) and bit(103)
assert sum(bit(95+i) for i in range(15))==1
print('PASS actual Uxie battle/Master Ball capture, party delivery, native flash/no-respawn and independent remaining field research',flush=True)
