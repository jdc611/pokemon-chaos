"""All 18 native type menus/drafts, rotating through all three hometowns."""
import concurrent.futures,os,pathlib,shutil,subprocess,sys
root=pathlib.Path(os.environ['CHAOS_QA_DIR'])
def run(type_):
 out=root/f'opening-type-{type_}';out.mkdir(exist_ok=True)
 for filename in ['emulator.so','layout.bin']:
  shutil.copyfile(root/filename,out/filename)
 env=dict(os.environ,CHAOS_QA_DIR=str(out),GLE_TEST_TYPE=str(type_),GLE_TEST_TOWN=str(type_%3))
 if type_==0:env['GLE_TEST_TOWN_NAME']='1'
 result=subprocess.run([sys.executable,'tests/gym_leader/native_opening.py'],env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
 (out/'result.log').write_text(result.stdout)
 return type_,result.returncode,result.stdout
failures=[]
with concurrent.futures.ThreadPoolExecutor(max_workers=3) as executor:
 for type_,code,output in executor.map(run,range(18)):
  print(f'type={type_}, town={type_%3}: {"PASS" if code==0 else "FAIL"}',flush=True)
  if code:failures.append((type_,output))
for type_,output in failures:print(type_,output)
raise SystemExit(bool(failures))
