"""Facility callback/party-restoration fixtures, not complete facility gameplay."""
from qa_ironmon_core import *
special_offset,ereader,trainer_flags=struct.unpack('<3I',(ROOT/'ironmon-facilities-layout.bin').read_bytes())
for mode in (4,5):
    start(mode);call('IronmonGiveStarter',BULBA)
    call('ChooseHalfPartyForBattle')
    assert rd('gSelectedOrderFromParty',1)==1 and rd(symbols['gSelectedOrderFromParty']+1,1)==0
    assert rd('gSpecialVar_Result',2)==1
    call('SavePlayerParty')
    setdata(p,HP,1);setdata(p,ITEM,0);setdata(p,PP,1)
    changed=bytes(rd(p+i,1) for i in range(6*MONSIZE))
    wr('gBattleTypeFlags',trainer_flags);wr('gBattleOutcome',1,1)
    wr(symbols['gBattleScripting']+special_offset,ereader,1)
    call('HandleSpecialTrainerBattleEnd');call('LoadPlayerParty')
    assert changed==bytes(rd(p+i,1) for i in range(6*MONSIZE))
    assert rd(s+IM+ENDED,1)==0
    print('PASS e-Reader callback preserves HP/PP/item and sole selection',mode,flush=True)
    for callback in ('HandleSpecialTrainerBattleEnd','CB2_EndTrainerTowerBattle'):
        start(mode);call('IronmonGiveStarter',BULBA);setdata(p,HP,0)
        call(callback)
        assert rd(s+IM+ENDED,1)==1
        assert rd(symbols['gMain']+4)==symbols['CB2_IronmonRunOver']|1
        print('PASS facility callback enters RUN OVER',mode,callback,flush=True)
