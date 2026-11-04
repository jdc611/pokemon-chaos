"""Automatic homecoming scenes, retained recognition and read-only Kanto news.

Titles, population, Honor and the Rocket incident are progression fixtures.
Celebrations and the Gazette are exercised through actual map/controller UI.
"""
import runpy,struct
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
data=(ROOT/'news-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(data)//4),data)
def result():return rd('gSpecialVar_Result',2)
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def settle():
 for n in range(250):
  if not call('ScriptContext_IsEnabled'):return n
  press(1,20)
 snap('champion-scene-timeout');raise AssertionError('championship scene did not release')
def seen():return call('GymLeaderCelebratedTitles')
def object_exists(i):return call('GetObjectEventIdByLocalId',i)<16
def untouched():return (call('GetMoney',rd('gSaveBlock1Ptr')+v[64]),rd(current()+c[3],2),rd(current()+c[4],2),rd(current()+v[31]),rd(current()+v[29],2),rd(current()+v[30],2))
def text():
 out=''
 for i in range(1024):
  ch=rd(symbols['gStringVar4']+i,1)
  if ch==255:break
  out+=chr(65+ch-0xbb) if 0xbb<=ch<=0xd4 else chr(97+ch-0xd5) if 0xd5<=ch<=0xee else chr(48+ch-0xa1) if 0xa1<=ch<=0xaa else ' '
 return out
wr(current()+v[54],24,2);wr(current()+v[56],800)
assert call('GleRecruitTrainer',current(),c[6],3,60)==0 and call('GleAssignTrainer',current(),0,1)==0
wr(current()+c[0],1,2);wr(current()+c[2]+c[1],0,1)
baseline=untouched();warp(v[58+town],10,10)
assert call('ScriptContext_IsEnabled') and seen()==0
assert all(object_exists(i) for i in [245,246,247,248])
snap('champion-first-homecoming');first=settle()
assert seen()==1 and untouched()==baseline and not object_exists(245) and not object_exists(246)
assert object_exists(247) and object_exists(248)
assert not call('MapGridGetCollisionAt',17,16) # Gym door's immediate approach.
warp(v[58+town],10,10);assert not call('ScriptContext_IsEnabled') and seen()==1
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
warp(v[58+town],10,10);assert seen()==1 and not call('ScriptContext_IsEnabled')
wr(current()+c[0],2,2);warp(v[58+town],10,10);snap('champion-short-homecoming');short=settle()
assert seen()==2 and short<first and untouched()==baseline
print('PASS town',town,'automatic first/shorter repeat celebration, active staff, native memorials, door access, no rewards, no repeat and flash',first,short,flush=True)
# Security takes priority; clearing a persistent incident releases the pending
# celebration without requiring another title or silently consuming it.
wr(current()+c[0],3,2);wr(current()+c[5],1,1);warp(v[58+town],10,10)
assert not call('ScriptContext_IsEnabled') and seen()==2 and not call('GymLeaderCelebrationPending')
wr(current()+c[5],0,1);call('GleRocketRefresh');call('GleTownArrival');frames(120);assert call('ScriptContext_IsEnabled');settle();assert seen()==3
wr(current()+c[0],4,2);warp(v[24],7,9);assert not call('GymLeaderCelebrationPending');call('GleCelebrationFinish');assert seen()==3
warp(v[58+town],10,10);settle();assert seen()==4
print('PASS Rocket postponement and resolution, home-only trigger and independent next-title recognition',flush=True)
call('GleRecognitionInfo');assert result()==1 and 'reigning' in text()
wr(current()+c[2]+c[1],1,1);call('GleRecognitionInfo');assert 'former' in text()
for honor,word in [(100,'admire'),(0,'achievements'),(-100,'wary')]:
 wr(current()+c[4],honor&65535,2);call('GleRecognitionInfo');assert word in text()
originals=['PalletTown_EventScript_FatMan','ViridianCity_EventScript_Boy','PewterCity_EventScript_Lass','CeruleanCity_EventScript_LittleBoy','VermilionCity_EventScript_OldMan1','LavenderTown_EventScript_Boy','SaffronCity_EventScript_Man','CeladonCity_EventScript_LittleGirl','FuchsiaCity_EventScript_OldMan','CinnabarIsland_EventScript_Woman']
for original in originals:assert call('GymLeaderChampionNpcScript',symbols[original])==symbols['GleChampion_NewsNpc']
for original in ['PalletTown_EventScript_SignLady','ViridianCity_EventScript_DreamEaterTutor','EventScript_ChaosOakMegaReward']:
 assert call('GymLeaderChampionNpcScript',symbols[original])==symbols[original]
# Inspect the actual loaded templates throughout Kanto. Rebuilt player/rival
# towns use their own resident recognition instead of discarded vanilla NPCs.
for m in c[10:20]:
 warp(m,10,10);events=rd(symbols['gMapHeader']+v[157]);ptr=rd(events+v[158]);scripts=[rd(ptr+i*v[116]+c[7]) for i in range(rd(events,1))]
 expected=symbols['GleTownResident'] if m in [v[58+town],v[58+rd(current()+c[20],1)]] else symbols['GleChampion_NewsNpc']
 assert expected in scripts,('Kanto recognition template missing',hex(m))
print('PASS reigning/former/Honor-aware recognition, ten Kanto town/city template integrations and preserved tutor/tool/Oak reward scripts',flush=True)
warp(v[24],7,9);press(64);press(1);until(yesno);choose(0);until(lambda:menu_count()==6)
snapshot=bytes(rd(current()+i,1) for i in range(v[1]))
for page in range(5):
 choose(page);frames(80);snap('kanto-gazette-page-'+str(page));until(lambda:menu_count()==6)
 assert bytes(rd(current()+i,1) for i in range(v[1]))==snapshot
press(2);settle();assert not call('ScriptContext_IsEnabled')
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and seen()==4
print('PASS actual GLA Gazette menu, five current-state news pages, no career mutations or rewards, native cancel and flash; champion recognition complete',flush=True)
