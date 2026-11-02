#include "global.h"
#include "gym_leader.h"
#include "battle.h"
#include "battle_setup.h"
#include "battle_transition.h"
#include "data.h"
#include "event_data.h"
#include "field_screen_effect.h"
#include "main.h"
#include "malloc.h"
#include "overworld.h"
#include "party_menu.h"
#include "palette.h"
#include "constants/rgb.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "constants/battle_ai.h"
#include "constants/battle_frontier.h"
#include "constants/vars.h"

// These survive the battle engine's heap resets. The persistent registration
// stores identities, not cloned Pokémon, so deposits and party reordering work.
static EWRAM_DATA bool8 sRegistering = FALSE;
static EWRAM_DATA bool8 sActive = FALSE;
static EWRAM_DATA u16 sOldFacility = 0;
static EWRAM_DATA u8 sTravelCount = 0;
static EWRAM_DATA struct Pokemon sTravelParty[PARTY_SIZE] = {0};
static EWRAM_DATA struct Pokemon sOriginalTeam[PARTY_SIZE] = {0};
static EWRAM_DATA s16 sSources[PARTY_SIZE] = {0};
static EWRAM_DATA u8 sTeamCount = 0;
static EWRAM_DATA struct Trainer sChallenger = {0};
static EWRAM_DATA struct TrainerMon sChallengerEntries[PARTY_SIZE] = {0};
static const u8 sDefeatText[] = _("That was a great GYM challenge!");

bool32 GymLeaderRegistering(void) { return sRegistering; }
const struct Trainer *GymLeaderBattleTrainer(void) { return sActive ? &sChallenger : NULL; }

static void ReturnFromRegistration(void)
{
    struct GleState *s = GymLeaderState();
    u32 i, count = 0;
    struct Pokemon team[PARTY_SIZE];
    sRegistering = FALSE;
    VarSet(VAR_FRONTIER_FACILITY, sOldFacility);
    gSpecialVar_Result = 0;
    for (i = 0; i < GymLeaderPartyLimit(); i++)
    {
        u32 slot = gSelectedOrderFromParty[i];
        if (!slot)
            break;
        if (slot > gPartiesCount[B_TRAINER_PLAYER])
            goto done;
        team[count++] = gParties[B_TRAINER_PLAYER][slot - 1];
    }
    if (count == 0)
        goto done;
    if (!GymLeaderTeamLegal(team, count))
    {
        gSpecialVar_Result = 2;
        goto done;
    }
    for (i = 0; i < count; i++)
    {
        s->registeredPersonality[i] = GetMonData(&team[i], MON_DATA_PERSONALITY);
        s->registeredTrainerId[i] = GetMonData(&team[i], MON_DATA_OT_ID);
    }
    s->registeredCount = count;
    gSpecialVar_Result = 1;
done:
    SetMainCallback2(CB2_ReturnToFieldContinueScript);
}

static void Task_RegisterTeam(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        DestroyTask(taskId);
        gMain.savedCallback = ReturnFromRegistration;
        InitChooseHalfPartyForBattle(0);
    }
}

void GymLeaderRegisterTeam(void)
{
    gSpecialVar_Result = 0;
    if (!IsGymLeaderRun() || GymLeaderState()->openingStage != 5 || sActive)
    {
        ScriptContext_Enable();
        return;
    }
    sOldFacility = VarGet(VAR_FRONTIER_FACILITY);
    VarSet(VAR_FRONTIER_FACILITY, FACILITY_MULTI_OR_EREADER);
    sRegistering = TRUE;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_RegisterTeam, 10);
}

// Party sources are negative; PC sources are box * 30 + slot. Locate the real
// owner by PID and trainer ID every time rather than trusting a stale slot.
static bool32 FindRegisteredMon(u32 index, struct Pokemon *out, s16 *source)
{
    const struct GleState *s = GymLeaderState();
    u32 i, b;
    for (i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        if (GetMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE
            && GetMonData(mon, MON_DATA_PERSONALITY) == s->registeredPersonality[index]
            && GetMonData(mon, MON_DATA_OT_ID) == s->registeredTrainerId[index])
        {
            *out = *mon;
            *source = -1 - i;
            return TRUE;
        }
    }
    for (b = 0; b < TOTAL_BOXES_COUNT; b++)
        for (i = 0; i < IN_BOX_COUNT; i++)
        {
            struct BoxPokemon *mon = &gPokemonStoragePtr->boxes[b][i];
            if (GetBoxMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE
                && GetBoxMonData(mon, MON_DATA_PERSONALITY) == s->registeredPersonality[index]
                && GetBoxMonData(mon, MON_DATA_OT_ID) == s->registeredTrainerId[index])
            {
                BoxMonToMon(mon, out);
                *source = b * IN_BOX_COUNT + i;
                return TRUE;
            }
        }
    return FALSE;
}

static bool32 PrepareChallenger(void)
{
    struct GleState *s = GymLeaderState();
    u16 *pool = Alloc(GLE_SPECIES_LIMIT * sizeof(u16));
    u32 i, average = 0, seed = GleMix(s->seed ^ s->defenses ^ ((u32)s->losses << 16));
    if (pool == NULL)
        return FALSE;
    for (i = 0; i < sTeamCount; i++)
        average += GetMonData(&sOriginalTeam[i], MON_DATA_LEVEL);
    average = min(average / sTeamCount, GymLeaderLevelCap());
    // Early novices bring fewer Pokémon; later ranks use the full allowance.
    sChallenger.partySize = s->rank == 1 ? 1 + seed % min(2, sTeamCount) : sTeamCount;
    sChallenger.trainerClass = TRAINER_CLASS_PKMN_TRAINER_1;
    sChallenger.trainerPic = TRAINER_PIC_CAMPER_FRLG;
    StringCopy(sChallenger.trainerName, COMPOUND_STRING("CHALLENGER"));
    sChallenger.aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT;
    sChallenger.party = sChallengerEntries;
    ZeroEnemyPartyMons();
    for (i = 0; i < sChallenger.partySize; i++)
    {
        u32 count, type = GleMix(seed ^ i) % 18;
        u8 level = max(3, average > 2 ? average - 2 : 3);
        enum Species species;
        count = GymLeaderBuildSpeciesPool(type, s->rank, GLE_STANDARD, s->rank == 1, pool, GLE_SPECIES_LIMIT);
        if (!count)
        {
            Free(pool);
            return FALSE;
        }
        species = pool[GleMix(seed ^ (i * 73)) % count];
        CreateMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][i], species, level,
            GleMix(seed ^ i ^ 0xC411), OTID_STRUCT_PRESET(seed), 10 + s->rank);
        GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][i]);
        sChallengerEntries[i].species = species;
        sChallengerEntries[i].lvl = level;
    }
    gPartiesCount[B_TRAINER_OPPONENT_A] = sChallenger.partySize;
    Free(pool);
    return TRUE;
}

// Returns 1 on success. Failure never changes the traveling party or queue.
u32 GymLeaderPrepareDefense(void)
{
    struct GleState *s = GymLeaderState();
    u32 i, j;
    if (!IsGymLeaderRun() || s->openingStage != 5 || sActive || !s->queueCount)
        return 0;
    if (!s->registeredCount || s->registeredCount > GymLeaderPartyLimit())
        return 2;
    sTeamCount = s->registeredCount;
    for (i = 0; i < sTeamCount; i++)
    {
        if (!FindRegisteredMon(i, &sOriginalTeam[i], &sSources[i]))
            return 3;
        for (j = 0; j < i; j++)
            if (sSources[i] == sSources[j])
                return 3;
    }
    if (!GymLeaderTeamLegal(sOriginalTeam, sTeamCount))
        return 2;
    if (!PrepareChallenger())
        return 0;
    memcpy(sTravelParty, gParties[B_TRAINER_PLAYER], sizeof(sTravelParty));
    sTravelCount = gPartiesCount[B_TRAINER_PLAYER];
    ZeroPlayerPartyMons();
    for (i = 0; i < sTeamCount; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        *mon = sOriginalTeam[i];
        if (GetMonData(mon, MON_DATA_LEVEL) > GymLeaderLevelCap())
        {
            enum Species species = GetMonData(mon, MON_DATA_SPECIES);
            u32 exp = gExperienceTables[gSpeciesInfo[species].growthRate][GymLeaderLevelCap()];
            u8 level = GymLeaderLevelCap();
            SetMonData(mon, MON_DATA_EXP, &exp);
            SetMonData(mon, MON_DATA_LEVEL, &level);
            CalculateMonStats(mon);
        }
    }
    gPartiesCount[B_TRAINER_PLAYER] = sTeamCount;
    HealPlayerParty();
    sActive = TRUE;
    GleTakeChallenger(s);
    return 1;
}

void GymLeaderFinishDefense(void)
{
    u32 i;
    struct GleState *s;
    if (!sActive)
        return;
    s = GymLeaderState();
    for (i = 0; i < sTeamCount; i++)
        if (GetMonData(&sOriginalTeam[i], MON_DATA_LEVEL) > GymLeaderLevelCap())
            gParties[B_TRAINER_PLAYER][i] = sOriginalTeam[i];
    HealPlayerParty();
    for (i = 0; i < sTeamCount; i++)
    {
        struct Pokemon *result = &gParties[B_TRAINER_PLAYER][i];
        // Demotion normalization is temporary. Preserve every original byte of
        // an over-cap Pokémon, including its stored level, experience and moves.
        if (sSources[i] < 0)
            sTravelParty[-1 - sSources[i]] = *result;
        else
            gPokemonStoragePtr->boxes[sSources[i] / IN_BOX_COUNT][sSources[i] % IN_BOX_COUNT] = result->box;
    }
    memcpy(gParties[B_TRAINER_PLAYER], sTravelParty, sizeof(sTravelParty));
    gPartiesCount[B_TRAINER_PLAYER] = sTravelCount;
    HealPlayerParty();
    GleDefenseResult(s, (gBattleOutcome & 0x7F) == B_OUTCOME_WON);
    GymLeaderAdvanceActivity();
    // Preserve a reliable queue while visiting the Gym; never force waiting.
    if (!s->queueCount)
        GleEnqueue(s, 0);
    gSpecialVar_Result = (gBattleOutcome & 0x7F) == B_OUTCOME_WON ? 1 : 2;
    sActive = FALSE;
}

static void ReturnFromDefense(void)
{
    GymLeaderFinishDefense();
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

static void Task_StartDefense(u8 taskId)
{
    if (IsBattleTransitionDone())
    {
        gMain.savedCallback = ReturnFromDefense;
        SetMainCallback2(CB2_InitBattle);
        DestroyTask(taskId);
    }
}

void GymLeaderStartDefense(void)
{
    gSpecialVar_Result = GymLeaderPrepareDefense();
    if (gSpecialVar_Result != 1)
    {
        gSpecialVar_Result += 10;
        ScriptContext_Enable();
        return;
    }
    InitTrainerBattleParameter();
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_NONE;
    TRAINER_BATTLE_PARAM.defeatTextA = (u8 *)sDefeatText;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    CreateTask(Task_StartDefense, 1);
    PlayMapChosenOrBattleBGM(0);
    BattleTransition_StartOnField(GetTrainerBattleTransition());
}
