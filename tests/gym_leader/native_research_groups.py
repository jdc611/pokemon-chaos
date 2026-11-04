"""Swords, beast/Paradox pairs and Creation Trio native field integration."""
import runpy,json
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
catalog=json.load(open('docs/gym-leader-edition/field-research-sites.json'));species=v[-(len(catalog)+30):-30];highest=v[-1]
assert len(catalog)==len(species)==31
wr('sWildEncountersDisabled',1,1)
for trainer in range(768):call('SetTrainerFlag',trainer)
def warp(slot,x=None,y=None):
 site=catalog[slot];m=site['map_id'];lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,site['x'] if x is None else x,site['y']+1 if y is None else y,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def bit(n):return bool(rd(current()+v[114]+n//8,1)&(1<<(n%8)))
def info(slot):wr('gSpecialVar_LastTalked',200+slot,2);call('GleFieldInfo');return rd('gSpecialVar_Result',2)
def floor(x,y):return call('MapGridGetCollisionAt',x+7,y+7)==0 and not call('MetatileBehavior_IsSurfableWaterOrUnderwater',call('MapGridGetMetatileBehaviorAt',x+7,y+7))
def object_exists(slot):
 events=rd(symbols['gMapHeader']+v[157]);p=rd(events+v[158]);return any(rd(p+i*v[116],1)==200+slot for i in range(rd(events,1)))
# Fresh unrelated-project state, then test each prerequisite independently.
state('more-research-base')
for slot in [21,25,26,27,30]:
 state('more-research-base',1);wr(current()+v[2],8,1);wr(current()+highest,8,1);warp(slot)
 assert info(slot)==4 and call('GymLeaderPrepareFieldBattle')==0,('missing evidence gate',slot)
print('PASS independent Keldeo, three ancient echoes and Giratina discovery gates without capture prerequisites',flush=True)
state('more-research-base',1);wr(current()+v[2],1,1);wr(current()+highest,1,1)
bad=[]
for slot in range(18,31):
 site=catalog[slot];warp(slot);x,y=site['x'],site['y']
 assert object_exists(slot) and bit(130+slot) and not bit(95+slot)
 assert info(slot)==0 and call('GymLeaderPrepareFieldBattle')==0
 assert call('GymLeaderSpeciesReserved',species[slot])
 existing=json.load(open('data/maps/'+site['map_name']+'/map.json'));avoid={(p['x'],p['y']) for field in ['object_events','warp_events','coord_events','bg_events'] for p in existing.get(field,[]) if 'x' in p}
 if not (floor(x,y) and floor(x,y+1)) or (x,y) in avoid or (x,y+1) in avoid:
  candidates=[]
  for dy in range(-6,7):
   for dx in range(-6,7):
    xx,yy=x+dx,y+dy
    if xx>=1 and yy>=1 and (xx,yy) not in avoid and (xx,yy+1) not in avoid and floor(xx,yy) and floor(xx,yy+1):candidates.append((abs(dx)+abs(dy),xx,yy))
  assert candidates,('no safe placement',slot);_,xx,yy=min(candidates);bad.append(dict(index=slot,x=xx,y=yy));print('SUGGEST',slot,xx,yy,flush=True)
(ROOT/'placement-fixes.json').write_text(json.dumps(bad))
assert not bad,('solid sites/approaches',bad)
for project,count in [(6,4),(10,6),(2,3)]:assert rd(current()+v[137]+project,1)==count
print('PASS all 13 new native site templates, floor/approach collision, universal reservation and rank-1 discovery records',flush=True)
def settle():
 for _ in range(60):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 assert not call('ScriptContext_IsEnabled')
def approach():press(64);press(1)
wr(current()+highest,8,1);enemy=symbols['gParties']+6*v[5]
for slot in range(18,31):
 warp(slot);assert info(slot)==3
 assert call('GetObjectEventIdByLocalId',200+slot)<16
 approach();until(lambda:menu_count()==3);choose((catalog[slot]['answer']+1)%3);settle();assert not bit(165+slot)
 approach();until(lambda:menu_count()==3);choose(catalog[slot]['answer']);until(yesno);choose(1);until(yesno);choose(1);settle();assert bit(165+slot)
 assert call('GymLeaderPrepareFieldBattle')==1
 assert call('GetMonData2',enemy,v[10])==species[slot] and call('GetMonData2',enemy,v[11])==15
 wr('gBattleOutcome',4,1);call('GleFieldFinish');assert not bit(95+slot)
 # Failed encounters retain the opened field seal and can be retried.
 assert call('GymLeaderPrepareFieldBattle')==1
 wr('gBattleOutcome',7,1);call('GleFieldFinish');assert bit(95+slot) and call('GymLeaderPrepareFieldBattle')==0
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
for slot in range(18,31):assert bit(130+slot) and bit(95+slot)
for project in [2,6,10]:wr('gSpecialVar_Result',project,2);call('GleFieldNotebook');assert rd('gSpecialVar_Result',2)==1
print('PASS all 13 native seals/wrong answers, earned-rank access after demotion/current-cap scaling, independent outcomes/recovery/anti-duplicates and flash; three project notebooks',flush=True)
