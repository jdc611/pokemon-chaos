#include "global.h"
#include "ironmon.h"
#include "run_settings.h"
#include "event_data.h"
#include "chaos_progression.h"
#include "load_save.h"
#include "party_menu.h"
#include "script_pokemon_util.h"
#include "constants/battle_frontier.h"
#include "constants/pokemon.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "constants/items.h"
#include "constants/form_change_types.h"

void ChaosMarkGymRematch(void)
{
    u16 bit = gSpecialVar_0x8004;
    if (bit == 1 || bit == 2 || bit == 4)
        VarSet(VAR_CHAOS_REMATCHES, VarGet(VAR_CHAOS_REMATCHES) | bit);
}

bool8 ChaosAllGymRematchesCleared(void)
{
    return (VarGet(VAR_CHAOS_REMATCHES) & 7) == 7;
}

static u16 GetOwnedMegaStone(u16 species)
{
    const struct FormChange *forms = GetSpeciesFormChanges(species);
    for (u32 i = 0; forms != NULL && forms[i].method != FORM_CHANGE_TERMINATOR; i++)
        if (forms[i].method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM
         && forms[i].param1 != ITEM_NIDOKINGITE)
            return forms[i].param1;
    return ITEM_NONE;
}

void ChaosChooseOakMegaStone(void)
{
    u16 stone;
    // Prefer a current party member, then a Pokemon in the player's Boxes.
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_IS_EGG))
            continue;
        stone = GetOwnedMegaStone(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES));
        if (stone != ITEM_NONE)
        {
            gSpecialVar_0x8004 = stone;
            gSpecialVar_Result = TRUE;
            return;
        }
    }
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
        {
            if (GetBoxMonDataAt(box, slot, MON_DATA_IS_EGG))
                continue;
            stone = GetOwnedMegaStone(GetBoxMonDataAt(box, slot, MON_DATA_SPECIES));
            if (stone != ITEM_NONE)
            {
                gSpecialVar_0x8004 = stone;
                gSpecialVar_Result = TRUE;
                return;
            }
        }
    // A random team need not contain any Mega-capable species. Route 4's
    // fixed Magikarp seller gives this fallback an obtainable evolution line.
    gSpecialVar_0x8004 = ITEM_GYARADOSITE;
    gSpecialVar_Result = FALSE;
}

static EWRAM_DATA bool8 sSilphPartnerBattleActive = FALSE;
static EWRAM_DATA u16 sSilphPreviousFacility = 0;

void ChaosPrepareSilphSelection(void)
{
    sSilphPreviousFacility = VarGet(VAR_FRONTIER_FACILITY);
    VarSet(VAR_FRONTIER_FACILITY, FACILITY_MULTI_OR_EREADER);
}

void ChaosCancelSilphSelection(void)
{
    VarSet(VAR_FRONTIER_FACILITY, sSilphPreviousFacility);
}

void ChaosBeginSilphPartnerBattle(void)
{
    if (IsIronmonRun()) return;
    SavePlayerParty();
    for (u32 i = 0; i < MULTI_PARTY_SIZE; i++)
        gSaveBlock2Ptr->frontier.selectedPartyMons[i] = gSelectedOrderFromParty[i];
    ReducePlayerPartyToSelectedMons();
    sSilphPartnerBattleActive = TRUE;
}

void ChaosRestoreSilphPartnerParty(void)
{
    if (!sSilphPartnerBattleActive)
        return;
    for (u32 i = 0; i < MULTI_PARTY_SIZE; i++)
    {
        u16 slot = gSaveBlock2Ptr->frontier.selectedPartyMons[i] - 1;
        if (slot < PARTY_SIZE)
            SavePlayerPartyMon(slot, &gParties[B_TRAINER_PLAYER][i]);
    }
    LoadPlayerParty();
    CalculatePlayerPartyCount();
    sSilphPartnerBattleActive = FALSE;
    VarSet(VAR_FRONTIER_FACILITY, sSilphPreviousFacility);
}

u16 ChaosGetRivalCounter(u8 level)
{
    u16 species = VarGet(VAR_CHAOS_RIVAL_STARTER);
    struct Pokemon mon;
    bool32 canStop;
    if (species == SPECIES_NONE || species >= NUM_SPECIES)
    {
        static const u16 defaults[] = {SPECIES_CHARMANDER, SPECIES_BULBASAUR, SPECIES_SQUIRTLE};
        species = defaults[VarGet(VAR_STARTER_MON) % ARRAY_COUNT(defaults)];
    }
    CreateMon(&mon, species, level, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 stage = 0; stage < 3; stage++)
    {
        u16 next = GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO);
        if (next == SPECIES_NONE || next == species)
            break;
        species = next;
        SetMonData(&mon, MON_DATA_SPECIES, &species);
    }
    return species;
}

// Retain the old NUZLOCKE enum only as a migration source. New runs select
// difficulty and challenge rules separately.
bool32 IsNuzlockeRun(void)
{
    return VarGet(VAR_CHAOS_NUZLOCKE) || gSaveBlock3Ptr->runDifficulty == RUN_DIFFICULTY_NUZLOCKE;
}
