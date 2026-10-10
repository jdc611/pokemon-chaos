"""New preset budgets, legacy preservation and lab Dynamax permissions."""
from qa_ironmon_core import *
STARTER,BST,CUSTOM,RANDOM,SHUFFLE,FULLRANDOM,DMAX,BLOCK,RIVAL1,RIVAL2,RIVAL3,GIMMICK,OLD,NEW,STATEBST=struct.unpack('<15I',(ROOT/'ironmon-profile-layout.bin').read_bytes())
getters=('GetSpeciesBaseHP','GetSpeciesBaseAttack','GetSpeciesBaseDefense','GetSpeciesBaseSpAttack','GetSpeciesBaseSpDefense','GetSpeciesBaseSpeed')
def stats(species):return tuple(call(fn,species) for fn in getters)
for mode in (4,5):
 for choice in (SHUFFLE,FULLRANDOM):
  for seed in (1,24680,0xffffffff):
   start(mode,seed)
   wr(s+BST,choice,1);call('IronmonInitializeRun')
   assert rd(s+IM+STATEBST,1)==choice
   wr(s+BST,FULLRANDOM if choice==SHUFFLE else SHUFFLE,1);call('IronmonEnforcePreset')
   assert rd(s+IM)==NEW
   assert rd(s+STARTER,1)==(CUSTOM if mode==4 else RANDOM)
   assert rd(s+BST,1)==choice
   values={sp:stats(sp) for sp in (1,6,25,68,150,445)}
   if choice==SHUFFLE:assert sum(values[1])==318 and sum(values[150])==680
   else:assert any(sum(values[sp])!=total for sp,total in ((1,318),(150,680)))
   wr('gRngValue',1234567)
   assert values=={sp:stats(sp) for sp in values}
   for rival in (RIVAL1,RIVAL2,RIVAL3):
    call('CreateNPCTrainerParty',p+6*MONSIZE,rival)
    assert data(p+6*MONSIZE,DMAX)==BLOCK
    assert data(p+6*MONSIZE,HPIV)==31 and data(p+6*MONSIZE,HPEV)==0
   call('IronmonGiveStarter',BULBA)
   call('SavePlayerParty');assert call('TrySavingData',0)==1
   wr(s+BST,0,1);wr(s+STARTER,0,1)
   assert call('LoadGameSave',0)==1;call('LoadPlayerParty');call('IronmonEnforcePreset')
   assert rd(s+BST,1)==choice
   assert values=={sp:stats(sp) for sp in values}
   print('PASS new preset, species/seed stats, lab Dynamax block, MGM and save/reload',mode,choice,seed,values[1],flush=True)
 # Emulate a real v1 header without altering the old run's stat budget.
 start(mode,24680);wr(s+IM,OLD);call('IronmonEnforcePreset')
 assert call('IsIronmonRun') and rd(s+BST,1)==SHUFFLE and rd(s+STARTER,1)==RANDOM
 assert sum(stats(1))==318 and sum(stats(150))==680
 print('PASS legacy IronMON preset stays unchanged',mode,flush=True)
