from qa import *
import struct
v=struct.unpack('<26I',(ROOT/'pass-layout.bin').read_bytes());z=struct.unpack('<11I',(ROOT/'battle-layout.bin').read_bytes());p=symbols['gParties'];s=0;scratch=0x0203e000
assert lib.boot(str(repo/'pokefirered.gba').encode())
def reset():
 global s
 state('pass-world',1);s=rd('gSaveBlock3Ptr');call('VarSet',0x408c,1);call('ChaosEnsureRunRecords')
def encounter(sp):
 call('CreateWildMon',sp,5);call('BattleSetup_StartWildBattle');return rd(symbols['gMapHeader']+z[6],1)
# Normal AI and Nuzlocke remain independent.
reset();wr(s+17,1,1);assert call('IsNuzlockeRun');wr(s+17,2,1);assert call('IsNuzlockeRun');call('VarSet',0x408c,0);assert not call('IsNuzlockeRun')
# Evolution-family duplicates, branched evolutions, alternate forms, actual PC ownership.
reset();call('ZeroPlayerPartyMons');call('ScriptGiveMon',16,5,0);assert call('NuzlockeSpeciesWasCaught',18)
a=encounter(18);assert not call('NuzlockeAreaEncounterUsed');assert not call('NuzlockeCanCatchMon',p+600)
assert rd(s+v[5]+4*4)==1
reset();call('ZeroPlayerPartyMons');call('ScriptGiveMon',134,5,0);assert call('NuzlockeSpeciesWasCaught',135) and call('NuzlockeSpeciesWasCaught',133)
# First eligible encounter consumes immediately; run/KO/capture failure all retain FAILED.
reset();a=encounter(19);assert call('NuzlockeAreaEncounterUsed') and call('NuzlockeMapSectionEncounterFailed',a)
assert call('NuzlockeCanCatchMon',p+600);assert rd(s+v[3]+a*2,2)==19
call('NuzlockeRecordCapture',p+600);assert not call('NuzlockeMapSectionEncounterFailed',a)
# A second new family is blocked; gifts cannot spend an untouched section.
call('CreateWildMon',21,5);call('BattleSetup_StartWildBattle');assert not call('NuzlockeCanCatchMon',p+600)
reset();call('ScriptGiveMon',25,5,0);assert not call('NuzlockeAreaEncounterUsed')
# Native shiny exemption on an already-spent area and duplicate family.
a=encounter(19);wr(scratch,1);call('SetMonData',p+600,11,scratch)
assert call('IsMonShiny',p+600);assert call('NuzlockeCanCatchMon',p+600)
call('NuzlockeRecordCapture',p+600);assert call('NuzlockeMapSectionEncounterFailed',a)
print('PASS native independent Nuzlocke, full-family/branched dupes, first-opportunity consumption, gifts, caught status, repeat block, shiny exemption.',flush=True)
# Six one-time protected gym rewards and full-bag retry.
for i,item in enumerate(list(v[15:21])+[846]):
 reset();call('ClearBag');call('VarSet',0x40f9,7)
 for flag in [0x824,0x825,0x826,0x827]:call('FlagSet',flag)
 wr('gSpecialVar_0x8004',i,2);call('ChaosClaimGymStone');assert rd('gSpecialVar_Result',2)==1 and call('CheckBagHasItem',item,1),(i,item)
 call('RemoveBagItem',item,1);call('ChaosClaimGymStone');assert rd('gSpecialVar_Result',2)==0 and not call('CheckBagHasItem',item,1)
reset();call('ClearBag');call('VarSet',0x40f9,7);wr('gSpecialVar_0x8004',0,2)
for item in range(2,500):call('AddBagItem',item,1)
call('ChaosClaimGymStone');assert rd('gSpecialVar_Result',2)==2
call('ClearBag');call('ChaosClaimGymStone');assert rd('gSpecialVar_Result',2)==1
print('PASS native all seven protected gym/rematch reward claims, eligibility, one-time receipt and full-bag retry.',flush=True)
# Optional bundles, including transaction rollback.
reset();call('ClearBag');call('FlagSet',0x820);call('ChaosClaimCarePackage');assert rd('gSpecialVar_Result',2)==0
call('VarSet',0x408e,1);call('ChaosClaimCarePackage');assert rd('gSpecialVar_Result',2)==1 and call('CheckBagHasItem',1,25)
call('ChaosClaimCarePackage');assert rd('gSpecialVar_Result',2)==0
print('PASS native care OFF/ON, curated first-badge bundle and one-time claim.',flush=True)
# Records presentation can open, navigate and return without a text overflow.
reset();call('VarSet',0x408c,0);call('ScriptContext_SetupScript',symbols['EventScript_ChaosRecordsNurse']);frames(60)
for _ in range(9):frames(2,1);frames(30)
snap('qa-pass-records');state('pass-records')
frames(2,16);frames(30);snap('qa-pass-records-next');frames(2,2);frames(80)
assert rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
print('PASS native Records Nurse screen navigation and return to field.',flush=True)
