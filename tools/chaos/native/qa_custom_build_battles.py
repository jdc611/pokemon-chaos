"""Custom Build actual opening outcomes and Oak Champion/Hall of Fame path.
Controlled battle fixtures validate progression, not balance.
"""
from qa_custom_build_ui import *
z=struct.unpack('<11I',(ROOT/'battle-layout.bin').read_bytes());b=symbols['gBattleMons']
def key(k=1,n=26):frames(8,k);frames(n)
def enter_lab():
 state('custom-ui-six',1)
 for i in range(6):
  for m in range(4):call('SetMonMoveSlot',p+i*MS,165,m)
 frames(18,128);frames(60)
 for _ in range(120):
  key()
  if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1 and rd('gBattlersCount',1)==2:break
 for _ in range(50):key()
 assert rd('gPartiesCount',1)==6 and rd(symbols['gPartiesCount']+1,1)==6
for win in (False,True):
 enter_lab();s=rd('gSaveBlock3Ptr')
 call('VarSet',NUZ,1)
 for i in range(6):
  setdata(p+i*MS,HP,data(p+i*MS,MAXHP) if win or i==0 else 0)
 if win:
  for i in range(6):setdata(p+(6+i)*MS,HP,1)
 for turn in range(1500):
  if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:
   for battler in range(2):
    addr=b+battler*z[0]
    if win:
     if battler==1:
      if rd(addr+z[3],2)>1:wr(addr+z[3],1,2)
     else:wr(addr+z[3],rd(addr+z[4],2),2)
    else:
     if battler==1:wr(addr+z[3],rd(addr+z[4],2),2)
     elif turn==0:wr(addr+z[3],1,2)
    if not battler:
     wr(addr+z[1],165,2) # Struggle: controlled attack, independent of draft moves.
     wr(addr+z[2],80,1)
   if not win:
    for i in range(6):setdata(p+(6+i)*MS,HP,data(p+(6+i)*MS,MAXHP))
  assert not call('CanDynamax',0) and not call('CanDynamax',1)
  key()
  if rd(s+DRAFT+ROUND,1)==6 and not call('OaksDraftOpeningBattle') and rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1:break
 else:raise AssertionError(('opening did not finish',win,rd('gBattleOutcome',1)))
 assert rd('gBattleOutcome',1)==(1 if win else 2)
 assert rd('gPartiesCount',1)==6 and all(data(p+i*MS,HP)==data(p+i*MS,MAXHP) for i in range(6))
 for _ in range(80):
  key()
  if call('VarGet',LABSCENE)==4 and not call('ArePlayerFieldControlsLocked'):break
 assert call('VarGet',LABSCENE)==4 and not call('ArePlayerFieldControlsLocked')
 state('draft-after-opening')
 print('PASS real 6v6 opening outcome, full heal and Nuzlocke exemption',win,turn,flush=True)
# Enter the real Champion room and verify the Oak presentation/team.
state('custom-ui-six',1);call('OaksDraftFinishOpening')
call('ScriptContext_Init');call('UnlockPlayerFieldControls')
for i in range(6):
 call('OaksDraftCreateCandidate',p+i*MS,i*3+rd(s+DRAFT+PICKS+i,1),100)
 for m in range(4):call('SetMonMoveSlot',p+i*MS,165,m)
import json
maps=json.loads((repo/'data/maps/map_groups.json').read_text())
for group,keyname in enumerate(maps['group_order']):
 if 'PokemonLeague_ChampionsRoom_Frlg' in maps[keyname]:num=maps[keyname].index('PokemonLeague_ChampionsRoom_Frlg');break
call('SetWarpDestinationToMapWarp',group,num,255);wr(symbols['sWarpDestination']+4,6,2);wr(symbols['sWarpDestination']+6,18,2);call('DoWarp');frames(230)
for turn in range(300):
 key()
 if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1 and rd('gBattlersCount',1)==2:break
else:raise AssertionError('Oak Champion battle did not begin')
assert rd(symbols['gPartiesCount']+1,1)==6
snap('draft-oak-champion')
# The team is evolved from the six retained identities, not regenerated.
for i in range(6):
 pick=rd(s+DRAFT+PICKS+i,1);rival=rd(s+DRAFT+RIVALPICKS+i,1)
 call('OaksDraftCreateCandidate',p+12*MS,i*3+3-pick-rival,5)
 assert data(p+(6+i)*MS,PID)==data(p+12*MS,PID)
print('PASS actual Oak Champion battle: six retained drafted identities',flush=True)
for i in range(6):
 setdata(p+(6+i)*MS,HP,1)
 for m in range(4):call('SetMonMoveSlot',p+(6+i)*MS,150,m)
for turn in range(2600):
 if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:
  for battler in range(2):
   addr=b+battler*z[0]
   if battler==1:
    if rd(addr+z[3],2)>1:wr(addr+z[3],1,2)
   else:wr(addr+z[3],rd(addr+z[4],2),2)
   wr(addr+z[5],0)
   if not battler:wr(addr+z[1],165,2);wr(addr+z[2],80,1)
   else:
    for m in range(4):wr(addr+z[1]+m*2,150,2);wr(addr+z[2]+m,80,1)
 key()
 if ui():break
else:
 state('draft-hof-stalled');snap('draft-oak-stalled');print('oak stalled',rd('gBattleOutcome',1),hex(rd(symbols['gMain']+4)),rd(rd('gSaveBlock1Ptr')+4,1),rd(rd('gSaveBlock1Ptr')+5,1),flush=True)
 print('draft',call('IsOaksDraft'),'ctx',hex(rd(symbols['sGlobalScriptContext']+8)),'native',hex(rd(symbols['sGlobalScriptContext']+4)), 'result',rd('gSpecialVar_Result',2),flush=True)
 raise AssertionError('Oak victory did not reach Hall of Fame comparison')
frames(100);snap('draft-hof-report');tap(2,100)
for turn in range(4000):
 key()
 if rd(rd('gSaveBlock3Ptr')+1457,1):break
assert rd(rd('gSaveBlock3Ptr')+1457,1), 'Hall of Fame did not snapshot the final team'
print('PASS Oak victory reaches final comparison and actual Hall of Fame recording',flush=True)
