"""Real GLA shop purchase and Type: Null placement; rank/money are fixtures."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(100):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('walk blocked',pos(),(x,y)))
def waitfor(check,limit=1200):
 for _ in range(limit):
  if check():return
  frames(1)
 snap('supply-failure');raise AssertionError(('timeout',hex(rd('gMain')),menu_count()))
def supply():
 walkto(6,13);walkto(10,13);walkto(10,11);press(64);press(1)
 until(lambda:menu_count()==6)
def listing():return [rd(symbols['sSupplyItems']+2*i,2) for i in range(rd('sSupplyCount',1))]
state('supply-opening')
# Actual rank-one clerk and purchase through the native buy/quantity/confirm UI.
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],20000)
supply();choose(0)
waitfor(lambda:task('Task_ShopMenu')<16)
assert v[139] in listing() and v[140] in listing()
assert v[141] not in listing()
price=call('GetItemPrice',v[139]);assert price>0
press(1);waitfor(lambda:task('Task_BuyMenu')<16)
frames(100);snap('gla-supply-list')
press(1);until(lambda:task('Task_BuyHowManyDialogueHandleInput')<16)
press(1);until(lambda:task('Task_CallYesOrNoCallback')<16);choose(0)
until(lambda:call('CheckBagHasItem',v[139],1)!=0)
# Money is deducted after the native thank-you message completes.
until(lambda:call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==20000-price)
press(1);waitfor(lambda:task('Task_BuyMenu')<16)
press(2);until(lambda:task('Task_ShopMenu')<16)
press(2);until(lambda:menu_count()==6)
print('PASS actual GLA clerk, rank-one stock, native purchase/Bag/money and shop return',flush=True)
# Rank/category gates and honored pricing, with no protected/reward items.
state('supply-after-buy')
for rank in (1,2,3,4,5,6,7,8):
 for cat in range(5):
  state('supply-after-buy',1);wr(current()+v[2],rank,1)
  choose(cat)
  if cat==4 or (cat==3 and rank<4) or (cat in (1,2) and rank<2):
   until(lambda:menu_count()==6);assert rd('sSupplyCount',1)==0
  else:
   waitfor(lambda:task('Task_ShopMenu')<16)
   stock=listing();assert stock and len(stock)<48 and len(stock)==len(set(stock))
   assert all(call('GetItemPrice',item)>0 for item in stock)
   assert not set(stock)&{v[42],878,v[115]}
   assert (v[141] in stock)==(cat==2 and rank>=6)
   for honor in (0,100,-100):
    wr(current()+v[136],honor&65535,2)
    assert call('GymLeaderSupplyPrice',stock[0])==call('GetItemPrice',stock[0])*(9 if honor==100 else 10)//10
print('PASS all ranks/categories, positive prices, protected-item exclusions and standing discount',flush=True)
# The actual long evolution stock must survive native shop filtering/scrolling.
state('supply-after-buy',1);wr(current()+v[2],4,1)
choose(1);waitfor(lambda:task('Task_ShopMenu')<16)
stock=listing()
import re
item_ids={name:int(value) for name,value in re.findall(r'\b(ITEM_[A-Z0-9_]+) = (\d+)',(repo/'include/constants/items.h').read_text())}
for name in ['ITEM_TART_APPLE','ITEM_BLACK_AUGURITE','ITEM_PEAT_BLOCK','ITEM_GALARICA_CUFF','ITEM_GALARICA_WREATH',
 'ITEM_CRACKED_POT','ITEM_CHIPPED_POT','ITEM_STRAWBERRY_SWEET','ITEM_RIBBON_SWEET','ITEM_AUSPICIOUS_ARMOR',
 'ITEM_MALICIOUS_ARMOR','ITEM_LEADERS_CREST','ITEM_SYRUPY_APPLE','ITEM_UNREMARKABLE_TEACUP','ITEM_MASTERPIECE_TEACUP']:
 assert item_ids[name] in stock,name
press(1);waitfor(lambda:task('Task_BuyMenu')<16);frames(100)
assert rd(symbols['sMartInfo']+12,2)==len(stock)
snap('gla-evolution-supplies')
print('PASS native long evolution shop, regional items and no hidden legacy-badge filtering',flush=True)
# Research is a one-time delivery and actual evolved partner report, not a free form change.
state('supply-after-buy',1);press(2);frames(40)
walkto(6,11);walkto(6,9);walkto(5,9);walkto(5,8);press(32);press(1)
until(lambda:menu_count()==18);choose(0);until(lambda:menu_count()==18)
assert not rd(current()+v[138],1)
wr(current()+v[2],3,1)
choose(0);until(yesno);choose(0);until(lambda:menu_count()==18)
assert rd(current()+v[137],1)==1 and rd(current()+v[138],1)==1
assert rd(current()+v[114]+9,1)&1
mon=symbols['gParties']+3*v[5]
assert rd('gPartiesCount',1)==4 and call('GetMonData2',mon,v[10])==v[142]
assert call('GetMonData2',mon,v[11])==32
assert any(call('GetMonData2',mon,v[38]+i) for i in range(4))
choose(0);until(lambda:menu_count()==18);assert rd('gPartiesCount',1)==4
# Use native form/evolution data setter only as a reporting fixture.
ptr=symbols['gStringVar4'];wr(ptr,v[130],2);call('SetMonData',mon,v[10],ptr)
choose(0);until(lambda:menu_count()==18);assert rd(current()+v[137],1)==2
expected=bytes(rd(current()+i,1) for i in range(v[1]))
assert call('TrySavingData',0)==1
wr(current()+v[137],0,1);assert call('LoadGameSave',0)==1
assert bytes(rd(current()+i,1) for i in range(v[1]))==expected
call('GleNullClaim');assert rd('gSpecialVar_Result',2)==5 and rd('gPartiesCount',1)==4
press(2);frames(40)
# A raw LoadGameSave fixture replaces saved coordinates without rebuilding
# live object events. Re-enter the field before subsequent controller walking.
lib.call6(symbols['SetWarpDestination'],v[24]>>8,v[24]&255,-1,6,13,0)
call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
supply();choose(4);waitfor(lambda:task('Task_ShopMenu')<16)
assert listing()==list(range(v[134],v[134]+17))
press(1);waitfor(lambda:task('Task_BuyMenu')<16);frames(100);snap('gla-memory-shop')
print('PASS native Type: Null rank gate, one-time delivery, moves/level, Silvally report, flash persistence and Memory unlock',flush=True)
# Research-only ability upgrade, with an ordinary Slaking control.
state('supply-before-slaking')
wr(current()+v[2],4,1)
ptr=symbols['gStringVar4'];wr(ptr,v[143],2);call('SetMonData',mon,v[10],ptr)
wr(ptr,0,1);call('SetMonData',mon,v[12],ptr)
call('GleSlakingResearch');assert rd('gSpecialVar_Result',2)==0
assert call('GetMonData2',mon,v[12])==0
wr(current()+v[2],5,1)
ordinary=symbols['gParties']+2*v[5]
wr(ptr,v[144],2);call('SetMonData',ordinary,v[10],ptr)
wr(ptr,0,1);call('SetMonData',ordinary,v[12],ptr)
call('GleSlakingResearch');assert rd('gSpecialVar_Result',2)==1
assert call('GetMonData2',mon,v[12])==2
assert call('GetMonData2',ordinary,v[12])==0
assert rd(current()+v[137]+14,1)==2
call('GleSlakingResearch');assert rd('gSpecialVar_Result',2)==2
print('PASS special Slaking rank gate, one-time Vital Spirit awakening and ordinary Slaking isolation',flush=True)
# One-time placement checks both party and PC before committing any research state.
state('supply-before-slaking',1)
wr(current()+v[114]+9,0,1);wr(current()+v[137],0,1);wr(current()+v[138],0,1)
mondata=bytes(rd(mon+i,1) for i in range(v[5]))
for slot in range(6):
 for i,b in enumerate(mondata):wr(symbols['gParties']+slot*v[5]+i,b,1)
wr('gPartiesCount',6,1)
storage=rd('gPokemonStoragePtr')+v[34]
for slot in range(v[76]*v[77]):
 for i,b in enumerate(mondata[:v[33]]):wr(storage+slot*v[33]+i,b,1)
assert call('IsPlayerPartyAndPokemonStorageFull')
expected=bytes(rd(current()+i,1) for i in range(v[1]))
call('GleNullClaim');assert rd('gSpecialVar_Result',2)==2
assert bytes(rd(current()+i,1) for i in range(v[1]))==expected
for i in range(v[33]):wr(storage+i,0,1)
call('GleNullClaim');assert rd('gSpecialVar_Result',2)==0
assert call('GetBoxMonData2',storage,v[10])==v[142]
assert rd(current()+v[114]+9,1)&1
print('PASS full storage preserves research placement; freed PC slot receives Type: Null exactly once',flush=True)
