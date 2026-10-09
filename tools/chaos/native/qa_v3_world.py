from qa import *
import struct
v=struct.unpack('<'+str(len((ROOT/'v3-world-layout.bin').read_bytes())//4)+'I',(ROOT/'v3-world-layout.bin').read_bytes());scratch=0x0203e000;p=symbols['gParties']
assert lib.boot(str(repo/'pokefirered.gba').encode())
def reset():state('pass-world',1)
def key(k=1,n=35):frames(2,k);frames(n)
def data(field,value,slot=0):wr(scratch,value);call('SetMonData',p+slot*100,field,scratch)
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
# Uniform 16-step cycles, not instant; ordinary/native egg and randomized vendor egg.
for species in [1,25,129]:
 reset();call('ZeroPlayerPartyMons');call('ScriptGiveMon',species,5,0);data(v[10],1);data(v[11],1)
 wr(rd('gSaveBlock1Ptr')+v[0],0,1)
 for _ in range(15):assert not call('ShouldEggHatch')
 assert call('GetMonData3',p,v[11],0)==1
 assert not call('ShouldEggHatch');assert call('GetMonData3',p,v[11],0)==0
 for _ in range(15):assert not call('ShouldEggHatch')
 assert call('ShouldEggHatch');assert rd('gSpecialVar_0x8004',2)==0
print('PASS uniform accelerated hatch counters for Bulbasaur/Pikachu/Magikarp: 16-step cycles, ordinary pending hatch',flush=True)
# Repeated DexNav opening at unchanged map/settings while unrelated RNG advances.
reset();wr(rd('gSaveBlock3Ptr')+v[1],1,1);wr(rd('gSaveBlock3Ptr')+v[2],123456)
warp(37,32,10,10)
baseline=None
for i in range(10):
 call('CreateTask',symbols['Task_OpenDexNavFromStartMenu']|1,0);frames(200)
 gui=rd('sDexNavUiDataPtr');assert gui
 land=tuple(rd(gui+v[3]+j*v[9],v[9]) for j in range(v[7]));water=tuple(rd(gui+v[4]+j*v[9],v[9]) for j in range(v[8]));methods=tuple(rd(gui+v[5]+j,1) for j in range(v[8]));count=rd(gui+v[6],1)
 now=(land,water,methods,count)
 if baseline is None:baseline=now;snap('v3-dexnav')
 else:assert now==baseline,(i,now,baseline)
 key(2,200)
 for _ in range(50):wr('gRngValue',(1103515245*rd('gRngValue')+24691)&0xffffffff)
assert baseline[3]>0;print('PASS DexNav ten open/close cycles with randomized encounters and advancing unrelated RNG; land/water/method data stable',baseline,flush=True)
print('V3 world probes complete',flush=True)
