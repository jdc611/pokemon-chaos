"""Genuine switch-in Oran Berry consumption and mode-specific refund policy."""
from qa_ironmon_core import *
for mode in (4,5,1):
    start(4 if mode==1 else mode);call('IronmonGiveStarter',BULBA)
    if mode==1:wr(s+DIFF,1,1);call('IronmonInitializeRun')
    setdata(p,ITEM,ORAN);setdata(p,HP,1)
    call('CreateWildMon',PIKA,3);call('BattleSetup_StartWildBattle')
    for _ in range(400):
        frames(3,1);frames(5)
        if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
    else:raise AssertionError(('action menu not reached',mode))
    frames(60)
    assert data(p,ITEM)==0 and data(p,HP)>1,(mode,data(p,ITEM),data(p,HP))
    hp=data(p,HP)
    call('TryRestoreHeldItems')
    assert data(p,ITEM)==(ORAN if mode==1 else 0),(mode,data(p,ITEM))
    assert data(p,HP)==hp
    print('PASS actual Oran consumption: '+('ordinary Chaos refund preserved' if mode==1 else 'IronMON item stays consumed')+' mode '+str(mode),flush=True)
