from qa import *
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
def key(k=1,n=24):frames(2,k);frames(n)
call('VarSet',0x408d,1);call('VarSet',0x408c,0);call('ClearBag');call('AddBagItem',1,50);call('ZeroPlayerPartyMons')
for s in [59,3,9,6,25,123]:call('ScriptGiveMon',s,30,0)
call('CreateWildMon',150,5);call('BattleSetup_StartWildBattle');frames(600)
for n in range(12000):
 if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
 frames(1,1 if n%25==0 else 0)
frames(80);wr('gLastThrownBall',1,2);wr('gBallToDisplay',1,2);key(256,1200)
# Stop on each full-screen capture menu, preserving the native choice states.
for i in range(150):
 cb=rd(symbols['gMain']+4)
 if cb!=symbols['BattleMainCB2']|1:
  print('MENU',i,hex(cb),[(k,hex(v)) for k,v in symbols.items() if v|1==cb],flush=True);snap('v2-capture-choice');state('v2-capture-choice');break
 key()
else:raise AssertionError('No naming screen')
