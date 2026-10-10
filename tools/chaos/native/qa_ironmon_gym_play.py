"""Actual Brock/Misty leader scripts on controlled level-50 integration saves.

Mandatory trainer flags, pivot species and initial EXP are fixture setup. All
combatants retain generated stats, ability, items, moves and HP; rewards,
completion, locks and saves execute through the actual scripts. Not a full
walking playthrough or a balance proof.
"""
from qa_ironmon_core import *
import json
SI,GROWTH,MI,EXP,_,STATUS_CATEGORY,PHYSICAL,SPECIAL,ATK,SPATK=struct.unpack('<10I',(ROOT/'ironmon-balance-layout.bin').read_bytes())
rows=struct.unpack('<112I',(ROOT/'ironmon-world-layout.bin').read_bytes())
def key(k=1):frames(3,k);frames(25)
for mode in (4,5):
 for index,species,level,event in ((0,6,50,'PewterCity_Gym_EventScript_Brock'),(1,445,50,'CeruleanCity_Gym_EventScript_Misty')):
  start(mode,1);call('ResolveChaosOakStarters');call('IronmonGiveStarter',call('GetStarterPokemon',0))
  if index:call('FlagSet',BADGE1)
  call('CreateWildMon',species,10);call('IronmonAcceptCapture',p+6*MONSIZE)
  growth=rd(symbols['gSpeciesInfo']+species*SI+GROWTH,1)
  setdata(p,EXP,rd(symbols['gExperienceTables']+(growth*101+level)*4));call('CalculateMonStats',p);call('GiveMonInitialMoveset',p)
  section,group,num,leader,won,n,*trainers=rows[index*14:(index+1)*14]
  for trainer in trainers[:n]:call('FlagSet',trainer+0x500)
  call('SetWarpDestinationToMapWarp',group,num,255);wr(symbols['sWarpDestination']+4,6 if index==0 else 8,2);wr(symbols['sWarpDestination']+6,6 if index==0 else 7,2);call('DoWarp');frames(240)
  assert call('IronmonEscapeLocked') and not call('IronmonLeaderLocked',leader)
  # Face and interact with the real leader object through ordinary input.
  frames(3,64);frames(25);key()
  entered=False
  for step in range(3000):
   cb=rd(symbols['gMain']+4)
   if cb==symbols['BattleMainCB2']|1:entered=True
   if entered and cb==symbols['CB2_Overworld']|1 and not call('ArePlayerFieldControlsLocked'):break
   if rd(s+IM+ENDED,1):raise AssertionError(('milestone main lost',mode,index))
   if cb==symbols['BattleMainCB2']|1 and rd('gBattlerControllerFuncs')==symbols['HandleInputChooseMove']|1:
    choices=[]
    for slot in range(4):
     move=data(p,MOVE+slot);pp=data(p,PP+slot)
     if not move or not pp or not call('IronmonMoveIsStarterAttack',move):continue
     word=rd(symbols['gMovesInfo']+move*MI+8);category=(word>>21)&3;power=word>>23
     choices.append((power*data(p,ATK if category==PHYSICAL else SPATK) if category in (PHYSICAL,SPECIAL) else -1,slot))
    chosen=max(choices)[1] if choices else 0;cursor=rd('gMoveSelectionCursor',1)
    if cursor&1!=chosen&1:key(16 if chosen&1 else 32)
    if cursor&2!=chosen&2:key(128 if chosen&2 else 64)
    key()
   else:key()
  else:
   snap('ironmon-gym-stalled');raise AssertionError((mode,index,entered,hex(cb)))
  assert rd('gBattleOutcome',1)==1
  assert call('FlagGet',won) and call('FlagGet',BADGE1+index)
  assert not call('IronmonEscapeLocked') and call('HasTrainerBeenFought',leader)
  # Dialogue helpers reuse script slots; recompute the deterministic reward
  # for assertion without awarding an additional item.
  call('IronmonPrepareGymTM');reward=rd('gSpecialVar_0x8000',2)
  assert reward and call('CountTotalItemQuantityInBag',reward)==1,(mode,index,reward,call('CountTotalItemQuantityInBag',reward))
  call('IronmonPermitItemAward',reward,1)
  call('SavePlayerParty');assert call('TrySavingData',0)==1
  assert call('LoadGameSave',0)==1;call('LoadPlayerParty')
  assert call('FlagGet',won) and call('FlagGet',BADGE1+index) and not call('IronmonEscapeLocked')
  assert rd('gPartiesCount',1)==1 and data(p,SP)==species
  print('PASS actual Gym leader victory, badge/random TM/exit unlock/save',mode,index+1,species,reward,flush=True)
