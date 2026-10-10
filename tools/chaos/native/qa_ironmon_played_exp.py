"""Real attack/faint/EXP flow with controlled difficulty, not a playthrough."""
from qa_ironmon_core import *
BS,BHP,BABILITY,BTYPES,BSTATUS,EXP,SWIFT,NORMAL,NONE,_,_,BSPEED,BSPATK=struct.unpack(
    '<13I',(ROOT/'ironmon-played-pair-layout.bin').read_bytes())
def key():frames(3,1);frames(25)
for trainer in (False,True):
    start();call('IronmonGiveStarter',BULBA)
    setdata(p,EXP,30000);call('CalculateMonStats',p)
    for m in range(4):call('SetMonMoveSlot',p,SWIFT,m)
    initial_exp=data(p,EXP)
    call('InitTrainerBattleParameter')
    if trainer:
        # Brock's battle is entered through the ordinary trainer transition.
        # Gym gating is separately tested; this fixture is about EXP commands.
        _,A,_,DA,_,_,_,_=struct.unpack('<8I',(ROOT/'ironmon-singles-layout.bin').read_bytes())
        wr(symbols['gTrainerBattleParameter']+A,BROCK,2)
        wr(symbols['gTrainerBattleParameter']+DA,symbols['Text_ChaosJessieDefeat'])
        wr('gNoOfApproachingTrainers',1,1)
        call('BattleSetup_StartTrainerBattle')
    else:
        call('CreateWildMon',PIKA,12)
        call('BattleSetup_StartWildBattle')
    entered=False
    for step in range(1700):
        if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:
            entered=True
            b=symbols['gBattleMons'];enemy=b+BS
            if rd(enemy+BHP,2)>1:wr(enemy+BHP,1,2)
            wr(enemy+BABILITY,NONE,2)
            for t in range(3):wr(enemy+BTYPES+t,NORMAL,1)
            wr(b+BSPEED,1000,2);wr(b+BSPATK,1000,2);wr(b+BSTATUS,0)
        key()
        if entered and rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1:break
    else:
        snap('ironmon-exp-stalled')
        raise AssertionError(('battle failed to return',trainer,step,hex(rd(symbols['gMain']+4))))
    assert rd('gBattleOutcome',1)==1
    final_exp=data(p,EXP)
    assert final_exp>initial_exp if trainer else final_exp==initial_exp,(trainer,initial_exp,final_exp)
    assert not rd(s+IM+ENDED,1) and rd('gPartiesCount',1)==1
    print('PASS played', 'trainer EXP awarded' if trainer else 'wild EXP zero', initial_exp, final_exp,flush=True)
