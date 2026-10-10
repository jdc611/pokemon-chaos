"""Controlled Singles setup/paired callbacks; not a completed story battle."""
from qa_ironmon_core import *
size,A,B,DA,DB,koga,jessie,james=struct.unpack('<8I',(ROOT/'ironmon-singles-layout.bin').read_bytes())
params=symbols['gTrainerBattleParameter']
def half(offset):return rd(params+offset,1) | (rd(params+offset+1,1)<<8)
def put(offset,value):wr(params+offset,value&255,1);wr(params+offset+1,value>>8,1)
start();call('IronmonGiveStarter',BULBA)
original=bytes(rd(p+i,1) for i in range(6*MONSIZE))
call('InitTrainerBattleParameter');put(A,koga)
wr('gNoOfApproachingTrainers',1,1);call('BattleSetup_StartTrainerBattle')
assert rd('gBattleTypeFlags')==8
start();call('IronmonGiveStarter',BULBA)
call('InitTrainerBattleParameter');put(A,jessie);put(B,james)
wr('gNoOfApproachingTrainers',2,1);call('BattleSetup_StartTrainerBattle')
assert rd('gBattleTypeFlags')==8 and half(B)==0 and rd('sIronmonPairPhase',1)==1
assert bytes(rd(p+i,1) for i in range(6*MONSIZE))==original
wr('gBattleOutcome',1,1);call('CB2_EndTrainerBattle')
assert half(A)==james and rd('sIronmonPairPhase',1)==2 and rd('gBattleTypeFlags')==8
assert call('FlagGet',jessie+0x500) and not call('FlagGet',james+0x500)
assert bytes(rd(p+i,1) for i in range(6*MONSIZE))==original
call('CB2_EndTrainerBattle')
assert half(A)==jessie and half(B)==james and rd('sIronmonPairPhase',1)==0
assert call('FlagGet',jessie+0x500) and call('FlagGet',james+0x500)
assert bytes(rd(p+i,1) for i in range(6*MONSIZE))==original
print('PASS Koga Singles setup; paired Jessie/James callbacks, identities, separate defeat flags and unchanged sole main.')
