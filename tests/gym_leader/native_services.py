"""Native GLA voucher preview/claim and repeatable training, after real defense.
Rank XP and voucher awards below are explicit progression fixtures.
"""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_defense.py');globals().update(ctx)
choose(6);frames(60)
call('GleWarpLeague');frames(240)
# The League guide stands at (8,12), reached from the south.
frames(32,16);frames(64,128);frames(20);press(1)
until(lambda:menu_count()==5)
state('services-base')
# A real practice battle must restore the party without official progress/pay.
money=call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
xp=rd(current()+v[31]);q=rd(current()+v[32],1)
wins=rd(current()+v[29],2);losses=rd(current()+v[30],2);activity=rd(current()+v[56])
choose(2);until(lambda:menu_count()==5);choose(0)
frames(320);assert call('GymLeaderTrainingActive')
snap('gla-training-battle')
for _ in range(1200):
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseMove']|1 and rd('gMoveSelectionCursor',1)==0:press(16,18)
 press(1,18)
 if not call('GymLeaderBattleTrainer'):break
else:raise AssertionError('training did not return')
until(lambda:menu_count()==5)
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==money
assert rd(current()+v[31])==xp and rd(current()+v[32],1)==q
assert rd(current()+v[29],2)==wins and rd(current()+v[30],2)==losses
assert rd(current()+v[56])==activity
assert rd('gPartiesCount',1)==3
for i in range(3):assert rd(symbols['gParties']+i*v[5]+v[13],2)==rd(symbols['gParties']+i*v[5]+v[14],2)
print('PASS actual practice battle, native return/healing, no money/official XP/queue/record/activity farming',flush=True)
# Earn rank two, collect its one-time reward from the counter.
call('GleAddRankXp',current(),100)
choose(1);until(lambda:menu_count()==5)
assert rd(current()+v[66],1)==1
choose(1);until(lambda:menu_count()==5)
assert rd(current()+v[66],1)==1,'duplicate milestone voucher'
choose(0);until(lambda:menu_count()==1);choose(0)
until(lambda:menu_count()==3)
# Save the account after the choices lock. Preview data remains deterministic.
voucher=bytes(rd(current()+v[67]+i,1) for i in range(v[68]))
preview=bytes(rd(symbols['sVoucherPreview']+i,1) for i in range(3*v[5]))
expected=bytes(rd(current()+i,1) for i in range(v[1]))
assert call('TrySavingData',0)==1
wr(current()+v[67],0,2)
assert call('LoadGameSave',0)==1
assert bytes(rd(current()+i,1) for i in range(v[1]))==expected
choose(0);until(lambda:menu_count()==3);choose(1);frames(150)
snap('gla-voucher-summary')
assert call('GetMonData2',symbols['sVoucherPreview'],v[11])==22
assert call('GetMonData2',symbols['sVoucherPreview'],v[70])==28
press(2,120);until(lambda:menu_count()==3)
choose(2);until(lambda:menu_count()==3)
press(2);until(lambda:menu_count()==5)
choose(0);until(lambda:menu_count()==1);choose(0);until(lambda:menu_count()==3)
assert bytes(rd(current()+v[67]+i,1) for i in range(v[68]))==voucher
assert bytes(rd(symbols['sVoucherPreview']+i,1) for i in range(3*v[5]))==preview
species=call('GetMonData2',symbols['sVoucherPreview'],v[10])
choose(0);until(lambda:menu_count()==3);choose(0);until(yesno);choose(0)
until(lambda:menu_count()==5)
assert rd(current()+v[66],1)==0
assert rd('gPartiesCount',1)==4
mon=symbols['gParties']+3*v[5]
assert call('GetMonData2',mon,v[10])==species
assert call('GetMonData2',mon,v[70])==28
assert call('GleSpeciesClaimed',current(),species,0)
snap('gla-voucher-claimed')
# Guaranteed shiny vouchers preserve the native shiny flag in all choices.
assert call('GleAwardVoucher',current(),3,2)==0
wr('gSpecialVar_Result',0,2);call('GleVoucherOpen')
assert rd('gSpecialVar_Result',2)==0
for i in range(3):assert call('GetMonData2',symbols['sVoucherPreview']+i*v[5],v[71])==1
# Original save migration preserves identities, town and opened vouchers.
wr(current()+20,4,2);call('GymLeaderState')
assert rd(current()+20,2)==5 and rd(current()+v[26],1)==1
assert rd(current()+v[66],1)==1
print('PASS one-time milestones, persistent choices, native Summary, GLA Ball delivery/claim, shiny guarantees and v4 migration',flush=True)

# Both storage destinations full: no delivery and no consumed account reward.
state('services-before-full')
mondata=bytes(rd(symbols['sVoucherPreview']+i,1) for i in range(v[5]))
for slot in range(6):
 for i,b in enumerate(mondata):wr(symbols['gParties']+slot*v[5]+i,b,1)
wr('gPartiesCount',6,1)
storage=rd('gPokemonStoragePtr')+v[34]
for slot in range(v[76]*v[77]):
 for i,b in enumerate(mondata[:v[33]]):wr(storage+slot*v[33]+i,b,1)
assert call('IsPlayerPartyAndPokemonStorageFull')
wr('gSpecialVar_Result',0,2);call('GleVoucherOpen')
expected=bytes(rd(current()+i,1) for i in range(v[1]))
call('VarSet',0x8007,0);call('GleVoucherClaim')
assert rd('gSpecialVar_Result',2)==2
assert bytes(rd(current()+i,1) for i in range(v[1]))==expected
# Free exactly one PC slot. The full party forces native delivery into it.
for i in range(v[33]):wr(storage+i,0,1)
call('GleVoucherClaim');assert rd('gSpecialVar_Result',2)==0
assert call('GetBoxMonData2',storage,v[10])>0
assert call('GetBoxMonData2',storage,v[70])==28
assert call('GetBoxMonData2',storage,v[71])==1
print('PASS full party/PC preserves voucher; native PC delivery retains GLA Ball and guaranteed shiny',flush=True)
