"""Real setup, mapped crates, Summary, cancellation and selection controls."""
from qa_oaks_draft import *
def tap(key,n=30):frames(3,key);frames(n)
state('draft-base',1)
call('SetMainCallback2',symbols['CB2_RunSetupForFireRed']|1);frames(90)
assert call('FindTaskIdByFunc',symbols['Task_RunSetup_Input']|1)!=255
tap(4);assert rd('sRunSetupDraft',1)
snap('draft-setup-mode')
for _ in range(3):tap(128)
tap(1);assert rd('sRunSetupPage',1)==1
snap('draft-setup-options')
tap(16);tap(128);tap(1);tap(128);tap(1);tap(128);tap(1)
assert rd('sRunSetupPage',1)==3
snap('draft-setup-seed')
tap(128);tap(128);tap(1);assert rd('sRunSetupConfirm',1)
tap(1);frames(600)
s=rd('gSaveBlock3Ptr');assert call('IsOaksDraft') and rd(s+BST,1)==1 and rd(s+ABILITY,1)==1 and rd(s+MOVESET,1)==1
print('PASS actual setup: Draft toggle, restricted options, seed/confirm and new-game persistence',flush=True)
# Enter the original lab through its real warp and introductory event.
call('VarSet',LABSCENE,1)
call('SetWarpDestinationToMapWarp',LABGROUP,LABNUM,0);call('DoWarp');frames(180)
for _ in range(45):tap(1,35)
snap('draft-room')
sav1=rd('gSaveBlock1Ptr')
assert rd(sav1+4,1)==ROOMGROUP and rd(sav1+5,1)==ROOMNUM, 'intro did not reach room'
call('ScriptContext_Init');call('UnlockPlayerFieldControls')
# Execute actual crate script; future crate is sealed and reveals no UI.
call('ScriptContext_SetupScript',symbols['OaksDraftRoom_Crate5']);frames(80)
assert rd('sDraftScreen')==0
for _ in range(5):tap(1)
call('ScriptContext_Init');call('UnlockPlayerFieldControls')
for i in range(6):
 call('ScriptContext_SetupScript',symbols['OaksDraftRoom_Crate'+str(i)]);frames(100)
 assert rd('sDraftScreen')!=0
 snap('draft-crate-'+str(i+1))
 tap(1,100);assert rd('sMonSummaryScreen')!=0
 snap('draft-summary-'+str(i+1))
 # Info -> skills (IVs supported), battle moves, growth; then back to picker.
 for _ in range(3):tap(16,50)
 tap(2,100);assert rd('sDraftScreen')!=0
 tap(4);tap(2);assert rd(s+DRAFT+ROUND,1)==i
 tap(4);tap(1,90)
 assert rd(s+DRAFT+ROUND,1)==i+1
 for _ in range(40):
  tap(1,65)
  if (i==5 and rd("sDraftScreen")) or (i<5 and not call("ArePlayerFieldControlsLocked")):break
 if i<5:
  call('ScriptContext_Init');call('UnlockPlayerFieldControls')
# Sixth crate automatically opens the original report.
frames(100);snap("draft-end-sixth")
assert rd('sDraftScreen')!=0
snap('draft-report-original')
tap(256);snap('draft-report-analysis')
tap(256);snap('draft-report-current')
tap(2,120)
for _ in range(8):tap(1)
assert rd('gPartiesCount',1)==6
sav1=rd("gSaveBlock1Ptr");s=rd("gSaveBlock3Ptr")
snap("draft-return-lab")
assert rd(sav1+4,1)==LABGROUP and rd(sav1+5,1)==LABNUM
print('PASS real room/intro, sealed future crate, all six Summary screens, No/Yes confirmation and final report/return',flush=True)
state('draft-before-lab-battle')
# Step south through the lab's actual rival trigger.
frames(18,128);frames(60)
for _ in range(70):
 tap(1,30)
 if rd('gBattleTypeFlags')&8:break
assert rd('gBattleTypeFlags')&8
frames(150)
assert rd('gPartiesCount',1)==6 and rd(symbols['gPartiesCount']+1,1)==6
snap('draft-opening-battle')
print('PASS genuine opening battle initializes 6v6',flush=True)
