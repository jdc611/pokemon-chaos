"""Use the actual field chord; direct overlay calls miss caller script locks."""
from qa_ironmon_core import *
for mode in (1,4,5):
 state('ironmon-core-base',1)
 wr('gRunSetupDifficulty',4 if mode==5 else mode,1);wr('gRunSetupWorldSeed',24680)
 wr('gRunSetupStartRegion',1,1);wr('gRunSetupNuzlocke',0,1);wr('gDebugForceKantoNewGame',1,1)
 call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(600)
 for _ in range(25):frames(3,1);frames(40)
 # Controlled stable field fixture: stop the lab's pending ball-selection
 # script, retaining the real overworld callbacks and shortcut caller.
 call('ScriptContext_Init');call('UnlockPlayerFieldControls');frames(10)
 assert not call('ArePlayerFieldControlsLocked')
 assert rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
 if mode==5:
  # Use the same stable field scene, avoiding Hardcore's automatic lab fight.
  wr(rd('gSaveBlock3Ptr')+DIFF,5,1);call('IronmonInitializeRun')
 call('ZeroPlayerPartyMons')
 wr(rd('gSaveBlock2Ptr')+0x13,2,1) # L=A must not open a drawer on denial.
 frames(1,512|4);frames(12)
 assert not call('ChaosTrackerIsOpen') and not call('ArePlayerFieldControlsLocked')
 call('ScriptGiveMon' if mode==1 else 'IronmonGiveStarter',BULBA,5,0)
 for _ in range(3):
  frames(1,512|4);frames(12)
  assert call('ChaosTrackerIsOpen')
  frames(3,2);frames(12)
  assert not call('ChaosTrackerIsOpen') and not call('ArePlayerFieldControlsLocked')
 call('PlayerGetDestCoords',scratch,scratch+2)
 before=(rd(scratch,2),rd(scratch+2,2))
 frames(24,32);frames(20)
 call('PlayerGetDestCoords',scratch,scratch+2)
 assert before!=(rd(scratch,2),rd(scratch+2,2)), 'player did not move after close'
 # The next Start press must reach a working field menu.
 frames(3,8);frames(60)
 assert call('ArePlayerFieldControlsLocked')
 frames(3,2);frames(60)
 assert not call('ArePlayerFieldControlsLocked')
 print('PASS actual field shortcut, repeated B close, restored controls and next Start/Cancel',mode,flush=True)
