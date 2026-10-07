from qa import *
import struct
v=struct.unpack('<26I',(ROOT/'pass-layout.bin').read_bytes());p=symbols['gParties'];sc=0x0203e000
assert lib.boot(str(repo/'pokefirered.gba').encode())
def key(k=1,n=23):frames(2,k);frames(n)
def adv(n):
 for _ in range(n):key()
def warp():
 call('SetWarpDestinationToMapWarp',38,22,255);wr(symbols['sWarpDestination']+4,6,2);wr(symbols['sWarpDestination']+6,6,2);call('DoWarp');frames(230)
def action():
 for n in range(12000):
  if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:return
  frames(1,1 if n%25==0 else 0)
 raise AssertionError('battle did not initialize action input')
for diff,mgm in [(1,0),(2,0),(1,1),(2,1)]:
 state('pass-world',1);s=rd('gSaveBlock3Ptr');wr(s+17,diff,1);wr(s+15,mgm,1);call('VarSet',0x408c,0)
 warp();call('ZeroPlayerPartyMons');call('ScriptGiveMon',1,5,0);wr(sc,1);call('SetMonData',p,10,sc);key(64,8);key();action();key();adv(350)
 assert not call('FlagGet',0x820);assert rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
 assert call('GetMonData3',p,10,0)>0
 warp();key(64,8);key();action();assert rd('gBattleTypeFlags')&8
 # second attempt accepts a real turn, not only a transition/intro.
 key();key();adv(12);assert rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1
 print('PASS Brock genuine loss / heal / retry / actionable second battle AI',diff,'MGM',mgm,flush=True)
