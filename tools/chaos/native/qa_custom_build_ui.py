"""Real crate/naming/ability/move/allocation controls at native GBA resolution."""
from qa_custom_build import *
PAGE=2048+MS+4+26+12+6+36+6
REC=2048+MS+4
FLOORS=REC+26
TARGET=PAGE-2
def ui():return rd('sCustom')
def page():return rd(ui()+PAGE,1)
def left():return rd(ui()+TARGET,2)-call('CustomBuildSum',ui()+REC)
def tap(key,n=25):frames(8,key);frames(n)
state('draft-base',1)
call('SetMainCallback2',symbols['CB2_RunSetupForFireRed']|1);frames(90)
tap(4);tap(4);assert rd('sRunSetupDraft',1)==2
snap('custom-mode-choice')
for _ in range(3):tap(128)
tap(1);assert rd('sRunSetupPage',1)==1
snap('custom-fixed-options');tap(1)
assert rd('sRunSetupPage',1)==3
# Existing seed/confirmation page.
tap(128);tap(128);tap(1);assert rd('sRunSetupConfirm',1)
tap(1);frames(600)
s=rd('gSaveBlock3Ptr');assert call('IsCustomBuild')
call('VarSet',LABSCENE,1)
call('SetWarpDestinationToMapWarp',LABGROUP,LABNUM,0);call('DoWarp');frames(180)
for _ in range(45):tap(1,35)
sav1=rd('gSaveBlock1Ptr');assert (rd(sav1+4,1),rd(sav1+5,1))==(ROOMGROUP,ROOMNUM)
assert not call('ArePlayerFieldControlsLocked')
snap('custom-room-introduction')
for round in range(6):
 call('ScriptContext_SetupScript',symbols['OaksDraftRoom_Crate'+str(round)]);frames(240)
 for _ in range(24):
  if ui():break
  tap(1,35)
 frames(90)
 assert ui() and page()==0,(round,ui(),page())
 for _ in range(round%3):tap(16,60)
 frames(60)
 assert rd(ui()+PAGE+2,1)==round%3,(round,"candidate navigation",rd(ui()+PAGE+2,1))
 snap('custom-crate-'+str(round))
 tap(1,60)
 if page()!=1:
  state("custom-first-pick-stall");snap("custom-first-pick-stall")
 assert page()==1,(round,page(),ui())
 tap(2);assert page()==0 # confirmation can be declined before commitment
 tap(1);tap(1,300)
 assert rd('sNamingScreen')
 snap('custom-name-'+str(round))
 tap(1,100);tap(8,70);tap(1,240)
 snap("custom-after-naming-"+str(round))
 assert ui() and page()==4,('ability page',round,page())
 snap('custom-abilities-'+str(round))
 tap(128);tap(16);tap(1)
 assert page()==5
 snap('custom-moves-'+str(round));tap(128);tap(256);tap(1)
 assert page()==6
 snap('custom-floors-'+str(round));tap(1);assert page()==7
 initial=left();assert initial>0
 tap(1);assert page()==7 # cannot confirm unspent points
 tap(256,3);assert left()<initial
 tap(4);assert left()==initial # reset only the uncommitted allocation
 for _ in range(100):
  if not left():break
  tap(256,2)
 assert left()==0
 assert call('CustomBuildSum',ui()+REC)==rd(ui()+TARGET,2)
 snap('custom-allocated-'+str(round))
 tap(8);assert page()==8
 tap(1);assert page()==9
 tap(2);assert page()==8
 tap(1);tap(1,300)
 assert rd(s+DRAFT+ROUND,1)==round+1
 for _ in range(45):
  if ui() and page()==11:break
  if not ui() and not call('ArePlayerFieldControlsLocked'):break
  tap(1,40)
 if round<5:assert not ui() and not call('ArePlayerFieldControlsLocked')
assert rd('gPartiesCount',1)==6
assert ui() and page()==11
snap('custom-draft-report');tap(2,180)
assert not ui() and not call('ArePlayerFieldControlsLocked')
state('custom-ui-six')
print('PASS actual Custom Build startup, intro, all six crate/naming/ability/move/floor/allocation/confirmation flows',flush=True)
frames(18,128);frames(60)
for _ in range(100):
 tap(1,30)
 if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:break
assert rd('gPartiesCount',1)==6 and rd(symbols['gPartiesCount']+1,1)==6
assert not call('CanActivateGimmick',1,2) and not call('CanTerastallize',1)
snap('custom-real-opening-battle');state('custom-real-opening')
print('PASS genuine Custom Build opening 6v6 with no removed battle gimmicks',flush=True)
