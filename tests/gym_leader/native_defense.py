"""Native opening plus real Gym Guide registration/defense smoke route.
Not a full campaign or balance test. Names are the opening fixture's only bypass.
"""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
assert rd(current()+v[26],1)==3
call('GleWarpHome');frames(240)
# Move from the introduction spawn to the Gym Guide and interact normally.
frames(32,16);frames(64,128);frames(20)
press(1)
until(lambda:menu_count()==8)
snap('guide-menu')
choose(1)
until(lambda:call('GymLeaderRegistering'))
frames(160);snap('guide-register-party')
# Register Snover, the third Grass candidate, through the native selector.
press(16,25);press(128,25)
press(1,25);press(1,25)
press(128)
press(1,100)
until(lambda:menu_count()==8)
assert rd(current()+v[26],1)==1
snap('guide-registered')
# Accept one real defense through the guide.
choose(0);until(yesno);choose(0)
frames(200);snap('guide-defense-intro')
assert call('GymLeaderBattleTrainer')!=0
before=rd(current()+v[29],2)
# Ordinary Fight/Powder Snow controller input; no forced outcome or altered HP.
for i in range(1200):
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseMove']|1 and rd('gMoveSelectionCursor',1)==0:
  press(16,18) # Snover learns Leer first; choose Powder Snow rather than spam Leer.
 press(1,18)
 if i==40:snap('guide-defense-battle')
 if call('GymLeaderBattleTrainer')==0:break
else:
 snap('guide-defense-timeout');raise AssertionError('defense did not return')
until(lambda:menu_count()==8)
assert rd('gPartiesCount',1)==3, 'traveling party was not restored'
assert rd(current()+v[29],2)==before+1, ('expected win',rd('gBattleOutcome',1))
for i in range(3):
 mon=symbols['gParties']+i*v[5]
 assert rd(mon+v[13],2)==rd(mon+v[14],2)
assert rd(current()+v[32],1)>0
snap('guide-defense-complete')
print('PASS native guide registration, actual defense victory, traveling-party restore, healing and queue',flush=True)

# Direct adapter failure/transaction scenarios, independently of battle balance.
state('gle-defense-baseline')
def copy_bytes(dst,src,n):
 data=bytes(rd(src+i,1) for i in range(n))
 for i,b in enumerate(data):wr(dst+i,b,1)
def set_data(mon,field,value,box=False):
 scratch=call('Alloc_',4,0);assert scratch
 wr(scratch,value)
 call('SetBoxMonData' if box else 'SetMonData',mon,field,scratch)
 call('Free',scratch)
party=symbols['gParties'];box=rd('gPokemonStoragePtr')+v[34]
# Deposit the sole registered defender, leaving two unrestricted travelers.
copy_bytes(box,party+2*v[5],v[33])
for i in range(v[5]):wr(party+2*v[5]+i,0,1)
wr('gPartiesCount',2,1)
travel=bytes(rd(party+i,1) for i in range(v[5]*2))
old_exp=call('GetBoxMonData2',box,v[35])
assert call('GymLeaderPrepareDefense')==1
assert rd('gPartiesCount',1)==1
set_data(party,v[35],old_exp+10)
wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense')
assert rd('gPartiesCount',1)==2
assert bytes(rd(party+i,1) for i in range(v[5]*2))==travel
assert call('GetBoxMonData2',box,v[35])==old_exp+10
assert rd(current()+v[30],2)==1
# A demoted high-level defender is normalized only for the match.
set_data(box,v[35],1000000,True)
assert call('GymLeaderPrepareDefense')==1
assert call('GetMonData2',party,v[11])==15
wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense')
assert call('GetBoxMonData2',box,v[35])==1000000
assert bytes(rd(party+i,1) for i in range(v[5]*2))==travel
# An invalid type is refused without consuming the queue or replacing travelers.
set_data(box,v[10],4,True) # Charmander cannot defend a Grass Gym without signature.
for j in range(4):set_data(box,v[38]+j,0,True)
q=rd(current()+v[32],1)
assert call('GymLeaderPrepareDefense')==2
assert rd(current()+v[32],1)==q
assert bytes(rd(party+i,1) for i in range(v[5]*2))==travel
state('gle-defense-baseline',1)
s= current();wr(s+v[27],rd(s+v[27])^0x12345678)
assert call('GymLeaderPrepareDefense')==3
state('gle-defense-baseline',1)
# Persist the actual registered identities through native flash sectors.
s=current();expected=bytes(rd(s+i,1) for i in range(v[1]))
assert call('TrySavingData',0)==1
for i in range(v[1]):wr(s+i,0,1)
assert call('LoadGameSave',0)==1
s=current();assert bytes(rd(s+i,1) for i in range(v[1]))==expected
print('PASS PC identity lookup, loss, EXP writeback, temporary cap normalization, invalid/missing team and registration flash save',flush=True)
