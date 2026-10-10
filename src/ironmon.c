#include "global.h"
#include "ironmon.h"
#include "battle.h"
#include "main.h"
#include "caps.h"
#include "event_data.h"
#include "item.h"
#include "mail.h"
#include "data.h"
#include "pokedex.h"
#include "move.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "random_mon_generation.h"
#include "run_settings.h"
#include "string_util.h"
#include "trainer_util.h"
#include "overworld.h"
#include "constants/random_mon_generation.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/battle.h"
#include "constants/opponents_frlg.h"

static EWRAM_DATA u16 sTrainerIdentity;
static EWRAM_DATA u16 sCenterPermit = 0;

bool32 IsIronmonDifficulty(u32 difficulty)
{
    return difficulty == RUN_DIFFICULTY_IRONMON_NORMAL || difficulty == RUN_DIFFICULTY_IRONMON_HARDCORE;
}

bool32 IsIronmonRun(void)
{
    return IS_FRLG && gSaveBlock3Ptr != NULL
        && (gSaveBlock3Ptr->ironmon.magic == IRONMON_STATE_MAGIC
            || gSaveBlock3Ptr->ironmon.magic == IRONMON_LEGACY_STATE_MAGIC)
        && gSaveBlock3Ptr->ironmon.seed == gSaveBlock3Ptr->worldSeed
        && IsIronmonDifficulty(gSaveBlock3Ptr->ironmon.mode);
}

bool32 IsIronmonHardcore(void)
{
    return IsIronmonRun() && gSaveBlock3Ptr->ironmon.mode == RUN_DIFFICULTY_IRONMON_HARDCORE;
}

void IronmonEnforcePreset(void)
{
    if (!IsIronmonRun()) return;
    gSaveBlock3Ptr->runDifficulty = gSaveBlock3Ptr->ironmon.mode;
    gSaveBlock3Ptr->ironmon.aiProfile = 0;
    gSaveBlock3Ptr->randomizerEnabled = RUN_WILD_RANDOM;
    // Preserve the stat budget of already-started v1 runs. New runs use v2.
    bool32 revised = gSaveBlock3Ptr->ironmon.magic == IRONMON_STATE_MAGIC;
    gSaveBlock3Ptr->starterMode = revised && !IsIronmonHardcore() ? RUN_STARTER_CHOOSE : RUN_STARTER_RANDOM;
    gSaveBlock3Ptr->rivalMode = RUN_RIVAL_RANDOM;
    gSaveBlock3Ptr->filterMode = RUN_FILTER_NONE;
    gSaveBlock3Ptr->filterValue = 0;
    gSaveBlock3Ptr->minimalGrindingMode = TRUE;
    gSaveBlock3Ptr->bstMode = revised && gSaveBlock3Ptr->ironmon.bstMode == RUN_BST_RANDOM
        ? RUN_BST_RANDOM : RUN_BST_SHUFFLE;
    gSaveBlock3Ptr->abilityMode = RUN_ABILITIES_RANDOM;
    gSaveBlock3Ptr->movesetMode = RUN_MOVESETS_RANDOM;
    gSaveBlock3Ptr->evolutionMode = RUN_EVOLUTIONS_RANDOM;
    gSaveBlock3Ptr->itemRandomization = TRUE;
    VarSet(VAR_CHAOS_NUZLOCKE, FALSE);
    VarSet(VAR_CHAOS_EZ_CATCH, FALSE);
    VarSet(VAR_CHAOS_CARE_PACKAGES, FALSE);
}

void IronmonInitializeRun(void)
{
    struct IronmonRunState *state = &gSaveBlock3Ptr->ironmon;
    memset(state, 0, sizeof(*state));
    sCenterPermit = 0;
    if (!IsIronmonDifficulty(gSaveBlock3Ptr->runDifficulty)) return;
    state->magic = IRONMON_STATE_MAGIC;
    state->mode = gSaveBlock3Ptr->runDifficulty;
    state->seed = gSaveBlock3Ptr->worldSeed;
    state->bstMode = gSaveBlock3Ptr->bstMode == RUN_BST_SHUFFLE || gSaveBlock3Ptr->bstMode == RUN_BST_RANDOM
        ? gSaveBlock3Ptr->bstMode : (IsIronmonHardcore() ? RUN_BST_RANDOM : RUN_BST_SHUFFLE);
    IronmonEnforcePreset();
}

u32 IronmonMix(u32 value)
{
    value = (value ^ (value >> 16)) * 0x7FEB352Du;
    value = (value ^ (value >> 15)) * 0x846CA68Bu;
    return value ^ (value >> 16);
}

// Initial narrow policy: Destiny Bond is excluded. Other candidate moves are
// deliberately NOT blanket-banned; the remaining interaction audit is pending.
bool32 IronmonMoveAllowed(enum Move move)
{
    return move != MOVE_NONE && move < MOVES_COUNT && move != MOVE_STRUGGLE
        && move != MOVE_DESTINY_BOND && GetMovePP(move) != 0;
}

bool32 IronmonMoveIsStarterAttack(enum Move move)
{
    if (!IronmonMoveAllowed(move) || GetMoveCategory(move) == DAMAGE_CATEGORY_STATUS || GetMovePower(move) == 0)
        return FALSE;
    switch (move)
    {
    case MOVE_EXPLOSION: case MOVE_SELF_DESTRUCT: case MOVE_MISTY_EXPLOSION:
    case MOVE_FINAL_GAMBIT: case MOVE_FAKE_OUT: case MOVE_LAST_RESORT:
    case MOVE_DREAM_EATER: case MOVE_SNORE: case MOVE_SUCKER_PUNCH:
    case MOVE_BELCH: case MOVE_FOCUS_PUNCH: case MOVE_SYNCHRONOISE:
    case MOVE_COUNTER: case MOVE_MIRROR_COAT: case MOVE_METAL_BURST:
    case MOVE_COMEUPPANCE: case MOVE_ENDEAVOR: case MOVE_FLAIL: case MOVE_REVERSAL:
        return FALSE;
    default:
        return TRUE;
    }
}

enum Move IronmonStarterAttack(enum Species species)
{
    u32 count = 0;
    for (u32 move = 1; move < MOVES_COUNT; move++)
        if (IronmonMoveIsStarterAttack(move)) count++;
    if (!count) return MOVE_TACKLE;
    u32 rank = IronmonMix(gSaveBlock3Ptr->worldSeed ^ species ^ 0x4154544Bu) % count;
    for (u32 move = 1; move < MOVES_COUNT; move++)
        if (IronmonMoveIsStarterAttack(move) && rank-- == 0) return move;
    return MOVE_TACKLE;
}

static bool32 IronmonHeldItemEligible(enum Item item)
{
    return GetItemHoldEffect(item) != HOLD_EFFECT_NONE && GetItemPocket(item) != POCKET_KEY_ITEMS
        && GetItemPocket(item) != POCKET_TM_HM && !ItemIsMail(item);
}

enum Item IronmonHeldItem(u32 seed)
{
    u32 count = 0;
    for (u32 item = 1; item < ITEMS_COUNT; item++)
        if (IronmonHeldItemEligible(item)) count++;
    if (!count) return ITEM_ORAN_BERRY;
    u32 rank = IronmonMix(seed ^ 0x48454C44u) % count;
    for (u32 item = 1; item < ITEMS_COUNT; item++)
        if (IronmonHeldItemEligible(item) && rank-- == 0) return item;
    return ITEM_ORAN_BERRY;
}

u32 IronmonTrainerSeed(const struct Trainer *trainer)
{
    // Find a stable table identity without including the ROM address itself.
    u32 identity = sTrainerIdentity;
    for (u32 difficulty = 0; !sTrainerIdentity && difficulty < DIFFICULTY_COUNT; difficulty++)
        for (u32 id = 0; id < TRAINERS_COUNT; id++)
            if (trainer == &gTrainers[difficulty][id]) identity = id + 1;
    // Overrides are temporary copies: their full template remains the fallback.
    u32 seed = gSaveBlock3Ptr->worldSeed ^ IronmonMix(identity) ^ Crc32B(trainer->trainerName, TRAINER_NAME_LENGTH + 1)
        ^ ((u32)trainer->trainerClass << 24) ^ ((u32)trainer->partySize << 16);
    for (u32 i = 0; i < trainer->partySize; i++)
        seed = IronmonMix(seed ^ trainer->party[i].species ^ ((u32)trainer->party[i].lvl << 16) ^ i);
    return seed;
}

static enum Species IronmonSpecies(u32 seed)
{
    struct FilterFuncArgs args = {FILTER_FUNC_ARG_NONE, FILTER_FUNC_ARG_NONE};
    // Reuse the existing ordinary, non-custom species eligibility pool.
    u32 count = 0;
    for (u32 species = 1; species < SPECIES_CUSTOM_START; species++)
        if (IsExactSpeciesEligibleRandomSpecies(SPECIES_GENERATOR_NO_SUPERMONS, species, &args)) count++;
    if (!count) return SPECIES_BULBASAUR;
    u32 rank = IronmonMix(seed) % count;
    for (u32 species = 1; species < SPECIES_CUSTOM_START; species++)
        if (IsExactSpeciesEligibleRandomSpecies(SPECIES_GENERATOR_NO_SUPERMONS, species, &args) && rank-- == 0)
            return species;
    return SPECIES_BULBASAUR;
}

static void IronmonAssignAbility(struct Pokemon *mon, enum Species species, u32 seed)
{
    u32 count = 0;
    for (u32 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
        if (GetSpeciesAbility(species, slot) != ABILITY_NONE) count++;
    if (count)
    {
        u32 rank = IronmonMix(seed ^ 0x4142494Cu) % count;
        for (u32 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
            if (GetSpeciesAbility(species, slot) != ABILITY_NONE && rank-- == 0)
            {
                SetMonData(mon, MON_DATA_ABILITY_NUM, &slot);
                break;
            }
    }
}

void IronmonPrepareWildMon(struct Pokemon *mon)
{
    if (!IsIronmonRun()) return;
    u32 seed = IronmonMix(gSaveBlock3Ptr->worldSeed ^ GetMonData(mon, MON_DATA_PERSONALITY) ^ 0x57494C44u);
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    IronmonAssignAbility(mon, species, seed);
    enum Item item = IronmonHeldItem(seed);
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    ApplyMinimalGrindingModeToMon(mon);
}

void IronmonGenerateTrainerMon(struct Pokemon *mon, const struct TrainerMon *entry, struct TrainerGenerator *trainer)
{
    u32 seed = LocalRandom32(&trainer->localRngState);
    enum Species species = IronmonSpecies(seed);
    CreateMon(mon, species, entry->lvl, IronmonMix(seed ^ 0x504944u), trainer->otID);
    GiveMonInitialMoveset(mon);
    IronmonAssignAbility(mon, species, seed);
    enum Item item = IronmonHeldItem(seed);
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    // Match ordinary trainer generation: an omitted flag forbids AI Dynamax.
    u32 dynamaxLevel = entry->shouldUseDynamax ? entry->dynamaxLevel : BLOCK_AI_DYNAMAX;
    SetMonData(mon, MON_DATA_DYNAMAX_LEVEL, &dynamaxLevel);
    ApplyMinimalGrindingModeToMon(mon);
    SetMonData(mon, MON_DATA_OT_NAME, trainer->name);
    u32 gender = trainer->gender;
    SetMonData(mon, MON_DATA_OT_GENDER, &gender);
}

void IronmonGenerateFacilityMon(struct Pokemon *mon, const struct BattleTowerPokemon *entry, u32 identity, u32 level)
{
    // Hash stable source identity, never live RNG or the player's current level.
    u32 seed = IronmonMix(gSaveBlock3Ptr->worldSeed ^ identity ^ entry->species
        ^ entry->personality ^ entry->otId);
    enum Species species = IronmonSpecies(seed);
    CreateMon(mon, species, max(1, min(MAX_LEVEL, level)), IronmonMix(seed ^ 0x504944u), OTID_STRUCT_PRESET(entry->otId));
    GiveMonInitialMoveset(mon);
    IronmonAssignAbility(mon, species, seed);
    enum Item item = IronmonHeldItem(seed);
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    ApplyMinimalGrindingModeToMon(mon);
}

void IronmonGiveStarter(enum Species species)
{
    struct IronmonRunState *state = &gSaveBlock3Ptr->ironmon;
    if (!IsIronmonRun() || state->starterGranted || state->ended) return;
    u32 seed = IronmonMix(state->seed ^ species ^ 0x53544152u);
    CreateMon(&gParties[B_TRAINER_PLAYER][0], species, 5, seed, OTID_STRUCT_PLAYER_ID);
    IronmonAssignAbility(&gParties[B_TRAINER_PLAYER][0], species, seed);
    GiveMonInitialMoveset(&gParties[B_TRAINER_PLAYER][0]);
    enum Item item = IronmonHeldItem(seed);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
    ApplyMinimalGrindingModeToMon(&gParties[B_TRAINER_PLAYER][0]);
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    HandleSetPokedexFlagFromMon(&gParties[B_TRAINER_PLAYER][0], FLAG_SET_SEEN);
    HandleSetPokedexFlagFromMon(&gParties[B_TRAINER_PLAYER][0], FLAG_SET_CAUGHT);
    state->starterGranted = TRUE;
    StringCopy(gPokemonStoragePtr->boxNames[IRONMON_RETIRED_BOX], COMPOUND_STRING("RETIRED"));
}

u32 IronmonPivotFloor(void)
{
    if (FlagGet(FLAG_BADGE08_GET)) return 64;
    if (FlagGet(FLAG_BADGE07_GET)) return 58;
    if (FlagGet(FLAG_BADGE05_GET)) return 52;
    if (FlagGet(FLAG_BADGE06_GET)) return 46;
    if (FlagGet(FLAG_BADGE04_GET)) return 39;
    if (FlagGet(FLAG_BADGE03_GET)) return 32;
    if (FlagGet(FLAG_BADGE02_GET)) return 25;
    if (FlagGet(FLAG_BADGE01_GET)) return 18;
    return 10;
}

u32 IronmonAcceptCapture(struct Pokemon *mon)
{
    struct IronmonRunState *state = &gSaveBlock3Ptr->ironmon;
    if (!IsIronmonRun() || state->ended || GetMonData(mon, MON_DATA_IS_EGG)) return MON_CANT_GIVE;
    if (state->starterGranted && gPartiesCount[B_TRAINER_PLAYER]
     && GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP) == 0)
    {
        state->ended = TRUE;
        return MON_CANT_GIVE;
    }
    struct Pokemon captured = *mon;
    mon = &captured;
    if (gPartiesCount[B_TRAINER_PLAYER] && GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES))
    {
        // The reserved record is never used as ordinary storage. On its 31st
        // entry only the oldest read-only snapshot is replaced, not restored.
        u32 slot = state->retiredCount % IN_BOX_COUNT;
        SetBoxMonAt(IRONMON_RETIRED_BOX, slot, &gParties[B_TRAINER_PLAYER][0].box);
        if (state->retiredCount < 65535) state->retiredCount++;
    }
    u32 species = GetMonData(mon, MON_DATA_SPECIES), level = GetMonData(mon, MON_DATA_LEVEL);
    u32 floor = IronmonPivotFloor();
    if (level < floor)
    {
        u32 exp = gExperienceTables[gSpeciesInfo[species].growthRate][floor];
        SetMonData(mon, MON_DATA_EXP, &exp);
    }
    ApplyMinimalGrindingModeToMon(mon);
    ZeroPartyMons(gParties[B_TRAINER_PLAYER]);
    CopyMon(&gParties[B_TRAINER_PLAYER][0], mon, sizeof(*mon));
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    // A pivot replaces the owner of slot zero. Its previous battle-form
    // restoration metadata must never be applied to the captured Pokémon.
    if (gMain.inBattle && gBattleStruct != NULL)
        memset(&gBattleStruct->partyState[B_TRAINER_PLAYER][0], 0,
               sizeof(gBattleStruct->partyState[B_TRAINER_PLAYER][0]));
    return MON_GIVEN_TO_PARTY;
}

void IronmonAuthorizeCenterHealing(void)
{
    if (!IsIronmonRun()) { gSpecialVar_Result = TRUE; return; }
    struct IronmonRunState *state = &gSaveBlock3Ptr->ironmon;
    u32 section = gMapHeader.regionMapSectionId;
    sCenterPermit = 0;
    if (IsIronmonHardcore() || state->ended || section >= 256
     || (state->centersUsed[section >> 3] & (1 << (section & 7))))
    { gSpecialVar_Result = FALSE; return; }
    sCenterPermit = section + 1;
    gSpecialVar_Result = TRUE;
}

bool32 IronmonPermitHealing(void)
{
    if (!IsIronmonRun()) return TRUE;
    struct IronmonRunState *state = &gSaveBlock3Ptr->ironmon;
    u32 section = gMapHeader.regionMapSectionId;
    if (sCenterPermit != section + 1 || state->ended || IsIronmonHardcore() || section >= 256) return FALSE;
    sCenterPermit = 0;
    if (state->centersUsed[section >> 3] & (1 << (section & 7))) return FALSE;
    bool32 needsHealing = FALSE;
    for (u32 i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        if (GetMonData(mon, MON_DATA_HP) < GetMonData(mon, MON_DATA_MAX_HP)
         || GetMonData(mon, MON_DATA_STATUS)) needsHealing = TRUE;
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        {
            enum Move move = GetMonData(mon, MON_DATA_MOVE1 + slot);
            if (move && GetMonData(mon, MON_DATA_PP1 + slot) < CalculatePPWithBonus(move, GetMonData(mon, MON_DATA_PP_BONUSES), slot))
                needsHealing = TRUE;
        }
    }
    if (needsHealing) state->centersUsed[section >> 3] |= 1 << (section & 7);
    return TRUE;
}

void IronmonRecordBattleEnd(void)
{
    if (!IsIronmonRun() || gSaveBlock3Ptr->ironmon.ended) return;
    if (gPartiesCount[B_TRAINER_PLAYER] && GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP) == 0)
        gSaveBlock3Ptr->ironmon.ended = TRUE;
    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && gBattleOutcome == B_OUTCOME_WON
     && gSaveBlock3Ptr->ironmon.trainersDefeated < 65535)
        gSaveBlock3Ptr->ironmon.trainersDefeated++;
}

void IronmonMarkFainted(u32 battler)
{
    if (IsIronmonRun() && GetBattlerSide(battler) == B_SIDE_PLAYER
     && gBattlerPartyIndexes[battler] == 0)
        gSaveBlock3Ptr->ironmon.ended = TRUE;
}

bool32 IronmonCheckRunOver(void)
{
    if (!IsIronmonRun()) return FALSE;
    if (gSaveBlock3Ptr->ironmon.starterGranted
     && gPartiesCount[B_TRAINER_PLAYER] && GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP) == 0)
        gSaveBlock3Ptr->ironmon.ended = TRUE;
    if (!gSaveBlock3Ptr->ironmon.ended) return FALSE;
    SetMainCallback1(NULL);
    SetMainCallback2(CB2_IronmonRunOver);
    return TRUE;
}

void IronmonSetTrainerIdentity(u32 id)
{
    sTrainerIdentity = id == 0xFFFF ? 0 : id + 1;
}

u32 IronmonTrainerPartySize(const struct Trainer *trainer)
{
    if (!IsIronmonRun()) return trainer->partySize;
    u32 identity = sTrainerIdentity;
    for (u32 difficulty = 0; !identity && difficulty < DIFFICULTY_COUNT; difficulty++)
        for (u32 id = 0; id < TRAINERS_COUNT; id++)
            if (trainer == &gTrainers[difficulty][id]) { identity = id + 1; break; }
    switch (identity - 1)
    {
    case TRAINER_LEADER_BROCK: case TRAINER_LEADER_MISTY: return 3;
    case TRAINER_LEADER_LT_SURGE: case TRAINER_LEADER_ERIKA: return 4;
    case TRAINER_LEADER_KOGA: case TRAINER_LEADER_SABRINA: return 5;
    case TRAINER_LEADER_BLAINE: case TRAINER_LEADER_GIOVANNI: return 6;
    default: return trainer->partySize;
    }
}

static bool32 OverworldItemEligible(enum Item item)
{
    // Preserve progression objects at their sources; generated rewards use a
    // uniform ordinary pool. TMs, HMs, mail and unobtainable dummy IDs are out.
    return item != ITEM_NONE && GetItemPocket(item) != POCKET_KEY_ITEMS
        && GetItemPocket(item) != POCKET_TM_HM && !ItemIsMail(item)
        && GetItemPrice(item) > 0;
}
enum Item IronmonOverworldItem(u32 seed)
{
    u32 count = 0;
    for (u32 item = 1; item < ITEMS_COUNT; item++)
        if (OverworldItemEligible(item)) count++;
    u32 rank = IronmonMix(seed) % count;
    for (u32 item = 1; item < ITEMS_COUNT; item++)
        if (OverworldItemEligible(item) && rank-- == 0) return item;
    return ITEM_POKE_BALL;
}
