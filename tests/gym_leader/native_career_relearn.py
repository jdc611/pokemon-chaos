"""Actual Party > Relearn access and level-up move teaching in the earned career."""
import runpy,struct,json
ctx=runpy.run_path('tests/gym_leader/native_campaign.py');globals().update(ctx)
blob=(ROOT/'relearner-layout.bin').read_bytes();r=struct.unpack('<'+'I'*(len(blob)//4),blob)
READ_ONLY.add('VarGet')
assert call('VarGet',r[6])==0 and call('VarGet',r[7])<2
print('PASS GLE relearner access with original Pewter/training gates unset',flush=True)
def leave_relearner():
 for _ in range(240):
  if rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1 and not call('ArePlayerFieldControlsLocked') and not call('ScriptContext_IsEnabled') and menu_count()==0:return
  if task('Task_MoveRelearner_Giveup_Answer')<16:press(1,50)
  else:press(2,25)
 snap('career-relearner-exit-failure');raise AssertionError('Native relearner did not close')
def open_relearner():
 leave_relearner();press(8,60);waitfor(lambda:task('Task_ShowStartMenu')<16 and rd('gMenuCallback')==symbols['HandleStartMenuInput']|1)
 actions=[rd(symbols['sCurrentStartMenuActions']+i,1) for i in range(rd('sNumStartMenuActions',1))];goal=actions.index(1)
 for _ in range(20):
  cursor=rd('sStartMenuCursorPos',1)
  if cursor==goal:break
  press(128 if cursor<goal else 64,15)
 press(1,160);waitfor(lambda:task('Task_HandleChooseMonInput')<16);frames(100)
 for _ in range(12):
  cursor=call('GetCursorSelectionMonId')
  if cursor==1:break
  press(16 if cursor==0 else 64,20)
 press(1,40);p=rd('sPartyMenuInternal');items=[rd(p+r[0]+i,1) for i in range(rd(p+r[1],1))]
 assert r[2] in items,('Native Party Relearn still locked',items)
 choose(items.index(r[2]));choose(0);waitfor(lambda:task('Task_MoveRelearner_HandleInput')<16);frames(100)
def available():
 p=rd('sMoveRelearnerStruct');return [rd(p+r[3]+2*i,2) for i in range(rd(p+r[4],2)-1)]
def teach(move,slot):
 if move in partymoves(1):return
 open_relearner()
 moves=available();assert move in moves,(move,moves);goal=moves.index(move)
 for _ in range(300):
  cursor=rd('sMoveRelearnerScrollState',2)+rd(symbols['sMoveRelearnerScrollState']+2,2)
  if cursor==goal:break
  press(128 if cursor<goal else 64,15)
 press(1,30);selected=False
 for _ in range(600):
  if move in partymoves(1):break
  if task('Task_HandleReplaceMoveInput')<16:
   if not selected:choose_forgotten_move(slot);selected=True
   else:frames(20)
  else:press(1,20)
 else:raise AssertionError(('Native relearning did not finish',move))
 leave_relearner()
 print('CAREER native relearned move',move,'slot',slot,flush=True)
open_relearner();offers=available();print('CAREER real level-up relearner offers',offers,flush=True);snap('career-relearner-offers');leave_relearner()
teach(483,0)
if 79 in offers:teach(79,1)
if 202 in offers:teach(202,2)
if 235 in offers:teach(235,3)
leave_relearner()
walkto(7,12);press(16);press(1);until(lambda:menu_count()==8)
state('career-native-relearned');snap('career-native-relearned')
print('PASS EARNED CAREER NATIVE LEVEL-UP RELEARNING',json.dumps(profile()),flush=True)
