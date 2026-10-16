"""Retained NPC identities, Custom Nuzlocke and removed-equipment policies."""
from qa_custom_build import *
# Data-level progression fixtures exercise real party generation, not balance.
for trainer in (RIVAL,ROUTE_RIVAL,CHAMP):
 call('CreateNPCTrainerParty',p+6*MS,trainer)
 team=bytesat(p+6*MS,6*MS)
 for i in range(6):
  save=rd('gSaveBlock3Ptr');pick=rd(save+DRAFT+PICKS+i,1);rival=rd(save+DRAFT+RIVALPICKS+i,1)
  index=i*3+(3-pick-rival if trainer==CHAMP else rival)
  assert data(p+(6+i)*MS,PID)==call('IronmonMix',rd(save+SEED)^(0x504f4b45+index))
  assert call('CustomBuildBaseStat',p+(6+i)*MS,0)>0
  assert call('GetMonAbility',p+(6+i)*MS)>0
 call('CreateNPCTrainerParty',p+6*MS,trainer)
 assert bytesat(p+6*MS,6*MS)==team
print('PASS Custom rival and Oak identities, independent bases/abilities and byte-stable repeat encounters',flush=True)
call('VarSet',NUZ,1);setdata(p,HP,0)
assert not call('Nuzlocke_ProcessBattleDeaths') and rd('gPartiesCount',1)==6
call('HealPlayerParty');call('OaksDraftFinishOpening');setdata(p,HP,0)
assert call('Nuzlocke_ProcessBattleDeaths') and rd('gPartiesCount',1)==5
assert call('ScriptGiveMon',133,5,0)==2 and call('ScriptGiveEgg',133)==2
assert call('GiveCapturedMonToPlayer',p)==2
print('PASS Custom opening death exemption, later grave, capture/free-gift/egg restrictions',flush=True)
# Explicitly opted-in optional exploration rewards are deterministic too.
wr('gSpecialVar_0x8000',TM1,2);call('CustomBuildOptionalReward');reward=rd('gSpecialVar_0x8000',2)
wr('gSpecialVar_0x8000',TM1,2);call('CustomBuildOptionalReward');assert reward==rd('gSpecialVar_0x8000',2)
assert TM1<=reward<=TM100
for seed in range(100):
 assert not call('ChaosRemovedItem',call('IronmonHeldItem',seed))
 assert not call('ChaosRemovedItem',call('IronmonOverworldItem',seed))
print('PASS optional reward persistence and 200 IronMon generated items exclude removed mechanics',flush=True)
