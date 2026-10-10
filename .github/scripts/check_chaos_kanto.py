#!/usr/bin/env python3
"""Focused regression checks for Chaos Kanto progression and party preservation."""
from pathlib import Path
import itertools, re, subprocess, tempfile
ROOT = Path(__file__).resolve().parents[2]
progression = (ROOT / 'src/chaos_progression.c').read_text()
caps = (ROOT / 'src/caps.c').read_text()
start = caps.index('u32 GetCurrentLevelCap(void)')
end = caps.index('\nu32 GetSoftLevelCapExpValue', start)
cap_function = caps[start:end]
flags = sorted(set(re.findall(r'FLAG_[A-Z0-9_]+', cap_function)))
variables = sorted(set(re.findall(r'VAR_[A-Z0-9_]+', progression + cap_function)))
preamble = r'''
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int bool8;
typedef int bool32;
#define EWRAM_DATA
#define TRUE 1
#define FALSE 0
#define IS_FRLG 1
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define PARTY_SIZE 6
#define MULTI_PARTY_SIZE 3
#define B_TRAINER_PLAYER 0
#define TOTAL_BOXES_COUNT 2
#define IN_BOX_COUNT 3
#define NUM_SPECIES 500
#define SPECIES_NONE 0
#define SPECIES_CHARMANDER 30
#define SPECIES_BULBASAUR 40
#define SPECIES_SQUIRTLE 50
#define ITEM_NIDOKINGITE 203
#define ITEM_NONE 0
#define ITEM_GYARADOSITE 202
#define FACILITY_MULTI_OR_EREADER 7
#define FORM_CHANGE_TERMINATOR 0
#define FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM 1
#define MON_DATA_SPECIES 0
#define MON_DATA_IS_EGG 1
#define MON_DATA_HP 2
#define EVO_MODE_NORMAL 0
#define CHECK_EVO 0
#define OTID_STRUCT_PLAYER_ID 0
struct Pokemon { u16 species, egg, hp, level, exp; };
struct FormChange { u16 method, targetSpecies, param1; };
struct SaveBlock2 { struct { u8 selectedPartyMons[3]; } frontier; } save2;
struct SaveBlock2 *gSaveBlock2Ptr = &save2;
#define RUN_DIFFICULTY_NUZLOCKE 3
struct {u8 runDifficulty;} save3,*gSaveBlock3Ptr=&save3;
struct Pokemon gParties[4][6], backup[6], boxes[2][3];
u8 gSelectedOrderFromParty[3];
u16 gSpecialVar_0x8004, gSpecialVar_Result;
u32 sDebugImportantBattleLevelCap;
u16 vars[32];
bool8 flags[32];
u16 VarGet(u16 id) { return vars[id]; }
void VarSet(u16 id, u16 value) { vars[id] = value; }
bool8 FlagGet(u16 id) { return flags[id]; }
u16 GetMonData(const struct Pokemon *m, u16 field) { return field==0 ? m->species : field==1 ? m->egg : m->hp; }
void SetMonData(struct Pokemon *m, u16 field, const u16 *v) { if(field==MON_DATA_SPECIES) m->species=*v; }
u16 GetBoxMonDataAt(u8 b,u8 s,u16 f) { return GetMonData(&boxes[b][s],f); }
const struct FormChange *GetSpeciesFormChanges(u16 species) {
 static const struct FormChange a[]={{1,11,200},{0,0,0}}, b[]={{1,21,201},{0,0,0}};
 return species==10 ? a : species==20 ? b : NULL;
}
void SavePlayerParty(void) { memcpy(backup,gParties[0],sizeof(backup)); }
void LoadPlayerParty(void) { memcpy(gParties[0],backup,sizeof(backup)); }
void SavePlayerPartyMon(u32 i,struct Pokemon *m) { backup[i]=*m; }
u8 CalculatePlayerPartyCount(void) { return 6; }
void ReducePlayerPartyToSelectedMons(void) {
 struct Pokemon tmp[6]={0};
 for(u32 i=0;i<3;i++) tmp[i]=gParties[0][gSelectedOrderFromParty[i]-1];
 memcpy(gParties[0],tmp,sizeof(tmp));
}
void CreateMon(struct Pokemon *m,u16 species,u8 level,u32 p,u32 ot) { memset(m,0,sizeof(*m));m->species=species;m->level=level; }
u16 GetEvolutionTargetSpecies(struct Pokemon *m,u16 mode,u16 item,void *unused,bool32 *stop,u16 check) {
 return m->species==30 && m->level>=16 ? 31 : m->species==31 && m->level>=36 ? 32 : 0;
}
'''
defines = '\n'.join(f'#define {name} {i}' for i, name in enumerate(flags)) + '\n'
defines += '\n'.join(f'#define {name} {i}' for i, name in enumerate(variables)) + '\n'
main = r'''
int main(void) {
 assert(GetCurrentLevelCap()==15);
 flags[FLAG_BADGE01_GET]=1; assert(GetCurrentLevelCap()==22);
 flags[FLAG_BADGE02_GET]=1; assert(GetCurrentLevelCap()==28);
 flags[FLAG_BADGE03_GET]=1; assert(GetCurrentLevelCap()==34);
 flags[FLAG_BADGE04_GET]=1; assert(GetCurrentLevelCap()==40);
 flags[FLAG_BADGE06_GET]=1; assert(GetCurrentLevelCap()==44);
 const u16 orders[6][3]={{1,2,4},{1,4,2},{2,1,4},{2,4,1},{4,1,2},{4,2,1}};
 for(u32 order=0;order<6;order++) {
  vars[VAR_CHAOS_REMATCHES]=0;
  for(u32 step=0;step<3;step++) {
   gSpecialVar_0x8004=orders[order][step]; ChaosMarkGymRematch();
   assert(GetCurrentLevelCap()==(step==2 ? 46 : 44));
   assert(ChaosAllGymRematchesCleared()==(step==2));
   u16 before=vars[VAR_CHAOS_REMATCHES];ChaosMarkGymRematch();assert(vars[VAR_CHAOS_REMATCHES]==before);
  }
 }
 gSpecialVar_0x8004=8;ChaosMarkGymRematch();assert(vars[VAR_CHAOS_REMATCHES]==7);
 flags[FLAG_BADGE05_GET]=1;assert(GetCurrentLevelCap()==52);
 flags[FLAG_BADGE07_GET]=1;assert(GetCurrentLevelCap()==58);
 flags[FLAG_BADGE08_GET]=1;assert(GetCurrentLevelCap()==66);
 flags[FLAG_SYS_GAME_CLEAR]=1;assert(GetCurrentLevelCap()==100);
 sDebugImportantBattleLevelCap=75;assert(GetCurrentLevelCap()==75);
 sDebugImportantBattleLevelCap=0;
 ChaosChooseOakMegaStone();assert(gSpecialVar_0x8004==202 && !gSpecialVar_Result);
 boxes[1][2].species=20;ChaosChooseOakMegaStone();assert(gSpecialVar_0x8004==201 && gSpecialVar_Result);
 gParties[0][0].species=10;ChaosChooseOakMegaStone();assert(gSpecialVar_0x8004==200);
 gParties[0][0].egg=1;ChaosChooseOakMegaStone();assert(gSpecialVar_0x8004==201);
 for(u32 run=0;run<2;run++) {
  for(u32 i=0;i<6;i++) gParties[0][i]=(struct Pokemon){100+i,0,80+i,40,1000+i};
  vars[VAR_FRONTIER_FACILITY]=9;ChaosPrepareSilphSelection();assert(vars[VAR_FRONTIER_FACILITY]==7);
  ChaosCancelSilphSelection();assert(vars[VAR_FRONTIER_FACILITY]==9);
  ChaosPrepareSilphSelection();
  gSelectedOrderFromParty[0]=6;gSelectedOrderFromParty[1]=2;gSelectedOrderFromParty[2]=4;
  ChaosBeginSilphPartnerBattle();assert(gParties[0][0].species==105);
  gParties[0][0].hp=run==0 ? 12 : 0;gParties[0][0].exp+=50;
  gParties[0][1].hp=0;gParties[0][2].hp=3;
  ChaosRestoreSilphPartnerParty();
  for(u32 i=0;i<6;i++)assert(gParties[0][i].species==100+i);
  assert(gParties[0][5].hp==(run==0 ? 12 : 0));assert(gParties[0][5].exp==1055);
  assert(gParties[0][1].hp==0 && gParties[0][3].hp==3);
  assert(gParties[0][0].hp==80 && gParties[0][2].hp==82 && gParties[0][4].hp==84);
  assert(vars[VAR_FRONTIER_FACILITY]==9);
  ChaosRestoreSilphPartnerParty();assert(gParties[0][0].species==100);
 }
 vars[VAR_CHAOS_RIVAL_STARTER]=30;assert(ChaosGetRivalCounter(39)==32);
 puts("PASS: cap progression, all rematch orders, compatible stone priority/fallback, party restoration with fainted mons, and rival evolution.");
}
'''
with tempfile.TemporaryDirectory() as directory:
 source=Path(directory)/'check.c'; binary=Path(directory)/'check'
 source.write_text('int IsIronmonRun(void){return 0;}\n#define MAX_LEVEL 100\n' + preamble + defines + re.sub(r'^#include.*\n','',progression,flags=re.M) + cap_function + main)
 subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(source),'-o',str(binary)],check=True)
 subprocess.run([str(binary)],check=True)
# Structural checks cover the hooks and native event dependencies.
setup_source=(ROOT/'src/battle_setup.c').read_text()
multi=setup_source.split('void BattleSetup_StartMultiBattle(void)\n{',1)[1].split('void BattleSetup_StartBattlePikeWildBattle',1)[0]
assert 'PARTNER_CHAOS_RIVAL' in multi and 'gMain.savedCallback = CB2_EndTrainerBattle;' in multi
setup=setup_source.split('static void CB2_EndTrainerBattle(void)\n{',1)[1]
assert setup.index('ChaosRestoreSilphPartnerParty();') < setup.index('Nuzlocke_ProcessBattleDeaths();')
party=(ROOT/'src/party_menu.c').read_text().split('static u8 GetMinBattleEntries(void)\n{',1)[1].split('static u8 GetBattleEntryLevelCap',1)[0]
assert 'MAP_SILPH_CO_7F' in party and 'return MULTI_PARTY_SIZE;' in party
koga=(ROOT/'data/maps/FuchsiaCity_Gym_Frlg/scripts.inc').read_text()
assert 'trainerbattle_double TRAINER_LEADER_KOGA' in koga
assert 'ChaosAllGymRematchesCleared' in koga and 'checkitem ITEM_MEGA_RING' in koga
silph=(ROOT/'data/maps/SilphCo_7F_Frlg/scripts.inc').read_text()
assert 'PARTNER_CHAOS_RIVAL' in silph and 'ChaosCancelSilphSelection' in silph
print('PASS: native event and party-restoration hooks.')

for town, leader, badge in [('PewterCity','Brock',1),('CeruleanCity','Misty',2),('VermilionCity','LtSurge',3)]:
    gym=(ROOT/f'data/maps/{town}_Gym_Frlg/scripts.inc').read_text()
    entry=gym.split(f'{town}_Gym_EventScript_{leader}::',1)[1].split('famechecker',1)[0]
    assert entry.index(f'FLAG_BADGE0{badge}_GET') < entry.index('VAR_CHAOS_OAK_MEGA_REWARD')
print('PASS: original badges remain obtainable before optional rematches.')
