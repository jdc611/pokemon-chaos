"""Run independent native opening fixtures for every Gym type (three hometowns)."""
import os,json,subprocess,concurrent.futures,time
from pathlib import Path
repo=Path(__file__).resolve().parents[2]
base=Path(os.environ['CHAOS_QA_DIR']).resolve();root=base/'all-type-openings';root.mkdir(exist_ok=True)
def run(kind):
 p=root/f'type-{kind:02}';p.mkdir(exist_ok=True)
 for name in ['emulator.so','layout.bin']:(p/name).write_bytes((base/name).read_bytes())
 env=dict(os.environ,CHAOS_QA_DIR=str(p),GLE_TEST_TYPE=str(kind),GLE_TEST_TOWN=str(kind%3))
 with (p/'results.log').open('w') as output:r=subprocess.run(['python3','-u','tests/gym_leader/native_opening.py'],cwd=repo,env=env,stdout=output,stderr=subprocess.STDOUT)
 log=(p/'results.log').read_text();ok=r.returncode==0 and 'PASS hall exit and native Viridian Gym doorway redirect' in log and f'type={kind},' in log
 return {'type_index':kind,'town_index':kind%3,'exit_code':r.returncode,'passed':ok}
results=[]
with concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:
 for future in concurrent.futures.as_completed([pool.submit(run,i) for i in range(18)]):
  r=future.result();results.append(r);print(json.dumps(r),flush=True);(root/'results.json').write_text(json.dumps(sorted(results,key=lambda x:x['type_index']),indent=2)+'\n')
print('ALL TYPE OPENINGS COMPLETE',sum(r['passed'] for r in results),'/18',flush=True)

raise SystemExit(0 if len(results)==18 and all(r['passed'] for r in results) else 1)
