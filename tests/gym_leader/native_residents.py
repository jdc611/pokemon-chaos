"""Resident construction requests, finite moral choices, standing and persistence."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
# Population/capacity/rank are fixture setup; each settlement uses real scripts.
wr(current()+v[54],24,2);wr(current()+v[55],24,2)
wr(current()+v[2],8,1);wr(current()+v[-1],8,1)
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],50000)
map_id=v[58+town]
def warp(request,other=False):
 m=v[58+(town+1)%3] if other else map_id;x=4+request*5;y=16+(request%3)*7
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y+1,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(240)
 # Freeze and place this wandering resident for deterministic dialogue input.
 call('FreezeObjectEvents')
 lib.call6(symbols['TryMoveObjectEventToMapCoords'],request+1,m&255,m>>8,x,y,0)
 frames(1)
def bit(i):return bool(rd(current()+v[114]+(200+i)//8,1)&(1<<((200+i)%8)))
def money():return call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
def honor():
 n=rd(current()+v[136],2);return n-65536 if n>=32768 else n
def info(i):wr('gSpecialVar_LastTalked',i+1,2);call('GleResidentRequestInfo');return rd('gSpecialVar_Result',2)
def settle():
 for _ in range(90):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 snap('residents-script-stuck');assert not call('ScriptContext_IsEnabled')
def approach():press(64);press(1)
warp(0);assert info(0)==1
before=money();approach();settle();assert not bit(0) and money()==before
# Make the requested Center via the real core transaction; plot UI is covered
# independently by native_town.py. Do not manufacture its completion ledger.
ptr=symbols['gStringVar4'];wr(ptr,10000,4)
assert call('GleConstruct',current(),0,1,ptr)==0
warp(0);assert info(0)==2
approach();until(yesno);choose(1);settle();assert not bit(0) and money()==before
approach();until(yesno);choose(0);settle()
assert bit(0) and money()==before+500 and honor()==10
before=money();approach();settle();assert money()==before
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert bit(0) and honor()==10
warp(0);assert info(0)==3
print('PASS native pending Center request, declined/accepted one-time contribution, repeated-talk anti-farm and flash persistence',flush=True)
state('residents-start')
# A local visitor cannot authorize another town's payout.
warp(4,True);assert info(4)==0
wr('gSpecialVar_Result',1,2);call('GleResidentRequestResolve');assert not bit(4)
warp(4);assert info(4)==4
approach();until(lambda:menu_count()==3);choose(2);settle();assert not bit(4)
# Honest path: four actual native choice menus produce Honorable standing.
for request,reward in [(4,400),(5,400),(6,1000),(7,0)]:
 warp(request);before=money();approach();until(lambda:menu_count()==3)
 if request==4:snap('resident-purse-choice')
 choose(0);settle();assert bit(request) and money()==before+reward
assert honor()==170
warp(0);approach();settle();call('GleTownStandingInfo');snap('resident-honorable')
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert honor()==170 and all(bit(i) for i in range(4,8))
print('PASS cross-town refusal, decide-later retry, four native honest choices and persistent Honorable standing',flush=True)
# Restore the independent starting record for the alternative fictional path.
state('residents-start',1)
for request,reward in [(4,1500),(5,800),(6,5000),(7,2000)]:
 warp(request);before=money();approach();until(lambda:menu_count()==3);choose(1);settle()
 assert bit(request) and money()==before+reward
 before=money();approach();settle();assert money()==before
assert honor()==-230
call('GleTownStandingInfo')
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert honor()==-230 and all(bit(i) for i in range(4,8))
# No research or official progress is minted by the dialogues.
assert not any(rd(current()+v[114]+i,1) for i in range(25))
assert rd(current()+v[29],2)==0 and rd(current()+v[30],2)==0 and rd(current()+v[31],4)==0
print('PASS four native shady alternatives, one-time payouts, persistent Notorious standing and independent research/official records',flush=True)
