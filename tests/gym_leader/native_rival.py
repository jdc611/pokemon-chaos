"""Native rival friendly, cap/type/identity, one-time settlement and save QA.

Metadata loops and adapter outcomes below are explicit fixtures. The in-world
battle uses three supplied level-cap Torterra with Earthquake, not forced enemy
HP/outcomes; it tests integration rather than early-game difficulty.
"""
import runpy,re
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
assert gym_type==4, 'this actual-battle fixture supplies a Grass Gym team'
state('rival-base')
trainer=call('Alloc_',v[101],0);entries=call('Alloc_',1024,0)
assert trainer and entries
enemy=symbols['gParties']+6*v[5]
types=[1,11,12,14,13,16,2,4,5,3,15,7,6,8,17,18,9,19]
caps=[15,25,35,45,55,65,80,100];sizes=[3,3,4,4,5,5,6,6]
# All eighteen rival specialties; multiple ranks and demotion keep the ace's
# PID/OT while every actual enemy mon remains legal at the current cap.
for typ in range(18):
 wr(current()+v[195],typ,1)
 wr(current()+v[189],0,2)
 starter=None;identity=None
 for rank in [1,4,8,1]:
  wr(current()+v[2],rank,1)
  assert call('GymLeaderPrepareRivalRoster',trainer,entries)
  assert rd('gPartiesCount',1)==3
  assert rd(symbols['gPartiesCount']+1,1)==sizes[rank-1]
  if starter is None:starter=rd(current()+v[189],2)
  assert rd(current()+v[189],2)==starter
  ident=tuple(call('GetMonData2',enemy,f) for f in [v[36],v[37]])
  if identity is None:identity=ident
  assert ident==identity
  for i in range(sizes[rank-1]):
   mon=enemy+i*v[5];species=call('GetMonData2',mon,v[10])
   assert call('GetMonData2',mon,v[11])==caps[rank-1]
   info=symbols['gSpeciesInfo']+species*v[194]+v[193]
   assert types[typ] in [rd(info,1),rd(info+1,1)],(typ,rank,species)
   assert not call('GymLeaderSpeciesReserved',species)
   assert call('GetMonData2',mon,v[12])<2
   assert any(call('GetMonData2',mon,v[38]+j) for j in range(4))
call('Free',trainer);call('Free',entries)
state('rival-base',1)
print('PASS all 18 rival types, current caps/team sizes, normal abilities, reserved-species exclusion, persistent starter and ace identity through rank changes',flush=True)

def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def finish_text():
 for _ in range(120):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('rival did not return field control')
species_ids={n:int(i) for n,i in re.findall(r'\b(SPECIES_[A-Z0-9_]+) = (\d+)',(repo/'include/constants/species.h').read_text())}
move_ids={n:int(i) for n,i in re.findall(r'\b(MOVE_[A-Z0-9_]+) = (\d+)',(repo/'include/constants/moves.h').read_text())}
for i in range(3):
 mon=symbols['gParties']+i*v[5]
 call('CreateRandomMonWithIVs',mon,species_ids['SPECIES_TORTERRA'],15,31)
 call('SetMonMoveSlot',mon,move_ids['MOVE_EARTHQUAKE'],0)
 for j in [1,2,3]:call('SetMonMoveSlot',mon,0,j)
 wr(current()+v[27]+i*4,call('GetMonData2',mon,v[36]))
 wr(current()+v[28]+i*4,call('GetMonData2',mon,v[37]))
wr(current()+v[26],3,1);call('HealPlayerParty')
warp(v[181],6,5);press(64);press(1)
until(yesno);snap('rival-friendly-confirm')
choose(1);finish_text()
assert not rd(current()+v[192],1) and not call('GymLeaderBattleTrainer')
# An unregistered team is refused through the actual script without a lock.
wr(current()+v[26],0,1);press(1);until(yesno);choose(0);finish_text()
assert not call('GymLeaderBattleTrainer') and not rd(current()+v[192],1)
wr(current()+v[26],3,1)
before={k:rd(current()+v[k],n) for k,n in [(29,2),(30,2),(31,4),(32,1),(56,4)]}
money=call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
press(1);until(yesno);choose(0)
until(lambda:call('GymLeaderBattleTrainer')!=0);frames(250)
snap('rival-friendly-battle')
for _ in range(1600):
 if task('Task_HandleChooseMonInput')<16:
  # Forced replacement opens on the fainted lead; choose a living reserve.
  slot=rd(symbols['gPartyMenu']+9,1)
  if slot<3 and rd(symbols['gParties']+slot*v[5]+v[13],2)>0:
   press(1,25);press(1,25)
  else:
   press(16 if slot==0 else 128,20)
  continue
 press(1,18)
 if not call('GymLeaderBattleTrainer'):break
else:
 state('rival-battle-failure');snap('rival-battle-timeout');raise AssertionError('actual rival battle did not finish')
finish_text()
assert rd('gBattleOutcome',1)==1, ('actual rival result',rd('gBattleOutcome',1))
assert rd(current()+v[192],1)==1 and rd(current()+v[190],2)==1
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==money+500
for k,n in [(29,2),(30,2),(31,4),(32,1),(56,4)]:assert rd(current()+v[k],n)==before[k]
assert rd('gPartiesCount',1)==3
for i in range(3):
 mon=symbols['gParties']+i*v[5]
 assert call('GetMonData2',mon,v[10])==species_ids['SPECIES_TORTERRA']
 assert rd(mon+v[13],2)==rd(mon+v[14],2)
 assert call('GetMonData2',mon,v[36])==rd(current()+v[27]+4*i)
press(1);finish_text()
assert not call('GymLeaderBattleTrainer')
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==money+500
assert call('GymLeaderPrepareRivalFriendly')==4
snap('rival-friendly-completed')
print('PASS native decline/refusal, actual rival victory, one-time prize, no rank/queue/defense/activity changes, healed original party and repeat refusal',flush=True)

# Adapter-only failure fixture: a loss permits retry without changing official
# loss streak/rank/queue. Returning to a previously won rank never reopens it.
wr(current()+v[2],2,1)
before=bytes(rd(current()+i,1) for i in range(v[1]))
assert call('GymLeaderPrepareRivalFriendly')==1
assert rd('gPartiesCount',1)==3
wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense')
assert rd(current()+v[191],2)==1 and rd(current()+v[192],1)==1
for k,n in [(29,2),(30,2),(31,4),(32,1),(56,4)]:
 assert bytes(rd(current()+v[k]+i,1) for i in range(n))==before[v[k]:v[k]+n]
assert call('GymLeaderPrepareRivalFriendly')==1
wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense')
assert rd(current()+v[192],1)==3
wr(current()+v[2],1,1)
assert call('GymLeaderPrepareRivalFriendly')==4
# Version-seven migration preserves every previous byte, including permanent
# tutors, Safari selections, staff and exclusive captures; new fields start0.
wr(current()+v[164],0x15555)
wr(current()+v[78],7,2)
expected=bytearray(rd(current()+i,1) for i in range(v[189]))
expected[v[78]:v[78]+2]=v[100].to_bytes(2,'little')
call('GymLeaderState')
assert bytes(rd(current()+i,1) for i in range(v[189]))==expected
assert not rd(current()+v[189],2) and not rd(current()+v[192],1)
wr(current()+v[192],3,1);wr(current()+v[190],2,2)
assert call('TrySavingData',0)==1
expected=bytes(rd(current()+i,1) for i in range(v[1]))
for i in range(v[1]):wr(current()+i,0,1)
assert call('LoadGameSave',0)==1
assert bytes(rd(current()+i,1) for i in range(v[1]))==expected
assert call('GymLeaderPrepareRivalFriendly')==4
print('PASS loss/retry and demotion guards, exact v7 data preservation, tutor license retention and native flash anti-refarm persistence',flush=True)

# Real Indigo lobby guard and every legacy ending-room warp remain closed to
# this career at low/high rank, even with a stale inherited Champion flag.
state('rival-before-league-guard')
for rank in [1,8]:
 wr(current()+v[2],rank,1)
 if rank==8:call('FlagSet',v[203])
 warp(v[196],4,3);frames(40,64);frames(30)
 location=rd('gSaveBlock1Ptr')+v[25]
 assert rd(location,1)==v[196]>>8 and rd(location+1,1)==v[196]&255
 assert (rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2))==(4,3)
 press(1);frames(35);snap('league-career-inspections-rank'+str(rank));finish_text()
 for destination in v[197:203]:
  warp(destination,4,3)
  location=rd('gSaveBlock1Ptr')+v[25]
  assert rd(location,1)==v[196]>>8 and rd(location+1,1)==v[196]&255
state('rival-before-league-guard',1)
print('PASS native Indigo guard/dialogue and legacy E4/Champion/Hall-of-Fame warp protection at ranks 1/8 with stale story-flag fixture',flush=True)
