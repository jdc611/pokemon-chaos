"""Native deterministic postgame directory, Summary and placement transactions."""
import runpy,struct,json,hashlib
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
data=(ROOT/'catalogue-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(data)//4),data)
def result():return rd('gSpecialVar_Result',2)
def money():return call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
def setmoney(n):call('SetMoney',rd('gSaveBlock1Ptr')+v[64],n)
def claimed(s,shiny=False):return bool(rd(current()+c[1 if shiny else 0]+s//8,1)&(1<<(s%8)))
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def pool():return [rd(rd('sCatalogPool')+i*2,2) for i in range(rd('sCatalogCount',2))]
def species_menu(ty):
 choose(ty);until(lambda:rd('sCatalogCount',2)>0 and menu_count()==rd('sCatalogCount',2));return pool()
def pick(index):choose(index);until(lambda:menu_count()==3)
def settle():
 for _ in range(100):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('directory did not release')
warp(v[24],4,9);call('GleCatalogStatus');assert result()==0
call('GleCatalogClaim');assert result()==3
wr(current()+v[114]+27,rd(current()+v[114]+27,1)|128,1)
press(64);press(1);until(lambda:menu_count()==18);choose(17);until(lambda:menu_count()==18)
water=species_menu(2);assert c[5] in water;pick(water.index(c[5]))
preview=symbols['gGlePreviewParty'];pid=call('GetMonData2',preview,v[36])
assert call('GetMonData2',preview,v[10])==c[5] and call('GetMonData2',preview,v[11])==5 and call('GetMonData2',preview,v[12])==0
snap('placement-directory-offer');choose(0);frames(180);snap('placement-directory-native-summary');press(2,120);until(lambda:menu_count()==3)
assert call('GetMonData2',preview,v[36])==pid
setmoney(0);choose(1);until(yesno);choose(0);until(lambda:menu_count()==3);assert not claimed(c[5]) and money()==0
setmoney(100000);choose(1);until(yesno);choose(1);until(lambda:menu_count()==3);assert not claimed(c[5]) and money()==100000
choose(1);until(yesno);choose(0);until(lambda:menu_count()==18)
bst=sum(rd(symbols['gSpeciesInfo']+c[5]*v[194]+i,1) for i in range(6));assert money()==100000-1000-10*bst and claimed(c[5]) and not claimed(c[5],True)
mon=symbols['gParties']+3*v[5];assert rd('gPartiesCount',1)==4 and call('GetMonData2',mon,v[10])==c[5] and call('GetMonData2',mon,v[36])==pid
print('PASS ending gate, cross-type native placement/Summary, stable identity, no-funds retry, explicit decline, paid delivery and normal-only ledger',flush=True)
# Browse every actual type list; all entries are deterministic and reservation-safe.
directory={c[5]};types=[];long_choice=False
for ty in range(18):
 choices=species_menu(ty);assert len(choices)==len(set(choices)) and choices
 for species in choices:
  assert not call('GymLeaderSpeciesReserved',species)
  actual=rd(symbols['sGleTypes']+ty,1);ptr=symbols['gSpeciesInfo']+species*v[194]+v[193]
  assert actual in [rd(ptr,1),rd(ptr+1,1)]
 directory.update(choices);types.append({'type':ty,'count':len(choices),'species':choices})
 if len(choices)>127 and not long_choice:
  pick(127);assert rd('sCatalogSpecies',2)==choices[127];snap('placement-directory-long-selection');press(2);until(lambda:menu_count()==18);long_choice=True
 else:press(2);until(lambda:menu_count()==18)
 assert not rd('sCatalogPool') and not rd('sCatalogSpecies',2)
(ROOT/'directory-access.json').write_text(json.dumps({'rom_sha256':hashlib.sha256((repo/'pokefirered.gba').read_bytes()).hexdigest(),'ending_fixture':True,'unique_species_count':len(directory),'species':sorted(directory),'types':types},indent=2)+'\n')
print('PASS all 18 native type/species lists, deterministic explicit choices, unique exclusions, long-list cancel-sentinel decoding and cleanup',len(directory),[t['count'] for t in types],flush=True)
# A normal open voucher keeps its promised partner. Declining the directory
# cannot consume it, and direct placement cannot steal it.
choices=species_menu(2);pick(0);species=choices[0];voucher=current()+v[67]
wr(current()+v[66],1,1);wr(voucher+c[2],1,1);wr(voucher+c[3],0,1);wr(voucher+c[4],1,1);wr(voucher+c[7],species,2)
funds=money();call('GleCatalogClaim');assert result()==3 and money()==funds and not claimed(species)
wr(voucher+c[2],0,1);call('GleCatalogClaim');assert result()==0 and claimed(species)
funds=money();call('GleCatalogClaim');assert result()==5 and money()==funds
press(2);until(lambda:menu_count()==18)
# The reverse path shares the same claim: a core voucher ledger fixture blocks
# a directory transaction without making up a second Pokemon delivery.
choices=species_menu(2);pick(0);species=choices[0]
wr(current()+v[66],1,1);wr(voucher+c[2],1,1);wr(voucher+c[3],0,1);wr(voucher+c[4],1,1);wr(voucher+c[7],species,2)
assert call('GleClaimVoucher',current(),0,0,symbols['gStringVar1'])==0
funds=money();call('GleCatalogClaim');assert result()==5 and money()==funds
press(2);until(lambda:menu_count()==18)
print('PASS viewed-voucher reservation, directory anti-repeat, shared normal voucher ledger and unchanged shiny claims',flush=True)
# Full party/PC refusal preserves cash and claims; freeing one slot delivers.
choices=species_menu(2);pick(0);species=choices[0];setmoney(100000)
clone=bytes(rd(symbols['gParties']+i,1) for i in range(v[5]))
for slot in range(6):
 for i,b in enumerate(clone):wr(symbols['gParties']+slot*v[5]+i,b,1)
wr('gPartiesCount',6,1);storage=rd('gPokemonStoragePtr')+v[34]
for slot in range(v[76]*v[77]):
 for i,b in enumerate(clone[:v[33]]):wr(storage+slot*v[33]+i,b,1)
assert call('IsPlayerPartyAndPokemonStorageFull')
funds=money();call('GleCatalogClaim');assert result()==2 and money()==funds and not claimed(species)
for i in range(v[33]):wr(storage+i,0,1)
call('GleCatalogClaim');assert result()==0 and money()<funds and claimed(species) and call('GetBoxMonData2',storage,v[10])==species
press(2);until(lambda:menu_count()==18);press(2);until(lambda:menu_count()==18);press(2);settle()
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and claimed(species) and claimed(c[5])
print('PASS full storage, freed-PC transaction, native flash and field exit; deterministic postgame catalogue complete',flush=True)
