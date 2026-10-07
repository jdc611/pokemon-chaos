from qa import *
import json,struct
z=struct.unpack('<11I',(ROOT/'battle-layout.bin').read_bytes());p=symbols['gParties'];b=symbols['gBattleMons'];sc=0x0203e000;groups=json.loads((repo/'data/maps/map_groups.json').read_text())
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
def key(k=1,n=24):frames(2,k);frames(n)
for gi,g in enumerate(groups['group_order']):
 if 'CeruleanCity_Frlg' in groups[g]:ni=groups[g].index('CeruleanCity_Frlg');break
call('SetWarpDestinationToMapWarp',gi,ni,255);wr(symbols['sWarpDestination']+4,23,2);wr(symbols['sWarpDestination']+6,7,2);call('DoWarp');frames(230)
for badge in range(0x820,0x828):call('FlagSet',badge)
call('ZeroPlayerPartyMons');call('ScriptGiveMon',59,80,0)
for m in range(4):call('SetMonMoveSlot',p,399,m)
assert not call('VarGet',0x4090)
call('ScriptContext_SetupScript',symbols['CeruleanCity_EventScript_RivalTriggerMid'])
for n in range(1200):
 if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:
  for i in range(12):
   if call('GetMonData3',p+600+i*100,10,0)>1:wr(sc,1);call('SetMonData',p+600+i*100,10,sc)
  if rd(b+z[0]+z[3],2)>1:wr(b+z[0]+z[3],1,2)
  wr(b+z[3],rd(b+z[4],2),2);wr(b+z[5],0)
  for m in range(4):wr(b+z[2]+m,80,1)
 key()
 if call('VarGet',0x4090)==1 and not call('ArePlayerFieldControlsLocked'):break
else:
 snap('qa-rider-stuck');state('rider-stuck');print('callback',hex(rd(symbols['gMain']+4)), 'locked',call('ArePlayerFieldControlsLocked'),flush=True);raise AssertionError(('rival gift not complete',rd('gBattleOutcome',1)))
import struct
fame=struct.unpack('<4I',(ROOT/'extra-layout.bin').read_bytes())[3]
assert not call('CheckBagHasItem',fame,1)
print('PASS actual Cerulean rival -> victory -> PokéRider unlock, no Fame Checker, return control.',flush=True)
