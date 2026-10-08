from qa import *
import struct
l=struct.unpack('<37I',(ROOT/'v2-layout.bin').read_bytes());v=struct.unpack('<5I',(ROOT/'v2-berry-layout.bin').read_bytes());assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-brock-retry',1)
b=symbols['gBattleMons'];p=symbols['gParties'];s=0x0203e000;bs=rd('gBattleStruct');ps=bs+l[25];lost=bs+l[26]
wr(b+v[0],l[21],2);wr(lost,l[21],2);wr(s,l[21]);call('SetMonData',p,l[14],s);wr(b+v[1],1,2)
assert call('ItemBattleEffects',0,0,v[4],symbols['IsOnHpThresholdActivation']|1)
# Execute the actual consume command selected by the berry battle script.
wr(s,0,1);wr(s+1,v[3],1);wr('gBattlescriptCurrInstr',s);wr('gBattlerAttacker',0,1);call('Cmd_removeitem')
assert rd(ps)&1<<31 and rd(ps+4)&1 and rd(b+v[0],2)==0
# Harvest/restored/transferred berry cannot reactivate for this holder in this battle.
wr(b+v[0],l[21],2)
assert call('GetBattlerHoldEffectInternal',0,0)==0
assert not call('ItemBattleEffects',0,0,v[4],symbols['IsOnHpThresholdActivation']|1)
wr(s,0);call('SetMonData',p,l[14],s);call('TryRestoreHeldItems');assert call('GetMonData3',p,l[14],0)==l[21]
print('PASS native Oran activation -> actual consume command -> repeat/Harvest prevention -> original-holder post-battle refund.',flush=True)
