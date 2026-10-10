"""IronMON wild generation/DexNav loadout isolation; no UI secrecy claim."""
from qa_ironmon_core import *
ABILITYNUM,PERSONALITY,SLOTS=struct.unpack('<3I',(ROOT/'ironmon-wild-layout.bin').read_bytes())
wild=p+6*MONSIZE
for mode in (4,5):
    start(mode)
    for species in (BULBA,PIKA):
        for rng in (1,123456,0xffffffff):
            wr('gRngValue',rng);call('CreateWildMon',species,12)
            ordinary=bytes(rd(wild+i,1) for i in range(MONSIZE))
            item=data(wild,ITEM)
            assert item>0 and call('GetItemHoldEffect',item)>0
            slot=data(wild,ABILITYNUM)
            assert slot<SLOTS and call('GetSpeciesAbility',species,slot)>0
            assert all(data(wild,HPIV+i)==31 and data(wild,HPEV+i)==0 for i in range(6))
            assert all(call('IronmonMoveAllowed',data(wild,MOVE+i)) for i in range(4) if data(wild,MOVE+i))
            call('IronmonPrepareWildMon',wild)
            assert ordinary==bytes(rd(wild+i,1) for i in range(MONSIZE))
# The IronMON helper is an exact no-op on an ordinary Chaos Pokemon.
wr(s+DIFF,1,1);call('IronmonInitializeRun')
call('CreateWildMon',PIKA,12)
ordinary=bytes(rd(wild+i,1) for i in range(MONSIZE))
call('IronmonPrepareWildMon',wild)
assert ordinary==bytes(rd(wild+i,1) for i in range(MONSIZE))
print('PASS wild MGM/legal randomized item+ability, idempotence and ordinary Chaos isolation. HUD needs rendered checks.',flush=True)
