"""Controller-only earned Rank 5 to Rank 6, including actual reserve PC deposit."""
import os,runpy,json
from pathlib import Path
os.environ['GLE_CAREER_HELPERS_ONLY']='1'
globals().update(runpy.run_path('tests/gym_leader/native_career.py',init_globals={'_campaign_context':_campaign_context} if '_campaign_context' in globals() else None))
assert rank()==5
controller_save('career-rank-5-native')
READ_ONLY.update({'MapGridGetMetatileBehaviorAt','MetatileBehavior_IsPC'})
src=Path('tests/gym_leader/native_career_town.py').read_text();exec(src[src.index('def deposit_reserve('):src.index("if not os.environ.get('GLE_CAREER_TOWN_HELPERS_ONLY')")])
deposit_reserve(2,672)
new=claim_reward();assert new==5
register_team([5,4,1,0,3])
for match in range(100):
 if rank()>=6:break
 assert not rd(current()+v[46],1)
 choose(0);until(yesno);choose(0);outcome=battle();until(lambda:menu_count()==8)
 row={'stage':'rank_five_defense','match':match+1,'outcome':outcome,'rank':rank(),'rank_xp':rd(current()+v[31]),'wins':rd(current()+v[29],2),'losses':rd(current()+v[30],2),'party':profile()}
 history.append(row);(ROOT/'career-progress.json').write_text(json.dumps(history,indent=2)+'\n');state('career-latest');print('CAREER',json.dumps(row),flush=True)
else:raise AssertionError('Rank six not reached within budget')
assert rank()==6
state('career-earned-rank-six');snap('career-earned-rank-six')
print('PASS EARNED CONTROLLER CAREER TO RANK SIX; FULL CAMPAIGN REMAINS INCOMPLETE',flush=True)
