from qa_ironmon_core import *
for mode in (1,3,4,5):
 state('ironmon-core-base',1)
 wr('gRunSetupDifficulty',mode,1);wr('gRunSetupWorldSeed',24680)
 wr('gRunSetupStartRegion',1,1);wr('gRunSetupNuzlocke',mode==3,1)
 wr('gDebugForceKantoNewGame',1,1)
 call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(600)
 s=rd('gSaveBlock3Ptr');assert s!=0
 assert bool(call('IsIronmonRun'))==(mode in (4,5))
 if mode==3:assert call('IsNuzlockeRun')
 for _ in range(25):frames(3,1);frames(40)
 if mode==5:
  assert rd('gPartiesCount',1)==1
  assert data(p,LV)==5 and data(p,ITEM)>0
  assert rd(s+IM+48,1)==1
 else:assert rd('gPartiesCount',1)==0
 snap('ironmon-start-mode-'+str(mode))
 print('PASS actual new-game callback mode',mode,'starter count',rd('gPartiesCount',1),flush=True)
