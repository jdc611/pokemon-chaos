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
  if 230<=local<240:result[local-230]=(rd(a+st[4],2),rd(a+st[5],2))
 return result
warp(7,12);press(16);press(1);until(lambda:menu_count()==8);choose(6);finishmenu()
def staffmenu():choose(6);until(lambda:menu_count()==6)
def station(staff,place):
 staffmenu();choose(staff);until(lambda:menu_count()==7);choose(place);finishmenu()
before=money();station(4,1);assert active(4)==2 and call('GleTrainerStation',current(),4)==0 and money()==before
snap('staff-station-saved')
station(0,1);assert active(0)==1 and active(4)==2 and money()==before
staffmenu();press(2);finishmenu();assert active(4)==2
staffmenu();choose(4);until(lambda:menu_count()==7);press(2);finishmenu();assert active(4)==2
print('PASS controller staff/station menus, conflict refusal, both cancellations and no fees',flush=True)
press(2);frames(40);press(2);settle();warp(7,12)
assert locations()[4]==(2,5) and locations()[0]==(10,5) and len(set(locations().values()))==5
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
warp(7,12);assert active(4)==2 and locations()[4]==(2,5)
assert call('GleAssignTrainer',current(),4,1)==0 and active(4)==2
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
  assert call('GetObjectEventIdByLocalId',230+i)<16
snap('staff-reserved-stations-all-actors')
print('PASS all five individual positions in all four layouts with puzzles/trophies and visible native actors',flush=True)
# A departure clears active placement in normal gameplay. Emulate the away flag
# separately to verify defensive rendering excludes stale encodings as well.
wr(recruit(4)+st[3],2,1);warp(5,5);assert 4 not in locations()
wr(recruit(4)+st[3],0,1);assert call('GleAssignTrainer',current(),4,0)==0
assert call('GleAssignTrainer',current(),4,1)==0 and active(4)==1
warp(5,5);assert len(set(locations().values()))==5
wr('gSpecialVar_LastTalked',234,2);call('GleStaffSelect');assert rd('gSpecialVar_Result',2)==0
call('GleStaffDismiss');warp(5,5);assert 4 not in locations() and rd(recruit(4),2)==0
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
warp(5,5);assert 4 not in locations()
print('PASS away exclusion, reserve/reactivation automatic reset, dismissal and flash without returning actor',flush=True)
print('PASS NATIVE STAFF STATIONS COMPLETE',flush=True)
