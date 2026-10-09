"""V3 compiled-ROM regression probes, using compiler-produced offsets/constants."""
from qa import *
import struct,json,re
source=(repo/'tools/chaos/native/v3-layout.c').read_text()
keys=source.split('= {',1)[1].split('};',1)[0].strip().split(',\n')
raw=(ROOT/'v3_layout.bin').read_bytes();L=dict(zip(keys,struct.unpack('<'+str(len(raw)//4)+'I',raw)))
get=lambda key:L[key]
assert lib.boot(str(repo/'pokefirered.gba').encode())
p=symbols['gParties'];b=symbols['gBattleMons'];scratch=0x0203e000
ps=get('sizeof(struct Pokemon)');bs=get('sizeof(struct BattlePokemon)')
lib.call6.restype=C.c_uint;lib.call6.argtypes=[C.c_uint]*7

def data(name,value,slot=0):wr(scratch,value);call('SetMonData',p+ps*slot,get(name),scratch)
def mon(name,slot=0):return call('GetMonData3',p+ps*slot,get(name),0)
def bf(name,value,battler=0,size=2):wr(b+bs*battler+get('offsetof(struct BattlePokemon,'+name+')'),value,size)
def ability(name,battler=0):bf('ability',get('ABILITY_'+name),battler,get('sizeof(enum Ability)'))
def key(k=1,n=35):frames(2,k);frames(n)
def bytes_at(a,n):return bytes(rd(a+i,1) for i in range(n))
def reset():state('pass-world',1)
def battle():
 state('pass-brock-retry',1)
 bf('species',25,size=get('sizeof(enum Species)'));bf('hp',100);bf('maxHP',100);bf('level',20,size=1)
 a=call('GetBattlerPartyState',0)
 for i in range(get('sizeof(struct PartyState)')):wr(a+i,0,1)
 return a
maskblob=(ROOT/'v3_masks.bin').read_bytes();masksize=get('sizeof(struct PartyState)')
def mask(a,index):
 for i,v in enumerate(maskblob[index*masksize:(index+1)*masksize]):wr(a+i,rd(a+i,1)|v,1)
def stat(name,kind):return call('ChaosAbilityStat',0,get('ABILITY_'+name),get('STAT_'+kind),100)

# Exact thresholds/multipliers, with a Pikachu rather than each original species.
for name,high,low in [
 ('ZEN_MODE',[100]*5,[100,100,150,150,100]),
 ('SCHOOLING',[130,130,130,130,100],[100]*5),
 ('SHIELDS_DOWN',[100,120,100,120,100],[130,100,130,100,130])]:
 battle();ability(name);bf('hp',100)
 assert [stat(name,s) for s in ['ATK','DEF','SPATK','SPDEF','SPEED']]==high,name
 bf('hp',50 if name!='SCHOOLING' else 25)
 assert [stat(name,s) for s in ['ATK','DEF','SPATK','SPDEF','SPEED']]==low,name
 print('PASS',name,'non-original species exact stat multipliers and threshold',flush=True)
battle();ability('SCHOOLING');bf('level',19,size=1);assert stat('SCHOOLING','ATK')==100
for name,index,expected in [('STANCE_CHANGE',3,[130,80,130,80,100]),('POWER_CONSTRUCT',2,[120,150,120,150,100]),('ZERO_TO_HERO',0,[130,120,130,120,100]),('TERA_SHIFT',5,[110,130,110,130,100]),('HUNGER_SWITCH',4,[120,100,120,100,120])]:
 a=battle();ability(name);mask(a,index)
 assert [stat(name,s) for s in ['ATK','DEF','SPATK','SPDEF','SPEED']]==expected,name
 assert rd(b+get('offsetof(struct BattlePokemon,maxHP)'),2)==100
 ability('TRACE');assert stat('TRACE','ATK')==100,(name,'replacement must end boost')
 print('PASS',name,'exact multipliers; no max-HP mutation; replacement ends ongoing boost',flush=True)
battle();ability('HUNGER_SWITCH');assert stat('HUNGER_SWITCH','DEF')==120 and stat('HUNGER_SWITCH','ATK')==100
# Battle Bond is once-only and boosts both offenses, independent of their ranking.
a=battle();ability('BATTLE_BOND');call('ClearStatChangeValues');stages=b+get('offsetof(struct BattlePokemon,statStages)')
for i in range(8):wr(stages+i,6,1)
assert call('ChaosAbilityBond',0);assert not call('ChaosAbilityBond',0)
queue=symbols['gSpecialStatuses']+get('offsetof(struct SpecialStatus,statStageQueue)')
assert [(rd(queue+i*get('sizeof(struct StatStages)'),1)&127,rd(queue+i*get('sizeof(struct StatStages)')+get('offsetof(struct StatStages,stage)'),1)) for i in range(3)]==[(get('STAT_ATK'),1),(get('STAT_SPATK'),1),(get('STAT_SPEED'),1)]
print('PASS Battle Bond both offenses and Speed, once per battle',flush=True)
# Pure temporary typing restores from the battle's unchanged base types.
for name,item,expected in [('MULTITYPE','ITEM_FLAME_PLATE','TYPE_FIRE'),('RKS_SYSTEM','ITEM_WATER_MEMORY','TYPE_WATER')]:
 battle();ability(name);bf('item',get(item));call('GetBattlerTypes',0,0,scratch);assert rd(scratch,1)==get(expected) and rd(scratch+1,1)==get(expected)
 bf('item',0);call('GetBattlerTypes',0,0,scratch);assert rd(scratch,1)!=get(expected)
 print('PASS',name,'non-original species held-item pure type/restoration',flush=True)
for weather,expected in [('B_WEATHER_SUN','TYPE_FIRE'),('B_WEATHER_RAIN','TYPE_WATER'),('B_WEATHER_HAIL','TYPE_ICE')]:
 battle();ability('FORECAST');wr('gBattleWeather',get(weather),2);call('GetBattlerTypes',0,0,scratch);assert rd(scratch,1)==get(expected)
 wr('gBattleWeather',0,2);call('GetBattlerTypes',0,0,scratch);assert rd(scratch,1)!=get(expected)
print('PASS Forecast sunlight/rain/hail temporary pure typing/restoration',flush=True)
# Singles support: conscious, not self, no duplicate stacking, Doubles separation.
reset();call('ZeroPlayerPartyMons')
for sp in [25,123,93]:call('ScriptGiveMon',sp,20,0)
call('CreateWildMon',19,20);call('BattleSetup_StartWildBattle');frames(600)
for slot,name in [(0,'TRACE'),(1,'BATTERY'),(2,'POWER_SPOT')]:data('MON_DATA_CHAOS_STARTER_ABILITY',get('ABILITY_'+name),slot)
assert call('ChaosHasBenchAbility',0,get('ABILITY_BATTERY'));assert call('ChaosHasBenchAbility',0,get('ABILITY_POWER_SPOT'));assert not call('ChaosHasBenchAbility',0,get('ABILITY_TRACE'))
last=rd('gLastUsedAbility',2);call('ChaosHasBenchAbility',0,get('ABILITY_BATTERY'));assert rd('gLastUsedAbility',2)==last
# Surf is special and spread. 100*1.3*1.3=169, once each.
assert call('ChaosAbilityDamage',0,1,get('MOVE_SURF'),100)==169
# Fainted donor no longer supports.
data('MON_DATA_HP',0,1);assert not call('ChaosHasBenchAbility',0,get('ABILITY_BATTERY'));assert call('ChaosAbilityDamage',0,1,get('MOVE_SURF'),100)==130
wr('gBattleTypeFlags',rd('gBattleTypeFlags')|get('BATTLE_TYPE_DOUBLE'));assert not call('ChaosHasBenchAbility',0,get('ABILITY_POWER_SPOT'))
print('PASS Singles Battery/Power Spot 1.69, no self/fainted support, native Doubles separation and activation-global preservation',flush=True)
# Full-party gift routing skips the Graveyard selected in the PC.
for box in [0,4,13]:
 reset();call('ZeroPlayerPartyMons');call('VarSet',0x408c,1)
 for sp in [59,3,9,6,25,123]:call('ScriptGiveMon',sp,20,0)
 storage=rd('gPokemonStoragePtr');wr(storage,box,1)
 result=call('ScriptGiveMon',84,5,0);assert result==1,result
 assert call('GetBoxMonDataAt',13,0,get('MON_DATA_SPECIES'))==0
 assert any(call('GetBoxMonDataAt',n,0,get('MON_DATA_SPECIES'))==84 for n in range(13))
 assert [mon('MON_DATA_SPECIES',i) for i in range(6)]==[59,3,9,6,25,123]
print('PASS full-party randomized-gift destination with ordinary/Graveyard selected; party preserved',flush=True)
# Transactional Summary editor through actual keypad inputs.
reset();call('VarSet',0x40fc,1);call('ZeroPlayerPartyMons');call('ScriptGiveMon',25,20,0)
before=bytes_at(p,ps);nature=mon('MON_DATA_HIDDEN_NATURE');oldability=call('GetMonAbility',p);gender=call('GetMonGender',p)
lib.call6(symbols['ShowPokemonSummaryScreen'],0,p,0,0,symbols['CB2_ReturnToField']|1,0);frames(180);key(4);snap('v3-summary-editor')
# One press, held 90 frames: exactly one nature change on the local draft.
frames(90,16);frames(35);assert bytes_at(p,ps)==before
key(2);assert bytes_at(p,ps)==before
key(4);key(16);key(128);key(16);key(128);key(16);snap('v3-summary-preview');assert bytes_at(p,ps)==before
key(128);key();snap('v3-summary-accepted');assert mon('MON_DATA_HIDDEN_NATURE')==(nature+1)%25
assert call('GetMonGender',p)!=gender
accepted=bytes_at(p,ps)
key(2,300)
# The fields are stored by a native save, then loaded back by the native loader.
state('v3-summary-exited')
call('SaveGame');frames(70)
for _ in range(90):key()
snap('v3-summary-save')
assert rd('gSaveAttemptStatus',1)==1,('save',rd('gSaveAttemptStatus',1))
assert call('LoadGameSave',0,0)==1
assert bytes_at(p,ps)==accepted
print('PASS Summary local previews, held-key filtering, Discard restores, Accept commits and native save/reload',flush=True)
print('V3 probes complete',flush=True)
