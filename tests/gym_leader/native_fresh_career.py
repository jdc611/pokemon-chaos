"""One continuous controller campaign prefix, with no snapshot/flash resumes.
No final complete-release sentinel exists until later gates are implemented here.
"""
import os,runpy,json
assert not os.environ.get('GLE_CAMPAIGN_RESUME') and not os.environ.get('GLE_CAMPAIGN_FLASH')
os.environ.pop('GLE_CAREER_HELPERS_ONLY',None)
ctx=runpy.run_path('tests/gym_leader/native_campaign.py')
for stage in ['native_career.py','native_bruno.py','native_rank_six.py','native_agatha.py']:
 if stage=='native_career.py':os.environ.pop('GLE_CAREER_HELPERS_ONLY',None)
 ctx=runpy.run_path('tests/gym_leader/'+stage,init_globals={'_campaign_context':ctx})
 ctx.pop('_campaign_context',None)
 ctx['state']('fresh-continuous-latest')
 ctx['ROOT'].joinpath('fresh-continuous-progress.json').write_text(json.dumps({'last_stage':stage,'rank':ctx['rd'](ctx['current']()+ctx['v'][2],1),'used_snapshot_or_flash_resume':False,'full_campaign_complete':False},indent=2)+'\n')
print('PASS CONTINUOUS FRESH CONTROLLER CAREER THROUGH AGATHA; LANCE, COMPETITIONS AND OAK STILL REQUIRED',flush=True)
