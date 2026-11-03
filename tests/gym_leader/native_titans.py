"""Ancient Titans: discovery, seals, independent captures and real Regirock catch."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
highest=v[-1];maps=v[-13:-7];species=v[-7:-1]
assert len(maps)==len(species)==6
state('titans-opening')
def warp(slot):
 m=maps[slot];lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,11,9,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def bit(n):return bool(rd(current()+v[114]+n//8,1)&(1<<(n%8)))
def guardian():
 events=rd(symbols['gMapHeader']+v[157]);p=rd(events+v[158])
 return next((p+i*v[116] for i in range(rd(events,1)) if rd(p+i*v[116],1)==251),None)
def settle():
 for _ in range(25):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 assert not call('ScriptContext_IsEnabled')
def approach():press(64);press(1)
enemy=symbols['gParties']+6*v[5]
# Visit the final temple first; rank alone cannot bypass the five discoveries.
wr(current()+v[2],6,1);wr(current()+highest,6,1);warp(5)
call('GleTitansInfo');assert rd('gSpecialVar_Result',2)==4
assert call('GymLeaderPrepareTitanBattle')==0
approach();settle();snap('titans-final-sealed')
# Discovery is remembered before rank eligibility, and does not require catches.
wr(current()+v[2],1,1);wr(current()+highest,1,1)
for slot in [3,0,4,1,2]:
 warp(slot);assert guardian()
 assert bit(85+slot) and not bit(79+slot)
 call('GleTitansInfo');assert rd('gSpecialVar_Result',2)==0
 assert call('GymLeaderPrepareTitanBattle')==0
 assert call('MapGridGetCollisionAt',18,15)==0,(slot,'guardian floor')
 assert call('MapGridGetCollisionAt',18,16)==0,(slot,'adjacent floor')
assert rd(current()+v[137]+3,1)==5
wr(current()+v[2],6,1);wr(current()+highest,6,1);warp(5)
call('GleTitansInfo');assert rd('gSpecialVar_Result',2)==1
assert call('GymLeaderPrepareTitanBattle')==1
assert not any(bit(79+i) for i in range(6))
assert call('GetMonData2',enemy,v[10])==species[5]
print('PASS five any-order early chamber discoveries unlock final titan without captures; native locked return and walkable placements',flush=True)
state('titans-discovered')
answers=[0,1,2,0,1]
for slot in range(5):
 warp(slot);call('GleTitansInfo');assert rd('gSpecialVar_Result',2)==3
 wr('gSpecialVar_Result',127,2);call('GleTitansSolve');assert not bit(90+slot)
 wr('gSpecialVar_Result',(answers[slot]+1)%3,2);call('GleTitansSolve');assert not bit(90+slot)
 # Use the real native inscription and answer menu, not a solved-state injection.
 approach();until(lambda:menu_count()==3);choose((answers[slot]+1)%3);settle()
 assert not bit(90+slot)
 approach();until(lambda:menu_count()==3);choose(answers[slot]);until(yesno)
 choose(1);until(yesno);choose(1);settle()
 assert bit(90+slot) and not bit(79+slot)
 call('GleTitansInfo');assert rd('gSpecialVar_Result',2)==1
 assert call('GymLeaderPrepareTitanBattle')==1
 assert call('GetMonData2',enemy,v[10])==species[slot]
 assert call('GetMonData2',enemy,v[11])==call('GymLeaderLevelCap')
 assert any(call('GetMonData2',enemy,v[38]+j) for j in range(4))
 for outcome in [1,3]:
  wr('gBattleOutcome',outcome,1);call('GleTitansFinish');assert not bit(79+slot)
snap('titans-seal-solved')
print('PASS five actual inscription menus, wrong/cancel retries, persistent seals, save/approach declines, native species/levels/moves and retreat/defeat retry',flush=True)
# Earned access persists on demotion; the final temple still requires rank 6 earned.
wr(current()+v[2],1,1)
for slot in range(6):
 warp(slot);call('GleTitansInfo');assert rd('gSpecialVar_Result',2)==1
 assert call('GymLeaderPrepareTitanBattle')==1
 assert call('GetMonData2',enemy,v[11])==15
assert call('TrySavingData',0)==1
assert call('LoadGameSave',0)==1
for slot in [5,4,2,0,3,1]:
 warp(slot);call('GleTitansInfo');assert call('GymLeaderPrepareTitanBattle')==1
 wr('gBattleOutcome',7,1);call('GleTitansFinish');assert bit(79+slot)
 assert call('GymLeaderPrepareTitanBattle')==0
 warp(slot);assert not guardian()
print('PASS demotion/current cap, flash persistence, all-six nonsequential capture records and no respawn',flush=True)
# Real capture with a test-supplied Master Ball, no altered enemy HP/outcome.
state('titans-discovered',1);warp(0)
approach();until(lambda:menu_count()==3);choose(0);until(yesno);choose(1);until(yesno);choose(1);settle()
assert call('AddBagItem',v[115],1)
approach();until(yesno);choose(1);until(yesno);choose(0)
for _ in range(1200):
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
 press(1,12)
else:raise AssertionError('Regirock action menu failed')
assert call('GetMonData2',enemy,v[10])==species[0]
snap('titans-regirock-battle')
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
 if bit(79):break
 if yesno():choose(1)
 else:press(1,20)
else:raise AssertionError('actual Regirock capture did not record')
settle();assert rd('gPartiesCount',1)==4
assert call('GetMonData2',symbols['gParties']+3*v[5],v[10])==species[0]
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
warp(0);assert not guardian() and bit(79)
assert not any(bit(80+i) for i in range(5))
print('PASS actual Regirock battle/Master Ball capture, party delivery, native flash/no-respawn and independent remaining titans',flush=True)

# Awakening uses the existing per-mon ability field, not a global stat edit.
state('titans-discovered',1)
assert call('ScriptGiveMon',species[5],35,0)==0
mon=symbols['gParties']+3*v[5]
normal=call('GetAbilityBySpecies',species[5],0)
assert call('GetMonAbility',mon)==normal and normal!=v[-15]
call('GleRegigigasResearch');assert rd('gSpecialVar_Result',2)==0
assert call('GetMonAbility',mon)==normal
warp(5);call('GleTitansInfo');assert call('GymLeaderPrepareTitanBattle')==1
wr('gBattleOutcome',7,1);call('GleTitansFinish');assert bit(84)
state('titans-before-awakening')
# Real GLA counter menu grants the party upgrade.
call('GleWarpLeague');frames(240)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(100):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('research desk blocked',pos(),(x,y)))
walkto(6,9);walkto(5,9);walkto(5,8);press(32);press(1)
until(lambda:menu_count()==16);choose(4);until(lambda:menu_count()==16)
mon=symbols['gParties']+3*v[5]
assert call('GetMonAbility',mon)==v[-15] and rd(current()+v[137]+4,1)==2
snap('titans-regigigas-awakening')
press(2);settle()
assert call('GetAbilityBySpecies',species[5],0)==normal
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
mon=symbols['gParties']+3*v[5]
assert call('GetMonAbility',mon)==v[-15]
call('GleRegigigasResearch');assert rd('gSpecialVar_Result',2)==2
# Independent PC-held target, with party target removed, exercises box scanning.
state('titans-before-awakening',1)
mon=symbols['gParties']+3*v[5];storage=rd('gPokemonStoragePtr')+v[34]
for i in range(v[33]):wr(storage+i,rd(mon+i,1),1)
call('ZeroMonData',mon);wr('gPartiesCount',3,1)
call('GleRegigigasResearch');assert rd('gSpecialVar_Result',2)==1
assert call('GetBoxMonAbility',storage)==v[-15]
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
storage=rd('gPokemonStoragePtr')+v[34]
assert call('GetBoxMonAbility',storage)==v[-15]
assert call('GetAbilityBySpecies',species[5],0)==normal
print('PASS native GLA party awakening, pre-capture refusal, permanent Clear Body party/PC flash persistence, one-time upgrade and unchanged ordinary species ability',flush=True)
