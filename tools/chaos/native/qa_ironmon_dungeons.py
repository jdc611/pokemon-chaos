"""Dungeon boundaries/prerequisites/completion fixtures, not puzzle playthroughs."""
from qa_ironmon_core import *
x=struct.unpack('<52I',(ROOT/'ironmon-dungeon-layout.bin').read_bytes())
fuji,rocket,silph,scope,key,strength,badge8,location,position,tm=x[42:]
def header(i):return call('Overworld_GetMapHeaderByGroupAndId',*x[i*2:i*2+2])
def enter(i):
 h=header(i)
 for j in range(28):wr(symbols['gMapHeader']+j,rd(h+j,1),1)
 save1=rd('gSaveBlock1Ptr');wr(save1+location,x[i*2],1);wr(save1+location+1,x[i*2+1],1)
 call('IronmonOnMapLoaded')
for group in range(7):
 start();call('IronmonGiveStarter',BULBA)
 for flag in (fuji,rocket,silph):call('FlagClear',flag)
 first,inner,out=group*3,group*3+1,group*3+2
 if group==2:
  enter(first);assert not call('IronmonEscapeLocked')
  assert call('AddBagItem',scope,1)
 if group==6:
  enter(first);assert not call('IronmonEscapeLocked')
  call('FlagSet',badge8);assert call('AddBagItem',strength,1)
 enter(first);assert call('IronmonEscapeLocked')
 assert call('IronmonWarpAllowed',header(inner))
 assert not call('IronmonWarpAllowed',header(out))
 assert not call('CanUseDigOrEscapeRopeOnCurMap')
 if group==0:enter(inner)
 elif group==1:wr(rd('gSaveBlock1Ptr')+position+2,37,2)
 elif group==2:call('FlagSet',fuji)
 elif group==3:call('FlagSet',rocket)
 elif group==4:call('FlagSet',silph)
 elif group==5:assert call('AddBagItem',key,1)
 elif group==6:enter(inner)
 assert call('IronmonWarpAllowed',header(out)),group
 call('IronmonOnMapTransition',header(out))
 assert not call('IronmonEscapeLocked'),group
 print('PASS dungeon',group+1,'prerequisites, internal warp, entry denial and intended completion.')
start();call('IronmonGiveStarter',BULBA)
assert not call('AddBagItem',tm,1) and not call('IsItemShopCriteriaFulfilled',tm)
# Seeded Gym reward authorizes exactly one TM award; later calls cannot reuse it.
world=struct.unpack('<112I',(ROOT/'ironmon-world-layout.bin').read_bytes())
section,g,n,leader,won,*_=world[:14]
h=call('Overworld_GetMapHeaderByGroupAndId',g,n)
for j in range(28):wr(symbols['gMapHeader']+j,rd(h+j,1),1)
call('FlagSet',won);call('IronmonPrepareGymTM');reward=rd('gSpecialVar_0x8000',2)
assert call('AddBagItem',reward,1) and not call('AddBagItem',reward,1)
call('IronmonPrepareGymTM');assert rd('gSpecialVar_0x8000',2)==reward
assert call('RemoveBagItem',reward,1) # Acquisition policy must not block ordinary item removal.
print('PASS TM policy: non-Gym award/shop denied; stable Gym reward; authorization spent once.')

wr(s+DIFF,1,1);call('IronmonInitializeRun')
assert not call('IronmonEscapeLocked') and not call('IronmonLeaderLocked',leader)
assert call('IsItemShopCriteriaFulfilled',tm) and call('AddBagItem',tm,1)
print('PASS ordinary Chaos mode remains outside IronMON world/item restrictions.')
