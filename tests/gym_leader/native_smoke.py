"""Direct ARM-call and flash-save fixtures; not a completed GLE playthrough."""
import sys,struct
sys.path.insert(0,'tools/chaos/native')
from qa import *
data=(ROOT/'layout.bin').read_bytes();v=struct.unpack('<'+'I'*(len(data)//4),data)
assert lib.boot(str(repo/'pokefirered.gba').encode())
frames(1800);frames(3,8);frames(100)
assert rd('gSaveBlock3Ptr')!=0
# Existing new-game fixture bypasses name entry, not GLE gameplay.
for dst,text in [(rd('gSaveBlock2Ptr'),bytes([0xc4,0xbb,0xbd,0xc5,0xff])),(rd('gSaveBlock1Ptr')+v[4],bytes([0xbc,0xc6,0xcf,0xbf,0xff]))]:
 for i,b in enumerate(text):wr(dst+i,b,1)
wr('gDebugForceKantoNewGame',1,1)
call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(600)
assert rd('gSaveBlock3Ptr')!=0
call('GymLeaderInitialize');assert call('IsGymLeaderRun')
s=call('GymLeaderState');assert s==rd('gSaveBlock1Ptr')+v[0]
assert call('GleChooseIdentity',s,1,4,2)==0
assert call('GetCurrentLevelCap')==15
assert call('GleAddRankXp',s,100)==0 and call('GetCurrentLevelCap')==25
assert call('GleAddRankXp',s,180)==6
assert call('GleInspectionResult',s,1)==0 and call('GetCurrentLevelCap')==35
assert call('GymLeaderSignaturePoolsMatchTypes')
counts=[]
for ty in range(18):
 count=call('GymLeaderPoolCount',ty,1,0,1)
 counts.append(count)
 assert count>=9,(ty,count)
for ty,base,evolution in [(4,1,2),(1,4,5),(2,7,8)]:
 scratch=call('Alloc_',1024,0)
 assert scratch
 n=call('GymLeaderBuildStarterPool',ty,scratch,512)
 pool=[rd(scratch+2*i,2) for i in range(n)]
 assert base in pool and evolution not in pool,(ty,base,evolution)
 call('Free',scratch)
print('Native starter counts:',counts,flush=True)
# Verify actual linked species data, not just the JSON source.
for species,total in [(v[7],600),(v[8],615),(v[9],650)]:assert call('GetSpeciesBaseStatTotal',species)==total
assert call('GymLeaderExclusiveSpecies',5,0)==v[6]
assert call('GymLeaderOfficialTeamLegal')==0 # no registered party yet
expected=bytes(rd(s+i,1) for i in range(v[1]))
result=call('TrySavingData',0);assert result==1,result
for i in range(v[1]):wr(s+i,0,1)
assert not call('IsGymLeaderRun')
result=call('LoadGameSave',0);assert result==1,result
s=call('GymLeaderState')
assert bytes(rd(s+i,1) for i in range(v[1]))==expected
assert call('GetCurrentLevelCap')==35
snap('gle-native-state-smoke')
print('PASS ARM state, rank/inspection, Glastrier, 3 custom forms and native flash save/reload; no UI/playthrough claim')
