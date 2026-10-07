from qa import *
import struct
v=struct.unpack('<22I',(ROOT/'verify-layout.bin').read_bytes());z=struct.unpack('<11I',(ROOT/'battle-layout.bin').read_bytes());p=symbols['gParties'];b=symbols['gBattleMons'];sc=0x0203e000
assert lib.boot(str(repo/'pokefirered.gba').encode())
def key(k=1,n=24):frames(2,k);frames(n)
def adv(n=45):
 for _ in range(n):key()
def wait_action():
 for n in range(12000):
  if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:return
  frames(1,1 if n%25==0 else 0)
 raise AssertionError('action input did not become ready')
# Actual trainer RUN action cannot cause defeat/whiteout.
state('pass-brock-retry',1);wait_action();wr('gActionSelectionCursor',3,1);key();adv(8)
assert rd('gBattleOutcome',1)==0 and rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1
snap('qa-pass-trainer-run');print('PASS trainer RUN blocked without voluntary loss.',flush=True)
# Native HUD refresh includes duration and scripted permanent fields, then hides.
state('pass-brock-retry',1);bs=rd('gBattleStruct');ft=symbols['gFieldTimers'];wr('gBattleWeather',v[10],2);wr(bs+v[0],3,1);wr(ft+v[1],v[11],1);wr(ft+v[1]+1,4,1);frames(2);snap('qa-pass-weather-terrain')
wr(bs+v[0],0,1);wr(ft+v[1]+1,0,1);frames(2);snap('qa-pass-weather-terrain-infinity')
wr('gBattleWeather',0,2);wr(ft+v[1],0,1);frames(2);snap('qa-pass-weather-terrain-hidden')
print('PASS native weather/terrain timed, permanent and cleared HUD refresh (screenshots).',flush=True)
# Full HP standard Poke Ball, no status, EZ ON through actual battle ball action.
state('pass-world',1);call('VarSet',0x408d,1);call('VarSet',0x408c,0);call('ClearBag');call('AddBagItem',1,5);call('ZeroPlayerPartyMons');call('ScriptGiveMon',59,60,0)
call('CreateWildMon',150,5);call('BattleSetup_StartWildBattle');frames(600);wait_action();assert call('CanThrowBall')
wr('gLastThrownBall',1,2);wr('gBallToDisplay',1,2);frames(3,256);frames(3);frames(1200);adv(130)
snap('qa-pass-ez-capture');print('Capture outcome',rd('gBattleOutcome',1),'balls',call('CountTotalItemQuantityInBag',1),flush=True)
assert rd('gBattleOutcome',1)==7
assert call('CheckBagHasItem',1,4) and not call('CheckBagHasItem',1,5)
print('PASS actual EZ legal full-HP Mewtwo capture with a consumed Poke Ball.',flush=True)
# Native used-area restriction remains independent of EZ.
state('pass-world',1);call('VarSet',0x408d,1);call('VarSet',0x408c,1);call('CreateWildMon',19,5);call('BattleSetup_StartWildBattle');call('CreateWildMon',21,5);call('BattleSetup_StartWildBattle');frames(500);assert not call('CanThrowBall');print('PASS EZ does not bypass used Nuzlocke area.',flush=True)
