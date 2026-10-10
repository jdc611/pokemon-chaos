"""Actual Normal custom species UI: preview, cancellation and award."""
from qa_ironmon_core import *
for offset in (0,3):
 state('ironmon-core-base',1)
 wr('gRunSetupDifficulty',4,1);wr('gRunSetupWorldSeed',24680)
 wr('gRunSetupStartRegion',1,1);wr('gRunSetupNuzlocke',0,1);wr('gDebugForceKantoNewGame',1,1)
 call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(600)
 s=rd('gSaveBlock3Ptr')
 for _ in range(25):frames(3,1);frames(40)
 assert call('ChaosOakStarterIsCustom') and rd('gPartiesCount',1)==0
 call('VarSet',0x800f,5)
 call('ScriptContext_SetupScript',symbols['PalletTown_ProfessorOaksLab_EventScript_BulbasaurBall']);call('ScriptContext_Enable');frames(300)
 task=call('FindTaskIdByFunc',symbols['Task_CustomStarterInput']|1)
 assert task!=255
 td=symbols['gTasks']+task*40+8
 for _ in range(60):
  if rd(td+4,2)==0:break
  frames(120)
 else:raise AssertionError('custom list did not finish loading')
 for _ in range(offset):frames(3,128);frames(25)
 chosen=rd(symbols['sCustomStarterList']+rd(td,2)*2,2)
 frames(3,1);frames(25);assert rd(td+4,2)==2
 snap('ironmon-custom-confirm-'+str(offset))
 frames(3,2);frames(25);assert rd(td+4,2)==0 and rd('gPartiesCount',1)==0
 frames(3,1);frames(25);frames(3,16);frames(25);frames(3,1);frames(25)
 assert rd(td+4,2)==0 and rd('gPartiesCount',1)==0
 frames(3,1);frames(25);frames(3,1);frames(25)
 assert rd('gCustomStarterSpecies',2)==chosen
 for _ in range(180):
  frames(3,1);frames(25)
  if rd('gPartiesCount',1):break
 else:raise AssertionError('custom starter not awarded')
 for _ in range(180):
  frames(3,2);frames(25)
  if not call('ArePlayerFieldControlsLocked'):break
 else:raise AssertionError('custom nickname/dialogue did not return')
 assert rd('gPartiesCount',1)==1 and data(p,SP)==chosen and data(p,LV)==5 and data(p,ITEM)>0
 assert any(call('IronmonMoveIsStarterAttack',data(p,MOVE+i)) for i in range(4))
 assert all(data(p,HPIV+i)==31 and data(p,HPEV+i)==0 for i in range(6))
 before=bytes(rd(p+i,1) for i in range(MONSIZE));call('IronmonGiveStarter',PIKA)
 assert before==bytes(rd(p+i,1) for i in range(MONSIZE))
 print('PASS actual Normal custom selection, B/No cancellation, exact species, traits/MGM/attack/item and no reroll',chosen,flush=True)
