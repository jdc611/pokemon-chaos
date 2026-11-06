"""Build a Center and reach its real PC in the earned career by controller."""
import runpy,json
ctx=runpy.run_path('tests/gym_leader/native_campaign.py');globals().update(ctx)
READ_ONLY.update({'MapGridGetMetatileBehaviorAt','MetatileBehavior_IsPC'})
def deposit_reserve(index=2,expected_species=None):
 close_to_field();walkto(6,13);frames(64,128);waitfor(lambda:mappair()==(v[58]>>8,v[58]&255));frames(150)
 if rd(current()+v[52],1)==0:
  walkto(18,16);press(64);press(1);until(lambda:menu_count()==11)
  cash=call('GetMoney',rd('gSaveBlock1Ptr')+v[64]);choose(0);until(yesno);choose(0)
  until(lambda:rd(current()+v[52],1)==1);until(lambda:not call('ScriptContext_IsEnabled'));frames(30)
  assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==cash-1500
 assert rd(current()+v[52],1)==1
 walkto(15,15);frames(24,64);waitfor(lambda:mappair()==(v[61]>>8,v[61]&255));frames(150)
 pc=[]
 layout=rd('gMapHeader');w=rd(layout);h=rd(layout+4)
 for y in range(h):
  for x in range(w):
   if call('MetatileBehavior_IsPC',call('MapGridGetMetatileBehaviorAt',x+7,y+7)):pc.append((x,y))
 assert pc, 'Constructed Center has no PC metatile'
 x,y=pc[0];walkto(x,y+1);press(64);press(1)
 until(lambda:task('Task_HandleMultichoiceInput')<16)
 state('career-center-pc-menu');snap('career-center-pc-menu')
 print('PASS EARNED CAREER PAID CENTER CONSTRUCTION AND REAL PC ACCESS',json.dumps({'pc':pc,'party':profile()}),flush=True)
 # Deposit the unregistered level-six reserve through the real storage UI.
 before=profile()
 if expected_species is not None:assert before[index]['species']==expected_species
 press(1);until(lambda:task('Task_PCMainMenu')<16);frames(60)
 press(128,25);press(1,160);waitfor(lambda:task('Task_PokeStorageMain')<16);frames(120)
 assert rd('sCursorArea',1)==1
 for _ in range(index):press(128,25)
 assert rd('sCursorPosition',1)==index
 press(1,30);press(1,30);waitfor(lambda:task('Task_DepositMenu')<16);frames(60);press(1,120)
 waitfor(lambda:task('Task_PokeStorageMain')<16);frames(60)
 press(2,60);press(2,160);waitfor(lambda:task('Task_PCMainMenu')<16);frames(80)
 assert rd('gPartiesCount',1)==len(before)-1
 after=profile();assert after==before[:index]+before[index+1:]
 state('career-center-reserve-deposited');snap('career-center-reserve-deposited')
 print('PASS EARNED CAREER NATIVE PC DEPOSIT PRESERVES OTHER PARTNERS',json.dumps(after),flush=True)
 close_to_field();walkto(7,7);frames(64,128);waitfor(lambda:mappair()==(v[58]>>8,v[58]&255));frames(120)
 walkto(10,9);frames(40,64);waitfor(lambda:mappair()==(v[23]>>8,v[23]&255));frames(180)
 walkto(7,12);press(16);press(1);until(lambda:menu_count()==8)
 print('PASS NATIVE CENTER PC ROUTE RETURNED TO OWN GYM',flush=True)

if not os.environ.get('GLE_CAREER_TOWN_HELPERS_ONLY'):deposit_reserve(2,672)
