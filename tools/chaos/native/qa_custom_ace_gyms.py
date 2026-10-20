"""Mapped Gym victory/reward checks with controlled HP/levels, not balance."""
from qa_custom_build import *
import json,re
z=struct.unpack('<11I',(ROOT/'battle-layout.bin').read_bytes());b=symbols['gBattleMons'];groups=json.loads((repo/'data/maps/map_groups.json').read_text())
def key(k=1,n=24):frames(8,k);frames(n)
def warp(mapname,x,y):
 for gi,g in enumerate(groups['group_order']):
  if mapname in groups[g]:ni=groups[g].index(mapname);break
 call('SetWarpDestinationToMapWarp',gi,ni,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
# Resolve leader IDs from the repository's existing native world table.
w=struct.unpack('<112I',(ROOT/'ironmon-world-layout.bin').read_bytes())
mapnames=['PewterCity_Gym','CeruleanCity_Gym','VermilionCity_Gym','CeladonCity_Gym','FuchsiaCity_Gym','SaffronCity_Gym','CinnabarIsland_Gym','ViridianCity_Gym']
for gym,mapname in enumerate(mapnames):
 start();s=rd('gSaveBlock3Ptr');wr(s+START,CUSTOM,1);call('OaksDraftInitialize')
 for i in range(6):
  work=scratch+800;r=work+MS
  call6('CustomBuildCandidate',work,r,i*3+i%3,0,5)
  assert call6('CustomBuildCommit',work,r,i%3,255,0)
 call('OaksDraftFinishOpening')
 for i in range(6):
  call('OaksDraftCreateCandidate',p+i*MS,i*3+rd(s+DRAFT+PICKS+i,1),100)
  for m in range(4):call('SetMonMoveSlot',p+i*MS,165,m)
 for flag in range(0x820,0x828):call('FlagSet',flag)
 call('VarSet',0x40fa,0 if gym<3 else 2) # original post-Sabrina progression gate
 call('VarSet',0x40f9,7)
 call('AddBagItem',703,1)
 for trainer in range(1,900):call('SetTrainerFlag',trainer)
 leader=w[gym*14+3];call('ClearTrainerFlag',leader);call('FlagClear',0x820+gym)
 m=json.loads((repo/'data/maps'/ (mapname+'_Frlg')/'map.json').read_text())
 obj=next(x for x in m['object_events'] if x.get('script','').endswith(('Brock','Misty','LtSurge','Erika','Koga','Sabrina','Blaine','Giovanni')))
 warp(mapname+'_Frlg',obj['x'],obj['y']+1);key(64,8);key()
 for turn in range(450):
  if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1 and rd('gBattlersCount',1)>=2:break
  key()
 else:raise AssertionError(('Gym did not begin',mapname))
 assert rd(symbols['gPartiesCount']+1,1)==6,('Gym party not six',mapname)
 if gym==4:assert rd('gBattlersCount',1)==4, 'Koga lost its Double Battle'
 for i in range(12):setdata(p+(6+i)*MS,HP,1)
 for turn in range(1600):
  if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:
   for bat in range(rd('gBattlersCount',1)):
    addr=b+bat*z[0]
    if bat%2:
     if rd(addr+z[3],2)>1:wr(addr+z[3],1,2)
    else:
     wr(addr+z[3],rd(addr+z[4],2),2);wr(addr+z[5],0)
     for move in range(4):wr(addr+z[2]+move,80,1)
  key()
  s=rd('gSaveBlock3Ptr')
  if rd(s+DRAFT+GYMTMS+gym*2,2) and rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1:break
 else:
  snap('draft-gym-stalled-'+str(gym));raise AssertionError(('Gym victory/reward incomplete',mapname,rd('gBattleOutcome',1)))
 assert call('FlagGet',0x820+gym) and call('HasTrainerBeenFought',leader)
 tm=rd(s+DRAFT+GYMTMS+gym*2,2);move=call('ItemIdToBattleMoveId',tm)
 assert call('CheckBagHasItem',tm,1) and any(call('CanLearnTeachableMove',data(p+i*MS,SP),move) for i in range(6))
 print('PASS mapped Gym battle, badge and compatible unique TM',mapname,tm,turn,flush=True)
