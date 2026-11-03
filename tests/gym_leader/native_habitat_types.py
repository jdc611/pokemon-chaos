"""Native habitat theme choice/cancel/purchase/upgrade, all pools and a wild battle."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
def mapid():
 a=rd('gSaveBlock1Ptr')+v[25];return rd(a+1,1)|(rd(a,1)<<8)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(160):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('walk blocked',pos(),(x,y)))
call('GleWarpHome');frames(240)
for _ in range(300):
 frames(1,128)
 if mapid()==v[58+town]:break
else:raise AssertionError('Gym exit failed')
frames(150)
px=13 if town==0 else 3
walkto(px+4,30);walkto(px+5,30);press(64)
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],50000)
plot=current()+v[52]+12*v[53]
press(1);until(lambda:menu_count()==11)
choose(8);until(yesno);choose(0);until(lambda:menu_count()==18)
snap('habitat-type-choice')
press(2)
for _ in range(30):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
assert not rd(plot,1) and call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==50000
press(1);until(lambda:menu_count()==11)
choose(8);until(yesno);choose(0);until(lambda:menu_count()==18)
choose(15) # DARK habitat in a GRASS Gym's town.
until(lambda:rd(plot,1)==9)
for _ in range(30):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
assert rd(plot+3,1)==15 and rd(plot+1,1)==1
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==47200
# A paid native upgrade must retain the selected type, without another theme menu.
wr(current()+v[2],2,1);wr(current()+v[54],24,2)
press(1);until(lambda:menu_count()==2)
choose(0);until(yesno);choose(0)
until(lambda:rd(plot+1,1)==2)
for _ in range(30):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
assert rd(plot+3,1)==15
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==36000
assert call('TrySavingData',0)==1
wr(plot+3,4,1);assert call('LoadGameSave',0)==1
assert rd(plot+3,1)==15 and rd(plot+1,1)==2
print('PASS native habitat B-cancel without charge, off-Gym type purchase, paid upgrade/theme preservation and flash reload',flush=True)
# Spawn fixture inside this habitat; all selector samples must belong to its
# selected-type catalog, independent of the Gym type and excluded rare species.
lib.call6(symbols['SetWarpDestination'],v[58+town]>>8,v[58+town]&255,-1,px+2,26,0)
call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
assert call('GymLeaderHabitatActive')
plot=current()+v[52]+12*v[53]
state('habitat-before-pools')
pool=call('Alloc_',4096,0);assert pool
for theme in range(18):
 wr(plot+3,theme,1)
 count=lib.call6(symbols['GymLeaderBuildSpeciesPool'],theme,3,0,0,pool,2048)
 allowed={rd(pool+i*2,2) for i in range(count)}
 assert allowed
 for i in range(12):
  species=call('GymLeaderHabitatSpecies',i)
  assert species in allowed,(theme,species)
  assert not call('GymLeaderSpeciesReserved',species)
call('Free',pool)
state('habitat-before-pools',1)
wr(current()+v[2],1,1)
enemy=symbols['gParties']+6*v[5]
for _ in range(160):
 frames(16,16);frames(16,32)
 if call('GetMonData2',enemy,v[10]):break
else:raise AssertionError('walking in DARK habitat failed to encounter')
frames(400)
pool=call('Alloc_',4096,0)
count=lib.call6(symbols['GymLeaderBuildSpeciesPool'],15,2,0,0,pool,2048)
assert call('GetMonData2',enemy,v[10]) in {rd(pool+i*2,2) for i in range(count)}
call('Free',pool)
assert not call('GymLeaderSpeciesReserved',call('GetMonData2',enemy,v[10]))
snap('off-type-habitat-wild')
print('PASS all 18 chosen-type habitat catalogs and actual off-type native walking encounter',flush=True)
