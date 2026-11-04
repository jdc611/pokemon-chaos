"""Native world ecology, query/battle agreement, RNG neutrality and rank limits."""
import runpy,struct,time
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
data=(ROOT/'ecology-layout.bin').read_bytes();e=struct.unpack('<'+'I'*(len(data)//4),data)
def warp(m,x=8,y=8):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def types(sp):return {rd(symbols['gSpeciesInfo']+sp*e[1]+e[2]+j,1) for j in [0,1]}
def bst(sp):return sum(rd(symbols['gSpeciesInfo']+sp*e[1]+off,1) for off in [e[3],*e[29:34]])
allowed=[None,{e[13],e[14],e[15],e[16]},{e[17],e[18],e[19],e[20],e[21],e[22]},{e[12]},{e[23],e[12]},{e[24],e[19]},{e[25],e[15],e[21],e[20]},{e[25],e[17],e[18]}]
wr(rd('gSaveBlock3Ptr')+e[0],0,1)
started=time.monotonic();observed=set()
for rank in range(1,9):
 wr(current()+v[2],rank,1)
 for biome,m in enumerate(e[4:12]):
  area=1 if biome==3 else 0;assert call('GymLeaderEcologyBiome',m,area)==biome
  for slot in range(6):
   rng=rd('gRngValue');sp=call('GymLeaderEcologySpecies',m,area,slot,0)
   assert sp and not call('GymLeaderSpeciesReserved',sp) and bst(sp)<=320+45*rank
   if allowed[biome]:assert types(sp)&allowed[biome],(biome,sp,types(sp))
   assert rd('gRngValue')==rng and call('GymLeaderEcologySpecies',m,area,slot,0)==sp
   observed.add(sp)
 print('PASS ecology biome/strength/reservation/RNG fixtures at rank',rank,flush=True)
print('ECOLOGY SAMPLE',len(observed),'species in',round(time.monotonic()-started,2),'seconds',flush=True)
# Both the lookup and actual wild generator must use GLE ecology with randomizer off.
wr(current()+v[2],1,1);warp(e[5],10,10)
header=call('GetCurrentMapWildMonHeaderId');assert header!=65535
tod=call('GetTimeOfDayForEncounters',header,0)
info=rd(symbols['gWildMonHeaders']+header*v[107]+v[108]+tod*v[109]);assert info
pool={call('GetRandomizedWildSpecies',info,0,i) for i in range(12)}
for _ in range(16):
 assert call('TryGenerateWildMon',info,0,0)
 enemy=symbols['gParties']+6*v[5];sp=call('GetMonData2',enemy,v[10]);assert sp in pool
 assert call('GetMonData2',enemy,v[11])<=call('GymLeaderLevelCap')
for sp in pool:
 level=call('DexNavTryGenerateMonLevel',sp,0);assert 3<=level<=call('GymLeaderLevelCap'),level
wr(rd('gSaveBlock3Ptr')+e[34],255,1)
for sp in pool:assert call('DexNavTryGenerateMonLevel',sp,0)==call('GymLeaderLevelCap')
wr(rd('gSaveBlock3Ptr')+e[34],0,1)
snap('ecology-forest-field')
print('PASS actual forest wild adapter and DexNav search levels agree with shared slots while randomizer is disabled',flush=True)
# Water and rod groups are aquatic and stable, with intentional weighted repeats.
for area in [1,3]:
 picks=[call('GymLeaderEcologySpecies',e[7],area,i,0) for i in range(10 if area==3 else 5)]
 assert all(e[12] in types(sp) for sp in picks)
 if area==3:assert picks[0]==picks[1]
 else:assert picks[0]==picks[3] and picks[1]==picks[4]
# A save's seed changes deterministic habitats; restoring it restores the original set.
seed=rd(current()+4);before=[call('GymLeaderEcologySpecies',e[5],0,i,0) for i in range(6)]
wr(current()+4,seed^0x13579bdf);after=[call('GymLeaderEcologySpecies',e[5],0,i,0) for i in range(6)];wr(current()+4,seed)
assert before!=after and before==[call('GymLeaderEcologySpecies',e[5],0,i,0) for i in range(6)]
print('PASS aquatic Surf/rod groups, deterministic seeded variety and cache key isolation',flush=True)
# Native DexNav renders the shared ecology rather than an inherited table.
call('CreateTask',symbols['Task_OpenDexNavFromStartMenu']|1,0);frames(360)
assert rd('sDexNavUiDataPtr')
snap('ecology-native-dexnav');press(2,180);press(2,80)
# Disabling GLE leaves the native, non-randomized species adapter intact.
magic=rd(current()+e[28]);wr(current()+e[28],0)
assert not call('IsGymLeaderRun') and call('GymLeaderEcologySpecies',e[5],0,0,0)==0
warp(e[5],10,10);header=call('GetCurrentMapWildMonHeaderId');tod=call('GetTimeOfDayForEncounters',header,0)
info=rd(symbols['gWildMonHeaders']+header*v[107]+v[108]+tod*v[109]);table=rd(info+e[27])
# WildPokemon is two one-byte levels followed by a two-byte species in this ARM ABI.
native={rd(table+i*4+2,2) for i in range(12)}
for _ in range(6):
 assert call('TryGenerateWildMon',info,0,0)
 assert call('GetMonData2',symbols['gParties']+6*v[5],v[10]) in native
wr(current()+e[28],magic)
print('PASS native DexNav screen and non-GLE species adapter preservation',flush=True)
print('PASS NATIVE ECOLOGY CHECKS COMPLETE',flush=True)
