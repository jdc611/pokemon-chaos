"""Persistent Rocket incidents, informal party battles, heists and save migration."""
import runpy,struct
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
data=(ROOT/'rocket-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(data)//4),data)
assert c[0]==1372 and c[1]>=1392 and c[2]>=10
state('rocket-migration-base')
wr(current()+v[2],2,1);wr(current()+v[-1],2,1)
ptr=symbols['gStringVar4'];wr(ptr,100000,4)
assert call('GleConstruct',current(),0,1,ptr)==0
assert call('GleConstruct',current(),1,3,ptr)==0
wr(current()+v[54],12,2);wr(current()+v[55],12,2)
for i in range(6):
 mon=symbols['gParties']+i*v[5]
 call('CreateRandomMonWithIVs',mon,c[17] if i<3 else c[19],25,31)
 call('SetMonMoveSlot',mon,c[18] if i<3 else c[20],0)
 for j in [1,2,3]:call('SetMonMoveSlot',mon,0,j)
wr('gPartiesCount',6,1);wr(current()+v[26],0,1);call('HealPlayerParty')
assert call('GymLeaderOfficialTeamLegal')==0
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],50000)
def rr(index,n=1):return rd(current()+c[0]+c[index],n)
def rw(index,value,n=1):wr(current()+c[0]+c[index],value,n)
def records():return tuple(rd(current()+v[k],n) for k,n in [(29,2),(30,2),(31,4),(32,1),(56,4)])
def money():return call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def finish_text():
 for _ in range(160):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 snap('rocket-script-stuck');raise AssertionError('Rocket dialogue did not return control')
def rocket_template():
 events=rd(symbols['gMapHeader']+v[157]);p=rd(events+v[158])
 return any(rd(p+i*v[116],1)==235 for i in range(rd(events,1)))
wr(current()+v[56],119,4);call('GymLeaderAdvanceActivity')
assert rr(9)==1 and rr(11)==1 and rr(4,2)==1
warp(v[58+town],15,9)
assert rocket_template() and call('GetObjectEventIdByLocalId',235)<16
assert call('MapGridGetCollisionAt',22,15)==0 and call('MapGridGetCollisionAt',22,16)==0
press(64);press(1);until(yesno);choose(1);finish_text()
assert rr(9)==1 and rr(11)==1
state('rocket-incident')
for i in range(6):wr(symbols['gParties']+i*v[5]+v[13],0,2)
press(1);until(yesno);choose(0);finish_text()
assert not call('GymLeaderBattleTrainer') and rr(11)==1
call('HealPlayerParty')
before=records();funds=money();identities=[call('GetMonData2',symbols['gParties']+i*v[5],v[36]) for i in range(6)]
press(1);until(yesno);choose(0);until(lambda:call('GymLeaderBattleTrainer')!=0);frames(200)
snap('rocket-town-defense-battle')
for _ in range(2200):
 if not call('GymLeaderBattleTrainer') and menu_count()==3:break
 if task('Task_HandleChooseMonInput')<16:
  slot=rd(symbols['gPartyMenu']+9,1)
  if slot<6 and rd(symbols['gParties']+slot*v[5]+v[13],2)>0:press(1,25);press(1,25)
  else:press(16 if slot==0 else 128,20)
 else:press(1,18)
else:snap('rocket-battle-timeout');raise AssertionError('actual town defense did not finish at settlement')
assert rd('gBattleOutcome',1)==1 and rr(9)==2 and rr(11)==0
assert records()==before and money()==funds and rd('gPartiesCount',1)==6 and rd(current()+v[26],1)==0
for i in range(6):
 mon=symbols['gParties']+i*v[5]
 assert call('GetMonData2',mon,v[36])==identities[i] and rd(mon+v[13],2)==rd(mon+v[14],2)
snap('rocket-settlement-choice');choose(2);finish_text()
assert rr(9)==2 and money()==funds
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert rr(9)==2
warp(v[58+town],15,9);press(64);press(1);until(lambda:menu_count()==3);choose(0);finish_text()
assert rr(9)==0 and rr(5,2)==1 and money()==funds+2000
assert not rocket_template() and call('GetObjectEventIdByLocalId',235)>=16
frames(500);assert call('GetObjectEventIdByLocalId',235)>=16
assert records()==before
print('PASS activity-triggered persistent raid, native decline/unhealthy refusal, actual unregistered mixed-type six-party victory, rank/queue neutrality, restored identities, deferred choice/flash and one-time settlement/NPC removal',flush=True)
# A direct loss fixture retains the incident and awards no official loss or cash.
rw(9,1);rw(11,2);rw(10,0);rw(4,7,2)
warp(v[58+town],15,9);before=records();funds=money()
assert call('GymLeaderPrepareTownDefense')==1
wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense')
assert rr(11)==2 and rr(6,2)==1 and records()==before and money()==funds
# Ordinary cap/type rules still reject this same unregistered party officially.
assert call('GymLeaderPrepareDefense')==2
print('PASS Rocket loss/retry with no official penalties and continued official-team refusal for the informal mixed party',flush=True)
# Every rank's actual roster adapter has moves, current-cap scaling and no reserved species.
enemy=symbols['gParties']+6*v[5]
for rank in [2,5,8]:
 wr(current()+v[2],rank,1)
 for kind in range(4):
  rw(9,1);rw(10,kind);rw(11,1)
  assert call('GymLeaderPrepareTownDefense')==1
  count=rd(symbols['gPartiesCount']+1,1)
  assert count==(6 if kind==3 else min(6,2+rank//2))
  for i in range(count):
   mon=enemy+i*v[5];sp=call('GetMonData2',mon,v[10])
   assert not call('GymLeaderSpeciesReserved',sp)
   assert call('GetMonData2',mon,v[11])==[15,25,35,45,55,65,80,100][rank-1]-2
   assert call('GetMonData2',mon,v[12])<2 and any(call('GetMonData2',mon,v[38]+j) for j in range(4))
  wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense')
print('PASS native street/habitat/museum/Giovanni roster adapters at ranks 2/5/8, levels, normal abilities, moves and reserved-species exclusion',flush=True)
# A successful museum theft remains missing after victory; the boss can recover it later.
wr(current()+v[2],5,1);rw(9,1);rw(10,2);rw(11,1);rw(13,1);rw(7,2,2)
assert call('GymLeaderPrepareTownDefense')==1
wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense');assert rr(9)==2 and rr(7,2)==2
wr('gSpecialVar_Result',0,2);call('GleRocketResolve');assert rd('gSpecialVar_Result',2)==0 and rr(7,2)==2 and rr(9)==0
wr(current()+v[2],7,1);rw(5,3,2);rw(3,0,4);call('GleRocketActivity',current())
assert rr(10)==3 and rr(9)==1
while rr(9)==1:
 assert call('GymLeaderPrepareTownDefense')==1
 wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense')
wr('gSpecialVar_Result',0,2);call('GleRocketResolve')
assert rr(14)==1 and rr(7,2)==0 and rr(8,2)==2 and rr(9)==0
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert rr(14)==1 and rr(8,2)==2
print('PASS native heist-success persistence after player victory, later Giovanni escalation/recovery and flash story records',flush=True)
# The incident contact remains physically approachable in every rebuilt town.
for home in range(3):
 wr(current()+v[3]-2,home,1);wr(current()+v[3]-1,(home+1)%3,1)
 rw(9,1);rw(11,1);rw(10,0)
 warp(v[58+home],15,9)
 assert rocket_template() and call('GetObjectEventIdByLocalId',235)<16
 assert call('MapGridGetCollisionAt',22,15)==0 and call('MapGridGetCollisionAt',22,16)==0
 press(64);press(1);until(yesno);choose(1);finish_text()
 assert rr(9)==1
rw(9,0)
print('PASS native incident contact, approach collision and decline/release in all three hometowns',flush=True)
# Migration is an independent old-save fixture, not the mutated town-layout matrix.
latest_flash_counter=rd('gSaveCounter')
state('rocket-migration-base',1);wr('gSaveCounter',latest_flash_counter)
# Migration from v9 preserves the exact previous prefix including pending E4 rewards.
for i in range(11):wr(current()+c[15]+i,0xA5,1)
for i in range(4):wr(current()+c[16]+i,0x55,1)
wr(current()+v[78],9,2)
expected=bytearray(rd(current()+i,1) for i in range(c[0]));expected[v[78]:v[78]+2]=c[2].to_bytes(2,'little')
activity=rd(current()+v[56],4);call('GymLeaderState')
assert bytes(rd(current()+i,1) for i in range(c[0]))==expected
assert rr(9)==0 and rr(3,4)==activity+80 and rr(5,2)==0
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert bytes(rd(current()+i,1) for i in range(c[0]))==expected
print('PASS exact version-9 prefix migration, retained friendlies/pending vouchers/research/resident data and native flash',flush=True)
