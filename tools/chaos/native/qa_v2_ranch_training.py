from qa import *
import json
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
def key(k=1,n=35):frames(2,k);frames(n)
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
call('VarSet',0x408e,0);warp(38,25,13,2)
# Enter through the actual rear doorway; return using the Ranch's real entrance.
frames(2,64);frames(230)
for _ in range(40):key()
groups=json.loads((repo/'data/maps/map_groups.json').read_text())
for gi,g in enumerate(groups['group_order']):
 if 'ChaosPokemonRanch' in groups[g]:ni=groups[g].index('ChaosPokemonRanch');break
mapid=rd(symbols['gMapHeader']);layout=rd(mapid+0) # Only callback and the visible native map are asserted below.
snap('v2-ranch-entry');assert rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
print('Ranch current section',rd(symbols['gMapHeader']+18,1),flush=True)
# Native MapHeader pointer equals the Ranch header; explicit pointer resolve from generated map header.
assert rd('gMapHeader')==symbols['ChaosPokemonRanch_Layout'],hex(rd('gMapHeader'))
frames(2,64);frames(300);snap('v2-ranch-return');assert rd('gMapHeader')==symbols['PokemonCenter_1F_FRLG_Layout']
call('VarSet',0x408f,1);call('ShowStartMenu');frames(90);wr('gMenuCallback',symbols['StartMenuTrainToCap']|1);frames(180);key(1,90);snap('v2-train-menu-field');key(2,120);assert rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
print('PASS actual Pewter Ranch doorway entry/exit and Train to Cap field-background menu/cancel.',flush=True)
