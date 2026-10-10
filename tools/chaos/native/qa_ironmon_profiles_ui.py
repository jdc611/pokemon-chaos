"""Real setup button presses select and persist both IronMON BST profiles."""
from qa_ironmon_core import *
STARTER,BST,CUSTOM,RANDOM,SHUFFLE,FULLRANDOM,DMAX,BLOCK,R1,R2,R3,GIMMICK,OLD,NEW,STATEBST=struct.unpack('<15I',(ROOT/'ironmon-profile-layout.bin').read_bytes())
def tap(key):frames(3,key);frames(25)
for mode in (4,5):
 for toggle in (False,True):
  state('ironmon-core-base',1)
  call('SetMainCallback2',symbols['CB2_RunSetupForFireRed']|1);frames(80)
  task=call('FindTaskIdByFunc',symbols['Task_RunSetup_Input']|1)
  assert task!=255
  td=symbols['gTasks']+task*40+8
  for _ in range(mode-2):tap(16)
  assert rd('sRunSetupDifficulty',1)==mode
  choice=SHUFFLE if mode==4 else FULLRANDOM
  assert rd('sRunSetupBstMode',1)==choice
  tap(128);tap(128)
  assert rd(td,2)==2
  if toggle:
   tap(16);choice=FULLRANDOM if choice==SHUFFLE else SHUFFLE
  assert rd('sRunSetupBstMode',1)==choice
  snap('ironmon-setup-'+str(mode)+'-'+str(choice))
  tap(128);assert rd(td,2)==5
  tap(1);tap(128);tap(128);tap(1);tap(1);frames(600)
  assert rd('gRunSetupDifficulty',1)==mode and rd('gRunSetupBstMode',1)==choice
  s=rd('gSaveBlock3Ptr')
  assert call('IsIronmonRun') and rd(s+IM+STATEBST,1)==choice and rd(s+BST,1)==choice
  print('PASS actual setup defaults, BST toggle, skipped hidden rows, confirmation and new-run persistence',mode,choice,flush=True)
