"""Actual Safari restoration, native owner menus, transaction/save and wild hook."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_landmark.py');globals().update(ctx)
assert town==2
# The landmark route ends outside. Re-enter and approach the lower attendant.
frames(100,64);frames(180)
assert mapid()==v[75]
def walkto(x,y):
 for _ in range(100):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('Safari walking blocked',pos(),(x,y)))
walkto(4,6);walkto(3,6);press(32);press(1)
until(lambda:menu_count()==3)
snap('safari-owner-slots')
choose(0);until(lambda:menu_count()==18)
choose(4);until(lambda:menu_count()>18)
snap('safari-owner-species')
# Select the first eligible Grass species from the native list.
pool=rd('sSafariPool');species=rd(pool,2);assert species and not call('GymLeaderSpeciesReserved',species)
money=call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
choose(0);until(yesno);snap('safari-settlement-confirm');choose(0)
until(lambda:menu_count()==3)
assert rd(current()+v[105],2)==species
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])<money
assert not rd('sSafariPool'),'species picker leaked its pool'
# Cancel a species list, including native B sentinel 127, without buying it.
choose(1);until(lambda:menu_count()==18);choose(4);until(lambda:menu_count()>18)
press(2);until(lambda:menu_count()==3)
assert not rd('sSafariPool')
assert rd(current()+v[105]+2,2)==0
press(2);frames(80)
assert not call('ScriptContext_IsEnabled')
# Save the configured habitat and reload its real flash state.
expected=bytes(rd(current()+i,1) for i in range(v[1]))
assert call('TrySavingData',0)==1
wr(current()+v[105],0,2);assert call('LoadGameSave',0)==1
assert bytes(rd(current()+i,1) for i in range(v[1]))==expected
# Duplicate placements preserve wildlife and money.
call('VarSet',0x8008,1);call('VarSet',0x8006,species);call('GleSafariConfigure')
assert rd('gSpecialVar_Result',2)==5
# All newly supported migrations preserve prior rewards and opened voucher level.
assert call('GleAwardVoucher',current(),0,3)==0
wr('gSpecialVar_Result',0,2);call('GleVoucherOpen')
voucher=bytes(rd(current()+v[67]+i,1) for i in range(v[68]))
mask=rd(current()+v[69],1)
wr(current()+v[78],5,2);call('GymLeaderState')
assert rd(current()+v[69],1)==(mask&254) and rd(current()+v[78],2)==v[100]
assert rd(current()+v[105],2)==0
assert bytes(rd(current()+v[67]+i,1) for i in range(v[68]))==voucher
wr(current()+v[105],species,2)
# Every managed slot is used by the ordinary wild generator in a Safari map.
lib.call6(symbols['SetWarpDestination'],v[106]>>8,v[106]&255,-1,26,30,0)
call('DoWarp');frames(250)
assert call('GymLeaderSafariSpecies',0)==species
assert call('GymLeaderSafariSpecies',3)==species
assert call('GymLeaderSafariSpecies',1)==0
# Test the actual wild-generation adapter with the native land table. Direct call
# is an encounter fixture, not a claim of walking every Safari grass tile.
header=call('GetCurrentMapWildMonHeaderId');assert header!=65535
scratch=call('Alloc_',4096,0)
n=lib.call6(symbols['GymLeaderBuildSpeciesPool'],4,3,0,0,scratch,2048)
selected=[rd(scratch+i*2,2) for i in range(4)];call('Free',scratch)
# A distinct eligible species is refused when the owner has no money.
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],0)
call('VarSet',0x8008,1);call('VarSet',0x8006,selected[3]);call('GleSafariConfigure')
assert rd('gSpecialVar_Result',2)==4 and rd(current()+v[105]+2,2)==0
selected=selected[:3]
assert len(set(selected))==3
for i,sp in enumerate(selected):wr(current()+v[105]+i*2,sp,2)
tod=call('GetTimeOfDayForEncounters',header,0)
info=rd(symbols['gWildMonHeaders']+header*v[107]+v[108]+tod*v[109])
assert info
for _ in range(12):
 assert call('TryGenerateWildMon',info,0,0)
 enemy=symbols['gParties']+6*v[5]
 assert call('GetMonData2',enemy,v[10]) in selected
 assert call('GetMonData2',enemy,v[11])<=call('GymLeaderLevelCap')
# Never allow the native B sentinel/error value to collide with a valid list ID.
counts=[call('GymLeaderPoolCount',ty,8,0,0) for ty in range(18)]
assert max(counts)<254,counts
print('PASS native owner menus/purchase/cancel, flash persistence, duplicate safety, v5 migration and actual Safari wild generation with rank-scaled levels',flush=True)
