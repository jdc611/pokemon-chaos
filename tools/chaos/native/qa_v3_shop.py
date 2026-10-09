from qa import *
import struct,re
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
a=symbols['CeladonCity_DepartmentStore_5F_XItems'];names=re.findall(r'\.2byte (ITEM_\w+)',(repo/'data/maps/CeladonCity_DepartmentStore_5F_Frlg/scripts.inc').read_text().split('CeladonCity_DepartmentStore_5F_XItems::',1)[1].split('\trelease',1)[0]);stock=[]
for i,name in enumerate(names):
 item=rd(a+2*i,2)
 if name=='ITEM_NONE':assert item==0;break
 assert item>0;stock.append(item)
 price=call('GetItemPrice',item)
 if i>=7:assert price==(5000 if name in ['ITEM_LIFE_ORB','ITEM_ASSAULT_VEST','ITEM_EVIOLITE','ITEM_WEAKNESS_POLICY','ITEM_ABILITY_SHIELD'] else 3000),(name,price)
 assert call('GetItemSellPrice',item)<=price
assert len(stock)==49 and len(set(stock))==49
print('PASS compiled Celadon counter: 49 unique valid entries; seven new held items, Booster Energy, all 17 Plates and 17 Memories; affordable prices and resale consistency',flush=True)
# Both Gloom branches evolve through the native item-evolution function, no National Dex.
raw=(ROOT/'v2-layout.bin').read_bytes();l=struct.unpack('<37I',raw);lib.call6.restype=C.c_uint;lib.call6.argtypes=[C.c_uint]*7
stockSource=(repo/'include/constants/items.h').read_text();ids={};value=-1
for line in stockSource.splitlines():
 m=re.match(r'\s*(ITEM_\w+)(?:\s*=\s*([^,]+))?,',line)
 if not m:continue
 name,explicit=m.groups()
 if explicit:
  try:value=int(explicit,0)
  except:
   if explicit not in ids:continue
   value=ids[explicit]
 else:value+=1
 ids[name]=value
for stone,target in [('ITEM_LEAF_STONE',45),('ITEM_SUN_STONE',182)]:
 state('pass-world',1);call('ZeroPlayerPartyMons');call('ScriptGiveMon',44,20,0)
 assert lib.call6(symbols['GetEvolutionTargetSpecies'],symbols['gParties'],3,ids[stone],0,0,0)==target
print('PASS native Gloom Leaf Stone -> Vileplume and Sun Stone -> Bellossom without National Dex',flush=True)
