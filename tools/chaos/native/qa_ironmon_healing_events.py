"""Execute free-healing scripts with ordinary confirmation input.

Controlled field fixtures, not a full progression playthrough.
"""
from qa_ironmon_core import *

events=('PalletTown_PlayersHouse_1F_EventScript_MomHeal',
        'PokemonTower_5F_EventScript_PurifiedZone',
        'OneIsland_KindleRoad_EmberSpa_EventScript_SpaHeal',
        'SSAnne_1F_Room6_EventScript_Woman',
        'SilphCo_9F_EventScript_HealWoman',
        'SevenIsland_SevaultCanyon_House_EventScript_ChanseyDanceMan')
for mode in (4,5):
    for event in events:
        start(mode);call('IronmonGiveStarter',BULBA)
        setdata(p,HP,1)
        initial=bytes(rd(p+i,1) for i in range(MONSIZE))
        call('ScriptContext_SetupScript',symbols[event]);call('ScriptContext_Enable')
        frames(20)
        assert call('ArePlayerFieldControlsLocked')
        for _ in range(150):
            frames(3,1);frames(25)
            if not call('ArePlayerFieldControlsLocked'):break
        else:raise AssertionError(('healing restriction failed to release',mode,event))
        assert initial==bytes(rd(p+i,1) for i in range(MONSIZE)),(mode,event)
        assert not rd(s+IM+ENDED,1)
        print('PASS free-healing event restriction and return control',mode,event,flush=True)

# A normal Chaos mother's healing service must still work.
start();call('IronmonGiveStarter',BULBA)
wr(s+DIFF,1,1);call('IronmonInitializeRun');setdata(p,HP,1)
call('ScriptContext_SetupScript',symbols[events[0]]);call('ScriptContext_Enable')
frames(20)
for _ in range(200):
    frames(3,1);frames(25)
    if not call('ArePlayerFieldControlsLocked'):break
else:raise AssertionError('regular Mom healing failed to release')
assert data(p,HP)==data(p,MAXHP)
print('PASS regular Chaos Mom healing preserved.',flush=True)
