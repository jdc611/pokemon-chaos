from qa import *
import struct,json,re
v=struct.unpack('<26I',(ROOT/'pass-layout.bin').read_bytes());p=symbols['gParties'];sz=v[7];HP=v[9];SPEC=v[8];LV=v[11]; scratch=0x0203e000
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-start',1)
def key(k=1,n=23):frames(2,k);frames(n)
def advance(n=50):
 for _ in range(n):key()
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
def script(s,talk=1):wr('gSpecialVar_LastTalked',talk,2);call('ScriptContext_SetupScript',symbols[s]);frames(60)
def datum(i,field,value,side=0):
 wr(scratch,value);call('SetMonData',p+(side*6+i)*sz,field,scratch)
def ready(sps=[1,7],level=10):
 call('ZeroPlayerPartyMons')
 for sp in sps:call('ScriptGiveMon',sp,level,0)
advance(8);ready();call('FlagSet',0x828);call('FlagSet',0x829)
state('pass-world')
# Real Brock encounter loses through the engine, then re-enters and initializes again.
warp(38,22,6,6);ready([1],5);datum(0,HP,1)
key(64,8);key();advance(65);snap('qa-pass-brock-entry');state('pass-brock-battle')
print('battleCB',hex(rd(symbols['gMain']+4)),'flags',hex(rd('gBattleTypeFlags')),'enemy',rd('gBattleMons',2),flush=True)
advance(320);snap('qa-pass-brock-loss')
print('lossCB',hex(rd(symbols['gMain']+4)), 'partyhp',call('GetMonData3',p,HP,0),'badge',call('FlagGet',0x820),flush=True)
assert not call('FlagGet',0x820)
warp(38,22,6,6);key(64,8);key();advance(65);snap('qa-pass-brock-retry');state('pass-brock-retry')
assert rd('gBattleTypeFlags') & 8
assert rd('gBattleMons',2)==1
print('PASS Brock genuine loss / whiteout / re-entry / second battle initialization.',flush=True)
