#!/usr/bin/env python3
"""Run affected native randomizer and menu eligibility functions with host fixtures."""
from pathlib import Path
import re, subprocess, tempfile
root=Path(__file__).resolve().parents[2]
def function(path,name):
 s=(root/path).read_text()
 m=re.search(r'^(?:static )?(?:enum \w+|[\w]+) '+name+r'\([^;]*?\)\n\{',s,re.M)
 assert m,name
 a=m.start();i=m.end();depth=1
 while depth:
  depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[a:i]+'\n'
def run(code):
 with tempfile.TemporaryDirectory() as d:
  p=Path(d)/'check.c';p.write_text(code)
  subprocess.run(['cc','-std=gnu11','-Wall','-Werror','-iquote',str(root/'include'),str(p),'-o',d+'/check'],check=True)
  subprocess.run([d+'/check'],check=True)
base=r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define min(a,b) ((a)<(b)?(a):(b))
#define AllocZeroed(n) calloc(1,n)
#define NUM_NORMAL_ABILITY_SLOTS 2
typedef uint8_t u8;typedef uint16_t u16;typedef uint32_t u32;typedef uint32_t rng_value_t;
typedef int bool32;typedef int bool8;
#define TRUE 1
#define FALSE 0
#define ARRAY_COUNT(x) (sizeof(x)/sizeof((x)[0]))
#define FILTER_FUNC_ARG_NONE 0
#define RUN_ABILITIES_RANDOM 1
#define RUN_WILD_NORMAL 0
#define RUN_WILD_RANDOM 1
#define RUN_WILD_SCALED 2
#define RUN_FILTER_NONE 0
#define RUN_FILTER_TYPE 1
#define RUN_FILTER_ABILITY 2
#define RUN_FILTER_TYPE_ABILITY 3
#define ABILITY_NONE 0
#define TYPE_NONE 0
#define TYPE_MYSTERY 10
#define NUMBER_OF_MON_TYPES 21
#define SPECIES_GENERATOR_NO_SUPERMONS 1
#define SPECIES_GENERATOR_SCALED_WILD 2
#define SPECIES_GENERATOR_SCRIPTED_ENCOUNTER 3
#define SPECIES_CUSTOM_START 100
#define NATIONAL_DEX_COUNT 12
#define ABILITIES_COUNT 5
#define GET_BASE_SPECIES_ID(x) (x)
enum Species {SPECIES_NONE=0};
struct FilterFuncArgs {u32 arg1,arg2;};
struct SpeciesInfo {u32 baseHP,baseAttack,baseDefense,baseSpeed,baseSpAttack,baseSpDefense; u16 abilities[2]; bool32 isMegaEvolution,isPrimalReversion,isRestrictedLegendary,isSubLegendary,isMythical,isUltraBeast,isParadox;} gSpeciesInfo[110];
struct Save3 {u32 randomizerEnabled,filterMode,filterValue,abilityMode,evolutionMode,runDifficulty,worldSeed,itemRandomization;} save3,*gSaveBlock3Ptr=&save3;
struct Save1 {struct {u8 mapGroup,mapNum;} location;} save1,*gSaveBlock1Ptr=&save1;
rng_value_t gRngValue;
void SeedRng(u32 x){gRngValue=x;}
u32 Random(void){gRngValue=gRngValue*1664525u+1013904223u;return gRngValue;}
// Seeded fixture: each line gains type 3 and ability 2 or 3 according to seed.
// It models reachable forms and normal ability slots; the menu must use the same predicate.
bool32 DoesSpeciesOrReachableFormMatchRunFilterForSettings(enum Species s,u8 mode,u16 val,u8 am,u8 ev,u8 diff,u32 seed){
 u32 type=s%3+1,ability=seed%2?3:2;
 if(mode==RUN_FILTER_NONE)return TRUE;
 if(mode==RUN_FILTER_TYPE)return val==type||val==3;
 if(mode==RUN_FILTER_ABILITY)return val==ability;
 return (val&31)==3||((val&31)==type) ? (val>>5)==ability:FALSE;
}
'''
source='src/random_mon_generation.c'
code=base+function(source,'GetOriginalSpeciesBst')+r'''
enum Species GetSpeciesPreEvolution(enum Species s){return s==5?1:SPECIES_NONE;}
'''+function(source,'IsScaledWildSpeciesFilterFunc')+function(source,'IsScriptedEncounterSpeciesFilterFunc')+r'''
enum Species GetRandomSpecies(u32 generator,const struct FilterFuncArgs*a){
 enum Species pool[20];u32 n=0;for(enum Species s=1;s<=12;s++)if(IsScriptedEncounterSpeciesFilterFunc(s,a))pool[n++]=s;
 return n?pool[Random()%n]:SPECIES_NONE;
}
'''+function(source,'GetRandomizedScriptedSpecies')+r'''
int main(void){
 for(int s=1;s<=12;s++)gSpeciesInfo[s].baseHP=300;
 gSpeciesInfo[8].baseHP=600;gSpeciesInfo[8].isRestrictedLegendary=TRUE;
 gSpeciesInfo[9].baseHP=560;gSpeciesInfo[10].baseHP=650;gSpeciesInfo[11].baseHP=651;
 gSpeciesInfo[12].isMegaEvolution=TRUE;
 save3.worldSeed=42;gRngValue=1234;
 assert(GetRandomizedScriptedSpecies(1,25,0)==1);
 save3.randomizerEnabled=RUN_WILD_RANDOM;
 enum Species a=GetRandomizedScriptedSpecies(1,25,0);assert(a!=1&&a!=8&&a!=12);assert(gRngValue==1234);
 assert(GetRandomizedScriptedSpecies(1,25,0)==a);
 assert(GetRandomizedScriptedSpecies(100,70,2)==100);
 struct FilterFuncArgs f={8,70};assert(IsScriptedEncounterSpeciesFilterFunc(9,&f));assert(IsScriptedEncounterSpeciesFilterFunc(10,&f));assert(!IsScriptedEncounterSpeciesFilterFunc(11,&f));
 save3.randomizerEnabled=RUN_WILD_SCALED;f.arg1=1;f.arg2=5;
 assert(IsScriptedEncounterSpeciesFilterFunc(2,&f));assert(!IsScriptedEncounterSpeciesFilterFunc(5,&f));assert(!IsScriptedEncounterSpeciesFilterFunc(9,&f));
 save3.filterMode=RUN_FILTER_ABILITY;save3.filterValue=4;
 assert(GetRandomizedScriptedSpecies(1,25,0)==1);assert(gRngValue==1234);
 puts("PASS: native gift/static replacement, Normal/Random/Scaled, deterministic retry, RNG restoration, legendary BST boundaries, evolution-stage gate, fixed custom species and empty-filter fallback.");
}
'''
run(code)
source='src/main_menu.c'
code=base+r'''
u8 sRunSetupRandomizer=RUN_WILD_RANDOM,sRunSetupType,sRunSetupFilter,sRunSetupTypeChoices[NUMBER_OF_MON_TYPES],sRunSetupTypeChoiceCount;
u8 sRunSetupAbilityMode,sRunSetupEvolutions,sRunSetupDifficulty,unusedByte;
u16 sRunSetupAbility,sRunSetupAbilityChoices[ABILITIES_COUNT],sRunSetupAbilityChoiceCount,sRunSetupTypeChoiceAbility;
u32 sRunSetupSeed,sRunSetupFinalEligible,sRunSetupBaseGenerator;
bool8 sRunSetupBasePoolValid,sRunSetupLowPoolConfirmed;
u16 *sRunSetupPoolCounts;
u32 sRunSetupLineSeen[NUMBER_OF_MON_TYPES][(ABILITIES_COUNT+31)/32];
int baseCalls;
u8 GetSpeciesType(enum Species s,u32 slot){return slot==0?s%3+1:3;}
u16 GetRandomizedAbilityForSeed(enum Species s,u8 slot,u32 seed){return seed%2?3:2;}
void VisitRunFilterReachableSpeciesForSettings(enum Species s,u8 ev,u8 diff,u32 seed,void(*v)(enum Species)){gSpeciesInfo[s].abilities[0]=seed%2?3:2;gSpeciesInfo[s].abilities[1]=0;v(s);}

enum Species NationalPokedexNumToSpecies(u32 x){return x;}
bool32 IsSpeciesEligibleRandomSpecies(u32 gen,enum Species s,const struct FilterFuncArgs*a){baseCalls++;return TRUE;}
'''+function(source,'RunSetup_RecordPair')+function(source,'RunSetup_RecordReachableSpecies')+function(source,'RunSetup_CountEligibleSelection')+function(source,'RunSetup_BuildTypeChoices')+function(source,'RunSetup_BuildAbilityChoices')+function(source,'RunSetup_InvalidateSeedFilters')+r'''
int main(void){
 sRunSetupSeed=42;sRunSetupType=3;RunSetup_BuildAbilityChoices();
 assert(sRunSetupAbilityChoiceCount==2&&sRunSetupAbilityChoices[1]==2);assert(baseCalls==12);
 RunSetup_InvalidateSeedFilters();sRunSetupAbility=2;RunSetup_BuildTypeChoices();
 assert(sRunSetupTypeChoiceCount==4);assert(sRunSetupTypeChoices[3]==3);
 assert(RunSetup_CountEligibleSelection(3,2,99)==12);
 sRunSetupSeed=43;RunSetup_InvalidateSeedFilters();assert(!sRunSetupType&&!sRunSetupAbility&&!sRunSetupBasePoolValid);
 RunSetup_BuildAbilityChoices();assert(sRunSetupAbilityChoiceCount==2&&sRunSetupAbilityChoices[1]==3);
 sRunSetupAbility=2;RunSetup_BuildTypeChoices();assert(sRunSetupTypeChoiceCount==1);
 puts("PASS: actual Type/Ability builders, either selection order, seeded viability, only valid options, base-pool caching and seed invalidation.");
}
'''
run(code)
# Native item mapping, with actual enum values and weighted reward pools.
source='src/field_specials.c'
items=(root/'include/constants/items.h').read_text()
# Header contains only enum/constants and no dependencies on the engine.
code=base+'\n'+items+r'''
#define POCKET_KEY_ITEMS 99
u32 GetItemPocket(enum Item item){return item==ITEM_OLD_ROD?POCKET_KEY_ITEMS:0;}
u16 gSpecialVar_Result;
'''+function(source,'ChaosFieldItemHash')+function(source,'ChaosFieldItemIsProtected')+function(source,'ChaosRandomizeOverworldItem')+r'''
int main(void){
 save3.worldSeed=42;gSpecialVar_Result=ITEM_POTION;ChaosRandomizeOverworldItem();assert(gSpecialVar_Result==ITEM_POTION);
 save3.itemRandomization=1;save1.location.mapGroup=3;save1.location.mapNum=4;
 u16 originals[]={ITEM_POTION,ITEM_ANTIDOTE,ITEM_RARE_CANDY};
 for(int i=0;i<3;i++){gSpecialVar_Result=originals[i];ChaosRandomizeOverworldItem();u16 mapped=gSpecialVar_Result;gSpecialVar_Result=originals[i];ChaosRandomizeOverworldItem();assert(mapped==gSpecialVar_Result);assert(mapped>ITEM_NONE);}
 int different=0;for(u32 seed=0;seed<30;seed++){save3.worldSeed=seed;gSpecialVar_Result=ITEM_POTION;ChaosRandomizeOverworldItem();different|=gSpecialVar_Result!=ITEM_POTION;}assert(different);
 u16 protected[]={ITEM_HM01,ITEM_NIDOKINGITE,ITEM_STRANGE_FOSSIL,ITEM_OLD_ROD,ITEM_VENUSAURITE};
 for(int i=0;i<5;i++){gSpecialVar_Result=protected[i];ChaosRandomizeOverworldItem();assert(gSpecialVar_Result==protected[i]);}
 gSpecialVar_Result=ITEM_TM01;ChaosRandomizeOverworldItem();assert(gSpecialVar_Result>=ITEM_TM01&&gSpecialVar_Result<=ITEM_TM100);
 puts("PASS: actual weighted item mapping, multiple pickup inputs, saved world seeds, repeatability, TM category and progression protection.");
}
'''
run(code)
obtain=(root/'data/scripts/obtain_item.inc').read_text()
assert obtain.count('callnative ChaosRandomizeOverworldItem')==2
assert 'callnative ChaosRandomizeOverworldItem' not in (root/'data/scripts/item_ball_scripts.inc').read_text()
assert 'GetRandomizedScriptedSpecies(monTemplate.species, monTemplate.level, 0)' in (root/'src/script_pokemon_util.c').read_text()
print('PASS: ordinary/hidden pickup script entry points and gift creation hook.')
# The optimized setup traversal must see exactly the same forms as the native
# eligibility predicate, including branching evolutions, Megas and depth limits.
source='src/pokemon.c'
code=base.replace('DoesSpeciesOrReachableFormMatchRunFilterForSettings(enum Species s,u8 mode,u16 val,u8 am,u8 ev,u8 diff,u32 seed)', 'DoesSpeciesMatchRunFilterForSettings(enum Species s,u8 mode,u16 val,u8 am,u32 seed)')+r'''
#define NUM_SPECIES 110
#define SPECIES_EGG 109
#define FORM_SPECIES_END 65535
#define EVOLUTIONS_END 0
#define RUN_EVOLUTIONS_RANDOM 1
struct Evolution {u32 method;enum Species targetSpecies;};
struct Evolution evolutions[13][3]={ [1]={{1,2},{1,5},{0,0}},[2]={{1,3},{0,0}},[3]={{1,4},{0,0}} };
u16 forms[13][3]={[1]={1,12,FORM_SPECIES_END},[4]={4,11,FORM_SPECIES_END}};
bool32 IsSpeciesEnabled(enum Species s){return s>0&&s<13;}
enum Species SanitizeSpeciesId(enum Species s){return s;}
const struct Evolution*GetSpeciesEvolutions(enum Species s){return evolutions[s];}
const u16*GetSpeciesFormTable(enum Species s){return s==1||s==4?forms[s]:NULL;}
u8 GetSpeciesType(enum Species s,u32 slot){return slot==0?s%3+1:3;}
enum Species GetRandomEvolutionTargetForSettings(enum Species s,u8 d,u32 seed){return s<4?s+1:SPECIES_NONE;}
'''+function(source,'DoesSpeciesOrReachableFormMatchRunFilterInternal')+function(source,'DoesSpeciesOrReachableFormMatchRunFilterForSettings')+function(source,'VisitRunFilterReachableSpeciesInternal')+function(source,'VisitRunFilterReachableSpeciesForSettings')+r'''
bool32 seen[110];void visit(enum Species s){seen[s]=TRUE;}
int main(void){
 gSpeciesInfo[11].isMegaEvolution=TRUE;gSpeciesInfo[12].isMegaEvolution=TRUE;
 for(u32 mode=0;mode<2;mode++)for(u32 seed=0;seed<2;seed++)for(enum Species start=1;start<=5;start++){
  memset(seen,0,sizeof(seen));VisitRunFilterReachableSpeciesForSettings(start,mode,0,seed,visit);
  for(u8 type=1;type<=3;type++)for(u16 ability=1;ability<=4;ability++){
   bool32 expected=FALSE;
   for(enum Species s=1;s<13;s++)if(seen[s]&&DoesSpeciesOrReachableFormMatchRunFilterForSettings(s,RUN_FILTER_TYPE_ABILITY,(ability<<5)|type,0,0,0,seed)){
    // Only the direct form's traits count in this aggregation.
    u32 t=s%3+1,a=seed%2?3:2;
    if((type==3||type==t)&&ability==a)expected=TRUE;
   }
   assert(expected==DoesSpeciesOrReachableFormMatchRunFilterForSettings(start,RUN_FILTER_TYPE_ABILITY,(ability<<5)|type,0,mode,0,seed));
  }
 }
 puts("PASS: cached traversal agrees with native reachable-line predicate across branches, Megas, random evolutions, seeds and combined filters.");
}
'''
run(code)
# DrawStdWindowFrame clears the pixel buffer. Chrome must draw the frame first,
# otherwise titles/dividers/footer disappear when the body is printed afterward.
code=base+r'''
#define WINDOW_WIDTH 0
#define FONT_NORMAL 1
#define FONT_SMALL 0
#define TEXT_SKIP_DRAW 0
#define PIXEL_FILL(x) (x)
u8 pixels[224*144];
u8 GetStartMenuWindowId(void){return 0;}
u32 GetWindowAttribute(u8 id,u32 attr){return 28;}
void FillWindowPixelBuffer(u8 id,u32 value){memset(pixels,value,sizeof(pixels));}
void DrawStdWindowFrame(u8 id,bool32 copy){FillWindowPixelBuffer(id,1);}
void AddTextPrinterParameterized(u8 id,u8 font,const u8*text,u8 x,u8 y,u8 speed,void*cb){pixels[y*224+x]=2;}
void FillWindowPixelRect(u8 id,u32 color,u32 x,u32 y,u32 w,u32 h){for(u32 i=0;i<w;i++)pixels[y*224+x+i]=color;}
'''+function('src/start_menu.c','DrawGamePageChrome')+r'''
int main(void){
 DrawGamePageChrome((const u8*)"GAME INFO",(const u8*)"A/B: Back");
 assert(pixels[5*224+8]==2&&pixels[5*224+9]==2);
 assert(pixels[27*224+6]==2&&pixels[126*224+6]==2&&pixels[129*224+8]==2);
 DrawGamePageChrome((const u8*)"GAME RULES",(const u8*)"B: Back");
 assert(pixels[129*224+8]==2);
 puts("PASS: actual shared chrome preserves title, dividers and footer after frame drawing.");
}
'''
run(code)
# A full informational page must not overwrite the reserved dialogue/frame tiles.
code=base+r'''
#define WINDOW_NONE 255
u8 sStartMenuWindowId=WINDOW_NONE;u32 firstTile,tileCount;
u16 AddWindowParameterized(u8 bg,u8 left,u8 top,u8 width,u8 height,u8 palette,u16 base){firstTile=base;tileCount=width*height;assert(left==1&&top==1&&height<=18);return 0;}
'''+function('src/menu.c','AddGameOptionsWindow')+function('src/menu.c','AddQuickToolsWindow')+r'''
int main(void){AddGameOptionsWindow(9);assert(firstTile>0&&firstTile+tileCount<=0x200);sStartMenuWindowId=WINDOW_NONE;AddQuickToolsWindow(6);assert(firstTile>0&&firstTile+tileCount<=0x200);puts("PASS: native full-page menu allocation preserves blank, dialogue and frame tiles.");}
'''
run(code)

# The actual bounded starter selector must retain matches supplied by the
# line-aware generator, including a sparse pool with exactly three members.
run(base+r'''
#define RANDOM_SPECIES_OPTIONS_COUNT 2
#define RANDOM_MON_DEX_HOENN 1
#define HOENN_DEX_COUNT 13
#define RNG_NONE 0
struct RandomSpeciesGeneratorOptions {u32 speciesPoolCount,dexMode;} sRandomSpeciesGeneratorOptions[2];
u32 checks;
enum Species GetRandomSpeciesAtIndex(const struct RandomSpeciesGeneratorOptions *o,u32 i){return i+1;}
enum Species GetSpeciesCandidateForm(enum Species s,const struct RandomSpeciesGeneratorOptions *o,const struct FilterFuncArgs *a){checks++;return a->arg1==0||s==1||s==4||s==9?s:SPECIES_NONE;}
u32 RandomUniform(u32 stream,u32 lo,u32 hi){return lo+Random()%(hi-lo+1);}
'''+function('src/random_mon_generation.c','PickRandomStarterSpecies')+r'''
int main(void){u16 starters[3],again[3];struct FilterFuncArgs a={1,0};
 assert(PickRandomStarterSpecies(0,&a,starters)==3);assert(checks==NATIONAL_DEX_COUNT);
 assert(starters[0]==1&&starters[1]==4&&starters[2]==9);
 a.arg1=0;SeedRng(123);assert(PickRandomStarterSpecies(0,&a,starters)==3);
 assert(starters[0]!=starters[1]&&starters[0]!=starters[2]&&starters[1]!=starters[2]);
 SeedRng(123);assert(PickRandomStarterSpecies(0,&a,again)==3);assert(memcmp(starters,again,sizeof(starters))==0);
 assert(PickRandomStarterSpecies(99,&a,starters)==0);assert(starters[0]==0&&starters[1]==0&&starters[2]==0);
 puts("PASS: actual starter selection is bounded, seeded, distinct and retains sparse evolution-line-eligible candidates.");}
''')

run(base+r'''
enum Ability {ABILITY_FIXTURE=3};
#define SPECIES_EGG 99
#define MON_DATA_SPECIES_OR_EGG 1
#define MON_DATA_ABILITY_NUM 2
struct Pokemon {u16 species,ability;};
struct BoxPokemon {u16 species,ability;};
bool32 hasCurrentAbility;
u32 GetMonData(struct Pokemon *m,u32 f){return m->species;}
u32 GetMonAbility(struct Pokemon *m){return m->ability;}
u32 GetBoxMonData(struct BoxPokemon *m,u32 f){return f==MON_DATA_ABILITY_NUM?m->ability:m->species;}
u32 GetSpeciesAbility(enum Species s,u8 slot){return slot;}
u32 GetActiveRunFilterAbility(void){return 3;}
bool32 SpeciesHasAbilityForSettings(enum Species s,u32 a,u8 mode,u32 seed){return hasCurrentAbility;}
enum Species SanitizeSpeciesId(enum Species s){return s;}
bool32 PlayerPartyHasPermanentMega(void){return FALSE;}
'''+function('src/pokemon.c','DoesSpeciesLineMatchActiveRunFilter')
+function('src/pokemon.c','DoesMonMatchActiveRunFilter')
+function('src/pokemon.c','DoesBoxMonMatchActiveRunFilter')
+function('src/pokemon.c','CanSpeciesJoinActiveRunParty')+r'''
int main(void){struct Pokemon m={1,1};struct BoxPokemon b={1,1};
 save3.filterMode=RUN_FILTER_TYPE_ABILITY;save3.filterValue=(3<<5)|3;save3.worldSeed=1;
 assert(CanSpeciesJoinActiveRunParty(1));assert(DoesMonMatchActiveRunFilter(&m));assert(DoesBoxMonMatchActiveRunFilter(&b));
 hasCurrentAbility=TRUE;assert(!DoesMonMatchActiveRunFilter(&m));assert(!DoesBoxMonMatchActiveRunFilter(&b));
 m.ability=3;b.ability=3;assert(DoesMonMatchActiveRunFilter(&m));assert(DoesBoxMonMatchActiveRunFilter(&b));
 hasCurrentAbility=FALSE;save3.filterValue=(4<<5)|4;m.ability=1;
 assert(!CanSpeciesJoinActiveRunParty(1));assert(!DoesMonMatchActiveRunFilter(&m));assert(!DoesBoxMonMatchActiveRunFilter(&b));
 puts("PASS: actual party/PC admission accepts future-line matches while rejecting wrong current abilities and incompatible lines.");}
''')

run(base+r'''
#define LAYOUT_POKEMON_CENTER_1F 1
#define LAYOUT_LAVARIDGE_TOWN_POKEMON_CENTER_1F 2
#define LAYOUT_POKEMON_CENTER_1F_FRLG 3
#define LAYOUT_ONE_ISLAND_POKEMON_CENTER_1F 4
#define LAYOUT_INDIGO_PLATEAU_POKEMON_CENTER_1F 5
struct {u32 mapLayoutId;} gMapHeader;
'''+function('src/pokemon.c','IsPokemonCenterLayout')+function('src/pokemon.c','IsPlayerInPokemonCenter')+r'''
int main(void){for(u32 i=1;i<=5;i++){gMapHeader.mapLayoutId=i;assert(IsPlayerInPokemonCenter());}
 gMapHeader.mapLayoutId=6;assert(!IsPlayerInPokemonCenter());
 puts("PASS: actual Center detection includes native Kanto, One Island and League centers while excluding other rooms.");}
''')
