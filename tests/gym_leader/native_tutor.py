"""Native permanent GLA lessons, payment, compatibility and relearner limits."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
assert call('ScriptGiveMon',v[160],5,0)==0
mon=symbols['gParties']+3*v[5]
assert call('GetMonData2',mon,v[10])==v[160]
buf=call('Alloc_',4096,0);assert buf
def move_list(fn):
 n=call(fn,mon,buf);assert n<2048
 return [rd(buf+2*i,2) for i in range(n)]
assert not call('GymLeaderTutorUnlocked',v[159])
assert not move_list('GetRelearnerTutorMoves')
assert v[165] not in move_list('GetRelearnerLevelUpMoves')
assert v[165] not in move_list('GetRelearnerTMMoves')
assert call('AddBagItem',v[166],1)
assert v[165] in move_list('GetRelearnerTMMoves')
call('Free',buf)
print('PASS current-level move recall, owned-TM recall and no unlicensed bulk tutor access',flush=True)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(100):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('walk blocked',pos(),(x,y)))
def offers():
 return [rd(symbols['sTutors']+rd(symbols['sTutorOffers']+i,1)*4,2) for i in range(rd('sTutorCount',1))]
def tutor():
 walkto(6,9);walkto(9,9);walkto(9,8);press(16);press(1)
 until(lambda:menu_count()>1)
def ice():choose(offers().index(v[159]))
# Rank-one visitors can leave the unavailable desk without a waitstate lock.
walkto(6,9);walkto(9,9);walkto(9,8);press(16);press(1)
for _ in range(80):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
else:raise AssertionError('rank-one empty tutor desk did not return control')
assert not rd(current()+v[164])
wr(current()+v[2],3,1)
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],50000)
tutor();assert v[159] in offers()
for move in offers():assert any(call('CanLearnTeachableMove',call('GetMonData2',symbols['gParties']+i*v[5],v[10]),move) for i in range(4))
snap('gla-tutor-menu')
ice();until(yesno);choose(1);until(lambda:menu_count()>1)
assert not call('GymLeaderTutorUnlocked',v[159])
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==50000
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],0)
ice();until(yesno);choose(0);until(lambda:menu_count()>1)
assert not call('GymLeaderTutorUnlocked',v[159])
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==0
call('SetMoney',rd('gSaveBlock1Ptr')+v[64],50000)
wr(current()+v[136],100,2)
ice();until(yesno);choose(0)
until(lambda:task('Task_PokeStorageMain')<16)
frames(150)
assert call('GymLeaderTutorUnlocked',v[159])
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==36500
snap('gla-tutor-native-selector')
state('tutor-native-selector')
print('PASS native tutor compatible offers, declined unlock, insufficient funds, paid honorable license and native partner selector',flush=True)
# Select the fourth traveling partner through the actual PC/party selector.
for _ in range(5):press(128,25)
press(1,120)
assert rd('sCursorArea',1)==1
for _ in range(3):press(128,25)
assert rd('sCursorPosition',1)==3
press(1,30);press(1,30)
until(lambda:any(call('GetMonData2',mon,v[38]+j)==v[159] for j in range(4)))
until(lambda:menu_count()>1)
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==36500
assert call('TrySavingData',0)==1
wr(current()+v[164],0)
assert call('LoadGameSave',0)==1 and call('GymLeaderTutorUnlocked',v[159])
# Version-six saves gain only the new license field, preserving Safari,
# vouchers, travel permits and every existing byte of Gym data.
wr(current()+v[105],v[160],2)
wr(current()+v[78],6,2)
expected=bytearray(rd(current()+i,1) for i in range(v[1]))
expected[v[78]:v[78]+2]=v[100].to_bytes(2,'little')
expected[v[164]:v[164]+4]=bytes(4)
call('GymLeaderState')
assert bytes(rd(current()+i,1) for i in range(v[1]))==bytes(expected)
assert call('LoadGameSave',0)==1 and call('GymLeaderTutorUnlocked',v[159])
print('PASS actual party lesson, one-time cost, native flash license and exact version-six migration preservation',flush=True)
# Put a compatible partner in the PC as a storage fixture; the actual lesson
# selection, compatibility check and move writeback use the native UI.
storage=rd('gPokemonStoragePtr')+v[34]
for i in range(v[33]):wr(storage+i,rd(mon+i,1),1)
ptr=symbols['gStringVar4'];wr(ptr,0,2)
for j in range(4):
 if call('GetBoxMonData2',storage,v[38]+j)==v[159]:call('SetBoxMonData',storage,v[38]+j,ptr)
assert all(call('GetBoxMonData2',storage,v[38]+j)!=v[159] for j in range(4))
wr(current()+v[2],1,1) # Paid lessons stay available after demotion.
ice();until(yesno);choose(0)
until(lambda:task('Task_PokeStorageMain')<16);frames(100)
assert rd('sCursorArea',1)==0 and rd('sCursorPosition',1)==0
press(1,60);press(1,60)
# Avoid injecting native getter calls while the box selector/teaching
# callbacks are active; inspect the result after the field menu returns.
until(lambda:menu_count()>1)
# Returning from the PC may relocate save/storage memory. Resolve its
# current address before inspecting the actual taught partner.
storage=rd('gPokemonStoragePtr')+v[34]
assert any(call('GetBoxMonData2',storage,v[38]+j)==v[159] for j in range(4))
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==36500
assert call('GymLeaderTutorUnlocked',v[159])
snap('gla-tutor-reused-pc')
print('PASS free repeat lesson to an actual PC partner via native selector and move-learning flow',flush=True)
