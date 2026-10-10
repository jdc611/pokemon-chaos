from qa_ironmon_core import *
import struct
source=(repo/'tools/chaos/native/v3-layout.c').read_text();keys=source.split('= {',1)[1].split('};',1)[0].strip().split(',\n');raw=(ROOT/'v3_layout.bin').read_bytes();L=dict(zip(keys,struct.unpack('<'+str(len(raw)//4)+'I',raw)))
source=(repo/'tools/chaos/native/v3-world-layout.c').read_text();keys=source.split('const unsigned layout[]={',1)[1].split('};',1)[0].strip().split(',\n');raw=(ROOT/'v3-world-layout.bin').read_bytes();W=dict(zip(keys,struct.unpack('<'+str(len(raw)//4)+'I',raw)))
start();call('IronmonGiveStarter',BULBA)
wr(s+DIFF,1,1);call('IronmonInitializeRun')
call('CreateWildMon',PIKA,10);call('BattleSetup_StartWildBattle')
for _ in range(500):
 frames(3,1);frames(25)
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
else:raise AssertionError('preview battle action not reached')
frames(60)

b=symbols['gBattleMons'];bs=L['sizeof(struct BattlePokemon)'];scratch=0x0203e000
def bf(field,value,who=1,size=2):wr(b+bs*who+L['offsetof(struct BattlePokemon,'+field+')'],value,size)
def ab(name,who=1):bf('ability',W.get('ABILITY_'+name,L.get('ABILITY_'+name)),who,L['sizeof(enum Ability)'])
def types(name):
 for i in range(3):wr(b+bs+L['offsetof(struct BattlePokemon,types)']+i,W.get('TYPE_'+name,L.get('TYPE_'+name)) if i<2 else W['TYPE_MYSTERY'],1)
def preview(move):
 wr(rd('gBattleResources')+W['offsetof(struct BattleResources,bufferA)']+4+W['offsetof(struct ChooseMoveStruct,moves)'],W['MOVE_'+move],2);wr('gMoveSelectionCursor',0,1);return call('CheckTypeEffectiveness',0,1)
bf('species',19,size=L['sizeof(enum Species)']);bf('hp',100);bf('maxHP',100);bf('item',0);types('NORMAL');ab('TRACE')
# Icons and arrows share Off / Seen / Revealed, with legacy save values.
call('VarSet',0x404E,1) # Seen
assert not call('ChaosBattleTypesKnown',1) and preview('TACKLE')==0
call('ChaosRevealBattleTypes',0,1,W['MOVE_TACKLE'],8192)
assert not call('ChaosBattleTypesKnown',1) # A hit is not a prior Dex sighting.
call('VarSet',0x404E,0) # Revealed; does not need a Dex sighting.
assert call('ChaosBattleTypesKnown',1) and preview('TACKLE')==4
call('VarSet',0x404E,3) # Off overrides knowledge.
assert not call('ChaosBattleTypesKnown',1) and preview('TACKLE')==0
call('VarSet',0x404E,2) # Saved legacy Caught normalizes to Seen.
assert not call('ChaosBattleTypesKnown',1) and preview('TACKLE')==0
call('VarSet',0x404E,0)
print('PASS shared Off/Seen/Revealed visibility and legacy mode; Revealed previews unknown species immediately',flush=True)
for ability,move,expected in [('EARTH_EATER','EARTHQUAKE',1),('WELL_BAKED_BODY','EMBER',1),('TERA_SHELL','TACKLE',3),('WONDER_GUARD','TACKLE',1)]:
 ab(ability);assert preview(move)==expected,(ability,preview(move));call('GetBattlerTypes',1,0,scratch);assert rd(scratch,1)==W['TYPE_NORMAL']
types('WATER');ab('PURIFYING_SALT');assert preview('SHADOW_BALL')==3;call('GetBattlerTypes',1,0,scratch);assert rd(scratch,1)==L['TYPE_WATER']
print('PASS actual effectiveness previews for Earth Eater, Well-Baked Body, Purifying Salt, full-HP Tera Shell and Wonder Guard without changing visible type',flush=True)
ab('TERA_SHELL');types('GHOST');assert preview('TACKLE')==1
ab('TRACE');ab('MINDS_EYE',0);assert preview('TACKLE')==4 and preview('MACH_PUNCH')==4
ab('MOLD_BREAKER',0);types('NORMAL');ab('EARTH_EATER');assert preview('EARTHQUAKE')==4
bf('item',W['ITEM_ABILITY_SHIELD']);assert preview('EARTHQUAKE')==1
print('PASS Tera Shell preserves type immunity; Mind\'s Eye hits Ghost; Mold Breaker bypass/Ability Shield protection are previewed',flush=True)
print('Battle preview probes complete',flush=True)
