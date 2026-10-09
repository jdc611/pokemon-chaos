"""Compile production Chaos stat functions against the V3 locked boundary cases.
Battle activation, copying and held-item behavior are exercised by native ROM suites.
"""
from pathlib import Path
import re, subprocess, tempfile
root=Path(__file__).resolve().parents[2]
source=(root/'src/chaos_abilities.c').read_text()
def function(name):
    match=re.search(r'^(?:bool32|u32) '+name+r'\([^;]*?\)\n\{',source,re.M)
    assert match,name
    i=match.end();depth=1
    while depth:
        depth+=(source[i]=='{')-(source[i]=='}');i+=1
    return re.sub(r'\benum \w+','int',source[match.start():i])
code='\n'.join(function(n) for n in ['ChaosAbilityIsFallback','ChaosAbilityStat','ChaosAbilityBeforeMove','ChaosAbilityHealing'])
tokens=sorted(set(re.findall(r'\b(?:SPECIES|ABILITY|STAT)_\w+',code)))
defines='\n'.join(f'#define {n} {i+1}' for i,n in enumerate(tokens))
prefix=r'''
#include <assert.h>
#include <stdio.h>
typedef unsigned u32;typedef int bool32;
#define TRUE 1
#define FALSE 0
#define NULL ((void *)0)
#define max(a,b) ((a)>(b)?(a):(b))
struct PartyState {int chaosBlade,chaosComplete,chaosHeroic,chaosTera,chaosHangry;};
struct PartyState state;
struct {unsigned species,hp,maxHP,level;} gBattleMons[4];
struct {unsigned natDexNum;} gSpeciesInfo[4096];
struct PartyState *GetBattlerPartyState(unsigned b){return &state;}
'''
tests=r'''
int main(void){
 for(unsigned i=0;i<4096;i++)gSpeciesInfo[i].natDexNum=i;
 gBattleMons[0].species=4000;gBattleMons[0].hp=100;gBattleMons[0].maxHP=100;gBattleMons[0].level=20;
 assert(ChaosAbilityStat(0,ABILITY_ZEN_MODE,STAT_SPATK,100)==100);
 gBattleMons[0].hp=50;assert(ChaosAbilityStat(0,ABILITY_ZEN_MODE,STAT_SPATK,100)==150);assert(ChaosAbilityStat(0,ABILITY_ZEN_MODE,STAT_SPDEF,100)==150);assert(ChaosAbilityStat(0,ABILITY_ZEN_MODE,STAT_ATK,100)==100);
 gBattleMons[0].hp=51;assert(ChaosAbilityStat(0,ABILITY_ZEN_MODE,STAT_SPATK,100)==100);
 for(unsigned level=19;level<=20;level++)for(unsigned hp=25;hp<=26;hp++){gBattleMons[0].level=level;gBattleMons[0].hp=hp;assert(ChaosAbilityStat(0,ABILITY_SCHOOLING,STAT_ATK,100)==(level==20 && hp==26?130:100));assert(ChaosAbilityStat(0,ABILITY_SCHOOLING,STAT_SPEED,100)==100);}
 gBattleMons[0].hp=51;assert(ChaosAbilityStat(0,ABILITY_SHIELDS_DOWN,STAT_DEF,100)==120);
 gBattleMons[0].hp=50;assert(ChaosAbilityStat(0,ABILITY_SHIELDS_DOWN,STAT_ATK,100)==130);assert(ChaosAbilityStat(0,ABILITY_SHIELDS_DOWN,STAT_SPEED,100)==130);assert(ChaosAbilityStat(0,ABILITY_SHIELDS_DOWN,STAT_DEF,100)==100);
 state.chaosBlade=1;assert(ChaosAbilityStat(0,ABILITY_STANCE_CHANGE,STAT_ATK,100)==130);assert(ChaosAbilityStat(0,ABILITY_STANCE_CHANGE,STAT_SPDEF,100)==80);state.chaosBlade=0;assert(ChaosAbilityStat(0,ABILITY_STANCE_CHANGE,STAT_ATK,100)==100);
 state.chaosComplete=1;assert(ChaosAbilityStat(0,ABILITY_POWER_CONSTRUCT,STAT_DEF,100)==150);assert(ChaosAbilityStat(0,ABILITY_POWER_CONSTRUCT,STAT_SPATK,100)==120);
 state.chaosHeroic=1;assert(ChaosAbilityStat(0,ABILITY_ZERO_TO_HERO,STAT_ATK,100)==130);assert(ChaosAbilityStat(0,ABILITY_ZERO_TO_HERO,STAT_DEF,100)==120);
 state.chaosTera=1;assert(ChaosAbilityStat(0,ABILITY_TERA_SHIFT,STAT_SPATK,100)==110);assert(ChaosAbilityStat(0,ABILITY_TERA_SHIFT,STAT_SPDEF,100)==130);
 assert(ChaosAbilityStat(0,ABILITY_HUNGER_SWITCH,STAT_DEF,100)==120);state.chaosHangry=1;assert(ChaosAbilityStat(0,ABILITY_HUNGER_SWITCH,STAT_SPEED,100)==120);assert(ChaosAbilityStat(0,ABILITY_HUNGER_SWITCH,STAT_DEF,100)==100);
 gBattleMons[0].species=SPECIES_DARMANITAN;assert(!ChaosAbilityIsFallback(0,ABILITY_ZEN_MODE));assert(ChaosAbilityStat(0,ABILITY_ZEN_MODE,STAT_SPATK,100)==100);
 assert(!ChaosAbilityBeforeMove(0,1));assert(ChaosAbilityHealing(0,93)==93);
 puts("PASS production V3 Chaos stat multipliers, exact HP/level boundaries, native-form exclusion and unchanged healing.");
}
'''
with tempfile.TemporaryDirectory() as temp:
 p=Path(temp)/'test.c';p.write_text(prefix+defines+'\n'+code+'\n'+tests)
 subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',str(p.with_suffix(''))],check=True)
 subprocess.run([str(p.with_suffix(''))],check=True)
