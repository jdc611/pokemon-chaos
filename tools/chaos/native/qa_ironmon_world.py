"""Canonical Gym requirements, seeded roster sizes and item policy fixtures."""
from qa_ironmon_core import *
rows=struct.unpack('<112I',(ROOT/'ironmon-world-layout.bin').read_bytes())
for i in range(8):
 section,group,num,leader,won,n,*trainers=rows[i*14:(i+1)*14]
 start();call('FlagClear',won)
 header=call('Overworld_GetMapHeaderByGroupAndId',group,num)
 for j in range(28):wr(symbols['gMapHeader']+j,rd(header+j,1),1)
 for trainer in trainers[:n]:call('FlagClear',trainer+0x500)
 call('IronmonOnMapLoaded')
 assert call('IronmonEscapeLocked') and not call('CanUseDigOrEscapeRopeOnCurMap')
 assert call('IronmonWarpAllowed',header)
 assert call('IronmonLeaderLocked',leader)
 for trainer in trainers[:n-1]:call('FlagSet',trainer+0x500)
 assert call('IronmonLeaderLocked',leader)
 call('FlagSet',trainers[n-1]+0x500)
 assert not call('IronmonLeaderLocked',leader)
 call('CreateNPCTrainerParty',p+6*MONSIZE,leader)
 count=sum(data(p+(6+j)*MONSIZE,SP)!=0 for j in range(6))
 assert count == (3,3,4,4,5,5,6,6)[i],(i,count)
 call('FlagSet',won);assert not call('IronmonEscapeLocked')
 print('PASS Gym',i+1,'all',n,'trainer flags; internal warp; escape lock; victory unlock; boss size',count)
# Same seed gives the same item; no generated item belongs to the TM/HM pocket.
start()
for seed in range(256):
 item=call('IronmonOverworldItem',seed)
 assert item == call('IronmonOverworldItem',seed) and item!=0
 assert call('GetItemPocket',item)!=2 # POCKET_TM_HM
print('PASS uniform-pool item policy: 256 deterministic rewards, no TMs/HMs.')
