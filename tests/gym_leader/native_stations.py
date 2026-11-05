"""Controller station menus, reserved floor positions and native flash.
Uses explicit rank/staff fixtures from the design regression; not a campaign.
"""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_gym_design.py');globals().update(ctx)
b=(ROOT/'stations-layout.bin').read_bytes();st=struct.unpack('<'+'I'*(len(b)//4),b)
def recruit(i):return current()+st[0]+i*st[1]
def active(i):return rd(recruit(i)+st[2],1)
def locations():
 events=rd(symbols['gMapHeader']+v[157]);objects=rd(events+v[158]);result={}
 for i in range(rd(events,1)):
  a=objects+i*v[116];local=rd(a,1)
  if st[6]<=local<st[6]+10:result[local-st[6]]=(rd(a+st[4],2),rd(a+st[5],2))
 return result
warp(7,12);press(16);press(1);until(lambda:menu_count()==8);choose(6);finishmenu()
def staffmenu():choose(6);until(lambda:menu_count()==6)
def station(staff,place):
 staffmenu();choose(staff);until(lambda:menu_count()==7);snap('staff-station-choices');choose(place);finishmenu()
before=money();station(4,1);assert active(4)==2 and call('GleTrainerStation',current(),4)==0 and money()==before
snap('staff-station-saved')
station(0,1);assert active(0)==1 and active(4)==2 and money()==before
staffmenu();press(2);finishmenu();assert active(4)==2
staffmenu();choose(4);until(lambda:menu_count()==7);press(2);finishmenu();assert active(4)==2
print('PASS controller staff/station menus, conflict refusal, both cancellations and no fees',flush=True)
press(2);frames(40);press(2);settle();warp(7,12)
assert locations()[4]==(2,5) and locations()[0]==(10,5) and len(set(locations().values()))==5,(locations(),[active(i) for i in range(5)],st)
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
warp(7,12);assert active(4)==2 and locations()[4]==(2,5)
assert call('GleAssignTrainer',current(),4,1)==0 and active(4)==2
# An older v15 save has 230..239 actor IDs. Retain the actual save position
# and enter the real Continue callback without warping to repair the map.
for i in range(5):
 old=230+i;new=st[6]+i
 for j in range(64):
  a=rd('gSaveBlock1Ptr')+st[7]+j*v[116]
  if rd(a,1)==new:wr(a,old,1)
 for j in range(16):
  a=symbols['gObjectEvents']+j*st[9]+st[8]
  if rd(a,1)==new:wr(a,old,1)
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
call('CB2_ContinueSavedGame');frames(600)
assert locations()[4]==(2,5)
for i in [1,2]:
 assert call('GetObjectEventIdByLocalId',st[6]+i)<16
 assert call('GetObjectEventIdByLocalId',230+i)==16
for i in range(5):
 a=call('GetObjectEventIdByLocalId',st[6]+i)
 if a<16:
  wr('gSpecialVar_LastTalked',st[6]+i,2);call('GleStaffMemberSelect');assert rd('gSpecialVar_0x8007',2)==i
print('PASS legacy actor IDs survive real Continue without a repair warp',flush=True)
print('PASS native flash, reserved station before automatic staff and repeat assignment retention',flush=True)
for i in range(5):assert call('GlePositionTrainer',current(),i,0)==0
for i in range(5):assert call('GlePositionTrainer',current(),i,5-i)==0
safe=[(2,5),(10,5),(2,8),(10,8),(4,4)]
wr(scratch,100000)
for layout in range(4):
 assert call('GleGymDesignPurchase',current(),0,layout,scratch) in [0,5];warp(5,5)
 placed=locations();assert placed=={i:safe[4-i] for i in range(5)},placed
 assert len(set(placed.values()))==5
 for i,(x,y) in placed.items():
  assert not call('MapGridGetCollisionAt',x+7,y+7)
  assert call('GetObjectEventIdByLocalId',st[6]+i)<16
snap('staff-reserved-stations-all-actors')
print('PASS all five individual positions in all four layouts with puzzles/trophies and visible native actors',flush=True)
# A departure clears active placement in normal gameplay. Emulate the away flag
# separately to verify defensive rendering excludes stale encodings as well.
wr(recruit(4)+st[3],2,1);warp(5,5);assert 4 not in locations()
wr(recruit(4)+st[3],0,1);assert call('GleAssignTrainer',current(),4,0)==0
assert call('GleAssignTrainer',current(),4,1)==0 and active(4)==1
warp(5,5);assert len(set(locations().values()))==5
wr('gSpecialVar_LastTalked',st[6]+4,2);call('GleStaffMemberSelect');assert rd('gSpecialVar_Result',2)==0
call('GleStaffDismiss');warp(5,5);assert 4 not in locations() and rd(recruit(4),2)==0
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
warp(5,5);assert 4 not in locations()
# Fill the ten-record ledger and activate its last five records. Their actor
# IDs must remain distinct from all three trophies and puzzle consoles.
existing={rd(recruit(i),2) for i in range(10)}
sources=(repo/'src/data/gym_leader_recruit_sources.h').read_text()
added=0
for name in re.findall(r'\{(TRAINER_[A-Z0-9_]+),',sources):
 if any(word in name for word in ['TEAM_ROCKET','LEADER','ELITE','CHAMPION']):continue
 match=re.search(r'^#define '+name+r'\s+(\d+)',constants,re.M)
 if not match:continue
 trainer=int(match[1])
 if trainer in existing:continue
 assert call('GleRecruitTrainer',current(),trainer,3,80)==0
 existing.add(trainer);added+=1
 if added==6:break
assert added==6
for i in range(4):assert call('GleAssignTrainer',current(),i,0)==0
for i in range(5,10):assert call('GleAssignTrainer',current(),i,1)==0
warp(5,5);placed=locations();assert set(placed)==set(range(5,10)) and len(set(placed.values()))==5
for i in range(5,10):
 assert call('GetObjectEventIdByLocalId',st[6]+i)<16
 wr('gSpecialVar_LastTalked',st[6]+i,2);call('GleStaffMemberSelect');assert rd('gSpecialVar_0x8007',2)==i
for local in [236,237,238,245,246,247]:assert call('GetObjectEventIdByLocalId',local)<16
print('PASS late recruitment slots have distinct trainer, trophy and puzzle actors',flush=True)
print('PASS away exclusion, reserve/reactivation automatic reset, dismissal and flash without returning actor',flush=True)
print('PASS NATIVE STAFF STATIONS COMPLETE',flush=True)
