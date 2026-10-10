"""Internal flag-profile comparison under the same seeded trainer fixture."""
from qa_ironmon_core import *
start();call('IronmonGiveStarter',BULBA)
wr('gBattleTypeFlags',8)
call('CreateNPCTrainerParty',p+6*MONSIZE,BROCK)
team=bytes(rd(p+6*MONSIZE+i,1) for i in range(6*MONSIZE))
smart=call('GetAiFlags',BROCK,1)
# aiProfile is the last byte in the 52-byte appended state.
wr(s+IM+51,1,1)
classic=call('GetAiFlags',BROCK,1)
assert classic==7 and smart!=classic,(smart,classic)
call('CreateNPCTrainerParty',p+6*MONSIZE,BROCK)
assert team==bytes(rd(p+6*MONSIZE+i,1) for i in range(6*MONSIZE))
call('IronmonEnforcePreset');assert rd(s+IM+51,1)==0
assert call('GetAiFlags',BROCK,1)==smart
wr(s+DIFF,1,1);call('IronmonInitializeRun');regular=call('GetAiFlags',BROCK,1)
wr(s+IM+51,1,1);assert call('GetAiFlags',BROCK,1)==regular
print('PASS same-seed smart/basic-profile comparison retains team bytes; preset restores smart; regular Chaos ignores comparison field.')
