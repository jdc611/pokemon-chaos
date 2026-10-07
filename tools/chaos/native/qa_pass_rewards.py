from qa import *
import struct,json,re
v=struct.unpack('<22I',(ROOT/'verify-layout.bin').read_bytes());scratch=0x0203e000
assert lib.boot(str(repo/'pokefirered.gba').encode())
def key(k=1,n=24):frames(2,k);frames(n)
def advance(n=90):
 for _ in range(n):key()
def warp(mapname,x,y):
 groups=json.loads((repo/'data/maps/map_groups.json').read_text())
 for gi,g in enumerate(groups['group_order']):
  if mapname in groups[g]:ni=groups[g].index(mapname);break
 call('SetWarpDestinationToMapWarp',gi,ni,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
items={k:int(n) for k,n in re.findall(r'(ITEM_\w+)\s*=\s*(\d+)',(repo/'include/constants/items.h').read_text())}
# Execute the exact event scripts on their maps, including item notification and hide flag.
for row in json.loads((repo/'docs/chaos-mega-pickups.json').read_text()):
 state('pass-world',1);call('ClearBag');flag=int(row['flag'],16);call('FlagClear',flag)
 m=json.loads((repo/f"data/maps/{row['map']}/map.json").read_text());o=next(o for o in m['object_events'] if o.get('x')==row['x'] and o.get('y')==row['y'] and 'ChaosMegaPickup' in o.get('script',''))
 for trainer in range(1,900):call('SetTrainerFlag',trainer)
 # Avoid trainer sight in map fixtures; native ball script and map object still execute normally.
 warp(row['map'],row['x'],row['y']+1);wr('gSpecialVar_LastTalked',m['object_events'].index(o)+1,2)
 call('ScriptContext_SetupScript',symbols[o['script']]);advance()
 assert call('CheckBagHasItem',items['ITEM_'+row['stone']],1),row
 assert call('FlagGet',flag),row
 assert not call('ArePlayerFieldControlsLocked'),row
 print('PASS native world item event',row['stone'],row['map'],flush=True)
# Actual post-victory script continuations, not only the claim function.
entries=[('PewterCity_Gym_Frlg','PewterCity_Gym_EventScript_BrockChaosRematchWon',0,308),('CeruleanCity_Gym_Frlg','CeruleanCity_Gym_EventScript_MistyChaosRematchWon',1,303),('VermilionCity_Gym_Frlg','VermilionCity_Gym_EventScript_LtSurgeChaosRematchWon',2,321),('FuchsiaCity_Gym_Frlg','FuchsiaCity_Gym_EventScript_DefeatedKoga',3,296),('SaffronCity_Gym_Frlg','SaffronCity_Gym_EventScript_DefeatedSabrina',4,298),('ViridianCity_Gym_Frlg','ViridianCity_Gym_EventScript_DefeatedGiovanni',5,333),('CinnabarIsland_Gym_Frlg','CinnabarIsland_Gym_EventScript_DefeatedBlaine',6,846)]
for mapname,script,index,item in entries:
 state('pass-world',1);call('ClearBag');call('ChaosEnsureRunRecords');warp(mapname,5,8)
 call('ScriptContext_SetupScript',symbols[script]);advance(160)
 assert call('CheckBagHasItem',item,1),(script,item)
 call('RemoveBagItem',item,1);wr('gSpecialVar_0x8004',index,2);call('ChaosClaimGymStone');assert not call('CheckBagHasItem',item,1),script
 print('PASS native victory continuation / one-time reward',script,item,flush=True)
# Save and reload the enlarged native SaveBlock3, keeping encounter state, toggles and league snapshot.
state('pass-world',1);call('VarSet',0x408c,1);call('VarSet',0x408d,1);call('ChaosEnsureRunRecords');s=rd('gSaveBlock3Ptr');wr(s+v[20],123456789);wr(s+v[8],27)
call('CreateWildMon',19,5);call('BattleSetup_StartWildBattle');call('ChaosSnapshotLeague');assert rd(s+v[9],1)
expected=bytes(rd(s+i,1) for i in range(1564));print('save result',call('TrySavingData',0),flush=True)
for i in range(1564):wr(s+i,0,1)
print('load result',call('LoadGameSave',0),flush=True);s=rd('gSaveBlock3Ptr');actual=bytes(rd(s+i,1) for i in range(1564));assert actual==expected
assert call('VarGet',0x408c)==1 and call('VarGet',0x408d)==1
print('PASS native flash save/reload of full 1564-byte run records, encounter state, league snapshot and independent settings.',flush=True)
