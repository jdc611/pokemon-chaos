from qa import *
import json,re
assert lib.boot(str(repo/'pokefirered.gba').encode())
def key(k=1,n=25):frames(2,k);frames(n)
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
state('pass-world',1);call('VarSet',0x408e,0)
# Actual repeated map loads, nurse healing, Records menu and every fifth cycle save/reload.
for cycle in range(24):
 warp(37,17,17,7);warp(51,0,6,5)
 call('ScriptContext_SetupScript',symbols['Route4_PokemonCenter_1F_EventScript_Nurse'])
 for _ in range(60):key()
 assert not call('ArePlayerFieldControlsLocked'),cycle
 if cycle%5==0:
  state('v2-center-save');state('v2-center-save',1)
 if cycle%6==0:
  call('ScriptContext_SetupScript',symbols['EventScript_ChaosRecordsNurse'])
  for _ in range(9):key()
  key(2,100)
 assert rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
 print('PASS MtMoon Center entry/heal/exit cycle',cycle+1,flush=True)
