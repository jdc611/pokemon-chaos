from qa import *
import struct
l=struct.unpack('<37I',(ROOT/'v2-layout.bin').read_bytes());z=struct.unpack('<11I',(ROOT/'battle-layout.bin').read_bytes());p=symbols['gParties'];s=0x0203e000
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-brock-retry',1)
bs=rd('gBattleStruct');ps=bs+l[25];lost=bs+l[26];b=symbols['gBattleMons']
# Native item consumption tracking uses former padding bits 29 and 30.
for current in [0,l[22]]:
 state('pass-brock-retry',1);bs=rd('gBattleStruct');ps=bs+l[25];lost=bs+l[26]
 wr(lost,l[21],2);wr(ps,rd(ps)|(1<<31));wr(ps+4,rd(ps+4)|1);wr(s,current);call('SetMonData',p,l[14],s);call('TryRestoreHeldItems')
 assert call('GetMonData3',p,l[14],0)==(l[21] if current==0 else current)
 call('TryRestoreHeldItems');assert call('GetMonData3',p,l[14],0)==(l[21] if current==0 else current)
 print('PASS berry restoration/no overwrite/idempotence current item',current,flush=True)
for flag in ['knocked','stolen','unused']:
 state('pass-brock-retry',1);bs=rd('gBattleStruct');ps=bs+l[25];lost=bs+l[26]
 wr(lost,l[21],2);wr(s,0);call('SetMonData',p,l[14],s)
 if flag!='unused':wr(ps,rd(ps)|(1<<31));wr(ps+4,rd(ps+4)|1)
 if flag=='knocked':wr(ps,rd(ps)|(1<<28))
 if flag=='stolen':wr(lost,rd(lost,2)|(1<<15),2)
 call('TryRestoreHeldItems');assert call('GetMonData3',p,l[14],0)==0
 print('PASS no berry refund for',flag,flush=True)
