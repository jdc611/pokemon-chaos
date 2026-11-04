"""Four weather surveys, persistent multiplexed Kanto roamers and Rider tracking."""
import runpy,json,struct
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
cdata=(ROOT/'winds-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(cdata)//4),cdata)
cat=json.load(open('docs/gym-leader-edition/field-research-sites.json'));species=v[-(len(cat)+30):-30];assert len(cat)>=35
assert c[0]==3072 and c[1]<=3200 and c[2]>=12 and c[3]==20
state('winds-migration-base')
def wp():return current()+c[0]
def mon(i):return wp()+i*c[3]
def data(i,field,n=1):return rd(mon(i)+c[field],n)
def setdata(i,field,value,n=1):wr(mon(i)+c[field],value,n)
def bit(n):return bool(rd(current()+v[114]+n//8,1)&(1<<(n%8)))
def info(slot):wr('gSpecialVar_LastTalked',200+slot,2);call('GleFieldInfo');return rd('gSpecialVar_Result',2)
def warp(slot,x=None,y=None):
 site=cat[slot];m=site['map_id'];lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,site['x'] if x is None else x,site['y']+1 if y is None else y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def floor(x,y):return call('MapGridGetCollisionAt',x+7,y+7)==0 and not call('MetatileBehavior_IsSurfableWaterOrUnderwater',call('MapGridGetMetatileBehaviorAt',x+7,y+7))
def settle():
 for _ in range(100):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 assert not call('ScriptContext_IsEnabled')
def approach():press(64);press(1)
def select(index):
 wr('gSpecialVar_Result',5,2);call('GleFieldNotebook');assert rd('gSpecialVar_Result',2)==1
 wr('gSpecialVar_Result',index,2);call('GleFieldReport');assert rd('gSpecialVar_Result',2)==1
 call('GleWindsTrack');return rd('gSpecialVar_Result',2)
def status(mon_addr,field,value):wr('gStringVar1',value);call('SetMonData',mon_addr,field,symbols['gStringVar1'])
# Exact v11 prefix migration includes every prior archive/license/final-Oak byte.
wr(current()+v[114]+27,rd(current()+v[114]+27,1)|128,1);wr(current()+v[78],11,2)
expected=bytearray(rd(current()+i,1) for i in range(c[0]));expected[v[78]:v[78]+2]=c[2].to_bytes(2,'little')
for i in range(c[1]-c[0]):wr(wp()+i,255,1)
call('GymLeaderState');assert bytes(rd(current()+i,1) for i in range(c[0]))==expected
assert rd(wp()+c[4],1)==255 and all(data(i,5)==0 for i in range(4))
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
wr('sWildEncountersDisabled',1,1)
for trainer in range(768):call('SetTrainerFlag',trainer)
print('PASS exact v11 prefix/competition/license/Oak preservation, zeroed new winds and native flash migration',flush=True)
# No new static legendary battle may bypass the roaming research.
wr(current()+v[2],1,1);wr(current()+v[-1],1,1);bad=[]
for i in range(4):
 slot=31+i;site=cat[slot];warp(slot);assert bit(130+slot) and info(slot)==0 and not select(i)
 x,y=site['x'],site['y'];original=json.load(open('data/maps/'+site['map_name']+'/map.json'));avoid={(p['x'],p['y']) for field in ['object_events','warp_events','coord_events','bg_events'] for p in original.get(field,[]) if 'x' in p}
 if not(floor(x,y) and floor(x,y+1)) or (x,y) in avoid or (x,y+1) in avoid:
  candidates=[]
  for dy in range(-6,7):
   for dx in range(-6,7):
    xx,yy=x+dx,y+dy
    if xx>=1 and yy>=1 and (xx,yy) not in avoid and (xx,yy+1) not in avoid and floor(xx,yy) and floor(xx,yy+1):candidates.append((abs(dx)+abs(dy),xx,yy))
  assert candidates;_,xx,yy=min(candidates);bad.append(dict(index=slot,x=xx,y=yy));print('SUGGEST',slot,xx,yy,flush=True)
(ROOT/'placement-fixes.json').write_text(json.dumps(bad));assert not bad,bad
wr(current()+v[2],8,1);wr(current()+v[-1],8,1)
identities=[]
for i in range(4):
 slot=31+i;warp(slot);assert info(slot)==3
 approach();until(lambda:menu_count()==3);choose((cat[slot]['answer']+1)%3);settle();assert not bit(165+slot)
 approach();until(lambda:menu_count()==3);choose(cat[slot]['answer']);until(yesno);choose(1);until(yesno);choose(0);settle()
 assert data(i,5)&3==1 and rd(wp()+c[4],1)==i and not bit(126+i)
 assert call('GymLeaderPrepareFieldBattle')==0
 identities.append((data(i,6,4),data(i,7,4),bool(data(i,5)&4)))
assert call('TryAddRoamer',c[30],40)==0
print('PASS four independent native weather seals/declines, release without captures, no static-battle bypass and legacy-roamer rejection',flush=True)
# Changing signals preserves all identities and native data; every route is in Kanto.
routes={rd(symbols['sWindRoutes']+i*2,2) for i in range(22)}
enemy=symbols['gParties']+6*v[5]
for _ in range(3):
 for i in [3,1,0,2]:
  assert select(i)
  assert (data(i,11)<<8|data(i,12)) in routes
  assert call('GymLeaderCreateWind')
  assert call('GetMonData2',enemy,v[10])==species[31+i] and call('GetMonData2',enemy,v[11])==100
  assert call('GetMonData2',enemy,v[36])==identities[i][1] and call('GetMonData2',enemy,v[12])<2
  assert call('GetMonData2',enemy,c[32])==identities[i][0] and bool(call('GetMonData2',enemy,c[23]))==identities[i][2]
  assert (data(i,6,4),data(i,7,4),bool(data(i,5)&4))==identities[i]
for i in range(4):
 assert select(i);assert call('GymLeaderCreateWind');hp=call('GetMonData2',enemy,c[27]);status(enemy,v[39],max(1,hp//2));status(enemy,c[22],8)
 wr('gBattleOutcome',4,1);call('UpdateRoamerHPStatus',enemy);assert data(i,8,4)==8 and data(i,9,2)==max(1,hp//2)
 assert select((i+1)%4) and select(i) and call('GymLeaderCreateWind');assert call('GetMonData2',enemy,c[22])==8
 wr('gBattleOutcome',c[24],1);call('UpdateRoamerHPStatus',enemy);call('SetRoamerInactive',0)
 assert rd(rd('gSaveBlock1Ptr')+c[14]+c[19],1)==1
 status(enemy,v[39],0);wr('gBattleOutcome',1,1);call('UpdateRoamerHPStatus',enemy);call('SetRoamerInactive',0);assert data(i,5)&3==2
 assert select(i) and data(i,5)&3==1 and data(i,8,4)==0 and data(i,9,2)==data(i,10,2)
 assert (data(i,6,4),data(i,7,4),bool(data(i,5)&4))==identities[i]
wr(current()+v[2],1,1)
for i in range(4):
 assert select(i) and call('GymLeaderCreateWind');assert call('GetMonData2',enemy,v[11])==15
# Arrival preserves a hunt opportunity; duplicate map loads do not reroll routes.
assert select(0);base=rd('gSaveBlock1Ptr');setdata(0,11,rd(base+c[28],1));setdata(0,12,rd(base+c[29],1));wr(wp()+c[4]+3,0,1)
call('GymLeaderMoveWinds');assert data(0,11)==rd(base+c[28],1) and data(0,12)==rd(base+c[29],1)
snapshot=bytes(rd(wp()+i,1) for i in range(80));call('GymLeaderMoveWinds');call('GymLeaderMoveWinds');assert bytes(rd(wp()+i,1) for i in range(80))==snapshot
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300);assert bytes(rd(wp()+i,1) for i in range(80))==snapshot
print('PASS all four native identities/IVs/shininess across signal changes, Kanto movement, HP/status persistence, Roar survival, faint/recovery and demotion-cap/flash',flush=True)
# The actual Rider map shows the selected signal in its native caption window.
assert select(0);warp(31)
wr(symbols['gMain']+c[31],symbols['CB2_ReturnToField']|1);call('SetMainCallback2',symbols['CB2_OpenFlyMap']|1);frames(400);snap('winds-rider-tracker');press(2,300)
# Place the selected signal on the current route, then use the normal native
# encounter decision and a real Master Ball capture without touching its HP.
base=rd('gSaveBlock1Ptr');setdata(0,11,rd(base+c[28],1));setdata(0,12,rd(base+c[29],1))
for _ in range(128):
 if call('TryStartRoamerEncounter'):break
else:raise AssertionError('native roaming encounter did not start')
assert call('GetMonData2',enemy,v[10])==species[31]
assert call('AddBagItem',v[115],1);call('BattleSetup_StartRoamerBattle')
for _ in range(1200):
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
 press(1,12)
else:raise AssertionError('wind action menu failed')
snap('winds-real-roamer-battle');press(16);press(1)
for _ in range(500):
 if task('Task_BagMenu_HandleInput')<16:break
 frames(1)
else:raise AssertionError('wind Bag did not open')
frames(100)
for _ in range(6):
 if rd(symbols['gBagPosition']+v[119],1)==v[120]:break
 press(16,50)
p=rd(symbols['gBagPockets']+v[118]*v[120]);target=next(i for i in range(v[124]) if rd(p+i*v[123],2)==v[115])
cursor=rd(symbols['gBagPosition']+v[121]+v[120]*2,2)+rd(symbols['gBagPosition']+v[122]+v[120]*2,2)
for _ in range(abs(target-cursor)):press(128 if target>cursor else 64)
press(1,25);press(1,25)
for _ in range(1200):
 if bit(126):break
 if yesno():choose(1)
 else:press(1,20)
else:raise AssertionError('actual wind capture did not record')
settle();assert data(0,5)&3==3 and not select(0) and rd('gPartiesCount',1)==4
captured=symbols['gParties']+3*v[5];assert call('GetMonData2',captured,v[10])==species[31] and call('GetMonData2',captured,v[36])==identities[0][1]
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and bit(126)
for i in range(1,4):assert not bit(126+i) and select(i)
print('PASS native Rider tracking, real roaming encounter/Master Ball capture/party identity and flash anti-respawn with three independent uncaught winds',flush=True)

# Existing non-GLE native roamer creation/location behavior still works.
old=rd(current());snapshot=bytes(rd(wp()+i,1) for i in range(80));wr(current(),0)
wr(rd('gSaveBlock1Ptr')+c[14]+c[19],0,1)
assert call('TryAddRoamer',c[30],40)
call('GetRoamerLocation',0,symbols['gStringVar1'],symbols['gStringVar2']);assert rd('gStringVar1',1)==0
assert bytes(rd(wp()+i,1) for i in range(80))==snapshot
wr(current(),old);call('GymLeaderMoveWinds')
print('PASS unchanged non-GLE native roamer creation/location path without modifying saved wind records',flush=True)
