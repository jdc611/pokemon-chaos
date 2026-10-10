#include "global.h"
#include "chaos_observations.h"
#include "pokemon_storage_system.h"
#include "battle.h"
#include "battle_util.h"
#include "pokemon.h"
#include "main.h"
#include "constants/battle.h"

#define OBSERVATION_MAGIC 0x434F4231
#define OBSERVATION_VERSION 1

const struct ChaosObservationJournal *ChaosObservationsRead(void)
{
    const struct ChaosObservationJournal *journal = &gPokemonStoragePtr->observations;
    if (journal->magic != OBSERVATION_MAGIC || journal->version != OBSERVATION_VERSION
     || journal->seed != gSaveBlock3Ptr->worldSeed || journal->count > CHAOS_OBSERVATION_CAPACITY)
        return NULL;
    for (u32 i = 0; i < journal->count; i++)
    {
        const struct ChaosObservation *record = &journal->facts[i];
        u32 kind = record->fact & 0xF000, value = record->fact & 0xFFF;
        if (record->species == SPECIES_NONE || record->species >= NUM_SPECIES || record->count == 0
         || (kind == CHAOS_OBS_SEEN && value != 0)
         || ((kind == CHAOS_OBS_MOVE || kind == CHAOS_OBS_DAMAGE) && (value == MOVE_NONE || value >= MOVES_COUNT))
         || (kind == CHAOS_OBS_ABILITY && (value == ABILITY_NONE || value >= ABILITIES_COUNT))
         || (kind == CHAOS_OBS_STAT && ((value >> 4) == STAT_HP || (value >> 4) >= NUM_BATTLE_STATS || (value & 15) > MAX_STAT_STAGE))
         || (kind == CHAOS_OBS_BATTLE && (value == 0 || value > B_OUTCOME_FORFEITED))
         || (kind != CHAOS_OBS_SEEN && kind != CHAOS_OBS_MOVE && kind != CHAOS_OBS_ABILITY
          && kind != CHAOS_OBS_DAMAGE && kind != CHAOS_OBS_STAT && kind != CHAOS_OBS_BATTLE))
            return NULL;
    }
    return journal;
}

static bool32 Eligible(u32 battler)
{
    return gMain.inBattle && battler < gBattlersCount
        && GetBattlerSide(battler) == B_SIDE_OPPONENT
        && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED));
}

static u32 VisibleSpecies(u32 battler)
{
    // Read an already established disguise; do not initialize/alter Illusion.
    if (gBattleStruct->illusion[battler].state == ILLUSION_ON
     && gBattleStruct->illusion[battler].mon != NULL)
        return GetMonData(gBattleStruct->illusion[battler].mon, MON_DATA_SPECIES);
    return gBattleMons[battler].species;
}

static void Observe(u32 species, u32 fact, u32 amount, bool32 maximum)
{
    if (species == SPECIES_NONE || species >= NUM_SPECIES) return;
    struct ChaosObservationJournal *journal = &gPokemonStoragePtr->observations;
    if (!ChaosObservationsRead())
    {
        memset(journal, 0, sizeof(*journal));
        journal->magic = OBSERVATION_MAGIC;
        journal->seed = gSaveBlock3Ptr->worldSeed;
        journal->version = OBSERVATION_VERSION;
    }
    for (u32 i = 0; i < journal->count; i++)
    {
        struct ChaosObservation *record = &journal->facts[i];
        if (record->species == species && record->fact == fact)
        {
            if (amount)
                record->count = maximum ? max(record->count, min(amount, 0xFFFF)) : min((u32)record->count + amount, 0xFFFF);
            return;
        }
    }
    if (journal->count == CHAOS_OBSERVATION_CAPACITY)
    {
        journal->full = TRUE;
        return;
    }
    journal->facts[journal->count++] = (struct ChaosObservation){species, fact, max(1, min(amount, 0xFFFF))};
}

void ChaosObserveOpponents(void)
{
    for (u32 battler = 0; battler < gBattlersCount; battler++)
        if (Eligible(battler) && gBattleMons[battler].hp != 0)
            Observe(VisibleSpecies(battler), CHAOS_OBS_SEEN, 0, FALSE);
}

void ChaosObserveMove(u32 battler, u32 move)
{
    if (!Eligible(battler) || move == MOVE_NONE || move >= MOVES_COUNT) return;
    u32 species = VisibleSpecies(battler);
    Observe(species, CHAOS_OBS_SEEN, 0, FALSE);
    Observe(species, CHAOS_OBS_MOVE | move, 1, FALSE);
}

void ChaosObserveAbility(u32 battler, u32 ability)
{
    if (!Eligible(battler) || ability == ABILITY_NONE || ability >= ABILITIES_COUNT) return;
    u32 species = VisibleSpecies(battler);
    Observe(species, CHAOS_OBS_SEEN, 0, FALSE);
    Observe(species, CHAOS_OBS_ABILITY | ability, 0, FALSE);
}

void ChaosObserveDamage(u32 attacker, u32 defender, u32 move, u32 damage)
{
    // Exact damage is observable against the player's numeric HP display.
    // Never turn the opponent's hidden maximum HP into tracker information.
    if (!Eligible(attacker) || defender >= gBattlersCount || GetBattlerSide(defender) != B_SIDE_PLAYER
     || damage == 0 || move == MOVE_NONE || move >= MOVES_COUNT) return;
    u32 species = VisibleSpecies(attacker);
    Observe(species, CHAOS_OBS_SEEN, 0, FALSE);
    Observe(species, CHAOS_OBS_DAMAGE | move, damage, TRUE);
}

void ChaosObserveStat(u32 battler, u32 stat, u32 stage)
{
    if (!Eligible(battler) || stat == STAT_HP || stat >= NUM_BATTLE_STATS || stage > MAX_STAT_STAGE) return;
    u32 species = VisibleSpecies(battler);
    Observe(species, CHAOS_OBS_SEEN, 0, FALSE);
    Observe(species, CHAOS_OBS_STAT | (stat << 4) | stage, 1, FALSE);
}

void ChaosObserveBattleEnd(void)
{
    u32 outcome = gBattleOutcome & 0x7F;
    if (outcome == 0 || outcome > B_OUTCOME_FORFEITED) return;
    for (u32 battler = 0; battler < gBattlersCount; battler++)
    {
        if (!Eligible(battler)) continue;
        u32 species = VisibleSpecies(battler);
        bool32 duplicate = FALSE;
        for (u32 earlier = 0; earlier < battler; earlier++)
            if (Eligible(earlier) && VisibleSpecies(earlier) == species) duplicate = TRUE;
        if (!duplicate)
        {
            Observe(species, CHAOS_OBS_SEEN, 0, FALSE);
            Observe(species, CHAOS_OBS_BATTLE | outcome, 1, FALSE);
        }
    }
}
