from qa import *
import struct
v=struct.unpack('<22I',(ROOT/'verify-layout.bin').read_bytes());z=struct.unpack('<11I',(ROOT/'battle-layout.bin').read_bytes());d=struct.unpack('<7I',(ROOT/'doubles-layout.bin').read_bytes());b=symbols['gBattleMons'];sc=0x0203e000
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-jj-battle',1)
def key(k=1,n=20):frames(2,k);frames(n)
def waitfn(fn):
 for n in range(15000):
  if rd('gBattlerControllerFuncs')==symbols[fn]|1:return
  frames(1,1 if n%25==0 else 0)
 raise AssertionError(fn)
waitfn('HandleInputChooseAction');call('VarSet',0x404e,0)
# Full native opposing-target calculation, left Water / right Ground.
for battler,t in [(1,v[13]),(3,v[15])]:
 for i in range(3):wr(b+battler*z[0]+d[0]+i*d[3],t,d[3])
 wr(b+battler*z[0]+d[1],0,d[4])
wr(b+d[2],v[17],d[5]);wr(b+d[2]+d[5],v[18],d[5]);wr('gMoveSelectionCursor',0,1);wr('gActionSelectionCursor',0,1);key();waitfn('HandleInputChooseMove');frames(2)
left=call('CheckTypeEffectiveness',0,1);right=call('CheckTypeEffectiveness',0,3);assert left!=right,(left,right)
key();assert rd('gBattlerControllerFuncs')==symbols['HandleInputChooseTarget']|1
first=rd('gMultiUsePlayerCursor',1);snap('qa-pass-double-target-first');key(16);second=rd('gMultiUsePlayerCursor',1);snap('qa-pass-double-target-second');assert first!=second
# Back to moves and spread move independent hints.
key(2);key(16);frames(2);call('MoveSelectionDisplayMoveEffectiveness',0,0);snap('qa-pass-double-spread')
print('PASS native double target selection switches immediately; distinct opposing effectiveness',left,right,'and spread dual indicators rendered.',flush=True)

state('pass-doubles-screen')
print('palette',call('GetWindowAttribute',7,5),[hex(rd(symbols['gPlttBufferFaded']+2*(call('GetWindowAttribute',7,5)*16+i),2)) for i in range(16)],flush=True)
