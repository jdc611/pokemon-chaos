from qa import *
import json,re
assert lib.boot(str(repo/'pokefirered.gba').encode())
def key(k=1,n=25):frames(2,k);frames(n)
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
# Full-party first eligible native Nuzlocke captures on Route 9, 10 and 12.
for n,sp in [(22,150),(23,143),(25,131)]:
 state('pass-world',1);call('VarSet',0x408e,0);call('VarSet',0x408d,1);call('VarSet',0x408c,1);call('ClearBag');call('AddBagItem',1,10);call('ZeroPlayerPartyMons')
 for species in [59,3,9,6,25,123]:call('ScriptGiveMon',species,30,0)
 warp(37,n,10,10);call('CreateWildMon',sp,5);call('BattleSetup_StartWildBattle');frames(600)
 for i in range(12000):
  if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
  frames(1,1 if i%25==0 else 0)
 assert call('CanThrowBall'),n
 frames(80);wr('gLastThrownBall',1,2);wr('gBallToDisplay',1,2);key(256,1200)
 # Decline at capture prompt: Nuzlocke must still open naming; blank OK cannot escape.
 for i in range(140):
  if rd(symbols['gMain']+4)==symbols['CB2_NamingScreen']|1:break
  key(2)
 else:raise AssertionError(('No mandatory naming',n))
 frames(150);key(8);key();frames(150)
 assert rd(symbols['gMain']+4)==symbols['CB2_NamingScreen']|1
 snap('v2-nuz-blank');state('v2-nuz-blank')
 # Return cursor from OK to first key, add a name, then complete.
 call('SetCursorPos',0,0);key(1,160);key(8,100);key(1,180)
 for _ in range(120):key(2)
 snap('v2-nuz-after');state('v2-nuz-after')
 assert rd('gBattleOutcome',1)==7 and rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1,(n,hex(rd(symbols['gMain']+4)))
 assert call('GetBoxMonDataAt',0,0,18)==sp
 assert call('CountTotalItemQuantityInBag',1)==9
 assert [call('GetMonData3',symbols['gParties']+100*j,18,0) for j in range(6)]==[59,3,9,6,25,123]
 print('PASS first Nuzlocke full-party route capture',n,sp,'mandatory nonblank nickname, PC delivery, party unchanged',flush=True)
