"""Fresh title-screen career progression using controller actions only.
Native calls are restricted to reading public game state. No ranks, outcomes,
Pokémon, levels, money, flags, warps or enemy HP are supplied by this runner.
This is an in-progress campaign driver; completion requires its final sentinel.
"""
import runpy,struct,json,os,time
from collections import deque
from pathlib import Path
ctx=runpy.run_path('tests/gym_leader/native_startup.py');globals().update(ctx)
_read_call=call
READ_ONLY={'IsGymLeaderRun','GymLeaderOfficialTeamLegal','GetCurrentLevelCap','GetMonData2','GymLeaderRegistering','GymLeaderBattleTrainer','ScriptContext_IsEnabled','GetMoney','GleXpNeeded','GetCursorSelectionMonId','MapGridGetCollisionAt'}
def call(fn,*args):
 assert fn in READ_ONLY,('mutating/unknown native call prohibited',fn)
 return _read_call(fn,*args)
def wr(*args):raise AssertionError('campaign RAM writes prohibited')
source=Path('tests/gym_leader/native_opening.py').read_text()
exec(compile(source[source.index('def current():'):],'native_opening_controller_route','exec'))
blob=(ROOT/'campaign-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(blob)//4),blob)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 goal=(x,y);blocked=set()
 for retry in range(32):
  if pos()==goal:frames(20);return
  layout=rd('gMapHeader');w=rd(layout);h=rd(layout+4);start=pos()
  walls={(i,j) for j in range(h) for i in range(w) if call('MapGridGetCollisionAt',i+7,j+7)}
  for i in range(16):
   actor=symbols['gObjectEvents']+i*c[14]
   if rd(actor)&1 and rd(actor+c[16],1)!=c[17]:
    walls.add((rd(actor+c[15],2)-7,rd(actor+c[15]+2,2)-7))
  walls.discard(start);queue=deque([start]);parent={start:None}
  while queue and goal not in parent:
   point=queue.popleft()
   for dx,dy,key in [(1,0,16),(-1,0,32),(0,1,128),(0,-1,64)]:
    nxt=(point[0]+dx,point[1]+dy)
    if 0<=nxt[0]<w and 0<=nxt[1]<h and nxt not in walls and nxt not in parent and (point,nxt) not in blocked:
     parent[nxt]=(point,key);queue.append(nxt)
  if goal not in parent:
   snap('campaign-no-walk-route');state('campaign-walk-debug');raise AssertionError(('no native walk route',start,goal,w,h,'goal wall',goal in walls,'blocked',sorted(blocked),'walls',sorted(walls)))
  path=[];point=goal
  while point!=start:
   previous,key=parent[point];path.append((point,key));point=previous
  for target,key in reversed(path):
   before=pos();frames(16,key);frames(16)
   if pos()!=target:
    if pos()==before:blocked.add((before,target))
    break
  else:frames(20);return
 snap('campaign-walk-blocked');raise AssertionError(('campaign walk blocked',pos(),goal))
def mappair():
 a=rd('gSaveBlock1Ptr')+v[25];return rd(a,1),rd(a+1,1)
def power(move):return (rd(symbols['gMovesInfo']+move*c[6]+8)>>23)&511 if move else 0
def movetype(move):return (rd(symbols['gMovesInfo']+move*c[6]+8)>>16)&31
def category(move):return (rd(symbols['gMovesInfo']+move*c[6]+8)>>21)&3
def partymoves(i):return [call('GetMonData2',symbols['gParties']+i*v[5],v[38]+j) for j in range(4)]
def profile():return [{'species':call('GetMonData2',symbols['gParties']+i*v[5],v[10]),'level':call('GetMonData2',symbols['gParties']+i*v[5],v[11]),'moves':partymoves(i),'exp':call('GetMonData2',symbols['gParties']+i*v[5],v[35])} for i in range(rd('gPartiesCount',1))]
def movevalue(move):
 value=power(move)
 flags=rd(symbols['gMovesInfo']+move*c[6]+16)
 if flags&(1<<8):value*=3
 else:value*=max(1,(flags>>4)&15)
 accuracy=rd(symbols['gMovesInfo']+move*c[6]+12)&127
 value*=(accuracy or 100)/100
 # Charge/semi-invulnerable moves spend two turns for one attack.
 if move in {19,76,91,130,143,291,340,467,553,554,566,601,611,669}:value*=0.55
 return value
def bestmove():
 own=symbols['gBattleMons'];enemy=own+c[0];scores=[]
 for i in range(4):
  move=rd(own+c[1]+i*c[4],c[4]);pp=rd(own+c[2]+i,1);score=movevalue(move)
  typ=movetype(move)
  for side in range(2):
   t=rd(enemy+c[3]+side*c[5],c[5])
   if typ<c[7] and t<c[7]:score*=rd(symbols['gTypeEffectivenessTable']+2*(typ*c[7]+t),2)/4096
   if side==0 and rd(enemy+c[3],c[5])==rd(enemy+c[3]+c[5],c[5]):break
  types=[rd(own+c[3]+j*c[5],c[5]) for j in range(2)]
  if typ in types:score*=1.5
  attack=rd(own+(c[8] if category(move)==0 else c[9]),2)
  defense=rd(enemy+(c[10] if category(move)==0 else c[11]),2)
  score*=attack/max(1,defense)
  scores.append(score if pp and move else -1)
 return max(range(4),key=lambda i:scores[i])
def battle():
 started=False;replacement=False
 for step in range(4500):
  active=bool(call('GymLeaderBattleTrainer'))
  if active:started=True
  elif started:return rd('gBattleOutcome',1)
  if task('Task_HandleReplaceMoveInput')<16:
   if not replacement:
    frames(80);replacement=True;new=rd('gMoveToLearn',2);getter=rd(rd('gBattleStruct')+c[13],1);moves=partymoves(getter);weak=min(range(4),key=lambda i:power(moves[i]))
    if power(new)<=power(moves[weak]):press(2,40)
    else:
     for _ in range(weak):press(128,15)
     press(1,40)
   else:frames(20)
   continue
  replacement=False
  if task('Task_HandleChooseMonInput')<16:
   alive=[i for i in range(rd('gPartiesCount',1)) if rd(symbols['gParties']+i*v[5]+v[13],2)>0 and i!=rd('gBattlerPartyIndexes',2)]
   if not alive:press(1,25);continue
   target=alive[0]
   for _ in range(12):
    cursor=call('GetCursorSelectionMonId')
    if cursor==target:break
    press(32 if target==0 else 16 if cursor==0 else 128 if cursor<target else 64,15)
   press(1,25);press(1,25)
   continue
  if rd('gBattlerControllerFuncs')==symbols['PlayerHandleYesNoInput']|1:
   press(2,20);continue
  if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseMove']|1:
   target=bestmove();cursor=rd('gMoveSelectionCursor',1)
   if cursor%2!=target%2:press(16 if target%2 else 32,8)
   cursor=rd('gMoveSelectionCursor',1)
   if cursor//2!=target//2:press(128 if target//2 else 64,8)
  press(1,18)
 snap('campaign-battle-stall');state('campaign-battle-debug');raise AssertionError(('campaign battle stalled',profile(),hex(rd(symbols['gMain']+4))))
# Return home through the real GLA desk.
walkto(7,12);press(16);press(1);until(lambda:menu_count()==5);choose(3)
for _ in range(200):
 if mappair()==(v[23]>>8,v[23]&255):break
 frames(12)
else:raise AssertionError('GLA home travel did not arrive')
frames(300);walkto(7,12);press(16);press(1);until(lambda:menu_count()==8)
print('CAMPAIGN initial party',json.dumps(profile()),flush=True)
# Register all three drafted partners through the native selector.
choose(1);until(lambda:call('GymLeaderRegistering'));frames(160)
for target in range(3):
 for _ in range(12):
  cursor=call('GetCursorSelectionMonId')
  if cursor==target:break
  press(16 if cursor==0 else 128,15)
 press(1,25);press(1,25)
press(8,25);press(1,100)
until(lambda:menu_count()==8);assert rd(current()+v[26],1)==3
history=[]
for match in range(20):
 if rd(current()+v[2],1)>=2:break
 before=rd(current()+v[29],2);choose(0);until(yesno);choose(0);outcome=battle();until(lambda:menu_count()==8)
 row={'match':match+1,'outcome':outcome,'rank':rd(current()+v[2],1),'rank_xp':rd(current()+v[31]),'wins':rd(current()+v[29],2),'losses':rd(current()+v[30],2),'party':profile()};history.append(row)
 (ROOT/'campaign-progress.json').write_text(json.dumps(history,indent=2)+'\n');print('CAMPAIGN',json.dumps(row),flush=True)
 state('campaign-latest')
 if outcome==1:assert rd(current()+v[29],2)==before+1
assert rd(current()+v[2],1)==2
snap('campaign-earned-rank-two');state('campaign-earned-rank-two')
print('PASS FRESH CONTROLLER CAREER TO EARNED RANK TWO',flush=True)
# Visit the actual GLA desk and collect only the milestone just earned.
choose(5)
for _ in range(200):
 if mappair()==(v[24]>>8,v[24]&255):break
 frames(12)
frames(300);walkto(7,12);press(16);press(1);until(lambda:menu_count()==5)
choose(1);until(lambda:menu_count()==5);assert rd(current()+v[66],1)==1
choose(0);until(lambda:menu_count()==1);choose(0);until(lambda:menu_count()==3)
# Choose from the real preview, favoring a strong Grass attack for Lorelei.
scores=[]
for i in range(3):
 mon=symbols['gGlePreviewParty']+i*v[5]
 moves=[call('GetMonData2',mon,v[38]+j) for j in range(4)]
 scores.append(max(movevalue(m)*(2 if movetype(m)==4 else 1) for m in moves))
choice=max(range(3),key=lambda i:scores[i]);choose(choice);until(lambda:menu_count()==3);choose(0);until(yesno);choose(0)
until(lambda:menu_count()==5);assert rd('gPartiesCount',1)==4 and rd(current()+v[66],1)==0
print('CAMPAIGN earned voucher',json.dumps(profile()),flush=True)
choose(3)
for _ in range(200):
 if mappair()==(v[23]>>8,v[23]&255):break
 frames(12)
frames(300);walkto(7,12);press(16);press(1);until(lambda:menu_count()==8)
choose(1);until(lambda:call('GymLeaderRegistering'));frames(160)
for target in [3,0,1]:
 for _ in range(20):
  cursor=call('GetCursorSelectionMonId')
  if cursor==target:break
  press(32 if target==0 else 16 if cursor==0 else 128 if cursor<target else 64,15)
 press(1,25);press(1,25)
press(8,25);press(1,100);until(lambda:menu_count()==8)
assert rd(current()+v[26],1)==3
for match in range(60):
 if rd(current()+v[2],1)>=3:break
 inspection=bool(rd(current()+v[46],1))
 choose(4 if inspection else 0);until(yesno);choose(0);outcome=battle();until(lambda:menu_count()==8)
 row={'stage':'first_inspection' if inspection else 'rank_two_defense','match':match+1,'outcome':outcome,'rank':rd(current()+v[2],1),'rank_xp':rd(current()+v[31]),'wins':rd(current()+v[29],2),'losses':rd(current()+v[30],2),'party':profile()};history.append(row)
 (ROOT/'campaign-progress.json').write_text(json.dumps(history,indent=2)+'\n');print('CAMPAIGN',json.dumps(row),flush=True);state('campaign-latest')
 if inspection and outcome!=1:
  snap('campaign-first-inspection-loss')
  print('CAMPAIGN needs training before retrying the first inspection',flush=True)
  break
if rd(current()+v[2],1)==3:
 snap('campaign-earned-rank-three');state('campaign-earned-rank-three')
 print('PASS FRESH CONTROLLER CAREER THROUGH FIRST INSPECTION',flush=True)
print('CAMPAIGN IN PROGRESS: later inspections, competitions and Oak not yet completed',flush=True)
