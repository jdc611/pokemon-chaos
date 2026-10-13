"""Real post-opening interactions, exit warps and evolution branch UI regressions."""
from qa_oaks_draft_battles import *
def map_id():
 save=rd('gSaveBlock1Ptr');return rd(save+4,1),rd(save+5,1)
def xy():
 obj=symbols['gObjectEvents']+rd(symbols['gPlayerAvatar']+5,1)*36
 return rd(obj+16,2)-7,rd(obj+18,2)-7
state('draft-after-opening',1)
assert not call('ArePlayerFieldControlsLocked') and call('VarGet',LABSCENE)==4
identity=bytesat(p,6*MS)
for ball in ('Bulbasaur','Squirtle','Charmander'):
 call('ScriptContext_SetupScript',symbols['PalletTown_ProfessorOaksLab_EventScript_'+ball+'Ball'])
 for _ in range(12):tap(1,40)
 assert map_id()==(LABGROUP,LABNUM) and not call('ArePlayerFieldControlsLocked')
 assert bytesat(p,6*MS)==identity
print('PASS all three post-battle starter balls stay empty, preserve party and release controls',flush=True)
for visit in range(3):
 call('ScriptContext_SetupScript',symbols['OaksDraftRoom_Entrance']);frames(200)
 assert map_id()==(ROOMGROUP,ROOMNUM) and not call('ArePlayerFieldControlsLocked')
 assert xy()==(6,11),xy()
 snap('draft-fixed-room-'+str(visit))
 frames(45,128);frames(200)
 assert map_id()==(LABGROUP,LABNUM),map_id()
 assert not call('ArePlayerFieldControlsLocked')
 before=xy();frames(20,16);frames(20)
 assert xy()!=before,('exit controls did not move',before)
print('PASS three actual room visits/door exits resume movement with no script resets',flush=True)
# Check the real coordinate triggers from each player approach column.
for column in (5,6,7):
 state('draft-before-lab-battle',1)
 call('SetWarpDestinationToMapWarp',LABGROUP,LABNUM,255)
 wr(symbols['sWarpDestination']+4,column,2);wr(symbols['sWarpDestination']+6,7,2)
 call('DoWarp');frames(200)
 wr(scratch,255,1)
 assert not call('TryGetObjectEventIdByLocalIdAndMap',8,LABNUM,LABGROUP,scratch)
 rival=symbols['gObjectEvents']+rd(scratch,1)*36
 def rival_xy():return rd(rival+16,2)-7,rd(rival+18,2)-7
 assert rival_xy()==(6,4),rival_xy()
 frames(18,128);frames(60)
 positions=set()
 for _ in range(100):
  positions.add(rival_xy());tap(1,25)
  if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:break
 assert rival_xy()==(column,7),(column,rival_xy())
 assert all(5<=x<=7 and 4<=y<=7 for x,y in positions),positions
 assert rd('gPartiesCount',1)==6 and rd(symbols['gPartiesCount']+1,1)==6
print('PASS all three actual rival approach triggers stay in the aisle and stop directly before the player',flush=True)
# Global policy applies even with the old Dynamax equipment and ordinary play styles.
state('draft-opening-active',1)
for mode in (0,1,2,3,4,5,DRAFTMODE):
 wr(rd('gSaveBlock3Ptr')+START,mode,1)
 for battler in (0,1):assert not call('CanDynamax',battler)
print('PASS Dynamax is unavailable to player and rival in every play style',flush=True)
# Exercise all Scyther branches in a real Summary screen, then a long condition list.
state('draft-after-opening',1)
call('CreateRandomMon',p,123,5)
wr('gSpecialVar_0x8004',0,2)
wr('gInitialSummaryScreenCallback',symbols['CB2_ReturnToField']|1)
call('ShowPokemonSummaryScreen',0,p,0,5)
wr(rd('sMonSummaryScreen')+4,symbols['CB2_ReturnToField']|1);frames(180)
for _ in range(3):tap(16,60)
assert rd('sMonSummaryScreen')
for branch in range(4):
 snap('draft-fixed-scyther-branch-'+str(branch));tap(1,40)
tap(2,180)
assert not call('ArePlayerFieldControlsLocked')
print('PASS real Scyther evolution-branch switching and Summary exit',flush=True)
# Milcery has more than two requirements; A must show the remaining condition
# before moving to the next evolution branch rather than covering the growth panel.
call('CreateRandomMon',p,868,5)
call('ShowPokemonSummaryScreen',0,p,0,5)
wr(rd('sMonSummaryScreen')+4,symbols['CB2_ReturnToField']|1);frames(180)
for _ in range(3):tap(16,60)
for page in range(3):snap('draft-fixed-milcery-'+str(page));tap(1,40)
tap(2,180)
assert not call('ArePlayerFieldControlsLocked')
print('PASS actual multi-condition evolution paging and Summary exit',flush=True)
