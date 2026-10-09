#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_stat_change.h"
#include "battle_util.h"
#include "battle_interface.h"
#include "battle_script_commands.h"
#include "battle_scripts.h"
#include "chaos_abilities.h"
#include "string_util.h"
#include "main.h"
#include "item.h"
#include "pokedex.h"
#include "random.h"
#include "pokemon_storage_system.h"
#include "constants/items.h"
#include "constants/hold_effects.h"
#include "constants/abilities.h"
#include "constants/species.h"
#include "constants/moves.h"

extern const u8 BattleScript_ChaosAbilityState[];
static bool32 CheckThreshold(u32 battler, enum Ability ability);
extern bool32 DoesSpeciesUseHoldItemToChangeForm(enum Species species, enum Item heldItemId);

static enum Ability ChaosMonAbility(struct Pokemon *mon)
{
    enum Ability saved = gLastUsedAbility;
    enum Ability ability = GetMonAbility(mon);
    gLastUsedAbility = saved;
    return ability;
}

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
    case ABILITY_GULP_MISSILE: native = SPECIES_CRAMORANT; break;
    case ABILITY_ZEN_MODE: native = SPECIES_DARMANITAN; break;
    case ABILITY_TERA_SHIFT: native = SPECIES_TERAPAGOS_NORMAL; break;
    case ABILITY_HUNGER_SWITCH: native = SPECIES_MORPEKO_FULL_BELLY; break;
    default: return FALSE;
    }
    return gSpeciesInfo[gBattleMons[battler].species].natDexNum != gSpeciesInfo[native].natDexNum;
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
    if (!IsDoubleBattle() && ability==ABILITY_HOSPITALITY && !state->chaosHospitality)
    {
        struct Pokemon *party=GetTrainerParty(GetBattlerTrainer(battler));
        u32 best=PARTY_SIZE,bestHP=1,bestMax=1;
        for (u32 slot=0;slot<PARTY_SIZE;slot++)
        {
            if(slot==gBattlerPartyIndexes[battler] || GetMonData(&party[slot],MON_DATA_IS_EGG)) continue;
            u32 hp=GetMonData(&party[slot],MON_DATA_HP),maximum=GetMonData(&party[slot],MON_DATA_MAX_HP);
            if(hp && maximum && hp<maximum && (best==PARTY_SIZE || hp*bestMax<bestHP*maximum)) {best=slot;bestHP=hp;bestMax=maximum;}
        }
        if(best<PARTY_SIZE)
        {
            state->chaosHospitality=TRUE;
            u32 hp=min(bestMax,bestHP+max(1,bestMax/4));
            SetMonData(&party[best],MON_DATA_HP,&hp);
            return StateMessage(battler,COMPOUND_STRING("Welcoming"));
        }
    }
    if (ChaosHasBenchAbility(battler,ABILITY_PASTEL_VEIL)
     && (gBattleMons[battler].status1 & (STATUS1_POISON|STATUS1_TOXIC_POISON)))
    {
        gBattleScripting.battler=gBattlerAbility=battler;gLastUsedAbility=ABILITY_PASTEL_VEIL;
        gBattleScripting.abilityPopupOverwrite=ABILITY_PASTEL_VEIL;
        BattleScriptCall(BattleScript_HealerActivates);
        return TRUE;
    }
    if (!ChaosAbilityIsFallback(battler, ability) || state->chaosEntryApplied)
        return FALSE;
    state->chaosEntryApplied = TRUE;
    state->chaosBlade = state->chaosHangry = FALSE;
    state->chaosGulpLoaded=FALSE;
    state->chaosIceWeather=!!(GetWeather() & (B_WEATHER_HAIL|B_WEATHER_SNOW));
    if (ability == ABILITY_ZERO_TO_HERO && state->chaosHeroic)
    {
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
    SetStatChange(battler, STAT_ATK, 1);
    SetStatChange(battler, STAT_SPATK, 1);
    SetStatChange(battler, STAT_SPEED, 1);
    return StateMessage(battler, COMPOUND_STRING("Bonded"));
}

static bool32 CheckThreshold(u32 battler, enum Ability ability)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    if (!ChaosAbilityIsFallback(battler, ability) || !IsBattlerAlive(battler))
        return FALSE;
    if (ability == ABILITY_POWER_CONSTRUCT && !state->chaosComplete && gBattleMons[battler].hp * 2 <= gBattleMons[battler].maxHP)
    {
        state->chaosComplete = TRUE;
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
    if (battler == gBattlerAttacker && !gBattleStruct->unableToUseMove
     && GetBattlerAbility(battler) == ABILITY_STANCE_CHANGE
     && ChaosAbilityIsFallback(battler, ABILITY_STANCE_CHANGE))
    {
        bool32 blade = !IsBattleMoveStatus(gCurrentMove);
        if (blade != state->chaosBlade)
        {
            state->chaosBlade = blade;
            return StateMessage(battler, blade ? COMPOUND_STRING("Blade-ready") : COMPOUND_STRING("Shielded"));
        }
    }
    return CheckThreshold(battler, GetBattlerAbility(battler));
}

bool32 ChaosAbilityBeforeMove(u32 battler, enum Move move)
{
    // Native Aegislash retains its engine path. Chaos stance changes after use.
    return FALSE;
}

u32 ChaosAbilityStat(u32 battler, enum Ability ability, enum Stat stat, u32 value)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    if (!ChaosAbilityIsFallback(battler, ability))
        return value;
    bool32 offense = stat == STAT_ATK || stat == STAT_SPATK;
    bool32 defense = stat == STAT_DEF || stat == STAT_SPDEF;
    u32 percent = 100;
    switch (ability)
    {
    case ABILITY_ZEN_MODE:
        if (gBattleMons[battler].hp * 2 <= gBattleMons[battler].maxHP
         && (stat == STAT_SPATK || stat == STAT_SPDEF)) percent = 150;
        break;
    case ABILITY_SCHOOLING:
        if (gBattleMons[battler].level >= 20 && gBattleMons[battler].hp * 4 > gBattleMons[battler].maxHP
         && (offense || defense)) percent = 130;
        break;
    case ABILITY_SHIELDS_DOWN:
        if (gBattleMons[battler].hp * 2 > gBattleMons[battler].maxHP)
        { if (defense) percent = 120; }
        else if (offense || stat == STAT_SPEED) percent = 130;
        break;
    case ABILITY_STANCE_CHANGE:
        if (state->chaosBlade) percent = offense ? 130 : defense ? 80 : 100;
        break;
    case ABILITY_POWER_CONSTRUCT:
        if (state->chaosComplete) percent = defense ? 150 : offense ? 120 : 100;
        break;
    case ABILITY_ZERO_TO_HERO:
        if (state->chaosHeroic) percent = offense ? 130 : defense ? 120 : 100;
        break;
    case ABILITY_TERA_SHIFT:
        if (state->chaosTera) percent = defense ? 130 : offense ? 110 : 100;
        break;
    case ABILITY_HUNGER_SWITCH:
        if (state->chaosHangry) { if (offense || stat == STAT_SPEED) percent = 120; }
        else if (defense) percent = 120;
        break;
    default: break;
    }
    return max(1, value * percent / 100);
}

s32 ChaosAbilityDamage(u32 attacker, u32 defender, enum Move move, s32 damage)
{
    if (damage <= 0 || IsBattleMoveStatus(move)) return damage;
    if (!IsDoubleBattle())
    {
        if (ChaosHasBenchAbility(attacker, ABILITY_BATTERY) && IsBattleMoveSpecial(move)) damage = max(1, damage * 13 / 10);
        if (ChaosHasBenchAbility(attacker, ABILITY_POWER_SPOT)) damage = max(1, damage * 13 / 10);
        if (ChaosHasBenchAbility(defender, ABILITY_FRIEND_GUARD)) damage = max(1, damage * 3 / 4);
        enum MoveTarget target = GetBattlerMoveTargetType(attacker, move);
        if (GetBattlerAbility(defender) == ABILITY_TELEPATHY && (target == TARGET_BOTH || target == TARGET_FOES_AND_ALLY))
            damage = max(1, damage * 4 / 5);
    }
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
    return amount;
}

const u8 *ChaosAbilityNamePrefix(u32 battler)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    if (state->chaosComplete) return COMPOUND_STRING("Perfect ");
    if (state->chaosBond) return COMPOUND_STRING("Ash-");
    if (state->chaosHeroic) return COMPOUND_STRING("Hero ");
    if (state->chaosTera) return COMPOUND_STRING("Tera ");
    return NULL;
}

const u8 *ChaosAbilityMarker(u32 battler)
{
    enum Ability ability = GetBattlerAbility(battler);
    struct PartyState *s = GetBattlerPartyState(battler);
    if (!ChaosAbilityIsFallback(battler, ability)) return NULL;
    switch (ability)
    {
    case ABILITY_ZEN_MODE: return gBattleMons[battler].hp * 2 <= gBattleMons[battler].maxHP ? COMPOUND_STRING("ZEN") : NULL;
    case ABILITY_TERA_SHIFT: return s->chaosTera ? COMPOUND_STRING("TERA") : NULL;
    case ABILITY_ZERO_TO_HERO: return s->chaosHeroic ? COMPOUND_STRING("HERO") : NULL;
    case ABILITY_BATTLE_BOND: return s->chaosBond ? COMPOUND_STRING("BOND") : NULL;
    case ABILITY_SCHOOLING: return gBattleMons[battler].level >= 20 && gBattleMons[battler].hp * 4 > gBattleMons[battler].maxHP ? COMPOUND_STRING("SCHOOL") : NULL;
    case ABILITY_SHIELDS_DOWN: return gBattleMons[battler].hp * 2 <= gBattleMons[battler].maxHP ? COMPOUND_STRING("OPEN") : COMPOUND_STRING("SHIELD");
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

bool32 ChaosAbilityTeraShift(u32 battler)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    if (GetBattlerAbility(battler) != ABILITY_TERA_SHIFT
     || !ChaosAbilityIsFallback(battler, ABILITY_TERA_SHIFT) || state->chaosTera)
        return FALSE;
    state->chaosTera = TRUE;
    return StateMessage(battler, COMPOUND_STRING("Tera"));
}

// The bench support is a presence check, so duplicate holders never stack.
bool32 ChaosHasBenchAbility(u32 battler, enum Ability ability)
{
    if (IsDoubleBattle() || !IsBattlerAlive(battler)) return FALSE;
    u32 trainer = GetBattlerTrainer(battler);
    struct Pokemon *party = GetTrainerParty(trainer);
    bool32 gas = IsAbilityOnField(ABILITY_NEUTRALIZING_GAS) != 0;
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        struct Pokemon *mon = &party[slot];
        if (slot == gBattlerPartyIndexes[battler] || !GetMonData(mon, MON_DATA_SPECIES)
         || GetMonData(mon, MON_DATA_IS_EGG) || !GetMonData(mon, MON_DATA_HP)) continue;
        if (gas && GetItemHoldEffect(GetMonData(mon, MON_DATA_HELD_ITEM)) != HOLD_EFFECT_ABILITY_SHIELD) continue;
        if (ChaosMonAbility(mon) == ability) return TRUE;
    }
    return FALSE;
}

void ChaosAbilityTypes(u32 battler, enum Type types[3])
{
    enum Ability ability = GetBattlerAbility(battler);
    enum Type type = TYPE_MYSTERY;
    static const enum Type itemTypes[] = {TYPE_FIRE,TYPE_WATER,TYPE_ELECTRIC,TYPE_GRASS,TYPE_ICE,TYPE_FIGHTING,TYPE_POISON,TYPE_GROUND,TYPE_FLYING,TYPE_PSYCHIC,TYPE_BUG,TYPE_ROCK,TYPE_GHOST,TYPE_DRAGON,TYPE_DARK,TYPE_STEEL,TYPE_FAIRY};
    enum Item item = gBattleMons[battler].item;
    if (ability == ABILITY_FORECAST
     && gSpeciesInfo[gBattleMons[battler].species].natDexNum != gSpeciesInfo[SPECIES_CASTFORM].natDexNum)
    {
        u32 weather = GetWeather();
        if (IsBattlerWeatherAffected(GetBattlerHoldEffect(battler), weather, B_WEATHER_SUN)) type = TYPE_FIRE;
        else if (IsBattlerWeatherAffected(GetBattlerHoldEffect(battler), weather, B_WEATHER_RAIN)) type = TYPE_WATER;
        else if (IsBattlerWeatherAffected(GetBattlerHoldEffect(battler), weather, B_WEATHER_HAIL | B_WEATHER_SNOW)) type = TYPE_ICE;
    }
    if (ability == ABILITY_MULTITYPE && item >= ITEM_FLAME_PLATE && item <= ITEM_PIXIE_PLATE)
        type = itemTypes[item - ITEM_FLAME_PLATE];
    if (ability == ABILITY_RKS_SYSTEM && item >= ITEM_FIRE_MEMORY && item <= ITEM_FAIRY_MEMORY)
        type = itemTypes[item - ITEM_FIRE_MEMORY];
    if (type != TYPE_MYSTERY) { types[0] = types[1] = type; types[2] = TYPE_MYSTERY; }
}

bool32 ChaosAbilityLoadGulp(u32 battler, enum Move move)
{
    struct PartyState *state=GetBattlerPartyState(battler);
    if (GetBattlerAbility(battler)!=ABILITY_GULP_MISSILE || !ChaosAbilityIsFallback(battler,ABILITY_GULP_MISSILE)
     || !IsBattlerAlive(battler) || state->chaosGulpLoaded || (move!=MOVE_SURF && move!=MOVE_DIVE)) return FALSE;
    state->chaosGulpLoaded=TRUE;
    state->chaosGulpGorging=gBattleMons[battler].hp*2<=gBattleMons[battler].maxHP;
    return StateMessage(battler,COMPOUND_STRING("Loaded"));
}

bool32 ChaosAbilityWeather(u32 battler)
{
    struct PartyState *state=GetBattlerPartyState(battler);
    bool32 icy=!!(GetWeather() & (B_WEATHER_HAIL|B_WEATHER_SNOW));
    bool32 begins=icy && !state->chaosIceWeather;
    state->chaosIceWeather=icy;
    if (begins && GetBattlerAbility(battler)==ABILITY_ICE_FACE && ChaosAbilityIsFallback(battler,ABILITY_ICE_FACE)
     && state->chaosIceBroken && !state->chaosIceRestored)
    {
        state->chaosIceBroken=FALSE;state->chaosIceRestored=TRUE;
        return StateMessage(battler,COMPOUND_STRING("Ice-protected"));
    }
    return FALSE;
}

bool32 ChaosBenchSymbiosis(u32 battler)
{
    if (!ChaosHasBenchAbility(battler,ABILITY_SYMBIOSIS) || gBattleMons[battler].item!=ITEM_NONE) return FALSE;
    struct Pokemon *party=GetTrainerParty(GetBattlerTrainer(battler));
    bool32 gas=IsAbilityOnField(ABILITY_NEUTRALIZING_GAS)!=0;
    for(u32 slot=0;slot<PARTY_SIZE;slot++)
    {
        struct Pokemon *mon=&party[slot];
        enum Item item=GetMonData(mon,MON_DATA_HELD_ITEM);
        if(slot==gBattlerPartyIndexes[battler] || !GetMonData(mon,MON_DATA_HP) || GetMonData(mon,MON_DATA_IS_EGG)
         || ChaosMonAbility(mon)!=ABILITY_SYMBIOSIS || item==ITEM_NONE)continue;
        if(gas && GetItemHoldEffect(item)!=HOLD_EFFECT_ABILITY_SHIELD)continue;
        enum Species species=GetMonData(mon,MON_DATA_SPECIES);
        if(DoesSpeciesUseHoldItemToChangeForm(species,item) || !CanBattlerGetOrLoseItem(battler,battler,item)
         || (GetItemHoldEffect(item)==HOLD_EFFECT_BOOSTER_ENERGY && gSpeciesInfo[species].isParadox))continue;
        u32 none=ITEM_NONE;SetMonData(mon,MON_DATA_HELD_ITEM,&none);
        gBattleStruct->itemLost[GetBattlerTrainer(battler)][slot].originalItem = ITEM_NONE;
        gLastUsedItem=gBattleMons[battler].item=item;
        SetMonData(GetBattlerMon(battler),MON_DATA_HELD_ITEM,&item);
        gBattleMons[battler].volatiles.unburdenActive=FALSE;
        GetBattlerPartyState(battler)->chaosBerryConsumed=FALSE;
        BtlController_EmitSetMonData(battler,B_COMM_TO_CONTROLLER,REQUEST_HELDITEM_BATTLE,0,sizeof(item),&gBattleMons[battler].item);
        MarkBattlerForControllerExec(battler);
        gEffectBattler=gBattleScripting.battler=gBattlerAbility=battler;
        gLastUsedAbility=gBattleScripting.abilityPopupOverwrite=ABILITY_SYMBIOSIS;
        return TRUE;
    }
    return FALSE;
}

bool32 ChaosBattleTypesKnown(u32 battler)
{
    if (IsOnPlayerSide(battler)) return TRUE;
    struct Pokemon *illusion = GetIllusionMonPtr(battler);
    enum Species species = GetBattlerVisualSpecies(battler);
    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_SEEN)) return TRUE;
    // Revealing the actual holder cannot leak a still-active disguise.
    return illusion == NULL && GetBattlerPartyState(battler)->chaosTypesRevealed;
}

void ChaosRevealBattleTypes(u32 attacker, u32 defender, enum Move move, u32 effectiveness)
{
    if (IS_FRLG && IsOnPlayerSide(attacker) && !IsOnPlayerSide(defender)
     && !IsBattleMoveStatus(move) && effectiveness != UQ_4_12(1.0))
        GetBattlerPartyState(defender)->chaosTypesRevealed = TRUE;
}
