"""Execute the real Chaos state implementation with a minimal battle harness."""
from pathlib import Path
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
code = (root / 'src/chaos_abilities.c').read_text()
code = re.sub(r'^#include .*\n', '', code, flags=re.M)
code = re.sub(r'\benum \w+', 'int', code)
tokens = sorted(set(re.findall(r'\b(?:SPECIES|ABILITY|MOVE|STAT|MON_DATA|B_WEATHER|HEALTHBOX)_\w+', code)))
fixed = {'STAT_HP': 0, 'STAT_ATK': 1, 'STAT_DEF': 2, 'STAT_SPEED': 3, 'STAT_SPATK': 4, 'STAT_SPDEF': 5,
         'MON_DATA_HP': 0, 'MON_DATA_MAX_HP': 1, 'B_WEATHER_HAIL': 1, 'B_WEATHER_SNOW': 2}
definitions = '\n'.join(f'#define {t} {fixed.get(t, i + 20)}' for i, t in enumerate(tokens))
prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned u32;
typedef int s32;
typedef int bool32;
#define TRUE 1
#define FALSE 0
#define COMPOUND_STRING(x) ((const u8 *)(x))
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#define MAX_BATTLE_TRAINERS 4
#define PARTY_SIZE 6
struct Pokemon { u16 hp, maxHP; } parties[4][6];
struct PartyState { u16 chaosBaseMaxHp; int chaosHeroic,chaosBond,chaosShieldBroken,chaosDisguiseBroken,chaosIceBroken,chaosIceRestored,chaosComplete,chaosBlade,chaosHangry,chaosEntryApplied,chaosHitPopup; };
struct { struct PartyState partyState[4][6]; } battle, *gBattleStruct=&battle;
struct { int inBattle; } gMain={1};
struct { int natDexNum; } gSpeciesInfo[1024];
struct { int species,ability; u16 hp,maxHP,attack,defense,spAttack,spDefense,speed; int statStages[8]; } gBattleMons[4];
struct { int battler; } gBattleScripting;
u32 gBattlerAbility,gEffectBattler,gLastUsedAbility,gBattleWeather,gHealthboxSpriteIds[4];
u8 gBattleTextBuff1[100];
const u8 BattleScript_ChaosAbilityState[]={0};
int popupCount, substitute;
int GetBattlerAbility(int b) { return gBattleMons[b].ability; }
struct PartyState *GetBattlerPartyState(int b) { return &battle.partyState[b%2][b/2]; }
struct Pokemon *GetBattlerMon(int b) { return &parties[b%2][b/2]; }
struct Pokemon *GetTrainerParty(int t) { return parties[t]; }
int GetMonData(struct Pokemon *p,int field) { return field==0?p->hp:p->maxHP; }
void SetMonData(struct Pokemon *p,int field,const void *v) { if(field==0)p->hp=*(const u16*)v;else p->maxHP=*(const u16*)v; }
void StringCopy(u8 *a,const u8 *b) { strcpy((char*)a,(const char*)b); }
void UpdateHealthboxAttribute(int a,struct Pokemon *p,int c) {}
void BattleScriptCall(const u8 *script) { popupCount++; }
void SetStatChange(int b,int stat,int amount) { gBattleMons[b].statStages[stat]=min(12,gBattleMons[b].statStages[stat]+amount); }
int IsBattlerAlive(int b) { return gBattleMons[b].hp!=0; }
int IsBattleMoveStatus(int m) { return m>=200; }
int IsBattleMovePhysical(int m) { return m==1; }
int DoesSubstituteBlockMove(int a,int d,int m) { return substitute; }
'''
tests = r'''
static void reset(int ability) {
    memset(&battle,0,sizeof battle); memset(gBattleMons,0,sizeof gBattleMons); memset(parties,0,sizeof parties);
    popupCount=substitute=gBattleWeather=0;
    for(int i=0;i<1024;i++)gSpeciesInfo[i].natDexNum=i;
    gBattleMons[0].species=900; gBattleMons[0].ability=ability;
    gBattleMons[0].hp=gBattleMons[0].maxHP=100;
    gBattleMons[0].attack=80;gBattleMons[0].spAttack=120;
    gBattleMons[0].defense=200;gBattleMons[0].spDefense=180;gBattleMons[0].speed=60;
    parties[0][0].hp=parties[0][0].maxHP=100;
    for(int stat=0;stat<8;stat++)gBattleMons[0].statStages[stat]=6;
}
int main(void) {
    reset(ABILITY_ZERO_TO_HERO);
    assert(!ChaosAbilitySwitchIn(0));assert(!GetBattlerPartyState(0)->chaosHeroic);
    assert(ChaosAbilitySwitchOut(0));assert(popupCount==1);
    assert(ChaosAbilitySwitchIn(0));assert(gBattleMons[0].statStages[STAT_SPATK]==7);assert(gBattleMons[0].statStages[STAT_DEF]==7);
    assert(!ChaosAbilitySwitchIn(0));assert(gBattleMons[0].statStages[STAT_SPATK]==7);
    assert(!ChaosAbilitySwitchOut(0));for(int i=0;i<8;i++)gBattleMons[0].statStages[i]=6;
    assert(ChaosAbilitySwitchIn(0));assert(gBattleMons[0].statStages[STAT_SPATK]==7);
    gBattleMons[0].species=SPECIES_PALAFIN_ZERO;assert(!ChaosAbilityIsFallback(0,ABILITY_ZERO_TO_HERO));
    reset(ABILITY_BATTLE_BOND);assert(ChaosAbilityBond(0));assert(!ChaosAbilityBond(0));assert(gBattleMons[0].statStages[STAT_SPATK]==7);assert(gBattleMons[0].statStages[STAT_SPEED]==7);
    reset(ABILITY_SCHOOLING);assert(ChaosAbilityStat(0,ABILITY_SCHOOLING,STAT_SPATK,100)==120);assert(ChaosAbilityStat(0,ABILITY_SCHOOLING,STAT_ATK,100)==100);
    gBattleMons[0].hp=25;assert(ChaosAbilityStat(0,ABILITY_SCHOOLING,STAT_DEF,100)==100);gBattleMons[0].hp=26;assert(ChaosAbilityStat(0,ABILITY_SCHOOLING,STAT_DEF,100)==120);
    reset(ABILITY_SHIELDS_DOWN);assert(ChaosAbilityStat(0,ABILITY_SHIELDS_DOWN,STAT_DEF,100)==150);gBattleMons[0].hp=50;assert(ChaosAbilityMoveEnd(0));assert(gBattleMons[0].statStages[STAT_SPEED]==7);gBattleMons[0].hp=100;assert(ChaosAbilityStat(0,ABILITY_SHIELDS_DOWN,STAT_DEF,100)==100);assert(!ChaosAbilityMoveEnd(0));
    reset(ABILITY_DISGUISE);assert(ChaosAbilityDamage(1,0,1,80)==20);assert(ChaosAbilityDamage(1,0,1,80)==20);ChaosAbilityCommitHit(0,1);assert(ChaosAbilityDamage(1,0,1,80)==80);assert(ChaosAbilityMoveEnd(0));assert(!ChaosAbilityMoveEnd(0));
    reset(ABILITY_DISGUISE);substitute=1;assert(ChaosAbilityDamage(1,0,1,80)==80);
    reset(ABILITY_ICE_FACE);assert(ChaosAbilityDamage(1,0,2,80)==80);ChaosAbilityCommitHit(0,2);assert(!GetBattlerPartyState(0)->chaosIceBroken);assert(ChaosAbilityDamage(1,0,1,80)==20);ChaosAbilityCommitHit(0,1);assert(ChaosAbilityDamage(1,0,1,80)==80);
    gBattleWeather=B_WEATHER_SNOW;assert(ChaosAbilityEndTurn(0));assert(ChaosAbilityDamage(1,0,1,80)==20);ChaosAbilityCommitHit(0,1);assert(!ChaosAbilityEndTurn(0));assert(ChaosAbilityDamage(1,0,1,80)==80);
    reset(ABILITY_POWER_CONSTRUCT);gBattleMons[0].hp=50;assert(ChaosAbilityMoveEnd(0));assert(gBattleMons[0].maxHP==125&&gBattleMons[0].hp==75);assert(gBattleMons[0].statStages[STAT_DEF]==7&&gBattleMons[0].statStages[STAT_SPDEF]==7);assert(!ChaosAbilityMoveEnd(0));
    ChaosAbilityBeforeStats(&parties[0][0]);assert(parties[0][0].maxHP==100&&parties[0][0].hp==50);parties[0][0].maxHP=104;parties[0][0].hp=54;ChaosAbilityAfterStats(&parties[0][0]);assert(parties[0][0].maxHP==130&&parties[0][0].hp==80);ChaosAbilityBattleEnd();assert(parties[0][0].maxHP==104&&parties[0][0].hp==54);
    reset(ABILITY_POWER_CONSTRUCT);gBattleMons[0].hp=50;ChaosAbilityMoveEnd(0);parties[0][0].hp=0;ChaosAbilityBattleEnd();assert(parties[0][0].hp==0);
    reset(ABILITY_POWER_CONSTRUCT);gBattleMons[0].hp=50;ChaosAbilityMoveEnd(0);ChaosAbilityPrepareCapture(0);assert(parties[0][0].maxHP==100&&parties[0][0].hp==50);assert(gBattleMons[0].maxHP==100&&gBattleMons[0].hp==50);assert(!GetBattlerPartyState(0)->chaosBaseMaxHp);ChaosAbilityBattleEnd();assert(parties[0][0].maxHP==100&&parties[0][0].hp==50);
    reset(ABILITY_STANCE_CHANGE);assert(ChaosAbilityStat(0,ABILITY_STANCE_CHANGE,STAT_DEF,100)==130);assert(ChaosAbilityStat(0,ABILITY_STANCE_CHANGE,STAT_ATK,100)==85);assert(ChaosAbilityBeforeMove(0,1));assert(ChaosAbilityStat(0,ABILITY_STANCE_CHANGE,STAT_ATK,100)==130);assert(!ChaosAbilityBeforeMove(0,201));assert(GetBattlerPartyState(0)->chaosBlade);assert(ChaosAbilityBeforeMove(0,MOVE_PROTECT));assert(!GetBattlerPartyState(0)->chaosBlade);ChaosAbilityBeforeMove(0,1);ChaosAbilitySwitchOut(0);assert(!GetBattlerPartyState(0)->chaosBlade);
    reset(ABILITY_HUNGER_SWITCH);assert(ChaosAbilityHealing(0,50)==60);assert(ChaosAbilityDamage(0,1,1,100)==100);ChaosAbilityEndTurn(0);assert(ChaosAbilityHealing(0,50)==50);assert(ChaosAbilityDamage(0,1,1,100)==110);assert(ChaosAbilityDamage(1,0,1,100)==110);ChaosAbilitySwitchOut(0);assert(!GetBattlerPartyState(0)->chaosHangry);
    int abilities[]={ABILITY_ZERO_TO_HERO,ABILITY_BATTLE_BOND,ABILITY_SCHOOLING,ABILITY_SHIELDS_DOWN,ABILITY_DISGUISE,ABILITY_ICE_FACE,ABILITY_POWER_CONSTRUCT,ABILITY_STANCE_CHANGE,ABILITY_HUNGER_SWITCH};
    int natives[]={SPECIES_PALAFIN_ZERO,SPECIES_GRENINJA,SPECIES_WISHIWASHI_SOLO,SPECIES_MINIOR_METEOR_RED,SPECIES_MIMIKYU_DISGUISED,SPECIES_EISCUE_ICE,SPECIES_ZYGARDE_50,SPECIES_AEGISLASH_SHIELD,SPECIES_MORPEKO_FULL_BELLY};
    for(int i=0;i<9;i++){reset(abilities[i]);gBattleMons[0].species=natives[i];assert(!ChaosAbilityIsFallback(0,abilities[i]));gBattleMons[0].species=901;gSpeciesInfo[901].natDexNum=gSpeciesInfo[natives[i]].natDexNum;assert(!ChaosAbilityIsFallback(0,abilities[i]));}
    puts("PASS: nine Chaos fallbacks, native form guards, thresholds, switch persistence, first-hit protection, weather restoration and temporary HP cleanup.");
}
'''
# Status move IDs must be distinct from our two damage-category test moves.
for token in re.findall(r'\bMOVE_\w+', definitions):
    definitions = re.sub(r'#define ' + token + r' \d+', f'#define {token} {200 + tokens.index(token)}', definitions)
with tempfile.TemporaryDirectory() as tmp:
    src, binary = Path(tmp) / 'test.c', Path(tmp) / 'test'
    src.write_text(definitions + '\n' + prefix + '\n' + code + '\n' + tests)
    subprocess.run(['cc', '-std=c99', '-Wall', '-Wno-unused-parameter', str(src), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
