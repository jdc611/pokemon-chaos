"""Controlled milestone saves with real unmodified generated combatants.

Controlled pivot species/EXP fixture, not a route playthrough or fairness proof. A greedy owned-move policy uses no
opponent hidden information, consumables or stat/HP/ability modifications.
"""
from qa_ironmon_core import *
import json
SI,GROWTH,MI,EXP,MISTY,STATUS_CATEGORY,PHYSICAL,SPECIAL,ATK,SPATK=struct.unpack('<10I',(ROOT/'ironmon-balance-layout.bin').read_bytes())
_,A,_,DA,_,_,_,_=struct.unpack('<8I',(ROOT/'ironmon-singles-layout.bin').read_bytes())
def key(k=1):frames(3,k);frames(25)
results=[]
for mode in (4,5):
 for seed in (1,2,24680):
  for pivot_species in (6,68,260,376,445):
   for trainer,level in ((BROCK,15),(MISTY,29)):
    start(mode,seed);call('ResolveChaosOakStarters');species=call('GetStarterPokemon',0)
    call('IronmonGiveStarter',species)
    call('CreateWildMon',pivot_species,10);call('IronmonAcceptCapture',p+6*MONSIZE);species=pivot_species
    growth=rd(symbols['gSpeciesInfo']+species*SI+GROWTH,1)
    setdata(p,EXP,rd(symbols['gExperienceTables']+(growth*101+level)*4))
    call('CalculateMonStats',p);call('GiveMonInitialMoveset',p);assert data(p,LV)==level
    call('InitTrainerBattleParameter');wr(symbols['gTrainerBattleParameter']+A,trainer,2)
    wr(symbols['gTrainerBattleParameter']+DA,symbols['Text_ChaosJessieDefeat'])
    wr('gNoOfApproachingTrainers',1,1);call('BattleSetup_StartTrainerBattle')
    entered=False;turns=0
    for step in range(2400):
     cb=rd(symbols['gMain']+4);controller=rd('gBattlerControllerFuncs')
     if cb==symbols['BattleMainCB2']|1:entered=True
     if entered and cb in (symbols['CB2_Overworld']|1,symbols['CB2_IronmonRunOver']|1):break
     if controller==symbols['HandleInputChooseMove']|1:
      choices=[]
      for slot in range(4):
       move=data(p,MOVE+slot);pp=data(p,PP+slot)
       if not move or not pp:continue
       word=rd(symbols['gMovesInfo']+move*MI+8)
       category=(word>>21)&3;power=word>>23
       score=power*data(p,ATK if category==PHYSICAL else SPATK) if category in (PHYSICAL,SPECIAL) else -1
       choices.append((score,slot))
      chosen=max(choices)[1] if choices else 0
      cursor=rd('gMoveSelectionCursor',1)
      if (cursor&1)!=(chosen&1):key(16 if chosen&1 else 32)
      if (cursor&2)!=(chosen&2):key(128 if chosen&2 else 64)
      key();turns+=1
     else:key()
    else:
     snap('ironmon-boss-stalled');raise AssertionError((mode,seed,trainer,hex(cb),step))
    outcome=rd('gBattleOutcome',1)
    assert outcome in (1,2,3),outcome
    assert rd('gPartiesCount',1)==1
    if outcome!=1:assert rd(s+IM+ENDED,1)
    row=dict(mode=mode,seed=seed,starter=species,controlled_level=level,pivot=True,trainer=trainer,outcome=outcome,turns=turns)
    results.append(row);print('PLAYED',row,flush=True)
(ROOT/'boss-results.json').write_text(json.dumps(dict(limitations=__doc__,results=results),indent=2)+'\n')
print('PASS representative Brock/Misty generated battles complete; no hidden opponent edits; losses terminate run.',flush=True)
