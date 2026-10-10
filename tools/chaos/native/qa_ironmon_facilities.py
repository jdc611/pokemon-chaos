"""Facility callback/party-restoration fixtures, not complete facility gameplay."""
from qa_ironmon_core import *
special_offset,ereader,trainer_flags,state_size,floors_offset,floor_size,challenge_offset,mons_offset,tower_id=struct.unpack('<9I',(ROOT/'ironmon-facilities-layout.bin').read_bytes())
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

# Generate from actual Tower data, changing only challenge classification for
# fixtures. Starting the transition exercises the real inlined party builder.
source=symbols['sTrainerTowerFloor_Single_4']
for mode in (4,5):
 for challenge,count in ((0,2),(1,2),(2,1)):
    start(mode);call('IronmonGiveStarter',BULBA)
    wr(rd('gSaveBlock1Ptr')+tower_id,0)
    state_ptr=call('AllocZeroed_',state_size,0)
    wr('sTrainerTowerState',state_ptr)
    floor_source=rd(rd(symbols['gTrainerTowerFloors']+challenge*4))
    for i in range(floor_size):wr(state_ptr+floors_offset+i,rd(floor_source+i,1),1)
    wr(state_ptr+floors_offset+challenge_offset,challenge,1)
    call('VarSet',0x4001,0) # VAR_TEMP_1
    call('DoTrainerTowerBattle')
    assert not rd('gBattleTypeFlags')&1
    enemy=p+6*MONSIZE
    assert sum(data(enemy+i*MONSIZE,SP)!=0 for i in range(6))==count
    for i in range(count):
        mon=enemy+i*MONSIZE
        assert data(mon,LV)==50 and data(mon,HPIV)==31 and data(mon,HPEV)==0
        assert data(mon,ITEM)!=0
    print('PASS real Tower party builder: Singles, fixed level 50, MGM and legal items',mode,challenge,flush=True)
# Same source identity stays byte-identical across changed global RNG/main level.
start();call('IronmonGiveStarter',BULBA)
entry=source+mons_offset;enemy=p+6*MONSIZE
call('IronmonGenerateFacilityMon',enemy,entry,123,50)
expected=bytes(rd(enemy+i,1) for i in range(MONSIZE))
wr('gRngValue',12345);setdata(p,LV,99);call('CalculateMonStats',p)
call('IronmonGenerateFacilityMon',enemy,entry,123,50)
assert expected==bytes(rd(enemy+i,1) for i in range(MONSIZE))
print('PASS facility seed independent of live RNG and player level',flush=True)
