"""Non-consuming Cyclizar study and independent ancient/future research captures."""
import runpy,json
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
all_sites=json.load(open('docs/gym-leader-edition/field-research-sites.json'))
sites=all_sites[15:18];species=v[-(len(all_sites)+30):-30][15:18]
assert len(sites)==len(species)==3
wr('sWildEncountersDisabled',1,1)
for trainer in range(768):call('SetTrainerFlag',trainer)
def bit(n):return bool(rd(current()+v[114]+n//8,1)&(1<<(n%8)))
def warp(slot):
 site=sites[slot];m=site['map_id'];lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,site['x'],site['y']+1,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def info(slot):wr('gSpecialVar_LastTalked',215+slot,2);call('GleFieldInfo');return rd('gSpecialVar_Result',2)
def object_exists(slot):
 events=rd(symbols['gMapHeader']+v[157]);p=rd(events+v[158])
 return any(rd(p+i*v[116],1)==215+slot for i in range(rd(events,1)))
def approach():press(64);press(1)
def settle():
 for _ in range(60):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 snap('paradox-script-failure');assert not call('ScriptContext_IsEnabled')
def solve(slot):
 warp(slot);approach();until(lambda:menu_count()==3);choose(sites[slot]['answer']);until(yesno);choose(1);until(yesno);choose(1);settle()
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(100):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('paradox desk blocked',pos(),(x,y)))
def notebook():
 call('GleWarpLeague');frames(240)
 walkto(6,9);walkto(5,9);walkto(5,8);press(32);press(1)
 until(lambda:menu_count()==18);choose(1);until(lambda:menu_count()==4)
 press(2);until(lambda:menu_count()==18);press(2);settle()
enemy=symbols['gParties']+6*v[5]
notebook();assert not bit(208)
wr(current()+v[2],8,1);wr(current()+v[-1],8,1)
# Future is initially locked by the live study, even when rank and evidence exist.
warp(2);assert info(2)==4 and call('GymLeaderPrepareFieldBattle')==0
assert bit(147) and not bit(146)
approach();settle()
warp(1);assert info(1)==4 and call('GymLeaderPrepareFieldBattle')==0
assert bit(146) and not bit(111)
for slot,site in enumerate(sites):
 warp(slot);x,y=site['x']+7,site['y']+7
 assert object_exists(slot) and call('GetObjectEventIdByLocalId',215+slot)<16
 assert not call('MapGridGetCollisionAt',x,y) and not call('MapGridGetCollisionAt',x,y+1)
 assert call('GymLeaderSpeciesReserved',species[slot])
print('PASS native Paradox notebook/missing-study refusal, remembered ancient/future evidence and three walkable reserved field actors',flush=True)
# A real Cyclizar capture supplies the live study, rather than a lab gift/clone.
wr(current()+v[2],2,1);wr(current()+v[-1],2,1);solve(0)
assert call('AddBagItem',v[115],1)
warp(0);approach();until(yesno);choose(1);until(yesno);choose(0)
for _ in range(1200):
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
 press(1,12)
else:raise AssertionError('Cyclizar action menu failed')
assert call('GetMonData2',enemy,v[10])==species[0]
snap('paradox-cyclizar-battle')
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
 if bit(110):break
 if yesno():choose(1)
 else:press(1,20)
else:raise AssertionError('actual Cyclizar capture did not record')
settle();assert rd('gPartiesCount',1)==4
mon=symbols['gParties']+3*v[5]
assert call('GetMonData2',mon,v[10])==species[0]
original=bytes(rd(mon+i,1) for i in range(v[5]))
state('paradox-before-study')
notebook();assert bit(208)
mon=symbols['gParties']+3*v[5]
assert bytes(rd(mon+i,1) for i in range(v[5]))==original
assert rd('gPartiesCount',1)==4
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert bit(208) and bytes(rd(mon+i,1) for i in range(v[5]))==original
print('PASS actual Cyclizar battle/Master Ball capture and non-consuming native GLA live study, exact party identity/stats/moves preservation and flash',flush=True)
# Neither legendary needs a prior capture. Future needs only the ancient survey.
wr(current()+v[2],8,1);wr(current()+v[-1],8,1)
for slot in [2,1]:
 warp(slot);assert info(slot)==3;solve(slot)
 assert info(slot)==1 and call('GymLeaderPrepareFieldBattle')==1
 assert call('GetMonData2',enemy,v[10])==species[slot]
 assert call('GetMonData2',enemy,v[11])==100
 assert any(call('GetMonData2',enemy,v[38]+j) for j in range(4))
 for outcome in [1,3]:
  wr('gBattleOutcome',outcome,1);call('GleFieldFinish');assert not bit(110+slot)
assert not bit(111) and not bit(112)
wr(current()+v[2],1,1)
for slot in [2,1]:
 warp(slot);assert info(slot)==1 and call('GymLeaderPrepareFieldBattle')==1
 assert call('GetMonData2',enemy,v[11])==15
 wr('gBattleOutcome',7,1);call('GleFieldFinish');assert bit(110+slot)
 warp(slot);assert not object_exists(slot)
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert bit(111) and bit(112) and bit(208)
print('PASS native ancient/future seals, independent Koraidon/Miraidon species/moves, no-capture prerequisite, defeat/flee retry, demotion/current cap and anti-respawn flash records',flush=True)
# The PC study finds the actual captured Cyclizar without modifying the box record.
# Emulator RAM snapshots do not restore flash; keep the newer flash generation
# when restoring this independent branch, so reload cannot select a later save.
latest_flash_counter=rd('gSaveCounter')
state('paradox-before-study',1)
wr('gSaveCounter',latest_flash_counter)
mon=symbols['gParties']+3*v[5];storage=rd('gPokemonStoragePtr')+v[34]
call('CopyMonToPC',mon)
assert call('GetBoxMonData2',storage,v[10])==species[0]
call('ZeroMonData',mon);wr('gPartiesCount',3,1)
original_box=bytes(rd(storage+i,1) for i in range(v[33]))
notebook();assert bit(208)
storage=rd('gPokemonStoragePtr')+v[34]
assert bytes(rd(storage+i,1) for i in range(v[33]))==original_box
assert rd('gPartiesCount',1)==3
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
storage=rd('gPokemonStoragePtr')+v[34]
after_box=bytes(rd(storage+i,1) for i in range(v[33]))
assert bit(208), 'study flag lost on flash reload'
assert after_box==original_box, ('PC bytes changed on flash',[(i,a,b) for i,(a,b) in enumerate(zip(original_box,after_box)) if a!=b],call('GetBoxMonData2',storage,v[10]))
print('PASS actual native PC-held Cyclizar study, exact box identity preservation, no gift/duplicate/evolution and native flash persistence',flush=True)
