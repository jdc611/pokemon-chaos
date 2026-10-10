"""Actual level-up relearner -> Summary replacement -> relearner return."""
from qa_ironmon_core import *
SI,GROWTH,MI,EXP,_=struct.unpack('<5I',(ROOT/'ironmon-balance-layout.bin').read_bytes()[:20])
def key(k=1,n=25):frames(3,k);frames(n)
for mode in (4,5):
 start(mode);call('IronmonGiveStarter',BULBA)
 growth=rd(symbols['gSpeciesInfo']+BULBA*SI+GROWTH,1)
 setdata(p,EXP,rd(symbols['gExperienceTables']+(growth*101+30)*4));call('CalculateMonStats',p)
 for slot in range(4):call('SetMonMoveSlot',p,TACKLE,slot)
 setdata(p,PP,1)
 item=data(p,ITEM);nature=call('GetNature',p);ability=call('GetMonAbility',p)
 wr('gSpecialVar_0x8004',0,2);wr('gMoveRelearnerState',0,1)
 call('TeachMoveRelearnerMove');frames(150);snap('ironmon-relearner-open')
 selected=0
 for step in range(450):
  key()
  if data(p,MOVE)!=TACKLE:
   selected=data(p,MOVE);break
 else:
  snap('ironmon-relearner-stalled');raise AssertionError(('replacement stalled',mode,hex(rd(symbols['gMain']+4))))
 assert data(p,PP)==1,(mode,selected,data(p,PP))
 assert [data(p,MOVE+i) for i in range(1,4)]==[TACKLE]*3
 for _ in range(120):key(2)
 key(1,180)
 for _ in range(80):
  key(2)
  if rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1:break
 else:raise AssertionError(('relearner failed field return',mode,hex(rd(symbols['gMain']+4))))
 assert data(p,ITEM)==item and call('GetNature',p)==nature and call('GetMonAbility',p)==ability
 assert all(data(p,HPIV+i)==31 and data(p,HPEV+i)==0 for i in range(6))
 print('PASS actual relearner UI replacement/PP retention/field return/unrelated data',mode,selected,flush=True)
