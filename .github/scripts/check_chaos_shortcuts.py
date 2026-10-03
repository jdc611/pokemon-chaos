#!/usr/bin/env python3
"""Run the real debug menu generator, team preparation and dispatch on the host."""
from pathlib import Path
import os, re, subprocess, tempfile
ROOT = Path(__file__).resolve().parents[2]
s = (ROOT / 'src/debug.c').read_text()
def function(name):
    match = re.search(r'^static [^\n]+\b'+name+r'\([^\n]*\)\n\{', s, re.M)
    assert match, name
    start=match.start(); pos=s.index('{', start); depth=1; end=pos+1
    while depth:
        depth += (s[end]=='{') - (s[end]=='}'); end+=1
    return s[start:end]
def declaration(name):
    start=s.index('static const struct DebugMenuOption '+name+'[]');end=s.index('\n};',start)+3
    return s[start:end]
menus=re.search(r'^#define KANTO_TEST_BATTLE[^\n]+',s,re.M).group()+'\n'+'\n'.join(declaration(n) for n in ['sChaosDebugRematches','sChaosDebugRocketBattles','sChaosDebugBattles'])
parts=[function(n) for n in ['Debug_GenerateListBasicMenu','DebugAction_Trainers_TryBattle','Debug_GetImportantBattleCap','Debug_PrepareImportantBattleParty','DebugAction_RocketBattle','DebugAction_ImportantBattle','DebugTask_HandleMenuInput_General']]
struct_start=s.index('struct ChaosDebugBattleMon\n');struct_end=s.index('\n};',struct_start)+3
parts.insert(0,s[struct_start:struct_end])
constants=sorted(set(re.findall(r'\b(?:TRAINER|PARTNER|SPECIES|MOVE|ABILITY|ITEM|NATURE|FLAG|MON_DATA)_[A-Z0-9_]+\b',menus+'\n'+'\n'.join(parts))))
# Read native IDs and actual trainer roster sizes/levels; do not duplicate them.
opponents=(ROOT/'include/constants/opponents_frlg.h').read_text()
ids={k:int(v) for k,v in re.findall(r'#define\s+(TRAINER_\w+)\s+(\d+)', opponents)}
raw=subprocess.check_output(['cpp','-iquote','include','-DMODERN=1','-DTESTING=0','-DFIRERED','-DIS_FRLG=1','-traditional-cpp','-'],input=(ROOT/'src/data/trainers_frlg.party').read_text(),text=True,cwd=ROOT)
rosters={}
for block in raw.split('=== ')[1:]:
    name=block.split(' ===',1)[0];levels=[int(x) for x in re.findall(r'^Level: (\d+)',block,re.M)]
    rosters[name]=(levels,'Double Battle: Yes' in block)
trainer_names=sorted(set(re.findall(r'\bTRAINER_(?!BATTLE_TYPE)[A-Z0-9_]+',menus)))
assert len(trainer_names)==23, trainer_names
for name in trainer_names:
    assert name in ids and name in rosters and 0 < len(rosters[name][0]) <= 6, name
normal_pairs=re.findall(r'\{ (SPECIES_\w+), \{ [^\n]+ \}, (ABILITY_\w+) \}', '\n'.join(parts))
preamble=r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint8_t u8; typedef uint16_t u16; typedef uint32_t u32; typedef int16_t s16; typedef int32_t s32; typedef int bool32;
#define TRUE 1
#define IS_FRLG 1
#define FALSE 0
#define PARTY_SIZE 6
#define MAX_MON_MOVES 4
#define NUM_ABILITY_SLOTS 3
#define DEBUG_MAX_MENU_ITEMS 20
#define EOS 0
#define COMPOUND_STRING(x) ((const u8 *)(x))
struct DebugMenuOption {const u8 *text; const void *action; const void *actionParams;};
struct ListMenuItem {const u8 *name; u32 id;};
struct DebugMenuListData {struct ListMenuItem listItems[21]; u8 itemNames[21][26]; int menuType; s16 data[8];};
static struct DebugMenuListData menuData; static struct DebugMenuListData *sDebugMenuListData=&menuData;
static u8 gStringVar4[1024]; static const u8 sDebugText_Arrow[]=" >";
static void StringExpandPlaceholders(u8 *d,const u8 *s){strcpy((char*)d,(const char*)s);}
static void StringAppend(u8 *d,const u8 *s){strcat((char*)d,(const char*)s);}
static void StringCopyN(u8 *d,const u8 *s,u32 n){strncpy((char*)d,(const char*)s,n);}
static void DebugAction_OpenSubMenu(u8 taskId,const void *params){(void)taskId;(void)params;}
static bool32 IsSubMenuAction(const void *action){return action==DebugAction_OpenSubMenu;}
struct Pokemon {u16 species,held,moves[4];u8 level,ability,nature,ivs[6],met;u32 ot;};
static struct Pokemon gPlayerParty[6]; static int partyCount,healCount; static u32 currentCap;
struct TrainerMon {u8 lvl;}; struct Trainer {u8 partySize,battleType;struct TrainerMon party[6];};
static struct Trainer trainers[768];
static const struct Trainer *GetTrainerStructFromId(u16 id){assert(id<768 && trainers[id].partySize);return &trainers[id];}
static void SetDebugImportantBattleLevelCap(u32 cap){currentCap=cap;}
static void FlagSet(u16 flag){(void)flag;}
static void ZeroPlayerPartyMons(void){memset(gPlayerParty,0,sizeof(gPlayerParty));partyCount=0;healCount=0;}
static void ScriptGiveMon(u16 species,u8 level,u16 held){assert(partyCount<6 && species && level);gPlayerParty[partyCount++]=(struct Pokemon){.species=species,.level=level,.held=held};}
static void SetMonMoveSlot(struct Pokemon *mon,u16 move,u8 slot){assert(slot<4 && move);mon->moves[slot]=move;}
static void CalculateMonStats(struct Pokemon *mon){assert(mon->species && mon->level);}
static void HealPlayerParty(void){healCount++;}
struct Save {u8 playerTrainerId[4]; struct {u8 selectedPartyMons[3];} frontier;};static struct Save save={ {1,2,3,4} };static struct Save *gSaveBlock2Ptr=&save;
static const u8 Debug_ChaosMoonBattle[]={1},Debug_ChaosSilphBattle[]={2},Debug_ChaosRocketGauntlet[]={3};
static u16 dispatchedId;static const u8 *dispatchedScript;static int launches;
static void Debug_DestroyMenu_Full_Script(u8 taskId,const u8 *script){(void)taskId;dispatchedScript=script;launches++;}
#define BATTLE_TYPE_TRAINER 1
#define BATTLE_TYPE_DOUBLE 2
#define BATTLE_TYPE_TWO_OPPONENTS 4
#define BATTLE_TYPE_MULTI 8
#define BATTLE_TYPE_INGAME_PARTNER 16
#define B_TRAINER_OPPONENT_A 1
#define B_TRAINER_OPPONENT_B 2
#define MAX_FRONTIER_PARTY_SIZE 3
#define REMATCHES_COUNT 6
#define TRAINER_PARTNER(id) (768+(id))
static u32 gBattleTypeFlags,gBattleEnvironment;static u16 gPartnerTrainerId;static u8 gSelectedOrderFromParty[6];
static struct Pokemon gParties[4][6];
static struct {u16 opponentA,opponentB;u8 earlyRival;} params;
#define TRAINER_BATTLE_PARAM params
static struct {u16 trainerIds[6];} gRematchTable[1];
static int CountBattledRematchTeams(s32 id){assert(id==0);return 0;}
static void InitTrainerBattleParameter(void){memset(&params,0,sizeof(params));}
static void CreateNPCTrainerPartyFromTrainer(struct Pokemon *party,const struct Trainer *trainer){for(int i=0;i<trainer->partySize;i++)party[i].level=trainer->party[i].lvl;}
static void SavePlayerParty(void){}
static void FillPartnerParty(u16 id){(void)id;}
static u32 BattleSetup_GetEnvironmentId(void){return 1;}
static void CalculateEnemyPartyCount(void){assert(gParties[1][0].level);}
static void BattleSetup_StartTrainerBattle_Debug(void){assert(!params.earlyRival && !gPartnerTrainerId);dispatchedId=params.opponentA;launches++;}
static void Debug_DestroyMenu_Full(u8 taskId){(void)taskId;}
typedef void (*DebugFuncWithParams)(u8,const void*); typedef void (*DebugFunc)(u8);
#define A_BUTTON 1
#define B_BUTTON 2
#define SE_SELECT 1
#define DEBUG_BASIC_MENU 0
#define JOY_NEW(button) (testKeys&(button))
static int testKeys,testInput,exits;static const struct DebugMenuOption *currentMenu;
static struct {u8 tMenuTaskId;} gTasks[1];
static const struct DebugMenuOption *Debug_GetCurrentCallbackMenu(void){return currentMenu;}
static s32 ListMenu_ProcessInput(u8 id){(void)id;return testInput;}
static void PlaySE(int sound){(void)sound;}
static u32 Debug_RemoveCallbackMenu(void){return 0;}
static void Debug_DestroyMenu(u8 taskId){(void)taskId;}
static void Debug_ShowMenu(DebugFunc callback,const struct DebugMenuOption *menu){(void)callback;(void)menu;}
static void ScriptContext_Enable(void){exits++;}


static void DebugAction_RocketBattle(u8 taskId,const void *params);
static void DebugAction_ImportantBattle(u8 taskId,const void *params);
'''
defines=[]
for i,c in enumerate(constants,1):
    if c in ["TRAINER_BATTLE_PARAM","TRAINER_PARTNER"]:continue
    value=ids.get(c,i+1000)
    if c in ['TRAINER_NONE','PARTNER_NONE','NATURE_HARDY']:value=0
    if c=='TRAINER_BATTLE_TYPE_DOUBLES':value=1
    if c=='MON_DATA_HP_IV':value=100
    if c=='MON_DATA_SPDEF_IV':value=105
    defines.append(f'#define {c} {value}')
getability='static u16 GetSpeciesAbility(u16 species,u8 slot){switch(species){\n'
for species,ability in dict(normal_pairs).items():
    slot=2 if species in ['SPECIES_EXCADRILL','SPECIES_GARCHOMP'] else 1 if species in ['SPECIES_AZUMARILL','SPECIES_SCIZOR'] else 0
    getability+=f'case {species}:return slot=={slot} ? {ability} : 0;\n'
getability+='default:assert(0);return 0;}}\n'
setdata=r'''
static void SetMonData(struct Pokemon *mon,u32 field,const void *ptr){
 const u8 *p=ptr;
 if(field>=MON_DATA_HP_IV && field<=MON_DATA_SPDEF_IV){mon->ivs[field-MON_DATA_HP_IV]=*p;return;}
 switch(field){case MON_DATA_ABILITY_NUM:mon->ability=*p;break;case MON_DATA_HIDDEN_NATURE:mon->nature=*p;break;
 case MON_DATA_MET_LEVEL:mon->met=*p;break;case MON_DATA_OT_ID:memcpy(&mon->ot,p,4);break;default:assert(0);}}
'''
main=r'''
static int checked;
static void check_menu(const struct DebugMenuOption *menu){
 memset(&menuData,0,sizeof(menuData));menuData.data[7]=1234;
 u32 count=Debug_GenerateListBasicMenu(menu);assert(count<=20 && menuData.data[7]==1234);
 for(u32 i=0;menu[i].text;i++){
  assert(i<count && menuData.listItems[i].id==i && strlen((const char*)menuData.listItems[i].name)<26);
  if(menu[i].action==DebugAction_OpenSubMenu)continue;
  dispatchedId=0;dispatchedScript=NULL;launches=0;params.earlyRival=1;gPartnerTrainerId=999;
  currentMenu=menu;testKeys=A_BUTTON;testInput=i;DebugTask_HandleMenuInput_General(0);
  u16 id=*(const u16*)menu[i].actionParams;const struct Trainer *trainer=GetTrainerStructFromId(id);
  int expected=trainer->partySize;
  if(menu[i].action==DebugAction_RocketBattle){
   expected=id==TRAINER_CHAOS_JESSIE_MOON?4:6;
   assert(dispatchedScript==(id==TRAINER_CHAOS_JESSIE_MOON?Debug_ChaosMoonBattle:id==TRAINER_CHAOS_JESSIE_SILPH?Debug_ChaosSilphBattle:Debug_ChaosRocketGauntlet));
  }else{assert(dispatchedId==id);assert(menuData.data[2]==TRAINER_NONE && menuData.data[4]==PARTNER_NONE);assert(menuData.data[5]==(trainer->battleType==TRAINER_BATTLE_TYPE_DOUBLES));assert(gBattleTypeFlags==(BATTLE_TYPE_TRAINER|(trainer->battleType==TRAINER_BATTLE_TYPE_DOUBLES?BATTLE_TYPE_DOUBLE:0)));}
  assert(launches==1 && partyCount==expected && healCount==1);
  for(int j=0;j<partyCount;j++){
   assert(gPlayerParty[j].level==currentCap && gPlayerParty[j].met==currentCap && gPlayerParty[j].held && gPlayerParty[j].ot==0x04030201);
   for(int m=0;m<4;m++)assert(gPlayerParty[j].moves[m]);for(int st=0;st<6;st++)assert(gPlayerParty[j].ivs[st]==15);
  }checked++;
 }
}
int main(void){
'''
for name in trainer_names:
    levels,doubles=rosters[name];main+=f'trainers[{ids[name]}]=(struct Trainer){{.partySize={len(levels)},.battleType={"TRAINER_BATTLE_TYPE_DOUBLES" if doubles else "0"},.party={{'+','.join('{'+str(l)+'}' for l in levels)+'}};\n'
main+=r'''
 check_menu(sChaosDebugBattles);check_menu(sChaosDebugRematches);check_menu(sChaosDebugRocketBattles);assert(checked==23);
 currentMenu=sChaosDebugBattles;launches=0;
 testKeys=0;testInput=-1;DebugTask_HandleMenuInput_General(0);assert(launches==0);
 testKeys=A_BUTTON;testInput=-1;DebugTask_HandleMenuInput_General(0);assert(launches==0);
 testInput=15;DebugTask_HandleMenuInput_General(0);assert(launches==0);
 testInput=100;DebugTask_HandleMenuInput_General(0);assert(launches==0);
 testKeys=B_BUTTON;testInput=-2;DebugTask_HandleMenuInput_General(0);assert(exits==1 && launches==0);
 currentMenu=NULL;testKeys=A_BUTTON;testInput=0;DebugTask_HandleMenuInput_General(0);assert(launches==0);
 struct DebugMenuOption excessive[32];for(int i=0;i<31;i++)excessive[i]=(struct DebugMenuOption){(const u8*)"A deliberately very long menu label that must not overwrite adjacent state",DebugAction_ImportantBattle,NULL};excessive[31]=(struct DebugMenuOption){0};
 memset(&menuData,0,sizeof(menuData));for(int i=0;i<8;i++)menuData.data[i]=2345;
 assert(Debug_GenerateListBasicMenu(excessive)==20);for(int i=0;i<8;i++)assert(menuData.data[i]==2345);
 for(int i=0;i<20;i++)assert(menuData.itemNames[i][25]==EOS);
 puts("PASS: all 23 shortcut dispatches, native trainer IDs/levels/party sizes, singles/doubles settings, Rocket script selection, usable test teams, oversized menus, long labels, idle/cancel/invalid inputs.");
}
'''
program=preamble+'\n'+'\n'.join(defines)+'\n'+getability+setdata+'\n'+menus+'\n'+'\n'.join(parts)+main
with tempfile.TemporaryDirectory() as directory:
    p=Path(directory);(p/'test.c').write_text(program)
    subprocess.run(['gcc','-std=gnu17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer',str(p/'test.c'),'-o',str(p/'test')],check=True)
    subprocess.run([str(p/'test')],check=True,env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0'})
# Review scripted formats and existing restoration hooks independently of menu dispatch.
debug=(ROOT/'data/scripts/debug.inc').read_text()
gauntlet=debug.split('Debug_ChaosRocketGauntlet::',1)[1].split('Debug_ChaosRocketDefeat::',1)[0]
assert gauntlet.count('trainerbattle_no_intro')==2 and 'HealPlayerParty' not in gauntlet
assert gauntlet.index('TRAINER_CHAOS_COLE_SILPH')<gauntlet.index('TRAINER_CHAOS_VESPER_SILPH')
silph=debug.split('Debug_ChaosSilphBattle::',1)[1].split('Debug_ChaosRocketGauntlet::',1)[0]
for hook in ['ChooseHalfPartyForBattle','ChaosBeginSilphPartnerBattle','PARTNER_CHAOS_RIVAL','MULTI_BATTLE_2_VS_2 | MULTI_BATTLE_CHOOSE_MONS','ChaosCancelSilphSelection']:
    assert hook in silph,hook
setup=(ROOT/'src/battle_setup.c').read_text()
assert setup.index('ChaosRestoreSilphPartnerParty();')<setup.index('Nuzlocke_ProcessBattleDeaths();',setup.index('ChaosRestoreSilphPartnerParty();'))
print('PASS: uninterrupted Cole/Vesper format, Silph selection/cancel format and restoration before death processing.')

input_handler=function('DebugTask_HandleMenuInput_General')
assert input_handler.index('input >= 0') < input_handler.index('options[input]')
assert '(u32)input < optionCount' in input_handler
assert 'optionCount < DEBUG_MAX_MENU_ITEMS' in input_handler
print('PASS: debug launch clears stale battle parameters/partner state and input checks precede menu indexing.')
