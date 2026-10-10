"""Actual Hardcore new-game/lab battle input and controlled low-HP loss."""
from qa_ironmon_core import *
state('ironmon-core-base',1)
wr('gRunSetupDifficulty',5,1);wr('gRunSetupWorldSeed',24680)
wr('gRunSetupStartRegion',1,1);wr('gRunSetupNuzlocke',0,1)
wr('gDebugForceKantoNewGame',1,1)
call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(600)
s=rd('gSaveBlock3Ptr')
for step in range(150):
 frames(3,2);frames(30)
 if rd('gBattleTypeFlags') & 8:break
frames(64,128);frames(20)
for step in range(100):
 frames(3,2);frames(30)
 if rd('gBattleTypeFlags') & 8:break
assert rd('gBattleTypeFlags') & 8, 'lab rival battle did not start'
assert rd('gPartiesCount',1)==1 and call('IsIronmonHardcore')
# Controlled injury fixture: retain the real generated starter, opponent, moves
# and AI, but lower the main to 1 HP before battle initialization.
setdata(p,HP,1)
for turn in range(300):
 frames(3,1);frames(30)
 if rd(s+IM+ENDED,1):break
else: raise AssertionError('no loss observed in the controlled lab battle')
for _ in range(150):
 frames(10)
 if rd(symbols['gMain']+4) == symbols['RunOverInput']|1:break
else:raise AssertionError('loss did not reach terminal input')
assert data(p,HP)==0 and rd('gPartiesCount',1)==1
snap('ironmon-lab-loss')
assert call('LoadGameSave',0)==1 and rd(s+IM+ENDED,1)==1
print('PASS actual Hardcore lab rival battle with a controlled 1-HP main: faint ends run, terminal saves, no scripted restoration.')
