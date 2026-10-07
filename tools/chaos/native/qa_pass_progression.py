from qa import *
import json,re
p=symbols['gParties'];groups=json.loads((repo/'data/maps/map_groups.json').read_text());assert lib.boot(str(repo/'pokefirered.gba').encode())
def key(k=1,n=24):frames(2,k);frames(n)
def warp(mapname,x,y):
 for gi,g in enumerate(groups['group_order']):
  if mapname in groups[g]:ni=groups[g].index(mapname);break
 call('SetWarpDestinationToMapWarp',gi,ni,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
for mapname,x,y,section in [('ViridianCity_Frlg',34,24,89),('PewterCity_Frlg',28,27,90)]:
 state('pass-world',1);call('VarSet',0x408c,1);warp(mapname,x,y);mb=call('MapGridGetMetatileBehaviorAt',x+7,y+7)
 assert call('MetatileBehavior_IsLandWildEncounter',mb),(mapname,mb)
 for n in range(1000):
  if call('StandardWildEncounter',mb,mb):break
 else:raise AssertionError(('no encounter',mapname))
 assert call('NuzlockeMapSectionEncounterUsed',section)
 assert 2<=call('GetMonData3',p+600,64,0)<=8
 print('PASS native usable city habitat / unique named-area first encounter',mapname,call('GetMonData3',p+600,18,0),flush=True)
# All script fishing paths report missing / better rod and return control.
for script in ['EventScript_FishOldRod','EventScript_FishGoodRod','EventScript_FishSuperRod']:
 state('pass-world',1);call('ClearBag');call('ScriptContext_SetupScript',symbols[script]);frames(120);snap('qa-pass-'+script)
 assert bytes(rd(symbols['gStringVar4']+i,1) for i in range(80)).startswith(bytes(rd(symbols['Text_ChaosNoRod']+i,1) for i in range(32)))
 for _ in range(40):key()
 assert not call('ArePlayerFieldControlsLocked')
 print('PASS native missing rod feedback',script,flush=True)
# Both karate NPCs award actual reusable TMs; duplicate conversation does not duplicate gift.
for script in ['Route4_EventScript_MegaPunchTutor','Route4_EventScript_MegaKickTutor']:
 state('pass-world',1);call('ClearBag');warp('Route4_Frlg',30,4);wr('gSpecialVar_LastTalked',1,2);call('ScriptContext_SetupScript',symbols[script])
 for _ in range(120):key()
 item=rd('gSpecialVar_0x8000',2)
 import struct
 tm=struct.unpack('<4I',(ROOT/'extra-layout.bin').read_bytes())[0 if 'Punch' in script else 1]
 assert call('CheckBagHasItem',tm,1)
 call('RemoveBagItem',tm,1);call('ScriptContext_SetupScript',symbols[script])
 for _ in range(100):key()
 assert not call('CheckBagHasItem',tm,1)
 print('PASS native karate TM award / saved one-time receipt',script,tm,flush=True)
# Starter reward immediately includes five Balls before the Dex/parcel.
state('pass-start',1)
for _ in range(8):key()
call('ClearBag');call('VarSet',0x4001,0);call('VarSet',0x4002,1);wr('gSpecialVar_LastTalked',5,2)
call('ScriptContext_SetupScript',symbols['PalletTown_ProfessorOaksLab_EventScript_ChoseStarter'])
for n in range(180):
 key()
 if call('CheckBagHasItem',1,5):break
assert call('CheckBagHasItem',1,5) and call('GetMonData3',p,18,0)==1
print('PASS native Oak starter + five Balls immediately.',flush=True)
