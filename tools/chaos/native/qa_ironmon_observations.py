"""Observation privacy, capacity, overlay and real flash persistence fixtures.

Direct observation calls test event ingestion; they are not proof of every
battle announcement/ability event or unlimited run history.
"""
from qa_ironmon_core import *
O,STORAGESIZE,JOURNALSIZE,FACTSIZE,FACTS,INBATTLE,BS,BSP,BMOVES,BABILITY,ILL,ILLSIZE,ILLSTATE,ILLMON,ILLON,SOUNDPROOF,ILLUSION=struct.unpack('<17I',(ROOT/'ironmon-observations-layout.bin').read_bytes())
assert O==34144 and JOURNALSIZE==1548 and FACTSIZE==6 and STORAGESIZE<=35712
def blob(ptr,size):return bytes(rd(ptr+i,1) for i in range(size))
def journal():return rd('gPokemonStoragePtr')+O
def facts():
    j=journal()
    return [struct.unpack('<3H',blob(j+FACTS+i*FACTSIZE,FACTSIZE)) for i in range(rd(j+8,2))]
def enter(known_attack=False):
    start();call('IronmonGiveStarter',BULBA)
    call('CreateWildMon',PIKA,10)
    if known_attack:
        for slot in range(4):call('SetMonMoveSlot',p+6*MONSIZE,TACKLE,slot)
    call('BattleSetup_StartWildBattle')
    for _ in range(400):
        frames(3,1);frames(5)
        if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
    else:raise AssertionError('action menu not reached')
    frames(60)
    assert call('ChaosObservationsRead')
    return rd(symbols['gBattleMons']+BS+BSP,2)
species=enter();j=journal()
assert (species,0,1) in facts()
assert not any((fact&0xf000)==0x1000 for _,fact,_ in facts()),facts()
# Entry abilities can already have produced a visible popup. No move has yet
# been announced; the generated moveset must remain absent from the journal.
old=blob(j,JOURNALSIZE)
call('ChaosObserveMove',0,TACKLE);call('ChaosObserveAbility',0,SOUNDPROOF)
assert old==blob(j,JOURNALSIZE),'owned events entered opponent journal'
call('ChaosObserveMove',1,TACKLE);call('ChaosObserveMove',1,TACKLE)
call('ChaosObserveAbility',1,SOUNDPROOF)
assert (species,0x1000|TACKLE,2) in facts()
assert (species,0x2000|SOUNDPROOF,1) in facts()
# Additional histories contain visible events, not inferred opponent HP/stats.
call('ChaosObserveDamage',1,0,TACKLE,12);call('ChaosObserveDamage',1,0,TACKLE,8)
call('ChaosObserveDamage',0,1,TACKLE,500)
assert (species,0x3000|TACKLE,12) in facts()
assert not any(fact==0x3000|TACKLE and count==500 for _,fact,count in facts())
call('ChaosObserveStat',1,1,8);call('ChaosObserveStat',1,1,8)
assert (species,0x4000|(1<<4)|8,2) in facts()
call('ChaosObserveStat',1,8,8);call('ChaosObserveStat',1,1,13)
wr('gBattleOutcome',1,1);call('ChaosObserveBattleEnd')
assert (species,0x5001,1) in facts()
print('PASS opponent damage-to-player maxima, public stage history and outcomes; invalid/private events rejected.',flush=True)

# Hidden slots and actual ability may disagree: only the explicitly shown event is ingested.
wr(symbols['gBattleMons']+BS+BMOVES,DB,2)
wr(symbols['gBattleMons']+BS+BABILITY,ILLUSION,2)
assert not any(fact==0x1000|DB or fact==0x2000|ILLUSION for _,fact,_ in facts())
# Illusion maps observations to the visible identity, without initializing it or exposing real species.
illusion=rd('gBattleStruct')+ILL+ILLSIZE
before=blob(illusion,ILLSIZE)
wr(illusion+ILLSTATE,ILLON,1);wr(illusion+ILLMON,p)
call('ChaosObserveMove',1,TACKLE)
assert (BULBA,0x1000|TACKLE,1) in facts()
for i,b in enumerate(before):wr(illusion+i,b,1)
# The journal is read-only while its two pages are open, even with repeated paging.
storage=blob(rd('gPokemonStoragePtr'),STORAGESIZE)
assert call('ChaosTrackerTryOpen');frames(60)
for _ in range(3):frames(12,256);frames(20)
snap('ironmon-observed-moves')
frames(12,256);frames(20);snap('ironmon-observed-abilities')
frames(12,1);frames(20)
for _ in range(3):frames(12,256);frames(20)
snap('ironmon-observed-damage')
frames(12,256);frames(20);snap('ironmon-observed-stages')
frames(12,256);frames(20);snap('ironmon-observed-outcomes')
frames(12,256);frames(20);snap('ironmon-move-coverage')
frames(3,2);frames(1)
assert storage==blob(rd('gPokemonStoragePtr'),STORAGESIZE)
assert not call('ChaosTrackerIsOpen')
# Flash save/reload, with no emulator state used to preserve the journal.
call('SavePlayerParty');assert call('TrySavingData',0)==1
saved=blob(j,JOURNALSIZE)
for i in range(JOURNALSIZE):wr(j+i,0,1)
assert not call('ChaosObservationsRead')
assert call('LoadGameSave',0)==1
j=journal();assert saved==blob(j,JOURNALSIZE)
assert call('ChaosObservationsRead')
assert call('ChaosHasObservedSpecies',species) and call('ChaosHasObservedSpecies',BULBA)
print('PASS visible identities/events only, no hidden move/ability reads; read-only pages and flash persistence.',flush=True)
# Exercise the actual used-move announcement through ordinary battle input.
# Damage/speed/ability are controlled for survival: this is an ingestion test,
# not a fair starter or battle-balancing playthrough.
species=enter(True)
_,BHP,_,BTYPES,BSTATUS,_,_,NORMAL,NONE,_,_,BSPEED,_=struct.unpack('<13I',(ROOT/'ironmon-played-pair-layout.bin').read_bytes())
b=symbols['gBattleMons'];enemy=b+BS
wr(b+BHP,500,2);wr(b+BSPEED,1,2);wr(enemy+BSPEED,1000,2)
wr(enemy+BABILITY,NONE,2)
for t in range(3):wr(b+BTYPES+t,NORMAL,1);wr(enemy+BTYPES+t,NORMAL,1)
for _ in range(200):
    frames(3,1);frames(10)
    if any(sp==species and fact==0x1000|TACKLE for sp,fact,_ in facts()):break
else:raise AssertionError('real opponent move announcement did not record')
print('PASS actual opponent used-move announcement records Tackle through battle inputs.',flush=True)
for _ in range(250):
    if any((fact&0xf000)==0x3000 for _,fact,_ in facts()):break
    frames(3,1);frames(12)
else:raise AssertionError('actual incoming Tackle damage was not journaled')
print('PASS actual opponent attack damage recorded without reading enemy maximum HP.',flush=True)

# No eviction on overflow; existing recorded use counters continue working.
species=enter();j=journal()
call('ChaosObserveMove',1,TACKLE)
first=blob(j+FACTS,FACTSIZE)
for i in range(1,N_SPECIES):
    wr(symbols['gBattleMons']+BS+BSP,i,2);call('ChaosObserveAbility',1,SOUNDPROOF)
    if rd(j+8,2)==256:break
assert rd(j+8,2)==256
wr(symbols['gBattleMons']+BS+BSP,N_SPECIES-1,2);call('ChaosObserveAbility',1,SOUNDPROOF)
assert rd(j+11,1)==1 and first==blob(j+FACTS,FACTSIZE)
assert call('ChaosHasObservedSpecies',N_SPECIES-1), 'full journal lost a newly observed identity'
assert not call('ChaosHasObservedSpecies',0) and not call('ChaosHasObservedSpecies',N_SPECIES)
wr(symbols['gBattleMons']+BS+BSP,species,2);call('ChaosObserveMove',1,TACKLE)
assert (species,0x1000|TACKLE,2) in facts()
print('PASS bounded 256-fact capacity explicitly marked; no eviction; existing counters preserved.',flush=True)
# Malformed payload must not become an unsafe move/ability array index.
wr(j+FACTS+2,0xffff,2);assert not call('ChaosObservationsRead')
wr(s+SEED,987654);assert not call('ChaosObservationsRead')
call('ResetPokemonStorageSystem');assert blob(j,JOURNALSIZE)==bytes(JOURNALSIZE)
assert not any(call('ChaosHasObservedSpecies',i) for i in range(1,N_SPECIES))
print('PASS malformed journal rejection, seed isolation and genuine storage/new-run reset.',flush=True)
