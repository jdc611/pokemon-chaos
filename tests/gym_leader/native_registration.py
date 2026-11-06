"""Regression for the native fifth/sixth registration slots.
The five-slot test uses the genuinely earned rank-five flash save. The six-slot
test explicitly sets a rank-seven fixture only after preserving that earned state;
it does not establish earned rank seven or campaign completion.
"""
import os,runpy,json
from pathlib import Path
os.environ['GLE_CAREER_HELPERS_ONLY']='1'
globals().update(runpy.run_path('tests/gym_leader/native_career.py'))
assert rank()==5
READ_ONLY.update({'MapGridGetMetatileBehaviorAt','MetatileBehavior_IsPC'})
src=Path('tests/gym_leader/native_career_town.py').read_text();exec(src[src.index('def deposit_reserve('):src.index("if not os.environ.get('GLE_CAREER_TOWN_HELPERS_ONLY')")])
deposit_reserve(2,672);new=claim_reward();assert new==5
register_team([5,4,1,0,3]);assert rank()==5
state('career-earned-rank-five-registered');snap('career-earned-rank-five-registered')
print('PASS EARNED RANK FIVE NATIVE FIVE-PARTNER REGISTRATION',flush=True)
# Explicit diagnostic fixture, never reused as earned career progress.
lib.wr(current()+v[2],7,1)
register_team([5,4,1,0,3,2])
assert rd(current()+v[26],1)==6
snap('fixture-rank-seven-six-partner-registration')
(ROOT/'registration-result.json').write_text(json.dumps({'earned_rank_five_slots':5,'fixture_rank_seven_slots':6,'earned_rank_seven':False,'campaign_complete':False,'pass':True},indent=2)+'\n')
print('PASS FIXTURE RANK SEVEN NATIVE SIX-PARTNER REGISTRATION; NOT EARNED RANK SEVEN',flush=True)
