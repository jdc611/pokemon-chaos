from qa import *
import struct
z=struct.unpack("<11I",(ROOT/"battle-layout.bin").read_bytes()); BS,MOV,PP,BHP=z[:4]
p=symbols['gParties'];b=symbols['gBattleMons'];scratch=0x0203e000
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
def key(k=1,n=23):frames(2,k);frames(n)
def advance(n=70):
 for _ in range(n):key()
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
def ready(level):
 call('ZeroPlayerPartyMons');call('ScriptGiveMon',59,level,0);call('ScriptGiveMon',9,level,0)
 for slot in range(2):
  for m in range(4):call('SetMonMoveSlot',p+slot*100,129,m)
def begin():
 call('ClearTrainerFlag',629);call('ClearTrainerFlag',630);warp(37,17,18,8)
 call('ScriptContext_SetupScript',symbols['EventScript_ChaosMtMoonRockets']);advance(105)
 assert rd('gBattlersCount',1)==4
 assert rd('gBattleTypeFlags') & 1
 assert rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1
 snap('qa-pass-jj-start');state('pass-jj-battle')
ready(5)
for i in range(2):wr(scratch,1);call('SetMonData',p+i*100,10,scratch)
begin();advance(400)
assert not call('HasTrainerBeenFought',629)
assert rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
snap('qa-pass-jj-loss');print('PASS Jessie/James genuine loss -> whiteout -> no completion flag.',flush=True)
ready(75);begin();advance(15)
# Force low enemy HP, then use real spread attacks to exercise faint/battle-end and automatic continuation.
for i in range(12):wr(scratch,1);call('SetMonData',p+600+i*100,10,scratch)
for battler in [1,3]:wr(b+battler*BS+BHP,1,2)
for battler,slot in [(0,0),(2,1)]:
 for m in range(4):wr(b+battler*BS+PP+m,80,1)
for n in range(1100):
 if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:
  for battler in [0,2]:wr(b+battler*BS+z[5],0)
  for battler in [1,3]:
   if rd(b+battler*BS+BHP,2)>1:wr(b+battler*BS+BHP,1,2)
 key()
 if call('HasTrainerBeenFought',629) and rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1:break
print('Win fixture frames/loops',n,'outcome',rd('gBattleOutcome',1),flush=True)
advance(100);snap('qa-pass-jj-win')
assert call('HasTrainerBeenFought',629) and call('HasTrainerBeenFought',630)
assert rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
assert not call('ArePlayerFieldControlsLocked')
warp(37,17,18,8);assert call('VarGet',0x4000)==1
print('PASS clean retry -> win -> automatic blast-off/completion -> return control -> no re-entry retrigger.',flush=True)
state('pass-jj-won')
