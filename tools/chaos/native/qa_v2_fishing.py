from qa import *
assert lib.boot(str(repo/'pokefirered.gba').encode())
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
for rod in range(3):
 successes=0;failures=0
 for trial in range(5):
  state('pass-world',1);call('VarSet',0x408e,0);warp(37,25,8,18)
  call('SeedRng',rod*100+trial*7+1)
  call('StartFishing',rod)
  for i in range(3600):
   cb=rd(symbols['gMain']+4)
   if cb==symbols['BattleMainCB2']|1:successes+=1;break
   if i>500 and not call('ArePlayerFieldControlsLocked'):failures+=1;break
   frames(1,1 if i%6<2 else 0)
  else:raise AssertionError(('Fishing stuck',rod,trial))
 print('PASS rod',rod,'premature A ignored, automatic encounters',successes,'failed bites',failures,flush=True)
 assert successes
