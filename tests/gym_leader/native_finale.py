"""Oak's final examination, true Badge, native ending and archive preservation."""
import runpy,struct,subprocess,os
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
cdata=(ROOT/'finale-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(cdata)//4),cdata)
def complete():return bool(rd(current()+v[114]+27,1)&128)
def money():return call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
def records():return tuple(rd(current()+v[k],n) for k,n in [(29,2),(30,2),(31,4),(32,1),(56,4)])
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def settle():
 for _ in range(150):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('Oak did not return field control')
def setdata(mon,field,value):wr('gStringVar1',value);call('SetMonData',mon,field,symbols['gStringVar1'])
# Empty final flag survives all current save data and cannot be earned at early rank.
assert not complete() and not call('GymLeaderFinalOakEligible')
wr(current()+v[2],7,1);wr(current()+c[16],1,2);assert not call('GymLeaderFinalOakEligible')
wr(current()+v[2],8,1);wr(current()+c[16],0,2);assert not call('GymLeaderFinalOakEligible')
warp(c[0],6,4);press(64);press(1);settle();assert not complete()
wr(current()+c[16],1,2);assert call('GymLeaderFinalOakEligible')
press(1);until(yesno);snap('oak-final-confirm');choose(1);settle();assert not call('GymLeaderBattleTrainer')
wr(current()+v[26],0,1);press(1);until(yesno);choose(0);settle();assert not complete() and not call('GymLeaderBattleTrainer')
print('PASS native Oak lab return, Rank 8/title gates, decline and unregistered-team refusal',flush=True)
# Supply a legal all-Grass test team with native moves, normal abilities and items.
for i in range(6):
 mon=symbols['gParties']+i*v[5];call('CreateRandomMonWithIVs',mon,c[1],100,31)
 for j in range(4):call('SetMonMoveSlot',mon,c[2+j],j)
 for field,value in [(c[7],252),(c[8],252),(c[9],0),(v[41],c[6]),(c[17],255),(c[18],c[19])]:setdata(mon,field,value)
 for j in range(4):setdata(mon,v[45]+j,call('CalculatePPWithBonus',c[2+j],255,j))
 call('CalculateMonStats',mon)
 wr(current()+v[27]+i*4,call('GetMonData2',mon,v[36]));wr(current()+v[28]+i*4,call('GetMonData2',mon,v[37]))
wr(current()+v[26],6,1);wr('gPartiesCount',6,1);call('HealPlayerParty')
# Native FAST text, SET style and animation-off options change presentation only.
opt=rd('gSaveBlock2Ptr')+0x14;wr(opt,(rd(opt,2)&~7)|2|0x600,2)
baseline=records();funds=money();assert call('GymLeaderPrepareFinalOak')==1
enemy=symbols['gParties']+6*v[5]
assert rd(symbols['gPartiesCount']+1,1)==6
for i in range(6):
 mon=enemy+i*v[5];assert call('GetMonData2',mon,v[10])==c[10+i] and call('GetMonData2',mon,v[11])==100
 assert call('GetMonData2',mon,v[12])<2 and all(call('GetMonData2',mon,v[38]+j) for j in range(4))
wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense');assert not complete() and money()==funds and records()==baseline
# Opaque sentinel data demonstrates neither final settlement nor credits saves use Hall sectors.
for bank in [0,1]:
 buf=call('Alloc_',3968,0);assert buf
 for i in range(0,3968,4):wr(buf+i,0x11223344+bank)
 assert call('GleWriteArchiveSector',bank,buf)==1;call('Free',buf)
def sector(bank):
 buf=symbols['gSaveDataBuffer'];call('ReadFlash',28+bank,0,buf,4096);return bytes(rd(buf+i,1) for i in range(4096))
archive=[sector(0),sector(1)]
print('PASS six-member native Oak roster, level-100 normal abilities/moves, loss/retry and official-record neutrality',flush=True)
# Play the complete battle through controller input; no HP or outcome injection.
move_handlers=[]
for line in subprocess.check_output([os.environ['CHAOS_ARM_NM'],'-n',str(repo/'pokefirered.elf')],text=True).splitlines():
 parts=line.split()
 if len(parts)==3 and parts[2]=='HandleInputChooseMove':move_handlers.append(int(parts[0],16)|1)
assert len(move_handlers)==1;move_handler=move_handlers[0]
press(1);until(yesno);choose(0);until(lambda:call('GymLeaderBattleTrainer')!=0);frames(250);snap('oak-final-battle')
for turn in range(5000):
 if turn and turn%500==0:print("Oak controller steps",turn,"player HP",rd(symbols["gBattleMons"]+v[50],2),"enemy",rd(symbols["gBattleMons"]+v[49],2),flush=True)
 if rd('gBattlerControllerFuncs')==move_handler:
  foe=rd(symbols['gBattleMons']+v[49],2);target=0 if foe==c[10] else 3 if foe==c[13] else 2 if foe==c[15] else 1
  mon=call('GetBattlerMon',0);hp=rd(symbols['gBattleMons']+v[50],2);maxhp=rd(symbols['gBattleMons']+v[51],2)
  if not call('GetMonData2',mon,v[45]+target):target=next((j for j in [0,1,3] if call('GetMonData2',mon,v[45]+j)),0)
  cursor=rd('gMoveSelectionCursor',1)
  if (cursor&1)!=(target&1):press(16 if target&1 else 32,12)
  elif (cursor&2)!=(target&2):press(128 if target&2 else 64,12)
  else:press(1,18)
 elif task('Task_HandleChooseMonInput')<16:
  slot=rd(symbols['gPartyMenu']+9,1)
  if slot<6 and rd(symbols['gParties']+slot*v[5]+v[13],2)>0:press(1,25);press(1,25)
  else:press(16 if slot==0 else 128,20)
 else:press(1,18)
 if not call('GymLeaderBattleTrainer'):break
else:snap('oak-final-timeout');raise AssertionError('actual Oak battle timed out')
assert rd('gBattleOutcome',1)==1,('Oak result',rd('gBattleOutcome',1))
until(yesno);snap('oak-final-badge');choose(1);settle()
assert complete() and money()==funds+20000 and records()==baseline
assert call('FlagGet',v[87]) and call('FlagGet',v[88])
for i in range(6):
 mon=symbols['gParties']+i*v[5];assert call('GetMonData2',mon,v[10])==c[1] and rd(mon+v[13],2)==rd(mon+v[14],2)
 assert call('GetMonData2',mon,v[36])==rd(current()+v[27]+i*4)
assert [sector(0),sector(1)]==archive and call('GymLeaderPrepareFinalOak')==4
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and complete()
print('PASS controller-played Oak victory, final Badge/game-clear flags, one-time reward, healed original team, rank/queue neutrality and Hall-sector preservation',flush=True)
# Actual post-victory credits choice saves first and enters native credits directly.
press(1);until(yesno);choose(0);frames(600);snap('oak-ending-credits')
assert rd(symbols['gMain']+4)==symbols['CB2_Credits']|1
assert [sector(0),sector(1)]==archive
print('PASS native ending credits entry after a normal save, preserved final flag and both championship archive sectors',flush=True)
