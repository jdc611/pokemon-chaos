"""Kanto colleague friendlies, optional E4 rematches and v8 save preservation."""
import runpy,struct
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
cdata=(ROOT/'colleague-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(cdata)//4),cdata)
assert c[0]==1356 and c[2]>=1372 and c[3]>=9
state('colleague-base')
trainer=call('Alloc_',v[101],0);entries=call('Alloc_',1024,0);assert trainer and entries
enemy=symbols['gParties']+6*v[5];caps=[15,25,35,45,55,65,80,100];sizes=[3,3,4,4,5,5,6,6]
for who in range(11):
 wr('sColleagueId',who,1)
 for rank in [1,4,8]:
  wr(current()+v[2],rank,1)
  assert call('GymLeaderPrepareColleagueRoster',trainer,entries)
  assert rd(symbols['gPartiesCount']+1,1)==sizes[rank-1]
  seen=set()
  for i in range(sizes[rank-1]):
   mon=enemy+i*v[5];species=call('GetMonData2',mon,v[10])
   assert species not in seen;(seen.add(species))
   assert call('GetMonData2',mon,v[11])==caps[rank-1]
   inf=symbols['gSpeciesInfo']+species*v[194]+v[193]
   assert c[11+who] in [rd(inf,1),rd(inf+1,1)],(who,rank,species)
   assert not call('GymLeaderSpeciesReserved',species)
   assert call('GetMonData2',mon,v[12])<2
   assert any(call('GetMonData2',mon,v[38]+j) for j in range(4))
call('Free',trainer);call('Free',entries);state('colleague-base',1)
print('PASS all 11 native colleagues at ranks 1/4/8, traditional types, distinct species, current caps/team limits, normal abilities and moves',flush=True)
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0)
 call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def finish_text():
 for _ in range(150):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 snap('colleague-dialog-failure');raise AssertionError('colleague did not release field control')
def mask(who):return rd(current()+c[0]+who,1)
def records():return tuple(rd(current()+v[k],n) for k,n in [(29,2),(30,2),(31,4),(32,1),(56,4)])
def money():return call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
# Supply a legal deterministic Grass team; play a real Brock battle, no HP/outcome injection.
for i in range(3):
 mon=symbols['gParties']+i*v[5]
 call('CreateRandomMonWithIVs',mon,c[22],15,31);call('SetMonMoveSlot',mon,c[23],0)
 for j in [1,2,3]:call('SetMonMoveSlot',mon,0,j)
 wr(current()+v[27]+i*4,call('GetMonData2',mon,v[36]))
 wr(current()+v[28]+i*4,call('GetMonData2',mon,v[37]))
wr(current()+v[26],3,1);call('HealPlayerParty')
warp(c[4],6,6);press(64);press(1);until(yesno);choose(1);finish_text()
assert not mask(0) and not call('GymLeaderBattleTrainer')
wr(current()+v[26],0,1);press(1);until(yesno);choose(0);finish_text()
assert not mask(0) and not call('GymLeaderBattleTrainer')
wr(current()+v[26],3,1);before=records();funds=money()
press(1);until(yesno);snap('brock-colleague-confirm');choose(0)
until(lambda:call('GymLeaderBattleTrainer')!=0);frames(250)
snap('brock-colleague-battle')
logged=False
for _ in range(1800):
 if not logged and rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:
  logged=True;snap('brock-action-start');state('brock-action-start')
  assert all(not call('FlagGet',v[80+i]) for i in range(8))
  assert not call('FlagGet',c[29])
  attacker=rd('gBattlerAttacker',1);wr('gBattlerAttacker',0,1)
  for _check in range(32):assert call('GetAttackerObedienceForAction')==c[28]
  wr('gBattlerAttacker',attacker,1)
  assert call('CanDynamax',0)==0 and call('CanDynamax',1)==0
  assert call('CanTerastallize',1)==0
  print('PASS real battle current-cap obedience without badges/debug bypass and shared player/opponent Dynamax refusal',flush=True)
 if task('Task_HandleChooseMonInput')<16:
  slot=rd(symbols['gPartyMenu']+9,1)
  if slot<3 and rd(symbols['gParties']+slot*v[5]+v[13],2)>0:press(1,25);press(1,25)
  else:press(16 if slot==0 else 128,20)
  continue
 press(1,18)
 if not call('GymLeaderBattleTrainer'):break
else:snap('colleague-battle-timeout');raise AssertionError('actual Brock battle failed to finish')
finish_text();snap('brock-colleague-result')
assert rd('gBattleOutcome',1)==1, ('actual Brock result',rd('gBattleOutcome',1),[(call('GetMonData2',enemy+i*v[5],v[10]),[call('GetMonData2',enemy+i*v[5],v[38]+j) for j in range(4)],rd(enemy+i*v[5]+v[13],2)) for i in range(3)])
assert mask(0)==1 and money()==funds+400 and records()==before
for i in range(3):
 mon=symbols['gParties']+i*v[5]
 assert call('GetMonData2',mon,v[10])==c[22] and rd(mon+v[13],2)==rd(mon+v[14],2)
 assert call('GetMonData2',mon,v[36])==rd(current()+v[27]+4*i)
press(1);finish_text();assert money()==funds+400 and not call('GymLeaderBattleTrainer')
print('PASS actual Brock native decline/refusal/victory, one-time stipend, original healed registered identities and unchanged rank/queue/official/activity records',flush=True)
# Direct adapter loss/retry/independent reward masks for every colleague.
for who in range(11):
 wr('sColleagueId',who,1);wr(current()+v[2],2,1)
 before=records();funds=money()
 assert call('GymLeaderPrepareColleagueFriendly')==1
 wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense')
 assert records()==before and money()==funds and not (mask(who)&2)
 assert call('GymLeaderPrepareColleagueFriendly')==1
 wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense')
 assert records()==before and money()==funds+(400 if who<7 else 600)*2 and mask(who)&2
 assert call('GymLeaderPrepareColleagueFriendly')==4
wr('sColleagueId',0,1);wr(current()+v[2],1,1)
assert call('GymLeaderPrepareColleagueFriendly')==4
print('PASS all 11 adapter loss/retry, independent per-rank win masks, rank-neutral one-time settlement and demotion anti-refarm',flush=True)
# Full voucher account preserves E4 rewards at the earned tier until room exists.
wr(current()+v[2],3,1);wr('sColleagueId',7,1);wr(current()+v[66],24,1)
assert call('GymLeaderPrepareColleagueFriendly')==1
wr('gBattleOutcome',1,1);call('GymLeaderFinishDefense')
assert rd(current()+c[1],1)==4 and mask(7)&4
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert rd(current()+c[1],1)==4
wr(current()+v[66],23,1)
warp(v[196],2,13);wr('sColleagueId',7,1);call('GleColleagueInfo')
assert rd(current()+v[66],1)==24 and rd(current()+c[1],1)==0
# Current fixture is rank 3; earned tier must remain 3.
assert rd(current()+v[67]+23*v[68]+c[27],1)==3
before_count=rd(current()+v[66],1);call('GleColleagueInfo');assert rd(current()+v[66],1)==before_count
print('PASS full-account E4 voucher deferral, native flash, earned-tier later collection and no duplicate claims',flush=True)
# Actual League coordinator menu, rank-completed return and cancel release.
wr(current()+v[2],1,1);press(64);press(1);until(lambda:menu_count()==5)
choose(0);until(yesno);choose(1);until(lambda:menu_count()==5);press(2);finish_text()
snap('elite-four-rematch-desk')
# Both displaced leaders are visible at GLA for each hometown pairing.
for home in range(3):
 for rival in range(3):
  if home==rival:continue
  # Identity fixture uses the saved town bytes immediately before Gym type.
  wr(current()+v[3]-2,home,1);wr(current()+v[3]-1,rival,1)
  warp(v[24],6,7)
  for local in [230,231]:assert call('GetObjectEventIdByLocalId',local)<16
print('PASS actual optional E4 coordinator decline/cancel, and both relocated leaders across all six home/rival town pairings',flush=True)
# Version-8 migration preserves the exact old 1,356-byte prefix except version.
wr(current()+v[164],0x15555);wr(current()+v[192],0x55,1)
for bit_id in [0,72,78,84,94,109,112,147,182,200,207,208]:
 address=current()+v[114]+bit_id//8;wr(address,rd(address,1)|(1<<(bit_id%8)),1)
wr(current()+v[78],8,2)
expected=bytearray(rd(current()+i,1) for i in range(c[0]));expected[v[78]:v[78]+2]=c[3].to_bytes(2,'little')
for i in range(11):wr(current()+c[0]+i,255,1)
for i in range(4):wr(current()+c[1]+i,255,1)
call('GymLeaderState')
assert bytes(rd(current()+i,1) for i in range(c[0]))==expected
assert not any(rd(current()+c[0]+i,1) for i in range(11))
assert not any(rd(current()+c[1]+i,1) for i in range(4))
assert rd(current()+v[192],1)==0x55 and rd(current()+v[164],4)==0x15555
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
assert rd(current()+v[192],1)==0x55 and rd(current()+v[164],4)==0x15555
print('PASS exact version-8 prefix preservation, retained rival/tutor/history/research data, zeroed new masks and native flash migration',flush=True)
