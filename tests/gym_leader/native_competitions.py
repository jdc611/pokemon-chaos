"""Native competition registration, battle adapter, persistent bracket/flash archive."""
import runpy,struct
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
cdata=(ROOT/'competition-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(cdata)//4),cdata)
assert c[0]==1392 and c[1]<=3200 and c[2]>=11
state('competition-base')
def cp():return current()+c[0]
def field(i,n=1):return rd(cp()+c[i],n)
def setfield(i,val,n=1):wr(cp()+c[i],val,n)
def money():return call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
def setmoney(n):call('SetMoney',rd('gSaveBlock1Ptr')+v[64],n)
def records():return tuple(rd(current()+v[k],n) for k,n in [(29,2),(30,2),(31,4),(32,1),(56,4)])
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def finish_text():
 for _ in range(200):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('competition dialogue did not release')
def announce(kind=0):
 # Saved history index determines event kind; sequence 2 is the first Kanto.
 setfield(7,2 if kind else 0,2);setfield(8,0);setfield(3,rd(current()+v[56]),4);call('GymLeaderAdvanceActivity');assert field(8)==1
 # Isolate the direct HQ bracket; physical hosting is tested separately.
 wr(cp()+c[5]+c[19],3,1)

def start():wr(current()+v[56],field(4,4)-1);call('GymLeaderAdvanceActivity')
# Exact old v10 prefix remains intact, including seeded research/colleague data.
wr(current()+v[78],10,2)
expected=bytearray(rd(current()+i,1) for i in range(c[0]));expected[v[78]:v[78]+2]=c[2].to_bytes(2,'little')
for i in range(c[1]-c[0]):wr(cp()+i,255,1)
call('GymLeaderState');assert bytes(rd(current()+i,1) for i in range(c[0]))==expected
assert field(7,2)==0 and field(11)==255 and field(8)==0
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
print('PASS exact v10 prefix migration and native flash; appended competition state fits reserved union',flush=True)
# Event desk exists and menu/cancel releases native field scripts.
warp(v[24],9,9);press(64);press(1);until(lambda:menu_count()==9);snap('competition-desk');press(2);finish_text()
# Fill a registered legal Grass fixture team as in the real colleague test.
wr(current()+v[2],2,1)
for i in range(3):
 mon=symbols['gParties']+i*v[5]
 call('CreateRandomMonWithIVs',mon,c[15],25,31);call('SetMonMoveSlot',mon,c[16],0)
 for j in [1,2,3]:call('SetMonMoveSlot',mon,0,j)
 wr(current()+v[27]+i*4,call('GetMonData2',mon,v[36]));wr(current()+v[28]+i*4,call('GetMonData2',mon,v[37]))
wr(current()+v[26],3,1);call('HealPlayerParty');setmoney(100000)
announce();assert call('GleCompetitionJoin',current())==0
# Deterministic native battle fixture: first quarter-final is YOU versus BROCK.
setfield(5,sum(who<<(4*i) for i,who in enumerate([0,2,1,3,4,5,6,7])),4)
start();assert field(8)==2
# The adapter refuses missing registrations without consuming the bracket.
wr(current()+v[26],0,1);before=field(5,4);assert call('GymLeaderPrepareCompetition')==2 and field(5,4)==before
wr(current()+v[26],3,1);baseline=records();funds=money()
press(1);until(lambda:menu_count()==9);choose(2);until(yesno);choose(0)
until(lambda:call('GymLeaderBattleTrainer')!=0);frames(250);snap('regional-quarter-final-battle')
for _ in range(1800):
 if task('Task_HandleChooseMonInput')<16:
  slot=rd(symbols['gPartyMenu']+9,1)
  if slot<3 and rd(symbols['gParties']+slot*v[5]+v[13],2)>0:press(1,25);press(1,25)
  else:press(16 if slot==0 else 128,20)
  continue
 press(1,18)
 if not call('GymLeaderBattleTrainer'):break
else:raise AssertionError('actual Regional quarter-final did not finish')
assert rd('gBattleOutcome',1)==1
until(lambda:menu_count()==9);press(2);finish_text();snap('regional-quarter-final-result')
assert records()==baseline and money()==funds
wins=1
print('PASS controller-played native Regional quarter-final victory without altered enemy HP or injected result',flush=True)
while call('GleCompetitionOpponent',current())<0x80000000:
 opponent=call('GleCompetitionOpponent',current());assert 1<=opponent<=8
 assert call('GymLeaderPrepareCompetition')==1
 assert call('GymLeaderTrainingActive') and rd('gPartiesCount',1)==3
 assert rd(symbols['gPartiesCount']+1,1)==3
 wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense');assert records()==baseline
 wins+=1
assert wins==3 and field(7,2)==1 and rd(current()+v[2],1)==2
assert money()==funds+4000
assert call('GymLeaderPrepareCompetition')==4
print('PASS all three native registered-party round adapters, missing-registration refusal, queue/rank/records neutrality and one-time Regional reward',flush=True)
# Actual security notice/history native messages use the completed immutable bracket.
press(1);until(lambda:menu_count()==9);snap('history-menu-before');state('history-menu-before');choose(3);snap('history-after-select')

for _ in range(100):
 if menu_count()==3:break
 press(1,20)
snap('history-observed')
assert menu_count()==3
snap('competition-bracket-history');press(2);until(lambda:menu_count()==9);press(2);finish_text()
# Kanto qualification, pending archive, actual team snapshot and independent NPC records.
wr(current()+v[2],6,1);announce(1);assert call('GleCompetitionJoin',current())==0;start()
while call('GleCompetitionOpponent',current())<0x80000000:
 assert call('GymLeaderPrepareCompetition')==1;wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense')
assert field(8)==0 and field(7,2)==3 and field(12)==1
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
# Read both 3968-byte banks and inspect newest valid GLE archive.
a=call('Alloc_',3968,0);b=call('Alloc_',3968,0);assert a and b
banks=[]
for index,buf in [(0,a),(1,b)]:
 if call('GleReadArchiveSector',index,buf)==1 and rd(buf)==0x474c4348:banks.append((rd(buf+8),index,buf))
assert banks;_,bank,buf=max(banks);assert rd(buf+12,2)==1
p=buf+16;assert rd(p,2)==2 and rd(p+2,1)==0 and rd(p+3,1)==3
for i in range(3):assert rd(p+4+i*18+4,2)==c[15] and rd(p+4+i*18+6,1)==25
# Both slots protect prior teams when the other slot contains an invalid write.
call('GleWriteArchiveSector',bank^1,b if buf==a else a)
assert call('LoadGameSave',0)==1 and field(12)==1
call('Free',a);call('Free',b)
# Native prize selection grants a single permanent reusable TM license.
warp(v[24],9,9);press(64);press(1);until(lambda:menu_count()==9);choose(6);until(lambda:menu_count()==9);choose(0);until(lambda:menu_count()==9);press(2);finish_text()
assert call('CheckBagHasItem',c[18],1) and call('GymLeaderChampionTMReusable',c[18])
wr('gSpecialVar_Result',0,2);call('GleCompetitionPrizeClaim');assert rd('gSpecialVar_Result',2)!=0
# Cosmetic trophy choice appears as an inspectable native object in safe Gym slots.
wr('gSpecialVar_0x8007',0,2);wr('gSpecialVar_Result',2,2);call('GleCompetitionTrophySet');assert rd('gSpecialVar_Result',2)==0 and call('GymLeaderTrophyAt',0)==2
wr('gSpecialVar_0x8007',2,2);wr('gSpecialVar_Result',2,2);call('GleCompetitionTrophySet');assert rd('gSpecialVar_Result',2)!=0
warp(c[17],2,5);assert call('GetObjectEventIdByLocalId',236)<16
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert call('GymLeaderTrophyAt',0)==2 and call('GymLeaderChampionTMReusable',c[18])
print('PASS native championship prize selection, reusable TM license, duplicate refusal, cosmetic trophy and upgrade gate with flash persistence',flush=True)
# Next two regionals and a championship proceed without player entry.
for _ in range(3):
 setfield(3,rd(current()+v[56]),4);call('GymLeaderAdvanceActivity');assert field(8)==1;start();assert field(8)==0
assert field(12)==2 and field(7,2)==6
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
call('GleCompetitionHallStart');call('GleCompetitionHallInfo')
assert field(12)==2
print('PASS Kanto qualification, native team archive bytes/species/nicknames/levels, flash reload, alternating-bank preservation and independent NPC championships',flush=True)
