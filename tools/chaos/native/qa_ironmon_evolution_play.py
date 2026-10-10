"""Actual evolution rendering/cancel/sole-party tests with controlled targets.

The Nincada->Ninjask fixture deliberately selects that original split target to
exercise extra acquisition. A separate fixture uses the seeded actual target.
"""
from qa_ironmon_core import *
SI,GROWTH,MI,EXP,_=struct.unpack('<5I',(ROOT/'ironmon-balance-layout.bin').read_bytes()[:20])
for mode in (1,4,5):
 start(4 if mode==1 else mode);call('IronmonGiveStarter',290)
 if mode==1:wr(s+DIFF,1,1);call('IronmonInitializeRun')
 growth=rd(symbols['gSpeciesInfo']+290*SI+GROWTH,1)
 setdata(p,EXP,rd(symbols['gExperienceTables']+(growth*101+20)*4));call('CalculateMonStats',p)
 call('AddBagItem',1,2)
 wr('gCB2_AfterEvolution',symbols['CB2_ReturnToField']|1)
 call('BeginEvolutionScene',p,291,mode!=1,0)
 for step in range(900):
  # B is held throughout the cancellable animation. Occasional A advances
  # text and acknowledges move-learning; target/party data is never injected.
  frames(20,2);frames(3,1);frames(5)
  if rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1:break
 else:
  snap('ironmon-evolution-stalled');raise AssertionError((mode,hex(rd(symbols['gMain']+4))))
 assert data(p,SP)==291,(mode,data(p,SP))
 assert rd('gPartiesCount',1)==(2 if mode==1 else 1),(mode,rd('gPartiesCount',1))
 assert data(p,HPIV)==31 and data(p,HPEV)==0
 print('PASS actual evolution, IronMON B cannot cancel and split reward denied; ordinary split retained',mode,flush=True)
for mode in (4,5):
 start(mode);call('IronmonGiveStarter',BULBA)
 target=call('GetRandomEvolutionTargetForSettings',BULBA,mode,123456)
 level=call('GetRandomEvolutionLevelForSettings',BULBA,123456)
 assert target
 growth=rd(symbols['gSpeciesInfo']+BULBA*SI+GROWTH,1)
 setdata(p,EXP,rd(symbols['gExperienceTables']+(growth*101+level)*4));call('CalculateMonStats',p)
 item=data(p,ITEM);nature=call('GetNature',p)
 wr('gCB2_AfterEvolution',symbols['CB2_ReturnToField']|1)
 call('BeginEvolutionScene',p,target,1,0)
 for _ in range(900):
  frames(20,2);frames(3,1);frames(5)
  if rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1:break
 else:raise AssertionError(('seeded evolution return',mode))
 assert data(p,SP)==target and rd('gPartiesCount',1)==1
 assert data(p,ITEM)==item and call('GetNature',p)==nature
 assert all(data(p,HPIV+i)==31 and data(p,HPEV+i)==0 for i in range(6))
 assert call('GetMonAbility',p)>0
 call('SavePlayerParty');assert call('TrySavingData',0)==1
 saved=bytes(rd(p+i,1) for i in range(MONSIZE))
 assert call('LoadGameSave',0)==1;call('LoadPlayerParty')
 assert saved==bytes(rd(p+i,1) for i in range(MONSIZE))
 print('PASS actual seeded evolution, item/nature/MGM and flash reload',mode,BULBA,target,flush=True)
