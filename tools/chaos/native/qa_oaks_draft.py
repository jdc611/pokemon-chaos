"""Native Draft integration checks; controlled fixtures do not prove balance."""
from qa import *
import struct
v=struct.unpack('<42I',(ROOT/'draft-layout.bin').read_bytes())
(SIZE,DRAFT,SEED,START,BST,ABILITY,MOVESET,MGM,MS,SP,LV,HP,NICK,MEMBER,REROLLED,ABILITY_OVERRIDE,PID,IV,MOVE,MAXHP,HELD,
 RIVAL,ROUTE_RIVAL,CHAMP,TM1,TM100,HMCUT,BADGE2,NUZ,RIVALNAME,DRAFTMODE,ROOMGROUP,ROOMNUM,LABGROUP,LABNUM,
 ROUND,PICKS,RIVALPICKS,GYMTMS,LABSCENE,HPEV,GETMON)=v
assert lib.boot(str(repo/'pokefirered.gba').encode())
frames(1000);frames(3,8);frames(100)
wr(rd('gSaveBlock1Ptr')+RIVALNAME,255,1);wr(rd('gSaveBlock2Ptr'),255,1)
wr('gDebugForceKantoNewGame',1,1)
call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(600)
s=rd('gSaveBlock3Ptr');p=symbols['gParties'];scratch=0x0203f000
state('draft-base')
def data(mon,field):return call('GetMonData3',mon,field,0)
def setdata(mon,field,value):wr(scratch,value);call('SetMonData',mon,field,scratch)
def start(seed=24680,bst=0,ability=0,moves=0,mgm=0):
 state('draft-base',1);wr(s+SEED,seed);wr(s+START,DRAFTMODE,1);wr(s+BST,bst,1);wr(s+ABILITY,ability,1);wr(s+MOVESET,moves,1);wr(s+MGM,mgm,1)
 call('OaksDraftInitialize');call('ZeroPlayerPartyMons');call('ScriptContext_Init');call('UnlockPlayerFieldControls')
 assert call('IsOaksDraft') and not call('IsIronmonRun')
def bytesat(addr,count):return bytes(rd(addr+i,1) for i in range(count))
for bst in range(3):
 for random_traits in range(2):
  start(24680,bst,random_traits,random_traits,random_traits)
  call('OaksDraftGenerateSpecies',scratch)
  species=[rd(scratch+i*2,2) for i in range(18)]
  assert len(set(species))==18 and all(call('OaksDraftSpeciesEligible',sp) for sp in species)
  for round in range(6):
   assert call('OaksDraftCommit',round%3)
   mon=p+round*MS
   assert data(mon,SP)==species[round*3+round%3] and data(mon,LV)==5 and data(mon,MEMBER)
   assert any(call('IronmonMoveIsStarterAttack',data(mon,MOVE+i)) for i in range(4))
   if random_traits:assert data(mon,IV)==31 and data(mon,HPEV)==0
  assert rd('gPartiesCount',1)==6 and not call('OaksDraftCommit',0)
  names=[]
  for i in range(6):
   call('GetMonData3',p+i*MS,NICK,scratch);names.append(bytesat(scratch,12))
  assert len(set(names))==6
  before=bytesat(p,6*MS)
  # Central nickname writes are rejected for existing companions.
  wr(scratch,0,1);call('SetMonData',p,NICK,scratch);assert bytesat(p,6*MS)==before
  call('SavePlayerParty');assert call('TrySavingData',0)==1
  expected=bytesat(s,SIZE)
  call('ClearSav3');assert call('LoadGameSave',0)==1
  assert bytesat(rd('gSaveBlock3Ptr'),SIZE)==expected
  call('LoadPlayerParty');assert bytesat(p,6*MS)==before
  for trainer in (RIVAL,ROUTE_RIVAL,CHAMP):
   call('CreateNPCTrainerParty',p+6*MS,trainer)
   assert all(data(p+(6+i)*MS,SP) for i in range(6))
   for i in range(6):
    pick=rd(s+DRAFT+PICKS+i,1);rival=rd(s+DRAFT+RIVALPICKS+i,1)
    index=i*3+(3-pick-rival if trainer==CHAMP else rival)
    call('OaksDraftCreateCandidate',p+12*MS,index,5)
    assert data(p+(6+i)*MS,PID)==data(p+12*MS,PID)
  print('PASS draft options, all choices, identities, name lock, save and three trainer stages',bst,random_traits,flush=True)
start();call('OaksDraftCommit',0);call('SavePlayerParty');assert call('TrySavingData',0)==1
call('ClearSav3');assert call('LoadGameSave',0)==1;call('LoadPlayerParty');assert rd(s+DRAFT+ROUND,1)==1
for i in range(1,6):assert call('OaksDraftCommit',i%3)
assert call('ScriptGiveMon',species[1],5,0)==2
assert call('ScriptGiveEgg',species[1])==2
assert call('GiveCapturedMonToPlayer',p)==2
# One living companion for one level-matched replacement; no vacancy trades.
oldpid=data(p,PID);oldlevel=data(p,LV)
assert call('OaksDraftGiftTrade',0,species[1],0)
assert data(p,LV)==oldlevel and data(p,PID)!=oldpid and rd('gPartiesCount',1)==6
setdata(p,HP,0);assert not call('OaksDraftGiftTrade',0,species[2],0)
call('HealPlayerParty');call('SetCoins',5000)
assert call('OaksDraftRerollAbility',0) and call('GetCoins')==3000 and data(p,REROLLED)
assert not call('OaksDraftRerollAbility',0) and call('GetCoins')==3000
for gym in range(8):
 wr('gSpecialVar_0x8004',gym,2);call('OaksDraftGiveGymTM');assert rd('gSpecialVar_Result',2)==1
 tm=rd(s+DRAFT+GYMTMS+2*gym,2);assert TM1<=tm<=TM100
 assert call('OaksDraftTMReserved',tm) and call('OaksDraftExternalItem',tm)!=tm
move=call('ItemIdToBattleMoveId',tm);assert any(call('CanLearnTeachableMove',data(p+i*MS,SP),move) for i in range(6))
print('PASS gifts, capture/egg blocking, living-only trade, ability cost/one-time marker and eight compatible exclusive TMs',flush=True)
# Nuzlocke opening exception versus subsequent permanent deaths.
call('VarSet',NUZ,1);setdata(p,HP,0)
assert not call('Nuzlocke_ProcessBattleDeaths') and rd('gPartiesCount',1)==6
call('HealPlayerParty');call('OaksDraftFinishOpening');setdata(p,HP,0)
assert call('Nuzlocke_ProcessBattleDeaths') and rd('gPartiesCount',1)==5
assert not call('OaksDraftGiftTrade',5,species[1],0)
print('PASS opening Nuzlocke exemption, later grave removal and no vacancy exchange',flush=True)
