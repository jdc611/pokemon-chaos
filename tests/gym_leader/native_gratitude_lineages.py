"""Native gratitude quest and postgame one-time research nursery placements."""
import runpy,struct,json
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
data=(ROOT/'lineage-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(data)//4),data)
site=json.load(open('docs/gym-leader-edition/field-research-sites.json'))[35]
def result():return rd('gSpecialVar_Result',2)
def bit(n):return bool(rd(current()+v[114]+n//8,1)&(1<<(n%8)))
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def settle():
 for _ in range(100):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('research script did not return')
def info():wr('gSpecialVar_LastTalked',235,2);call('GleFieldInfo');return result()
def floor(x,y):return not call('MapGridGetCollisionAt',x+7,y+7) and not call('MetatileBehavior_IsSurfableWaterOrUnderwater',call('MapGridGetMetatileBehaviorAt',x+7,y+7))
wr('sWildEncountersDisabled',1,1)
for trainer in range(768):call('SetTrainerFlag',trainer)
warp(site['map_id'],site['x'],site['y']+1)
x,y=site['x'],site['y']
if not(floor(x,y) and floor(x,y+1)):
 candidates=[(abs(dx)+abs(dy),x+dx,y+dy) for dx in range(-6,7) for dy in range(-6,7) if x+dx>0 and y+dy>0 and floor(x+dx,y+dy) and floor(x+dx,y+dy+1)]
 raise AssertionError(('gratitude floor placement',sorted(candidates)[:5]))
original=json.load(open('data/maps/'+site['map_name']+'/map.json'))
assert all((x,y)!=(p.get('x'),p.get('y')) and (x,y+1)!=(p.get('x'),p.get('y')) for field in ['object_events','warp_events','coord_events','bg_events'] for p in original.get(field,[]))
assert bit(241) and not bit(240) and info()==0
wr(current()+c[12],6,1);assert info()==4 and not call('GymLeaderPrepareFieldBattle')
# Native habitat construction, upgrade and type selection satisfy the request.
wr(current()+v[2],6,1);wr(current()+v[54],12,2)
plot=next(i for i in range(c[13]) if not rd(current()+v[52]+i*v[53],1) and rd(current()+v[52]+i*v[53]+2,1)>=2)
scratch=symbols['gStringVar1'];wr(scratch,100000)
assert call('GleConstruct',current(),plot,9,scratch)==0
assert call('GleConstruct',current(),plot,9,scratch)==0
wr(current()+v[52]+plot*v[53]+3,4,1);assert info()==3
wr(current()+v[54],11,2);assert info()==4;wr(current()+v[54],12,2)
wr(current()+v[2],1,1);assert info()==3
press(64);press(1);until(lambda:menu_count()==3);choose(1);settle();assert not bit(242)
press(1);until(lambda:menu_count()==3);choose(0);until(yesno);choose(1);until(yesno);choose(1);settle();assert bit(242) and not bit(240)
assert call('AddBagItem',v[115],1)
press(1);until(yesno);choose(1);until(yesno);choose(0)
until(lambda:rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1,1200)
assert call('GetMonData2',symbols['gParties']+6*v[5],v[10])==c[5]
assert call('GetMonData2',symbols['gParties']+6*v[5],v[11])==15
snap('gratitude-shaymin-battle');press(16);press(1)
for _ in range(500):
 if task('Task_BagMenu_HandleInput')<16:break
 frames(1)
else:raise AssertionError('gratitude Bag unavailable')
frames(100)
for _ in range(6):
 if rd(symbols['gBagPosition']+v[119],1)==v[120]:break
 press(16,50)
ptr=rd(symbols['gBagPockets']+v[118]*v[120]);target=next(i for i in range(v[124]) if rd(ptr+i*v[123],2)==v[115])
cursor=rd(symbols['gBagPosition']+v[121]+v[120]*2,2)+rd(symbols['gBagPosition']+v[122]+v[120]*2,2)
for _ in range(abs(target-cursor)):press(128 if target>cursor else 64)
press(1,25);press(1,25)
for _ in range(1000):
 if bit(240):break
 if yesno():choose(1)
 else:press(1,20)
else:raise AssertionError('gratitude capture not recorded')
settle();assert call('GetMonData2',symbols['gParties']+3*v[5],v[10])==c[5] and not call('GymLeaderPrepareFieldBattle')
print('PASS gratitude site floor/approach/events, early discovery/rank, native Grass habitat upgrade/population gates, demotion, wrong/correct seal, actual Shaymin capture and exhaustion',flush=True)
# Nursery refuses early visits; the same career ending unlocks all Gym types.
warp(v[24],4,9);wr('gSpecialVar_Result',0,2);call('GleLineageInfo');assert result()==2
call('GleLineageClaim');assert result()==3 and not bit(243)
wr(current()+v[114]+27,rd(current()+v[114]+27,1)|128,1)
press(64);press(1);until(lambda:menu_count()==18);choose(16);until(lambda:menu_count()==5)
choose(0);until(yesno);snap('rare-lineage-placement');choose(1);until(lambda:menu_count()==5);assert not bit(243)
choose(0);until(yesno);choose(0);until(lambda:menu_count()==5);assert bit(243)
press(2);until(lambda:menu_count()==18);press(2);settle()
funds=call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
for slot in range(4):
 wr('gSpecialVar_Result',slot,2);call('GleLineageInfo');assert result()==(1 if slot==0 else 0)
 call('GleLineageClaim');assert result()==(5 if slot==0 else 0) and bit(243+slot)
 call('GleLineageClaim');assert result()==5
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==funds
owned=[]
for i in range(6):owned.append(call('GetMonData2',symbols['gParties']+i*v[5],v[10]))
storage=rd('gPokemonStoragePtr')+v[34]
for i in range(v[76]*v[77]):owned.append(call('GetBoxMonData2',storage+i*v[33],v[10]))
assert all(s in owned for s in [c[0],c[2],c[3],c[4]])
# Native level-evolution lookup proves Cosmoem is reachable from the placement.
mon=call('Alloc_',v[5],0);call('CreateRandomMonWithIVs',mon,c[0],43,31)
target=lib.call6(symbols['GetEvolutionTargetSpecies'],mon,c[10],0,0,symbols['gStringVar1'],c[11]);assert target==c[1]
call('Free',mon)
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and all(bit(243+i) for i in range(4)) and bit(240)
print('PASS nursery ending gate, actual native menu decline/accept, four independent one-time party/PC placements, no money charge, native Cosmog-to-Cosmoem evolution and flash',flush=True)
# Explicit full-storage fixture: failure must not spend the placement, and
# freeing one PC slot must deliver before the persistent bit is committed.
wr(current()+v[114]+243//8,rd(current()+v[114]+243//8,1)&~(1<<(243%8)),1)
mondata=bytes(rd(symbols['gParties']+i,1) for i in range(v[5]))
for i in range(v[76]*v[77]):
 for j,b in enumerate(mondata[:v[33]]):wr(storage+i*v[33]+j,b,1)
assert call('IsPlayerPartyAndPokemonStorageFull')
wr('gSpecialVar_Result',0,2);call('GleLineageInfo');assert result()==0
before=bytes(rd(current()+i,1) for i in range(v[1]));call('GleLineageClaim');assert result()==2
assert bytes(rd(current()+i,1) for i in range(v[1]))==before and not bit(243)
for i in range(v[33]):wr(storage+i,0,1)
call('GleLineageClaim');assert result()==0 and bit(243) and call('GetBoxMonData2',storage,v[10])==c[0]
call('GleLineageClaim');assert result()==5
print('PASS full storage retains placement; native freed-PC delivery commits it exactly once',flush=True)
