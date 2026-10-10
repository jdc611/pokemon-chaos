from qa_ironmon_core import *
start(seed=0x1234abcd)
args=0x0203e100
wr(args,0xffffffff)
eligible=[species for species in range(1,N_SPECIES)
          if call('IsExactSpeciesEligibleRandomSpecies',0,species,args)]
for seed in (0x1234abcd,1,0xffffffff):
 wr(s+SEED,seed);wr(s+IM+IMSEED,seed)
 for index,species in enumerate(eligible):
  wr(s+IM+48,0,1) # starterGranted, stable checkpoint field
  call('IronmonGiveStarter',species)
  assert data(p,SP)==species and data(p,LV)==5,(seed,species,'identity')
  assert any(call('IronmonMoveIsStarterAttack',data(p,MOVE+i)) for i in range(4)),(seed,species,'no attack')
  assert data(p,ITEM)>0,(seed,species,'no held item')
  assert all(data(p,HPIV+i)==31 and data(p,HPEV+i)==0 for i in range(6)),(seed,species,'MGM')
  assert all(data(p,PP+i)>0 for i in range(4) if data(p,MOVE+i)),(seed,species,'zero PP')
  if index%250==0:print('pool progress',seed,index,len(eligible),flush=True)
 print('PASS seed',seed,'all',len(eligible),'eligible starter species',flush=True)
