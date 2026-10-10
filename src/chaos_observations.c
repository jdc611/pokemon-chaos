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
         || (kind == CHAOS_OBS_MOVE && (value == MOVE_NONE || value >= MOVES_COUNT))
         || (kind == CHAOS_OBS_ABILITY && (value == ABILITY_NONE || value >= ABILITIES_COUNT))
         || (kind != CHAOS_OBS_SEEN && kind != CHAOS_OBS_MOVE && kind != CHAOS_OBS_ABILITY))
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

static void Observe(u32 species, u32 fact, bool32 increment)
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
            if (increment && record->count < 0xFFFF) record->count++;
            return;
        }
    }
    if (journal->count == CHAOS_OBSERVATION_CAPACITY)
    {
        journal->full = TRUE;
        return;
    }
    journal->facts[journal->count++] = (struct ChaosObservation){species, fact, 1};
}

void ChaosObserveOpponents(void)
{
    for (u32 battler = 0; battler < gBattlersCount; battler++)
        if (Eligible(battler) && gBattleMons[battler].hp != 0)
            Observe(VisibleSpecies(battler), CHAOS_OBS_SEEN, FALSE);
}

void ChaosObserveMove(u32 battler, u32 move)
{
    if (!Eligible(battler) || move == MOVE_NONE || move >= MOVES_COUNT) return;
    u32 species = VisibleSpecies(battler);
    Observe(species, CHAOS_OBS_SEEN, FALSE);
    Observe(species, CHAOS_OBS_MOVE | move, TRUE);
}

void ChaosObserveAbility(u32 battler, u32 ability)
{
    if (!Eligible(battler) || ability == ABILITY_NONE || ability >= ABILITIES_COUNT) return;
    u32 species = VisibleSpecies(battler);
    Observe(species, CHAOS_OBS_SEEN, FALSE);
    Observe(species, CHAOS_OBS_ABILITY | ability, FALSE);
}
