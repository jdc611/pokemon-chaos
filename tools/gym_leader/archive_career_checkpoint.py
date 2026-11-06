"""Archive an observed earned milestone and its independently checked game flash.
This tool compresses existing test output only; it never changes game state.
"""
import argparse,base64,gzip,hashlib,json
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('rank',type=int,choices=range(3,9));p.add_argument('qa',type=Path);p.add_argument('save_qa',type=Path);a=p.parse_args()
repo=Path(__file__).resolve().parents[2];out=repo/'tests/gym_leader/checkpoints'
word={3:'three',4:'four',5:'five',6:'six',7:'seven',8:'eight'}[a.rank];name='earned-rank-'+word
history=json.loads((a.qa/'career-progress.json').read_text());row=next(r for r in reversed(history) if 'rank' in r)
assert row['rank']==a.rank
result=json.loads((a.save_qa/'native-save-result.json').read_text());assert result['pass'] and result['before']==result['after']
assert result['after']['rank']==a.rank and result['after']['wins']==row['wins'] and result['after']['losses']==row['losses'] and result['after']['party']==row['party']
assert 'PASS EARNED CAREER NATIVE SAVE, POWER RESET AND TITLE CONTINUE' in (a.save_qa/'results.log').read_text()
raw=(a.qa/('career-'+name+'.state')).read_bytes();flash=(a.save_qa/'earned-career.sav').read_bytes()
rom=hashlib.sha256((repo/'pokefirered.gba').read_bytes()).hexdigest()
for suffix,data in [('state',raw),('sav',flash)]:
 (out/(name+'.'+suffix+'.gz.b64')).write_text(base64.b64encode(gzip.compress(data,mtime=0)).decode()+'\n')
meta={'rom_sha256':rom,'state_sha256':hashlib.sha256(raw).hexdigest(),'state_bytes':len(raw),'save_sha256':hashlib.sha256(flash).hexdigest(),'flash_bytes':len(flash),'earned_rank':a.rank,'official_wins':row['wins'],'official_losses':row['losses'],'rank_xp':row['rank_xp'],'party':row['party'],'campaign_complete':False,'history':'Controller-earned continuation from prior earned checkpoint. Development resumes were used. No supplied ranks, outcomes, Pokemon, money or HP. Final uninterrupted campaign replay remains required.'}
(out/(name+'.json')).write_text(json.dumps(meta,indent=2)+'\n');result.update({'rom_sha256':rom,'save_sha256':meta['save_sha256'],'full_campaign_complete':False});(out/(name+'-save.json')).write_text(json.dumps(result,indent=2)+'\n')
p=repo/'docs/gym-leader-edition/fresh-career-progress.json';d=json.loads(p.read_text());d['earned_rank_'+word]=meta;p.write_text(json.dumps(d,indent=2)+'\n')
print(name,rom,'archived; complete campaign remains false')
