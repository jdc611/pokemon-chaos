"""Central ownership/service guards; not every NPC dialogue or menu traversal."""
from qa_ironmon_core import *
for mode in (4,5):
    start(mode);call('IronmonGiveStarter',BULBA)
    party=bytes(rd(p+i,1) for i in range(6*MONSIZE))
    storage=rd('gPokemonStoragePtr')
    boxes=bytes(rd(storage+i,1) for i in range(STORAGE))
    call('SetCoins',5000)
    for function in ('StoreSelectedPokemonInDaycare','PutMonInRoute5Daycare',
                     'GiveEggFromDaycare','CreateInGameTradePokemon'):
        call(function)
        assert party==bytes(rd(p+i,1) for i in range(6*MONSIZE)),function
        assert boxes==bytes(rd(storage+i,1) for i in range(STORAGE)),function
    assert call('TakePokemonFromDaycare')==0
    assert call('TakePokemonFromRoute5Daycare')==0
    call('ScriptContext_Stop')
    call('DoInGameTradeScene')
    task=call('FindTaskIdByFunc',symbols['Task_IronmonRejectTrade']|1)
    assert task<16 and not call('ScriptContext_IsEnabled')
    call('Task_IronmonRejectTrade',task)
    assert call('ScriptContext_IsEnabled')
    assert rd('gSpecialVar_Result',2)==0
    assert party==bytes(rd(p+i,1) for i in range(6*MONSIZE))
    # Reject stale/replayed arcade transactions even without their NPC guard.
    call('ChaosArcadePrizeBuy');assert rd('gSpecialVar_Result',2)==4
    wr('sItemCategory',0,1);wr('sItemChoice',0,2)
    call('ChaosArcadeItemBuy');assert rd('gSpecialVar_Result',2)==4
    assert call('GetCoins')==5000
    assert rd('gPartiesCount',1)==1
print('PASS both IronMON modes: central daycare/trade/prize/TM guards preserve party/storage/coins. NPC/menu coverage remains separate.',flush=True)
