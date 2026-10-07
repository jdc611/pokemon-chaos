from qa import *
import json,re,struct
z=struct.unpack('<11I',(ROOT/'battle-layout.bin').read_bytes());p=symbols['gParties'];b=symbols['gBattleMons'];sc=0x0203e000;groups=json.loads((repo/'data/maps/map_groups.json').read_text())
assert lib.boot(str(repo/'pokefirered.gba').encode())
def key(k=1,n=24):frames(2,k);frames(n)
def warp(mapname,x,y):
 for gi,g in enumerate(groups['group_order']):
  if mapname in groups[g]:ni=groups[g].index(mapname);break
 call('SetWarpDestinationToMapWarp',gi,ni,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
items={k:int(n) for k,n in re.findall(r'(ITEM_\w+)\s*=\s*(\d+)',(repo/'include/constants/items.h').read_text())}
rows=[('PewterCity_Gym',6,5,624,308),('CeruleanCity_Gym',8,6,625,303),('VermilionCity_Gym',5,2,626,321),('FuchsiaCity_Gym',7,13,318,296),('SaffronCity_Gym',14,11,320,298),('CinnabarIsland_Gym',5,4,319,846),('ViridianCity_Gym',2,2,250,333)]
for index,(mapname,x,y,trainer,item) in enumerate(rows):
 state('pass-world',1);call('ChaosEnsureRunRecords');call('ClearBag');call('VarSet',0x408c,0);wr(rd('gSaveBlock3Ptr')+17,2,1);call('ZeroPlayerPartyMons')
 for sp in [59,9]:
  call('ScriptGiveMon',sp,100,0)
 for i in range(2):
  for m in range(4):call('SetMonMoveSlot',p+i*100,399,m)
 for flag in range(0x820,0x828):call('FlagSet',flag)
 if index>=3:call('FlagClear',0x820+{3:4,4:5,5:6,6:7}[index])
 call('VarSet',0x40fa,2);call('VarSet',0x40f9,7 if index>=3 else 7&~(1<<index));call('AddBagItem',items['ITEM_MEGA_RING'],1)
 for t in range(1,900):call('SetTrainerFlag',t)
 call('ClearTrainerFlag',trainer);warp(mapname+'_Frlg',x,y+1);key(64,8);key()
 for n in range(350):
  if rd('gBattlersCount',1)>=2 and rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:break
  key()
 else:raise AssertionError(('battle not entered',mapname))
 for i in range(12):wr(sc,1);call('SetMonData',p+600+i*100,10,sc)
 for n in range(1800):
  if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:
   for bat in range(rd('gBattlersCount',1)):
    if bat%2:
     if rd(b+bat*z[0]+z[3],2)>1:wr(b+bat*z[0]+z[3],1,2)
    else:
     wr(b+bat*z[0]+z[3],rd(b+bat*z[0]+z[4],2),2);wr(b+bat*z[0]+z[5],0)
     for m in range(4):wr(b+bat*z[0]+z[2]+m,80,1)
  key()
  if call('CheckBagHasItem',item,1) and rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1:break
 else:raise AssertionError(('victory or reward failed',mapname,rd('gBattleOutcome',1)))
 for _ in range(100):key()
 assert call('HasTrainerBeenFought',trainer)
 assert call('CheckBagHasItem',item,1)
 print('PASS actual Hard-AI battle victory -> item -> one-time reward',mapname,item,'loops',n,flush=True)
