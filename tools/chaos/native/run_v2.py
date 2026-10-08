import pathlib,subprocess,os,concurrent.futures,json
from qa import ROOT
root=ROOT;source=pathlib.Path(__file__).resolve().parent;env=os.environ.copy()
groups=[['qa_pass_jj','qa_pass_training','qa_pass_core','qa_pass_battle','qa_pass_doubles','qa_v2_berry','qa_v2_berry_activation'],['qa_v2_capture_control','qa_v2_capture_swap','qa_v2_capture_slots','qa_v2_nuz_capture','qa_v2_exp','qa_v2_features','qa_v2_audit','qa_v2_tms','qa_v2_growth_audit','qa_v2_ui','qa_v2_visual','qa_v2_fishing','qa_v2_integration','qa_v2_centers'],['qa_pass_retry','qa_pass_rewards','qa_pass_gym_battles','qa_pass_hof','qa_pass_progression','qa_pass_rider']]
def group(scripts):
 results=[]
 for name in scripts:
  out=root/(name+'-final.log')
  with out.open('w') as f:r=subprocess.run(['python',str(source/(name+'.py'))],stdout=f,stderr=subprocess.STDOUT,env=env)
  results.append({'test':name,'exit':r.returncode,'log':str(out)})
  print(name,'PASS' if r.returncode==0 else 'FAIL',flush=True)
  if r.returncode:break
 return results
with concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:
 results=sum(list(pool.map(group,groups)),[])
(root/'v2-final-qa.json').write_text(json.dumps(results,indent=2))
assert len(results)==sum(map(len,groups)) and all(r['exit']==0 for r in results)
