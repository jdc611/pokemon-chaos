"""Actual ball throw and nickname paths; native setup controls encounter only."""
from qa_ironmon_core import *
PARTYSTATE,PARTYSTATESIZE=struct.unpack('<2I',(ROOT/'ironmon-capture-layout.bin').read_bytes())
def key(k=1,n=25):frames(12,k);frames(n)
for mode in (4,5):
 for nickname in (False,True):
  start(mode);call('IronmonGiveStarter',BULBA)
  oldspecies=data(p,SP)
  call('ClearBag');call('AddBagItem',4,3)
  call('CreateWildMon',PIKA,12);call('BattleSetup_StartWildBattle')
  for _ in range(600):
   if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
   key()
  else:raise AssertionError('capture action not reached')
  # Simulate the outgoing main's valid battle-form restoration marker.
  # changedSpecies occupies bits 14..24 in the ARM APCS PartyState word.
  outgoing=rd('gBattleStruct')+PARTYSTATE
  wr(outgoing,(rd(outgoing)&~(0x7ff<<14))|(BULBA<<14))
  frames(60);key(16);key(1,180);key(16,120);snap('ironmon-capture-bag');key(1,60);key(1,1200)
  named=False
  for _ in range(350):
   cb=rd(symbols['gMain']+4)
   if cb==symbols['CB2_NamingScreen']|1:
    assert nickname
    frames(120);call('SetCursorPos',0,0);key();key(8,100);key(1,180);named=True
   elif cb==symbols['CB2_Overworld']|1 and rd('gBattleOutcome',1)==7:break
   else:key(1 if nickname and not named else 2)
  else:
   snap('ironmon-capture-stalled');raise AssertionError((mode,nickname,hex(cb)))
  assert rd('gPartiesCount',1)==1 and data(p,SP)==PIKA
  assert rd(s+IM+RETIRED,2)==1
  assert call('GetBoxMonDataAt',13,0,SP)==oldspecies
  assert named==nickname
  call('SavePlayerParty');assert call('TrySavingData',0)==1
  expected=bytes(rd(p+i,1) for i in range(MONSIZE))
  assert call('LoadGameSave',0)==1
  call('LoadPlayerParty')
  assert expected==bytes(rd(p+i,1) for i in range(MONSIZE))
  print('PASS actual capture pivot/nickname/retirement/old-form isolation/flash reload',mode,nickname,flush=True)
