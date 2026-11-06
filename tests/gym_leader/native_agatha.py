"""Earn the third inspection through controller actions, including native PC use."""
import os,runpy,json
from pathlib import Path
os.environ['GLE_CAREER_HELPERS_ONLY']='1'
globals().update(runpy.run_path('tests/gym_leader/native_career.py'))
assert rank()==6
READ_ONLY.update({'MapGridGetMetatileBehaviorAt','MetatileBehavior_IsPC'})
src=Path('tests/gym_leader/native_career_town.py').read_text();exec(src[src.index('def deposit_reserve('):src.index("if not os.environ.get('GLE_CAREER_TOWN_HELPERS_ONLY')")])
deposit_reserve(2,1334);new=claim_reward((move_meta[12],c[29],c[30]));assert new==5
indices=[5,4,2,1,0];register_team(indices)
prepared=False
for match in range(100):
 if rank()>=7:break
 inspection=bool(rd(current()+v[46],1))
 if inspection and not prepared:
  goleague();desk();cap=call('GetCurrentLevelCap')
  before=(rd(current()+v[31]),rd(current()+v[29],2),rd(current()+v[30],2),call('GetMoney',rd('gSaveBlock1Ptr')+v[64]))
  for training in range(120):
   if min(call('GetMonData2',symbols['gParties']+i*v[5],v[11]) for i in indices)>=cap:break
   choose(2);until(lambda:menu_count()==5);choose(training%2);outcome=battle();until(lambda:menu_count()==5)
   after=(rd(current()+v[31]),rd(current()+v[29],2),rd(current()+v[30],2),call('GetMoney',rd('gSaveBlock1Ptr')+v[64]));assert after==before
   row={'stage':'rank_six_practice','battle':training+1,'outcome':outcome,'party':profile()};history.append(row)
   (ROOT/'career-progress.json').write_text(json.dumps(history,indent=2)+'\n');state('career-practice-latest');print('CAREER',json.dumps(row),flush=True)
  else:raise AssertionError('Practice did not reach cap within budget')
  gohome();prepared=True
 choose(4 if inspection else 0);until(yesno);choose(0);outcome=battle();until(lambda:menu_count()==8)
 row={'stage':'agatha_inspection' if inspection else 'rank_six_defense','match':match+1,'outcome':outcome,'rank':rank(),'rank_xp':rd(current()+v[31]),'wins':rd(current()+v[29],2),'losses':rd(current()+v[30],2),'party':profile()}
 history.append(row);(ROOT/'career-progress.json').write_text(json.dumps(history,indent=2)+'\n');state('career-latest');print('CAREER',json.dumps(row),flush=True)
 if inspection and outcome!=1:state('career-agatha-loss');raise AssertionError('Agatha defeated the earned team; strategy review required')
else:raise AssertionError('Rank seven not reached within budget')
assert rank()==7
state('career-earned-rank-seven');snap('career-earned-rank-seven')
print('PASS EARNED CONTROLLER CAREER THROUGH AGATHA; FULL CAMPAIGN REMAINS INCOMPLETE',flush=True)
