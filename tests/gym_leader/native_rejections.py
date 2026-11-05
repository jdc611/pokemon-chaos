"""Rejected async specials must resume native scripts instead of softlocking."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
# Empty voucher account, from the actual League guide.
call('GleWarpLeague');frames(240)
frames(32,16);frames(64,128);frames(20);press(1)
until(lambda:menu_count()==5)
choose(0);until(lambda:menu_count()==5)
assert rd(current()+v[66],1)==0
# Expert practice is rank-locked; after refusal return to normal services.
choose(2);until(lambda:menu_count()==5);choose(3)
until(lambda:menu_count()==5)
assert not call('GymLeaderBattleTrainer')
press(2);frames(50)
assert not call('ScriptContext_IsEnabled')
# Actual official battle refusal with no registered team.
call('GleWarpHome');frames(240)
wr(current()+v[26],0,1)
frames(32,16);frames(64,128);frames(20);press(1)
until(lambda:menu_count()==8)
q=rd(current()+v[32],1);wins=rd(current()+v[29],2);loss=rd(current()+v[30],2)
choose(0);until(yesno);choose(0)
until(lambda:menu_count()==8)
assert rd(current()+v[32],1)==q
assert rd(current()+v[29],2)==wins and rd(current()+v[30],2)==loss
assert not call('GymLeaderBattleTrainer')
press(2);frames(50);assert not call('ScriptContext_IsEnabled')
print('PASS actual empty vouchers, locked practice, invalid registered-team defense and field control after refusal',flush=True)
