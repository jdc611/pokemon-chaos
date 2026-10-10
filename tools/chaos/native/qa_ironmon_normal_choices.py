"""Real new-game callback and each lab-ball script, no injected gift outcome."""
from qa_ironmon_core import *
import json
objects=json.loads((repo/'data/maps/PalletTown_ProfessorOaksLab_Frlg/map.json').read_text())['object_events']
balls=[o for o in objects if o.get('script','').endswith(('BulbasaurBall','SquirtleBall','CharmanderBall'))]
assert len(balls)==3
for choice in range(3):
    state('ironmon-core-base',1)
    wr('gRunSetupDifficulty',4,1);wr('gRunSetupWorldSeed',24680)
    wr('gRunSetupStartRegion',1,1);wr('gRunSetupNuzlocke',0,1)
    wr('gDebugForceKantoNewGame',1,1)
    call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(600)
    s=rd('gSaveBlock3Ptr')
    for _ in range(25):frames(3,1);frames(40)
    assert call('IsIronmonRun') and not call('IsIronmonHardcore')
    assert rd('gPartiesCount',1)==0
    offered=[]
    for i in range(3):
        wr('gSpecialVar_0x8004',i,2);call('ResolveChaosOakStarters')
        offered.append(rd('gSpecialVar_0x8005',2))
    assert all(offered)
    for i in range(3):
        wr('gRngValue',i+999)
        wr('gSpecialVar_0x8004',i,2);call('ResolveChaosOakStarters')
        assert offered[i]==rd('gSpecialVar_0x8005',2)
    event=('BulbasaurBall','SquirtleBall','CharmanderBall')[choice]
    ball=next(o for o in balls if o['script'].endswith(event))
    # Set the event engine's ordinary last-talked local ID, then execute the
    # actual ball interaction script and accept using controller input.
    call('VarSet',0x800f,5+choice)
    call('ScriptContext_SetupScript',symbols[ball['script']]);call('ScriptContext_Enable')
    for _ in range(180):
        frames(3,1);frames(25)
        if rd('gPartiesCount',1):break
    else:raise AssertionError(('starter not awarded',choice))
    for _ in range(180):
        frames(3,2);frames(25)
        if not call('ArePlayerFieldControlsLocked'):break
    else:raise AssertionError(('nickname/rival pick did not return',choice))
    assert rd('gPartiesCount',1)==1 and data(p,SP)==offered[choice]
    assert data(p,ITEM)>0 and data(p,LV)==5
    assert any(call('IronmonMoveIsStarterAttack',data(p,MOVE+i)) for i in range(4))
    before=bytes(rd(p+i,1) for i in range(MONSIZE))
    call('IronmonGiveStarter',offered[(choice+1)%3])
    assert before==bytes(rd(p+i,1) for i in range(MONSIZE))
    print('PASS Normal lab choice',choice,'offered',offered,'actual',data(p,SP),flush=True)
