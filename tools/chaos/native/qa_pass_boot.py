from qa import *
import struct
v=struct.unpack('<26I',(ROOT/'pass-layout.bin').read_bytes());rival=v[0]
assert lib.boot(str(repo/'pokefirered.gba').encode())
frames(900);frames(3,8);frames(100)
wr(rd('gSaveBlock1Ptr')+rival,255,1);wr(rd('gSaveBlock2Ptr'),255,1)
wr('gDebugForceKantoNewGame',1,1);call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(600)
snap('qa-pass-start');state('pass-start')
print('save3',hex(rd('gSaveBlock3Ptr')),'cb2',hex(rd(symbols['gMain']+4)),flush=True)
