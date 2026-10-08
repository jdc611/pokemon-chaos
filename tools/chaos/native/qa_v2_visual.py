from qa import *
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
def key(k=1,n=35):frames(2,k);frames(n)
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
warp(38,2,35,25);snap('v2-viridian-grass')
warp(38,18,32,27);snap('v2-pewter-grass')
warp(38,25,6,5);snap('v2-pewter-center')
warp(51,0,6,5);snap('v2-mtmoon-center')
call('ZeroPlayerPartyMons')
for sp in [123,133,93,1,25,130]:call('ScriptGiveMon',sp,20,0)
call('CB2_PartyMenuFromStartMenu');frames(180);snap('v2-party');key();snap('v2-party-actions');key();frames(120);snap('v2-summary')
for i in range(3):key(16,120)
snap('v2-growth');key();snap('v2-growth-branch');state('v2-growth')
