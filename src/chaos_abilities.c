#include "global.h"
#include "battle.h"
#include "battle_stat_change.h"
#include "battle_util.h"
#include "battle_interface.h"
#include "battle_script_commands.h"
#include "battle_scripts.h"
#include "chaos_abilities.h"
#include "string_util.h"
#include "main.h"
#include "constants/abilities.h"
#include "constants/species.h"
#include "constants/moves.h"

extern const u8 BattleScript_ChaosAbilityState[];
static bool32 CheckThreshold(u32 battler, enum Ability ability);

// Native forms share a National Dex number. This covers all native variants,
// not just their base-form species IDs, without introducing alternate sprites.
bool32 ChaosAbilityIsFallback(u32 battler, enum Ability ability)
{
    enum Species native;
    switch (ability)
    {
    case ABILITY_ZERO_TO_HERO: native = SPECIES_PALAFIN_ZERO; break;
    case ABILITY_BATTLE_BOND: native = SPECIES_GRENINJA; break;
    case ABILITY_SCHOOLING: native = SPECIES_WISHIWASHI_SOLO; break;
    case ABILITY_SHIELDS_DOWN: native = SPECIES_MINIOR_METEOR_RED; break;
    case ABILITY_DISGUISE: native = SPECIES_MIMIKYU_DISGUISED; break;
    case ABILITY_ICE_FACE: native = SPECIES_EISCUE_ICE; break;
    case ABILITY_POWER_CONSTRUCT: native = SPECIES_ZYGARDE_50; break;
    case ABILITY_STANCE_CHANGE: native = SPECIES_AEGISLASH_SHIELD; break;
    case ABILITY_HUNGER_SWITCH: native = SPECIES_MORPEKO_FULL_BELLY; break;
    default: return FALSE;
    }
    return gSpeciesInfo[gBattleMons[battler].species].natDexNum != gSpeciesInfo[native].natDexNum;
}

static enum Stat HigherOffense(u32 battler)
{
    return gBattleMons[battler].attack >= gBattleMons[battler].spAttack ? STAT_ATK : STAT_SPATK;
}

static u32 RawStat(u32 battler, enum Stat stat)
{
    switch (stat)
    {
    case STAT_ATK: return gBattleMons[battler].attack;
    case STAT_DEF: return gBattleMons[battler].defense;
    case STAT_SPATK: return gBattleMons[battler].spAttack;
    case STAT_SPDEF: return gBattleMons[battler].spDefense;
    case STAT_SPEED: return gBattleMons[battler].speed;
    default: return 0;
    }
}

static bool32 StateMessage(u32 battler, const u8 *state)
{
    gBattleScripting.battler = gBattlerAbility = gEffectBattler = battler;
    gLastUsedAbility = GetBattlerAbility(battler);
    StringCopy(gBattleTextBuff1, state);
    UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], GetBattlerMon(battler), HEALTHBOX_NICK);
    BattleScriptCall(BattleScript_ChaosAbilityState);
    return TRUE;
}

bool32 ChaosAbilitySwitchOut(u32 battler)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    enum Ability ability = GetBattlerAbility(battler);
    state->chaosEntryApplied = FALSE;
    state->chaosBlade = FALSE;
    state->chaosHangry = FALSE;
    if (ChaosAbilityIsFallback(battler, ability) && ability == ABILITY_ZERO_TO_HERO && !state->chaosHeroic && gBattleMons[battler].hp != 0)
    {
        state->chaosHeroic = TRUE;
        return StateMessage(battler, COMPOUND_STRING("Heroic"));
    }
    return FALSE;
}

bool32 ChaosAbilitySwitchIn(u32 battler)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    enum Ability ability = GetBattlerAbility(battler);
    if (!ChaosAbilityIsFallback(battler, ability) || state->chaosEntryApplied)
        return FALSE;
    state->chaosEntryApplied = TRUE;
    state->chaosBlade = state->chaosHangry = FALSE;
    if (ability == ABILITY_ZERO_TO_HERO && state->chaosHeroic)
    {
        enum Stat offense = HigherOffense(battler), second = STAT_HP;
        u32 highest = 0;
        for (enum Stat stat = STAT_ATK; stat <= STAT_SPDEF; stat++)
        {
            if (stat != offense && RawStat(battler, stat) > highest)
            {
                second = stat;
                highest = RawStat(battler, stat);
            }
        }
        SetStatChange(battler, offense, 1);
        SetStatChange(battler, second, 1);
        return StateMessage(battler, COMPOUND_STRING("Heroic"));
    }
    return CheckThreshold(battler, ability);
}

bool32 ChaosAbilityBond(u32 battler)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    if (!ChaosAbilityIsFallback(battler, ABILITY_BATTLE_BOND) || state->chaosBond)
        return FALSE;
    state->chaosBond = TRUE;
    SetStatChange(battler, HigherOffense(battler), 1);
    SetStatChange(battler, STAT_SPEED, 1);
    return StateMessage(battler, COMPOUND_STRING("Bonded"));
}

static bool32 CheckThreshold(u32 battler, enum Ability ability)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    if (!ChaosAbilityIsFallback(battler, ability) || !IsBattlerAlive(battler))
        return FALSE;
    if (ability == ABILITY_SHIELDS_DOWN && !state->chaosShieldBroken && gBattleMons[battler].hp * 2 <= gBattleMons[battler].maxHP)
    {
        state->chaosShieldBroken = TRUE;
        SetStatChange(battler, HigherOffense(battler), 1);
        SetStatChange(battler, STAT_SPEED, 1);
        return StateMessage(battler, COMPOUND_STRING("Exposed"));
    }
    if (ability == ABILITY_POWER_CONSTRUCT && !state->chaosComplete && gBattleMons[battler].hp * 2 <= gBattleMons[battler].maxHP)
    {
        struct Pokemon *mon = GetBattlerMon(battler);
        u32 delta;
        state->chaosComplete = TRUE;
        state->chaosBaseMaxHp = gBattleMons[battler].maxHP;
        delta = max(1, state->chaosBaseMaxHp / 4);
        gBattleMons[battler].maxHP += delta;
        gBattleMons[battler].hp += delta;
        SetMonData(mon, MON_DATA_MAX_HP, &gBattleMons[battler].maxHP);
        SetMonData(mon, MON_DATA_HP, &gBattleMons[battler].hp);
        SetStatChange(battler, STAT_DEF, 1);
        SetStatChange(battler, STAT_SPDEF, 1);
        UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], mon, HEALTHBOX_ALL);
        return StateMessage(battler, COMPOUND_STRING("Complete"));
    }
    return FALSE;
}

bool32 ChaosAbilityEndTurn(u32 battler)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    enum Ability ability = GetBattlerAbility(battler);
    if (!ChaosAbilityIsFallback(battler, ability))
        return FALSE;
    if (CheckThreshold(battler, ability))
        return TRUE;
    if (ability == ABILITY_HUNGER_SWITCH)
        state->chaosHangry ^= TRUE;
    if (ability == ABILITY_ICE_FACE && state->chaosIceBroken && !state->chaosIceRestored && (gBattleWeather & (B_WEATHER_HAIL | B_WEATHER_SNOW)))
    {
        state->chaosIceBroken = FALSE;
        state->chaosIceRestored = TRUE;
        return StateMessage(battler, COMPOUND_STRING("Ice-protected"));
    }
    UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], GetBattlerMon(battler), HEALTHBOX_NICK);
    return FALSE;
}

bool32 ChaosAbilityMoveEnd(u32 battler)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    if (state->chaosHitPopup)
    {
        state->chaosHitPopup = FALSE;
        return StateMessage(battler, GetBattlerAbility(battler) == ABILITY_ICE_FACE ? COMPOUND_STRING("Exposed") : COMPOUND_STRING("Unmasked"));
    }
    return CheckThreshold(battler, GetBattlerAbility(battler));
}

bool32 ChaosAbilityBeforeMove(u32 battler, enum Move move)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    bool32 blade = state->chaosBlade;
    if (!ChaosAbilityIsFallback(battler, GetBattlerAbility(battler)) || GetBattlerAbility(battler) != ABILITY_STANCE_CHANGE)
        return FALSE;
    if (!IsBattleMoveStatus(move))
        blade = TRUE;
    else if (move == MOVE_PROTECT || move == MOVE_DETECT || move == MOVE_KINGS_SHIELD || move == MOVE_SPIKY_SHIELD || move == MOVE_BANEFUL_BUNKER || move == MOVE_OBSTRUCT || move == MOVE_SILK_TRAP || move == MOVE_BURNING_BULWARK || move == MOVE_ENDURE)
        blade = FALSE;
    if (blade == state->chaosBlade)
        return FALSE;
    state->chaosBlade = blade;
    return StateMessage(battler, blade ? COMPOUND_STRING("Blade-ready") : COMPOUND_STRING("Shielded"));
}

u32 ChaosAbilityStat(u32 battler, enum Ability ability, enum Stat stat, u32 value)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    if (!ChaosAbilityIsFallback(battler, ability))
        return value;
    if (ability == ABILITY_SCHOOLING && gBattleMons[battler].hp * 4 > gBattleMons[battler].maxHP
     && (stat == STAT_DEF || stat == STAT_SPDEF || stat == HigherOffense(battler)))
        return max(1, value * 6 / 5);
    if (ability == ABILITY_SHIELDS_DOWN && !state->chaosShieldBroken && gBattleMons[battler].hp * 2 > gBattleMons[battler].maxHP && (stat == STAT_DEF || stat == STAT_SPDEF))
        return max(1, value * 3 / 2);
    if (ability == ABILITY_STANCE_CHANGE && (stat == STAT_ATK || stat == STAT_SPATK || stat == STAT_DEF || stat == STAT_SPDEF))
    {
        bool32 offense = stat == STAT_ATK || stat == STAT_SPATK;
        return max(1, value * ((offense == state->chaosBlade) ? 130 : 85) / 100);
    }
    return value;
}

s32 ChaosAbilityDamage(u32 attacker, u32 defender, enum Move move, s32 damage)
{
    enum Ability atk = GetBattlerAbility(attacker), def = GetBattlerAbility(defender);
    struct PartyState *state = GetBattlerPartyState(defender);
    if (damage <= 0)
        return damage;
    if (atk == ABILITY_HUNGER_SWITCH && ChaosAbilityIsFallback(attacker, atk) && GetBattlerPartyState(attacker)->chaosHangry)
        damage = max(1, damage * 11 / 10);
    if (def == ABILITY_HUNGER_SWITCH && ChaosAbilityIsFallback(defender, def) && state->chaosHangry)
        damage = max(1, damage * 11 / 10);
    if (!DoesSubstituteBlockMove(attacker, defender, move) && ChaosAbilityIsFallback(defender, def)
     && ((def == ABILITY_DISGUISE && !state->chaosDisguiseBroken)
      || (def == ABILITY_ICE_FACE && !state->chaosIceBroken && IsBattleMovePhysical(move))))
        damage = max(1, damage / 4);
    return damage;
}

void ChaosAbilityCommitHit(u32 defender, enum Move move)
{
    enum Ability ability = GetBattlerAbility(defender);
    struct PartyState *state = GetBattlerPartyState(defender);
    if (!ChaosAbilityIsFallback(defender, ability))
        return;
    if (ability == ABILITY_DISGUISE && !state->chaosDisguiseBroken)
        state->chaosHitPopup = state->chaosDisguiseBroken = TRUE;
    if (ability == ABILITY_ICE_FACE && !state->chaosIceBroken && IsBattleMovePhysical(move))
        state->chaosHitPopup = state->chaosIceBroken = TRUE;
}

u32 ChaosAbilityHealing(u32 battler, u32 amount)
{
    if (GetBattlerAbility(battler) == ABILITY_HUNGER_SWITCH && ChaosAbilityIsFallback(battler, ABILITY_HUNGER_SWITCH) && !GetBattlerPartyState(battler)->chaosHangry)
        return max(1, amount * 6 / 5);
    return amount;
}

const u8 *ChaosAbilityMarker(u32 battler)
{
    enum Ability ability = GetBattlerAbility(battler);
    struct PartyState *s = GetBattlerPartyState(battler);
    if (!ChaosAbilityIsFallback(battler, ability)) return NULL;
    switch (ability)
    {
    case ABILITY_ZERO_TO_HERO: return s->chaosHeroic ? COMPOUND_STRING("HERO") : NULL;
    case ABILITY_BATTLE_BOND: return s->chaosBond ? COMPOUND_STRING("BOND") : NULL;
    case ABILITY_SCHOOLING: return gBattleMons[battler].hp * 4 > gBattleMons[battler].maxHP ? COMPOUND_STRING("SCHOOL") : NULL;
    case ABILITY_SHIELDS_DOWN: return s->chaosShieldBroken ? COMPOUND_STRING("OPEN") : COMPOUND_STRING("SHIELD");
    case ABILITY_DISGUISE: return s->chaosDisguiseBroken ? COMPOUND_STRING("BROKEN") : NULL;
    case ABILITY_ICE_FACE: return s->chaosIceBroken ? NULL : COMPOUND_STRING("ICE");
    case ABILITY_POWER_CONSTRUCT: return s->chaosComplete ? COMPOUND_STRING("COMPLETE") : NULL;
    case ABILITY_STANCE_CHANGE: return s->chaosBlade ? COMPOUND_STRING("BLADE") : COMPOUND_STRING("SHIELD");
    case ABILITY_HUNGER_SWITCH: return s->chaosHangry ? COMPOUND_STRING("HANGRY") : COMPOUND_STRING("FULL");
    default: return NULL;
    }
}

void ChaosAbilityBattleEnd(void)
{
    for (u32 trainer = 0; trainer < MAX_BATTLE_TRAINERS; trainer++)
    {
        struct Pokemon *party = GetTrainerParty(trainer);
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            struct PartyState *s = &gBattleStruct->partyState[trainer][slot];
            if (s->chaosBaseMaxHp != 0)
            {
                u32 hp = GetMonData(&party[slot], MON_DATA_HP);
                u32 extra = max(1, s->chaosBaseMaxHp / 4);
                hp = hp == 0 ? 0 : (hp > extra ? hp - extra : 1);
                hp = min(hp, s->chaosBaseMaxHp);
                SetMonData(&party[slot], MON_DATA_MAX_HP, &s->chaosBaseMaxHp);
                SetMonData(&party[slot], MON_DATA_HP, &hp);
                s->chaosBaseMaxHp = 0;
            }
        }
    }
}

// Level-up and form-stat recalculation must not silently discard COMPLETE HP
// or restore an obsolete pre-level-up maximum at the end of the battle.
static struct PartyState *CompleteStateForMon(struct Pokemon *mon)
{
    if (!gMain.inBattle || gBattleStruct == NULL)
        return NULL;
    for (u32 trainer = 0; trainer < MAX_BATTLE_TRAINERS; trainer++)
    {
        struct Pokemon *party = GetTrainerParty(trainer);
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
            if (mon == &party[slot] && gBattleStruct->partyState[trainer][slot].chaosBaseMaxHp != 0)
                return &gBattleStruct->partyState[trainer][slot];
    }
    return NULL;
}

void ChaosAbilityBeforeStats(struct Pokemon *mon)
{
    struct PartyState *s = CompleteStateForMon(mon);
    if (s != NULL)
    {
        u32 hp = GetMonData(mon, MON_DATA_HP), extra = max(1, s->chaosBaseMaxHp / 4);
        hp = hp == 0 ? 0 : (hp > extra ? hp - extra : 1);
        SetMonData(mon, MON_DATA_HP, &hp);
        SetMonData(mon, MON_DATA_MAX_HP, &s->chaosBaseMaxHp);
    }
}

void ChaosAbilityAfterStats(struct Pokemon *mon)
{
    struct PartyState *s = CompleteStateForMon(mon);
    if (s != NULL)
    {
        u32 hp = GetMonData(mon, MON_DATA_HP), maximum = GetMonData(mon, MON_DATA_MAX_HP);
        s->chaosBaseMaxHp = maximum;
        u32 extra = max(1, maximum / 4);
        maximum += extra;
        if (hp != 0)
            hp += extra;
        SetMonData(mon, MON_DATA_HP, &hp);
        SetMonData(mon, MON_DATA_MAX_HP, &maximum);
    }
}

void ChaosAbilityPrepareCapture(u32 battler)
{
    struct PartyState *s = GetBattlerPartyState(battler);
    if (s->chaosBaseMaxHp != 0)
    {
        struct Pokemon *mon = GetBattlerMon(battler);
        u32 hp = GetMonData(mon, MON_DATA_HP), extra = max(1, s->chaosBaseMaxHp / 4);
        hp = hp == 0 ? 0 : (hp > extra ? hp - extra : 1);
        hp = min(hp, s->chaosBaseMaxHp);
        gBattleMons[battler].maxHP = s->chaosBaseMaxHp;
        gBattleMons[battler].hp = hp;
        SetMonData(mon, MON_DATA_MAX_HP, &s->chaosBaseMaxHp);
        SetMonData(mon, MON_DATA_HP, &hp);
        s->chaosBaseMaxHp = 0;
    }
}
