"""Fresh title-screen career progression using controller actions only.
Native calls are restricted to reading public game state. No ranks, outcomes,
Pokémon, levels, money, flags, warps or enemy HP are supplied by this runner.
This is an in-progress campaign driver; completion requires its final sentinel.
"""
import runpy,struct,json,os,time
from pathlib import Path
ctx=runpy.run_path('tests/gym_leader/native_startup.py');globals().update(ctx)
_read_call=call
READ_ONLY={'IsGymLeaderRun','GymLeaderOfficialTeamLegal','GetCurrentLevelCap','GetMonData2','GymLeaderRegistering','GymLeaderBattleTrainer','ScriptContext_IsEnabled','GetMoney','GleXpNeeded','GetCursorSelectionMonId'}
def call(fn,*args):
 assert fn in READ_ONLY,('mutating/unknown native call prohibited',fn)
 return _read_call(fn,*args)
def wr(*args):raise AssertionError('campaign RAM writes prohibited')
source=Path('tests/gym_leader/native_opening.py').read_text()
exec(compile(source[source.index('def current():'):],'native_opening_controller_route','exec'))
blob=(ROOT/'campaign-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(blob)//4),blob)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(180):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('campaign walk blocked',pos(),(x,y)))
def mappair():
 a=rd('gSaveBlock1Ptr')+v[25];return rd(a,1),rd(a+1,1)
def power(move):return (rd(symbols['gMovesInfo']+move*c[6]+8)>>23)&511 if move else 0
def movetype(move):return (rd(symbols['gMovesInfo']+move*c[6]+8)>>16)&31
def category(move):return (rd(symbols['gMovesInfo']+move*c[6]+8)>>21)&3
def partymoves(i):return [call('GetMonData2',symbols['gParties']+i*v[5],v[38]+j) for j in range(4)]
def profile():return [{'species':call('GetMonData2',symbols['gParties']+i*v[5],v[10]),'level':call('GetMonData2',symbols['gParties']+i*v[5],v[11]),'moves':partymoves(i),'exp':call('GetMonData2',symbols['gParties']+i*v[5],v[35])} for i in range(rd('gPartiesCount',1))]
def bestmove():
 own=symbols['gBattleMons'];enemy=own+c[0];scores=[]
 for i in range(4):
  move=rd(own+c[1]+i*c[4],c[4]);pp=rd(own+c[2]+i,1);score=power(move)
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
    replacement=True;new=rd('gMoveToLearn',2);getter=rd(rd('gBattleStruct')+c[13],1);moves=partymoves(getter);weak=min(range(4),key=lambda i:power(moves[i]))
    if power(new)<=power(moves[weak]):press(2,40)
    else:
     for _ in range(weak):press(128,15)
     press(1,40)
   else:frames(20)
   continue
  replacement=False
  if task('Task_HandleChooseMonInput')<16:
   alive=[i for i in range(rd('gPartiesCount',1)) if rd(symbols['gParties']+i*v[5]+v[13],2)>0]
   if not alive:press(1,25);continue
   target=alive[0]
   for _ in range(12):
    cursor=call('GetCursorSelectionMonId')
    if cursor==target:break
    press(32 if target==0 else 16 if cursor==0 else 128 if cursor<target else 64,15)
   press(1,25);press(1,25)
   continue
  if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseMove']|1:
   target=bestmove();cursor=rd('gMoveSelectionCursor',1)
   if cursor%2!=target%2:press(16 if target%2 else 32,8)
   cursor=rd('gMoveSelectionCursor',1)
   if cursor//2!=target//2:press(128 if target//2 else 64,8)
  press(1,18)
 snap('campaign-battle-stall');raise AssertionError(('campaign battle stalled',profile(),hex(rd(symbols['gMain']+4))))
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
print('CAMPAIGN IN PROGRESS: inspections, competitions and Oak not yet completed',flush=True)
