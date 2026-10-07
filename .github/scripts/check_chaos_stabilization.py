#!/usr/bin/env python3
"""Targeted host regressions for the full FireRed stabilization directive.
Compiles production encounter/reward logic with bounded ownership/bag fixtures.
Native emulator battle/event/UI paths are logged separately in the playtest report.
"""
from pathlib import Path
import json,re,subprocess,tempfile
root=Path(__file__).resolve().parents[2]
def function(path,name):
 s=(root/path).read_text();m=re.search(r'^(?:static )?(?:[\w]+) '+name+r'\([^;]*?\)\n\{',s,re.M);assert m,name
 i=m.end();d=1
 while d:d+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[m.start():i]+'\n'
def run(s):
 with tempfile.TemporaryDirectory() as d:
  p=Path(d)/'test.c';p.write_text(s);subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/test'],check=True);subprocess.run([d+'/test'],check=True)
base=r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
typedef uint8_t u8;typedef uint16_t u16;typedef uint32_t u32;typedef int16_t s16;typedef int bool8;typedef int bool32;
#define TRUE 1
#define FALSE 0
#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))
#define RECORDS_MAGIC 0x43525231
#define NUM_SPECIES 12
#define PARTY_SIZE 6
#define TOTAL_BOXES_COUNT 2
#define IN_BOX_COUNT 4
#define B_TRAINER_PLAYER 0
#define B_TRAINER_OPPONENT_A 1
#define MON_DATA_SPECIES 0
#define FLAG_GET_CAUGHT 0
#define NATIONAL_DEX_NONE 0
#define EVOLUTIONS_END 65535
#define RUN_DIFFICULTY_NUZLOCKE 3
#define VAR_CHAOS_NUZLOCKE 1
#define WE_FLAG_NO_CATCHING 2
enum Species {SPECIES_NONE=0};enum NationalDexOrder {D_NONE=0};
struct Evolution{u16 method,targetSpecies;} ev[12][3]={[1]={{1,2},{EVOLUTIONS_END,0}},[2]={{1,3},{EVOLUTIONS_END,0}},[4]={{1,5},{1,6},{EVOLUTIONS_END,0}}};
struct Pokemon{u16 species;bool8 shiny;} gParties[2][6];u16 boxes[2][4];bool8 caught[12];
struct SaveBlock3 {u8 runDifficulty;u8 nuzlockeEncounterUsed[32];bool8 nuzlockeCurrentEncounterCatchable;u32 recordsMagic;u16 encounterSpecies[256];u8 encounterFailed[32];u32 runCounters[16];} save,*gSaveBlock3Ptr=&save;
struct {u16 regionMapSectionId;} gMapHeader;
u16 sNuzlockeFamily[NUM_SPECIES];bool8 sNuzlockeFamiliesReady;s16 sNuzlockeEligibleSection;
u16 vars[20];u32 VarGet(u16 i){return vars[i];}bool32 FlagGet(u16 i){return vars[i];}
bool32 IsSpeciesEnabled(u16 s){return s>0&&s<12;}
u16 SpeciesToNationalPokedexNum(u16 s){return s==7?3:s;}
u16 NationalPokedexNumToSpecies(u16 d){return d;}
const struct Evolution*GetSpeciesEvolutions(u16 s){return s<=4?ev[s]:NULL;}
u32 GetSetPokedexFlag(u16 d,u32 action){return caught[d];}
u32 GetMonData(struct Pokemon*m,u32 f){return m->species;}
u32 GetBoxMonDataAt(u32 b,u32 n,u32 f){return boxes[b][n];}
bool8 IsMonShiny(struct Pokemon*m){return m->shiny;}
'''
# Blank non-evolving fixtures need the real evolution terminator.
fns=['NuzlockeFamilyRoot','NuzlockeBuildFamilies','NuzlockeSpeciesWasCaught','NuzlockeMonIsShiny','NuzlockeMapSectionEncounterUsed','NuzlockeAreaEncounterUsed','NuzlockeMarkAreaEncounterUsed','NuzlockeAccountStandardEncounter','NuzlockeCanCatchMon','NuzlockeRecordCapture']
code=base+function('src/chaos_progression.c','IsNuzlockeRun')+function('src/chaos_records.c','ChaosEnsureRunRecords')+''.join(function('src/battle_setup.c',n) for n in fns)+r'''
int main(void){
 ev[3][0].method=EVOLUTIONS_END;
 vars[VAR_CHAOS_NUZLOCKE]=1;save.runDifficulty=1;assert(IsNuzlockeRun());save.runDifficulty=2;assert(IsNuzlockeRun());vars[1]=0;assert(!IsNuzlockeRun());vars[1]=1;
 gMapHeader.regionMapSectionId=101;
 gParties[0][0].species=1;gParties[1][0].species=7;NuzlockeAccountStandardEncounter(0);assert(!NuzlockeAreaEncounterUsed());assert(save.runCounters[4]==1);
 gParties[0][0].species=5;assert(NuzlockeSpeciesWasCaught(6));gParties[0][0].species=0;boxes[1][0]=2;assert(NuzlockeSpeciesWasCaught(3));boxes[1][0]=0;caught[3]=1;assert(NuzlockeSpeciesWasCaught(1));caught[3]=0;
 gParties[1][0].species=8;NuzlockeAccountStandardEncounter(0);assert(NuzlockeAreaEncounterUsed());assert(NuzlockeCanCatchMon(&gParties[1][0]));assert(save.encounterFailed[101>>3]&(1<<(101&7)));NuzlockeRecordCapture(&gParties[1][0]);assert(!(save.encounterFailed[101>>3]&(1<<(101&7))));
 gParties[1][0].species=9;NuzlockeAccountStandardEncounter(0);assert(!NuzlockeCanCatchMon(&gParties[1][0]));gParties[1][0].shiny=1;assert(NuzlockeCanCatchMon(&gParties[1][0]));
 gMapHeader.regionMapSectionId=102;NuzlockeAccountStandardEncounter(0);assert(!NuzlockeAreaEncounterUsed());gParties[1][0].shiny=0;vars[WE_FLAG_NO_CATCHING]=1;NuzlockeAccountStandardEncounter(0);assert(!NuzlockeAreaEncounterUsed());vars[WE_FLAG_NO_CATCHING]=0;
 gParties[1][0].species=1;gParties[0][0].species=1;gParties[1][1].species=9;NuzlockeAccountStandardEncounter(1);assert(save.encounterSpecies[102]==9);assert(!NuzlockeCanCatchMon(&gParties[1][0]));assert(NuzlockeCanCatchMon(&gParties[1][1]));
 puts("PASS production family union/alternate forms, past and boxed ownership, named-area first opportunity, capture transition, shiny and uncatchable exceptions, doubles and independent difficulty.");}
'''
run(code)
# Source binding/catalog checks supplement the compiled behavior checks above.
variables=(root/'include/constants/vars.h').read_text()
new=['NUZLOCKE','EZ_CATCH','CARE_PACKAGES','TRAINING_UNLOCKED','POKERIDER_UNLOCKED']
values=[int(re.search(r'#define VAR_CHAOS_'+x+r'\s+(0x[0-9A-Fa-f]+)',variables)[1],16) for x in new]
assert len(set(values))==5 and all(0x408c<=x<=0x4090 for x in values)
assert not set(values)&{0x40db,0x40dc,0x40e0,0x40e4,0x40e5,*range(0x40f9,0x4100)}
assert 'giveitem ITEM_FAME_CHECKER' not in (root/'data/maps/CeruleanCity_Frlg/scripts.inc').read_text()
assert 'giveitem ITEM_POKE_BALL, 5' in (root/'data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc').read_text()
assert '#define B_RUN_TRAINER_BATTLE' in (root/'include/config/battle.h').read_text()
ai=(root/'src/battle_ai_main.c').read_text();assert 'AI_FLAG_MOVE_OMNISCIENCE' in ai
for p in (root/'src').glob('battle_ai*.c'):
 assert not re.search(r'\bgChosenMoveByBattler\b|\bgChosenActionByBattler\b|gBattleResources->bufferB',p.read_text()),p
rows=json.loads((root/'docs/chaos-tm-audit.json').read_text())
for r in rows:
 arcade=any(s['label']=='Chaos Arcade' for s in r['sources']);ordinary=any(s['label']!='Chaos Arcade' for s in r['sources']);assert not(arcade and ordinary),r
shop=(root/'data/maps/CeladonCity_DepartmentStore_4F_Frlg/scripts.inc').read_text().split('ChaosCeladonSpecialStock_Items::')[1]
pickups=json.loads((root/'docs/chaos-mega-pickups.json').read_text());assert len({p['flag'] for p in pickups})==len(pickups)
for p in pickups:
 assert 'ITEM_'+p['stone']+'\n' not in shop,p
 m=json.loads((root/f"data/maps/{p['map']}/map.json").read_text());assert any(o.get('x')==p['x'] and o.get('y')==p['y'] and 'ChaosMegaPickup' in o.get('script','') for o in m['object_events']),p
for item in ['ARCANITE','NIDOKINGITE','STEELIXITE','GYARADOSITE','MANECTITE','BEEDRILLITE','ALAKAZITE','GARCHOMPITE','PYROARITE','MEWTWONITE_X','MEWTWONITE_Y','SCIZORITE']:assert 'ITEM_'+item+'\n' not in shop
print('PASS native FireRed variable isolation, gift bindings, AI input boundary, TM Arcade uniqueness, and protected/nonduplicated Mega acquisition bindings.')

hof=(root/'src/hall_of_fame_frlg.c').read_text();assert 'ChaosSnapshotLeague();' in hof and 'ChaosBuildRecordsPage();' in hof and 'LEAGUE SUMMARY' in hof

hints=(root/'src/battle_controller_player.c').read_text();assert 'BG_PLTT_ID(palette) + TEXT_COLOR_GREEN' in hints # PP shading must not overwrite the second spread hint.
