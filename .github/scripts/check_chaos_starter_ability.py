#!/usr/bin/env python3
"""Exercise individual starter guarantees using the actual native functions."""
from pathlib import Path
import re,subprocess,tempfile
root=Path(__file__).resolve().parents[2]
def fn(name,path="src/pokemon.c"):
 s=(root/path).read_text();m=re.search(r'^(?:static )?(?:bool32|void|enum Ability) '+name+r'\([^;]*?\)\n\{',s,re.M);assert m,name
 i=m.end();depth=1
 while depth:depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[m.start():i]
code=r'''
#include <assert.h>
#include <string.h>
#include <stdio.h>
typedef unsigned char u8;typedef unsigned short u16;typedef int bool32;
enum Species {SPECIES_NONE,SPECIES_ONE,SPECIES_TWO,SPECIES_EGG};
enum Ability {ABILITY_NONE,ABILITY_ONE,ABILITY_TWO,ABILITY_FILTERED,ABILITIES_COUNT=400};
#define TRUE 1
#define FALSE 0
#define NUM_NORMAL_ABILITY_SLOTS 2
#define RUN_STARTER_CHOOSE 1
#define RUN_ABILITIES_RANDOM 1
#define RUN_FILTER_NONE 0
#define RUN_FILTER_TYPE 1
#define RUN_FILTER_ABILITY 2
#define RUN_FILTER_TYPE_ABILITY 3
#define MON_DATA_SPECIES 1
#define MON_DATA_SPECIES_OR_EGG 1
#define MON_DATA_ABILITY_NUM 2
#define MON_DATA_CHAOS_STARTER_ABILITY 3
struct BoxPokemon {u16 species,slot,override;};
struct Pokemon {struct BoxPokemon box;};
struct {u8 starterMode,abilityMode,filterMode;u16 filterValue;unsigned worldSeed;} save,*gSaveBlock3Ptr=&save;
enum Ability gLastUsedAbility,required=ABILITY_FILTERED;
u16 slots[4][2]={{0,0},{1,2},{2,1},{0,0}};
u16 GetBoxMonData(struct BoxPokemon *m,int f){return f==1?m->species:f==2?m->slot:m->override;}
u16 GetMonData(struct Pokemon *m,int f){return GetBoxMonData(&m->box,f);}
void SetMonData(struct Pokemon *m,int f,const void *v){if(f==2){m->box.slot=*(const u8*)v;m->box.override=0;}else if(f==3)m->box.override=*(const u16*)v;else m->box.species=*(const u16*)v;}
enum Ability GetSpeciesAbility(enum Species s,u8 slot){return slots[s][slot];}
enum Ability GetAbilityBySpecies(enum Species s,u8 slot){return gLastUsedAbility=GetSpeciesAbility(s,slot);}
enum Ability GetActiveRunFilterAbility(void){return required;}
enum Ability GetActiveRunFilterAbilityForMonChanges(void){return required;}
bool32 TrySetMonAbilityToActiveRunFilter(struct Pokemon *m);
struct {unsigned natDexNum;u8 types[2];} gSpeciesInfo[4]={ {0,{0,0}}, {1,{9,9}}, {2,{10,10}} };
u16 sCustomStarterCount,sCustomStarterList[4];
bool32 IsCustomStarterBaseEligible(enum Species s){return s==SPECIES_ONE||s==SPECIES_TWO;}
bool32 DoesSpeciesMatchRunFilterForSettings(enum Species s,u8 mode,u16 value,u8 abilities,unsigned seed){return mode==RUN_FILTER_TYPE?gSpeciesInfo[s].types[0]==value:slots[s][0]==value||slots[s][1]==value;}
'''+fn('GetBoxMonAbility')+fn('GetMonAbility')+fn('ApplyCustomStarterRunAbility')+fn('TrySetMonAbilityToActiveRunFilter')+fn('IsCustomStarterEligible','src/starter_choose.c')+r'''
int main(void){
 struct Pokemon m={{SPECIES_ONE,0,0}},ordinary=m,restored;
 save.starterMode=RUN_STARTER_CHOOSE;save.abilityMode=0;save.filterMode=RUN_FILTER_ABILITY;save.filterValue=3;
 assert(!IsCustomStarterEligible(SPECIES_ONE));
 save.abilityMode=RUN_ABILITIES_RANDOM;assert(IsCustomStarterEligible(SPECIES_ONE)&&IsCustomStarterEligible(SPECIES_TWO));
 save.filterMode=RUN_FILTER_TYPE_ABILITY;save.filterValue=(3<<5)|9;
 assert(IsCustomStarterEligible(SPECIES_ONE)&&!IsCustomStarterEligible(SPECIES_TWO));
 save.abilityMode=0;
 ApplyCustomStarterRunAbility(&m);assert(GetMonAbility(&m)==ABILITY_ONE&&m.box.override==0);
 save.abilityMode=RUN_ABILITIES_RANDOM;ApplyCustomStarterRunAbility(&m);
 assert(GetMonAbility(&m)==required&&m.box.slot==0);
 assert(GetMonAbility(&ordinary)==ABILITY_ONE&&slots[SPECIES_ONE][0]==ABILITY_ONE);
 memcpy(&restored,&m,sizeof(m));assert(GetBoxMonAbility(&restored.box)==required);
 restored.box.species=SPECIES_TWO;assert(TrySetMonAbilityToActiveRunFilter(&restored));assert(GetMonAbility(&restored)==required);
 u8 slot=1;SetMonData(&restored,MON_DATA_ABILITY_NUM,&slot);assert(restored.box.override==0&&GetMonAbility(&restored)==ABILITY_ONE);
 required=300;ApplyCustomStarterRunAbility(&m);assert(GetMonAbility(&m)==300);
 puts("PASS: normal abilities stay natural; custom random starter guarantee is individual, normal-slot, persistent through copying/boxing/evolution, and explicit ability changes replace it.");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'check.c';p.write_text(code)
 subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/check'],check=True)
 subprocess.run([d+'/check'],check=True)
