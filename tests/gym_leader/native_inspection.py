"""Inspector adapter gates plus native Gym Guide launch/loss/return.
Gate outcomes below are explicit transaction fixtures, not balance wins.
"""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_defense.py');globals().update(ctx)
state('inspection-base')
s=current()
assert call('GleAddRankXp',s,280)==6
assert rd(s+v[2],1)==2 and rd(s+v[46],1)==1
q=rd(s+v[32],1);losses=rd(s+v[30],2);xp=rd(s+v[31])
assert call('GymLeaderPrepareInspection')==1
first=bytes(rd(symbols['gParties']+6*v[5]+i,1) for i in range(3*v[5])) # opponent A follows the player party
assert rd('gPartiesCount',1)==1
wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense')
assert rd(current()+v[2],1)==2 and rd(current()+v[46],1)==1
assert rd(current()+v[30],2)==losses and rd(current()+v[31])==xp
assert rd(current()+v[32],1)==q
assert call('GymLeaderPrepareInspection')==1
second=bytes(rd(symbols['gParties']+6*v[5]+i,1) for i in range(3*v[5]))
assert first==second,'inspection retry changed its core team'
wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense')
assert rd(current()+v[2],1)==3 and call('GetCurrentLevelCap')==35
for target,cap in [(5,55),(7,80),(8,100)]:
 while not rd(current()+v[46],1):
  needed=call('GleXpNeeded',current())-rd(current()+v[31])
  assert needed>0
  call('GleAddRankXp',current(),needed)
 assert call('GymLeaderPrepareInspection')==1
 wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense')
 assert rd(current()+v[2],1)==target and call('GetCurrentLevelCap')==cap
assert call('GymLeaderPrepareInspection')==4
state('inspection-base',1)
assert call('GleAddRankXp',current(),280)==6
# Actual UI invocation and unassisted failure at level 5 against rank-2 Lorelei.
choose(4);until(yesno);choose(0)
frames(350);snap('inspection-lorelei-intro')
assert call('GymLeaderBattleTrainer')
for i in range(1000):
 press(1,18)
 if not call('GymLeaderBattleTrainer'):break
else:raise AssertionError('inspection did not return')
until(lambda:menu_count()==7)
assert rd(current()+v[2],1)==2 and rd(current()+v[46],1)==1
assert rd(current()+v[30],2)==losses
snap('inspection-retry-menu')
print('PASS all four promotion gates, cap changes, fixed retries, queue/XP preservation and native inspector launch/loss/return',flush=True)
