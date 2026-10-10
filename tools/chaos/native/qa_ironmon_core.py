"""Internal core checkpoint. Native calls are not a complete gameplay test."""
from qa import *
import struct
v=struct.unpack('<35I', (ROOT/'ironmon-layout.bin').read_bytes())
SIZE,IM,SEED,DIFF,RETIRED,ENDED,MONSIZE,STORAGE=v[:8]
SP,LV,ITEM,HP,HPIV,HPEV,ATKIV,ATKEV,MOVE,PP,STATUS,MAXHP=v[8:20]
DB,TACKLE,ORAN,BULBA,PIKA,BADGE1,BADGE2,GIVEN,CANT,BROCK,MAPSECTION,N_SPECIES,MODE,IMSEED,RIVALNAME=v[20:]
assert lib.boot(str(repo/'pokefirered.gba').encode())
frames(3000); frames(3,8); frames(100)
wr(rd('gSaveBlock1Ptr')+RIVALNAME,255,1);wr(rd('gSaveBlock2Ptr'),255,1)
wr('gDebugForceKantoNewGame',1,1)
call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(600)
s=rd('gSaveBlock3Ptr');assert s!=0;p=symbols['gParties'];scratch=0x0203e000
state('ironmon-core-base')
def data(mon,field):return call('GetMonData3',mon,field,0)
def setdata(mon,field,value):wr(scratch,value);call('SetMonData',mon,field,scratch)
def start(mode=4,seed=123456):
 state('ironmon-core-base',1)
 wr(s+SEED,seed);wr(s+DIFF,mode,1);call('IronmonInitializeRun')
 call('ZeroPlayerPartyMons')
 assert call('IsIronmonRun') and bool(call('IsIronmonHardcore'))==(mode==5)
 assert call('IsMinimalGrindingMode') and call('GetCurrentLevelCap')==100
# Old padding must not opt an ordinary save into IronMON.
for fill in (0,255):
 for offset in range(IM,SIZE):wr(s+offset,fill,1)
 assert not call('IsIronmonRun')
start()
before=bytes(rd(s+i,1) for i in range(IM,SIZE))
call('ChaosEnsureRunRecords')
assert before==bytes(rd(s+i,1) for i in range(IM,SIZE))
assert not call('IronmonMoveAllowed',DB) and call('IronmonMoveAllowed',TACKLE)
call('IronmonGiveStarter',BULBA)
assert rd('gPartiesCount',1)==1 and data(p,ITEM)!=0
assert any(call('IronmonMoveIsStarterAttack',data(p,MOVE+i)) for i in range(4))
assert data(p,HPIV)==31 and data(p,ATKIV)==31 and data(p,HPEV)==0 and data(p,ATKEV)==0
saved=bytes(rd(p+i,1) for i in range(MONSIZE));call('IronmonGiveStarter',PIKA)
assert saved==bytes(rd(p+i,1) for i in range(MONSIZE))
# Central free-gift and capture routing, including the alias defense.
assert call('ScriptGiveMon',PIKA,5,0)==CANT
call('CreateWildMon',PIKA,3);wild=p+6*MONSIZE
assert call('IronmonAcceptCapture',wild)==GIVEN
assert rd('gPartiesCount',1)==1 and data(p,SP)==PIKA and data(p,LV)==10
assert rd(s+IM+RETIRED,2)==1 and call('Nuzlocke_IsGraveBox',13)
assert call('IronmonAcceptCapture',p)==GIVEN and data(p,SP)==PIKA
call('FlagSet',BADGE1);assert call('IronmonPivotFloor')==18
call('FlagSet',BADGE2);assert call('IronmonPivotFloor')==25
# Repeated generation is byte-identical even after unrelated RNG changes.
call('CreateNPCTrainerParty',p+6*MONSIZE,BROCK)
team=bytes(rd(p+6*MONSIZE+i,1) for i in range(6*MONSIZE))
wr('gRngValue',987654321)
call('CreateNPCTrainerParty',p+6*MONSIZE,BROCK)
assert team==bytes(rd(p+6*MONSIZE+i,1) for i in range(6*MONSIZE))
# Center eligibility and usage consumption: full HP/status/PP costs no use.
start();call('IronmonGiveStarter',BULBA)
wr(symbols['gMapHeader']+MAPSECTION,42,1)
call('IronmonAuthorizeCenterHealing');assert rd('gSpecialVar_Result',2)==1
call('HealPlayerParty');assert rd(s+IM+8+(42>>3),1)==0
setdata(p,HP,1)
call('HealPlayerParty');assert data(p,HP)==1 # no outstanding authorization
call('IronmonAuthorizeCenterHealing');call('HealPlayerParty')
assert data(p,HP)==data(p,MAXHP)
call('IronmonAuthorizeCenterHealing');assert rd('gSpecialVar_Result',2)==0
start(5);call('IronmonGiveStarter',BULBA);setdata(p,HP,1)
call('IronmonAuthorizeCenterHealing');assert rd('gSpecialVar_Result',2)==0
call('HealPlayerParty');assert data(p,HP)==1
# Actual GBA flash save/reload retains mode, seed, Center use and retirees.
call('SavePlayerParty')
expected=bytes(rd(s+i,1) for i in range(SIZE))
assert call('TrySavingData',0)==1
call('ClearSav3')
assert not call('IsIronmonRun')
assert call('LoadGameSave',0)==1
assert expected==bytes(rd(s+i,1) for i in range(SIZE))
assert call('IsIronmonHardcore') and data(p,HP)==1
# End state is recorded, new ordinary runs clear it rather than inherit it.
setdata(p,HP,0);call('IronmonRecordBattleEnd');assert rd(s+IM+ENDED,1)==1
assert call('IronmonAcceptCapture',p)==CANT
wr(s+DIFF,1,1);call('IronmonInitializeRun');assert not call('IsIronmonRun')
print('PASS IronMON core: mode isolation, save tail guard, preset/MGM/no caps, starter reentry, capture pivot/alias/floors, trainer byte determinism, Center use, Hardcore denial and end-state reset.')
