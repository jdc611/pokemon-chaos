"""Actual Pewter nurse policy and ordinary Start-menu flash save/reload.

Map placement, main species and initial injury are controlled fixtures. Nurse
choices and saving use ordinary inputs; no debug-injected save call is used.
"""
from qa_ironmon_core import *
def interact():
 frames(3,64);frames(25);frames(3,1);frames(25)
 assert call('ArePlayerFieldControlsLocked')
 for _ in range(400):
  frames(3,1);frames(25)
  if not call('ArePlayerFieldControlsLocked'):return
 snap('center-stall');raise AssertionError('Center script stalled')
def warp():
 call('SetWarpDestinationToMapWarp',38,25,255)
 wr(symbols['sWarpDestination']+4,7,2);wr(symbols['sWarpDestination']+6,4,2)
 call('DoWarp');frames(240)
for mode in (4,5):
 start(mode);call('IronmonGiveStarter',BULBA);setdata(p,HP,1);warp()
 interact()
 assert data(p,HP)==(data(p,MAXHP) if mode==4 else 1)
 frames(240);frames(3,8);frames(60)
 for _ in range(4):frames(3,128);frames(25)
 assert rd('sStartMenuCursorPos',1)==4
 frames(3,1);frames(60)
 for _ in range(150):frames(3,1);frames(25)
 frames(3,2);frames(60)
 assert rd('gSaveAttemptStatus',1)==1
 print('PASS actual Start-menu flash save',mode,flush=True)
 assert call('LoadGameSave',0)==1;call('LoadPlayerParty');setdata(p,HP,1)
 interact();assert data(p,HP)==1
 print('PASS actual nurse interaction, mode healing policy, save/reload and reentry denial',mode,flush=True)
