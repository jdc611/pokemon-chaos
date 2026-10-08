from qa import *
import struct,json,ctypes
raw=(ROOT/'v2-layout.bin').read_bytes();l=struct.unpack('<'+str(len(raw)//4)+'I',raw)
lib.call6.restype=C.c_uint;lib.call6.argtypes=[C.c_uint]*7
assert lib.boot(str(repo/'pokefirered.gba').encode())
p=symbols['gParties'];scratch=0x0203e000
items={}
# enum names via compiler-stable constants extracted from the item enum, including aliases.
import re
source=(repo/'include/constants/items.h').read_text();val=-1
for line in source.splitlines():
 m=re.match(r'\s*(ITEM_\w+)(?:\s*=\s*([^,]+))?,',line)
 if m:
  name,value=m.groups()
  if value:
   try:val=int(value,0)
   except:
    if value in items:val=items[value]
    else:continue
  else:val+=1
  items[name]=val

def reset():state('pass-world',1)
def data(field,value,slot=0):wr(scratch,value);call('SetMonData',p+slot*100,field,scratch)
def key(k=1,n=25):frames(2,k);frames(n)
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
# MGM suppresses every SetMonData EV write and rejects vitamins; normal mode still accepts them.
reset();wr(rd('gSaveBlock3Ptr')+l[30],1,1);call('ZeroPlayerPartyMons');call('ScriptGiveMon',123,20,0)
for i in range(6):
 assert call('GetMonData3',p,l[12]+i,0)==31
 data(l[13]+i,100);assert call('GetMonData3',p,l[13]+i,0)==0
for item in [l[23],l[24]]:assert call('PokemonUseItemEffects',p,item,0,0)
call('MonGainEVs',p,150);assert all(call('GetMonData3',p,l[13]+i,0)==0 for i in range(6))
reset();call('ZeroPlayerPartyMons');call('ScriptGiveMon',123,20,0);assert not call('PokemonUseItemEffects',p,l[23],0,0)
print('PASS MGM perfect IVs, zero battle/direct-write EVs, vitamin rejection; normal vitamin use.',flush=True)
# Independent EXP All default and toggle.
reset();assert call('IsGen6ExpShareEnabled');call('VarSet',0x4091,2);assert not call('IsGen6ExpShareEnabled');call('VarSet',0x4091,1);assert call('IsGen6ExpShareEnabled')
print('PASS EXP All default ON and independent OFF/ON setting.',flush=True)
# Eggs charge exactly once, full-party PC delivery remains a gift, no funds failure.
for vendor in range(3):
 reset();call('ZeroPlayerPartyMons');money=rd('gSaveBlock1Ptr')+l[31];call('SetMoney',money,10000);call('VarSet',0x4093,0)
 if vendor==2:
  for sp in [59,3,9,6,25,123]:call('ScriptGiveMon',sp,20,0)
 wr('gSpecialVar_0x8004',vendor,2);call('ChaosBuyMysteryEgg');assert rd('gSpecialVar_Result',2)==0
 assert call('GetMoney',money)==5000 and call('VarGet',0x4093)==1<<vendor
 if vendor<2:assert call('GetMonData3',p,l[16],0)
 else:assert call('GetBoxMonDataAt',0,0,l[16])
 call('ChaosBuyMysteryEgg');assert rd('gSpecialVar_Result',2)==3 and call('GetMoney',money)==5000
 print('PASS egg vendor',vendor,'one delivery, one charge, own receipt and full-party PC handling',flush=True)
reset();money=rd('gSaveBlock1Ptr')+l[31];call('SetMoney',money,4999);wr('gSpecialVar_0x8004',0,2);call('ChaosBuyMysteryEgg');assert rd('gSpecialVar_Result',2)==1 and call('GetMoney',money)==4999
# Native care transactions, all three exact packages and Rare Candy maximum.
for i,(g,n,x,y) in enumerate([(37,16,5,13),(37,17,32,5),(37,23,8,19)]):
 reset();call('ClearBag');call('VarSet',0x4092,0);call('VarSet',0x408e,1);warp(g,n,x,y)
 call('ChaosTryCareMilestone',x,y)
 assert call('VarGet',0x4092)&1<<i,(i,g,n,call('VarGet',0x4092))
 assert call('CountTotalItemQuantityInBag',l[20])==999
 arrays=['sCare1','sCare2','sCare3'];text=(repo/'src/chaos_v2.c').read_text();chunk=text.split('static const struct CareItem '+arrays[i]+'[] = ')[1].split(';')[0]
 for j,(name,q) in enumerate(re.findall(r'\{(ITEM_\w+),(\d+)\}',chunk)):
  q=int(q);nativeItem=rd(symbols[arrays[i]]+4*j,2)
  if q:assert call('CountTotalItemQuantityInBag',nativeItem)==q,(name,nativeItem,q)
 before=call('CountTotalItemQuantityInBag',12);assert not call('ChaosTryCareMilestone',x,y);assert call('CountTotalItemQuantityInBag',12)==before
 print('PASS care package',i+1,'all item quantities, 999 candies and replay guard',flush=True)
# Native evo getter with all six ABI arguments, preserving held-item requirements.
for sp,held,item,target in [(64,0,l[18],65),(123,l[19],l[18],212),(79,items['ITEM_KINGS_ROCK'],l[18],199),(133,items['ITEM_NONE'],items['ITEM_ICE_STONE'],471),(102,0,items['ITEM_SUN_STONE'],972)]:
 reset();call('ZeroPlayerPartyMons');call('ScriptGiveMon',sp,30,0);data(l[14],held)
 result=lib.call6(symbols['GetEvolutionTargetSpecies'],p,3,item,0,0,0)
 print('EVO',sp,held,item,'->',result,flush=True);assert result==target,(sp,result,target)
 if held:
  data(l[14],0);assert lib.call6(symbols['GetEvolutionTargetSpecies'],p,3,item,0,0,0)!=target
print('PASS representative Link Cable, held-item, stone and regional single-player evolutions.',flush=True)
