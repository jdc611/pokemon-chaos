"""Native hosting migration, controller booking, physical venue and flash checks."""
import runpy,struct
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
def layout(name):
 data=(ROOT/(name+'-layout.bin')).read_bytes();return struct.unpack('<'+'I'*(len(data)//4),data)
h=layout('hosting');c=layout('competition')
def cp():return current()+c[0]
def hp():return current()+h[0]
def money():return call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
def setmoney(n):call('SetMoney',rd('gSaveBlock1Ptr')+v[64],n)
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def finish_text():
 for _ in range(200):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('hosting dialogue did not release')
assert h[:3]==(3188,3196,14)
wr(current()+h[16],13,2)
expected=bytearray(rd(current()+i,1) for i in range(h[0]));expected[h[16]:h[16]+2]=h[2].to_bytes(2,'little')
for i in range(8):wr(hp()+i,255,1)
call('GymLeaderState')
assert bytes(rd(current()+i,1) for i in range(h[0]))==expected
assert rd(hp()+h[5],1)==0 and rd(hp()+h[3])==rd(current()+v[56])+180
print('PASS native v13 migration preserves every prior byte and initializes only hosting tail',flush=True)
wr(current()+v[2],3,1);wr(current()+v[54],12,2);wr(current()+h[9],100,2);wr(current()+h[11],0xffffffff)
plot=next(i for i in range(h[20]) if not rd(current()+v[52]+i*v[53],1) and rd(current()+v[52]+i*v[53]+2,1)>=2)
scratch=symbols['gStringVar1'];wr(scratch,100000)
assert call('GleConstruct',current(),plot,h[17],scratch)==0
# Native activity creates an event; no manufactured booking or host assignment.
wr(cp()+c[3],rd(current()+v[56]));call('GymLeaderAdvanceActivity');assert rd(cp()+c[8],1)==1
warp(v[24],6,6);press(64);press(1);until(lambda:menu_count()==4);snap('hosting-director')
def proposal(accept):
 choose(0);until(lambda:menu_count()==4);choose(1);until(yesno);snap('hosting-defense-quote');choose(0 if accept else 1);until(lambda:menu_count()==4)
setmoney(0);proposal(True);assert money()==0 and rd(hp()+h[5],1)!=2
setmoney(100000);proposal(False);assert money()==100000 and rd(hp()+h[5],1)!=2
proposal(True);assert money()==95000 and rd(hp()+h[5],1)==2
assert call('GleEventVenue',cp()+c[5])==0 and call('GleEventFormat',cp()+c[5])==1
# Duplicate applications cannot charge again.
choose(0);until(lambda:menu_count()==4);assert money()==95000
print('PASS native director, insufficient funds, proposal cancellation, doubles booking and one-time fee',flush=True)
choose(2);frames(360);finish_text()
loc=rd('gSaveBlock1Ptr')+v[25];assert (rd(loc,1)<<8|rd(loc+1,1))==v[58]
assert call('GetObjectEventIdByLocalId',249)<16
assert call('GymLeaderCompetitionAtVenue')
snap('hosting-town-venue')
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert rd(hp()+h[5],1)==2 and money()==95000 and call('GleEventFormat',cp()+c[5])==1
warp(v[24],6,6);assert not call('GymLeaderCompetitionAtVenue')
print('PASS physical hometown venue, travel, coordinator, wrong-venue gate and native flash persistence',flush=True)
# Registered real team and deterministic first opponent, without altered battle HP.
for i in range(3):
 mon=symbols['gParties']+i*v[5];call('CreateRandomMonWithIVs',mon,h[12],35,31);call('SetMonMoveSlot',mon,h[13],0)
 for j in [1,2,3]:call('SetMonMoveSlot',mon,0,j)
 wr(current()+v[27]+i*4,call('GetMonData2',mon,v[36]));wr(current()+v[28]+i*4,call('GetMonData2',mon,v[37]))
wr(current()+v[26],3,1);wr('gPartiesCount',3,1);call('HealPlayerParty')
assert call('GleCompetitionJoin',current())==0
wr(cp()+c[5],sum(who<<(4*i) for i,who in enumerate([0,2,1,3,4,5,6,7])))
wr(current()+v[56],rd(cp()+c[4])-1);call('GymLeaderAdvanceActivity');assert rd(cp()+c[8],1)==2
assert call('GymLeaderPrepareCompetition')==4
warp(v[58],10,9);wr(current()+v[26],1,1);assert call('GymLeaderPrepareCompetition')==2;wr(current()+v[26],3,1)
opt=rd('gSaveBlock2Ptr')+0x14;wr(opt,(rd(opt,2)&~7)|2|0x600,2)
def records():return tuple(rd(current()+v[k],n) for k,n in [(29,2),(30,2),(31,4),(32,1),(56,4)])
baseline=records();funds=money()
press(16);press(1);until(lambda:menu_count()==9);choose(2);until(yesno);choose(0)
until(lambda:call('GymLeaderBattleTrainer')!=0);frames(250)
assert rd('gBattleTypeFlags')&h[14] and rd('gBattlersCount',1)==4
for i in range(4):assert not call('CanDynamax',i)
assert not call('CanTerastallize',1) and not call('CanTerastallize',3)
snap('hosting-defense-real-battle')
for _ in range(2400):
 if task('Task_HandleChooseMonInput')<16:
  slot=rd(symbols['gPartyMenu']+9,1);active=set()
  for battler in [0,2]:
   who=rd(symbols['gBattlerPartyIndexes']+battler*h[15],h[15])
   if who<3 and rd(symbols['gParties']+who*v[5]+v[13],2)>0:active.add(who)
  if slot<3 and slot not in active and rd(symbols['gParties']+slot*v[5]+v[13],2)>0:press(1,25);press(1,25)
  else:press(16 if slot==0 else 128,20)
 else:press(1,18)
 if not call('GymLeaderBattleTrainer'):break
else:snap('hosting-battle-timeout');raise AssertionError('actual hosting doubles battle did not finish')
assert rd('gBattleOutcome',1)==1
until(lambda:menu_count()==9);press(2);finish_text()
assert records()==baseline and money()==funds
for i in range(3):
 mon=symbols['gParties']+i*v[5];assert rd(mon+v[13],2)==rd(mon+v[14],2)
 assert call('GetMonData2',mon,v[36])==rd(current()+v[27]+i*4)
print('PASS controller-played hometown doubles victory, four battlers, gimmick restrictions, healed identities and rank/queue neutrality',flush=True)
# Other round settlements exercise native adapter; only the first was controller-played.
rounds=1
while call('GleCompetitionOpponent',current())<0x80000000:
 assert call('GymLeaderPrepareCompetition')==1
 wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense');rounds+=1
assert rounds==3 and rd(hp()+h[5],1)==0 and rd(cp()+c[7],2)==1
assert money()>funds+6000 and records()==baseline
settled=money();scratch=symbols['gStringVar1'];wr(scratch,settled);call('GleHostingFinish',current(),scratch);assert rd(scratch)==settled
assert call('GleEventFormat',cp()+c[14])==1 and call('GleEventVenue',cp()+c[14])==0
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and money()==settled and rd(hp()+h[5],1)==0
print('PASS all hosting round adapters, visitor income once, immutable format/venue history and completed flash state',flush=True)
print('PASS HOSTING NATIVE CHECKS COMPLETE',flush=True)
