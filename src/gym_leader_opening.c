#include "global.h"
#include "gym_leader.h"
#include "gym_leader_script.h"
#include "item.h"
#include "event_data.h"
#include "field_screen_effect.h"
#include "constants/rgb.h"
#include "constants/battle.h"
#include "main.h"
#include "malloc.h"
#include "move.h"
#include "naming_screen.h"
#include "overworld.h"
#include "palette.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "script.h"
#include "script_menu.h"
#include "string_util.h"
#include "task.h"
#include "constants/flags.h"
#include "constants/maps.h"
#include "constants/pokemon.h"
#include "constants/vars.h"

// A separate preview array avoids the Summary screen's link-opponent path.
// Passing the enemy party there makes it read an uninitialized link trainer name.
static EWRAM_DATA struct Pokemon sDraftPreview[3] = {0};

static const u8 *const sTownNames[] = {COMPOUND_STRING("SAFFRON"), COMPOUND_STRING("CELADON"), COMPOUND_STRING("FUCHSIA")};
static const u8 *const sTypeNames[] = {
    COMPOUND_STRING("NORMAL"), COMPOUND_STRING("FIRE"), COMPOUND_STRING("WATER"),
    COMPOUND_STRING("ELECTRIC"), COMPOUND_STRING("GRASS"), COMPOUND_STRING("ICE"),
    COMPOUND_STRING("FIGHTING"), COMPOUND_STRING("POISON"), COMPOUND_STRING("GROUND"),
    COMPOUND_STRING("FLYING"), COMPOUND_STRING("PSYCHIC"), COMPOUND_STRING("BUG"),
    COMPOUND_STRING("ROCK"), COMPOUND_STRING("GHOST"), COMPOUND_STRING("DRAGON"),
    COMPOUND_STRING("DARK"), COMPOUND_STRING("STEEL"), COMPOUND_STRING("FAIRY")
};
static const u8 sCounterTypes[] = {6,2,3,8,1,6,10,8,2,3,15,1,2,15,17,6,1,7};

void GymLeaderMenu(const u8 *const *names, u32 count, bool8 ignoreB)
{
    struct ListMenuItem *items = AllocZeroed(count * sizeof(*items));
    u32 i;
    if (items == NULL)
    {
        gSpecialVar_Result = 255;
        GymLeaderResumeScriptNextFrame();
        return;
    }
    for (i = 0; i < count; i++)
    {
        u8 *name = AllocZeroed(StringLength(names[i]) + 1);
        if (name == NULL)
        {
            while (i != 0)
                Free((void *)items[--i].name);
            Free(items);
            gSpecialVar_Result = 255;
            GymLeaderResumeScriptNextFrame();
            return;
        }
        StringCopy(name, names[i]);
        items[i].name = name;
        // Preserve the native B-cancel sentinel even in long species lists.
        items[i].id = i < 127 ? i : i + 1;
    }
    ScriptMenu_MultichoiceDynamic(1, 1, count, items, ignoreB, 6, 0, 0);
}

void GleOpeningStage(void)
{
    gSpecialVar_Result = IsGymLeaderRun() ? GymLeaderState()->openingStage : 255;
}

void GleTownMenu(void)
{
    GymLeaderMenu(sTownNames, ARRAY_COUNT(sTownNames), TRUE);
}

void GleTownChoose(void)
{
    struct GleState *s = GymLeaderState();
    if (!IsGymLeaderRun() || gSpecialVar_Result >= ARRAY_COUNT(sTownNames))
        return;
    s->town = gSpecialVar_Result;
    StringCopy(s->townName, sTownNames[s->town]);
    s->openingStage = 1;
}

static void ReturnFromTownName(void)
{
    struct GleState *s = GymLeaderState();
    if (s->townName[0] == EOS)
        StringCopy(s->townName, sTownNames[s->town]);
    SetMainCallback2(CB2_ReturnToFieldContinueScript);
}

void GleTownRename(void)
{
    DoNamingScreen(NAMING_SCREEN_GLE_TOWN, GymLeaderState()->townName, 0, 0, 0, ReturnFromTownName);
}

void GleTypeMenu(void)
{
    GymLeaderMenu(sTypeNames, ARRAY_COUNT(sTypeNames), TRUE);
}

static bool32 RollDraft(void)
{
    struct GleState *s = GymLeaderState();
    u32 count = GymLeaderPoolCount(s->gymType, 1, GLE_STANDARD, TRUE);
    u16 *pool;
    u32 chosen, i;
    if (count < 9 || count > GLE_SPECIES_LIMIT)
        return FALSE;
    pool = Alloc(count * sizeof(*pool));
    if (pool == NULL)
        return FALSE;
    if (GymLeaderBuildStarterPool(s->gymType, pool, count) != count)
    {
        Free(pool);
        return FALSE;
    }
    for (chosen = 0; chosen < 9; chosen++)
    {
        u32 bestScore = 0;
        enum Species best = SPECIES_NONE;
        for (i = 0; i < count; i++)
        {
            enum Species species = pool[i];
            u32 j, score;
            for (j = 0; j < chosen; j++)
                if (gSpeciesInfo[s->draftSpecies[j]].natDexNum == gSpeciesInfo[species].natDexNum)
                    break;
            if (j < chosen)
                continue;
            score = GleMix(s->seed ^ species ^ (chosen * 0x9E3779B9u));
            if (best == SPECIES_NONE || score > bestScore)
            {
                best = species;
                bestScore = score;
            }
        }
        if (best == SPECIES_NONE)
        {
            memset(s->draftSpecies, 0, sizeof(s->draftSpecies));
            Free(pool);
            return FALSE;
        }
        s->draftSpecies[chosen] = best;
        s->draftAbilities[chosen] = gSpeciesInfo[best].abilities[1] != ABILITY_NONE
            ? (GleMix(s->seed ^ best ^ 0xAB17u) & 1) : 0;
    }
    Free(pool);
    return TRUE;
}

void GleTypeChoose(void)
{
    struct GleState *s = GymLeaderState();
    u32 choice = gSpecialVar_Result;
    gSpecialVar_Result = 0;
    if (!IsGymLeaderRun() || choice >= 18 || s->openingStage != 1)
        return;
    if (GleChooseIdentity(s, s->town, choice, sCounterTypes[choice]) != GLE_OK)
        return;
    if (!RollDraft())
    {
        s->gymType = GLE_NONE;
        return;
    }
    EnableNationalPokedex();
    s->openingStage = 2;
    gSpecialVar_Result = 1;
}

void GleSignatureMenu(void)
{
    const u8 *names[3];
    u32 i;
    for (i = 0; i < 3; i++)
        names[i] = GetMoveName(GymLeaderSignatureOption(GymLeaderState()->gymType, i));
    GymLeaderMenu(names, 3, TRUE);
}

void GleSignatureInfo(void)
{
    static const u8 *const categories[] = {COMPOUND_STRING("reliable attack"), COMPOUND_STRING("tactical move"), COMPOUND_STRING("momentum move")};
    if (gSpecialVar_0x8007 >= 3)
        return;
    StringCopy(gStringVar1, GetMoveName(GymLeaderSignatureOption(GymLeaderState()->gymType, gSpecialVar_0x8007)));
    StringCopy(gStringVar2, categories[gSpecialVar_0x8007]);
}

void GleSignatureChoose(void)
{
    struct GleState *s = GymLeaderState();
    gSpecialVar_Result = 0;
    if (IsGymLeaderRun() && s->openingStage == 2 && gSpecialVar_0x8007 < 3)
    {
        if (!CheckBagHasItem(ITEM_GYM_SIGNATURE_TM, 1) && !AddBagItem(ITEM_GYM_SIGNATURE_TM, 1))
            return;
        s->signatureMove = GymLeaderSignatureOption(s->gymType, gSpecialVar_0x8007);
        gSpecialVar_Result = 1;
        s->openingStage = 3;
    }
}

void GleDraftMenu(void)
{
    struct GleState *s = GymLeaderState();
    const u8 *names[3];
    u32 i;
    if (!IsGymLeaderRun() || s->draftRound >= 3)
    {
        gSpecialVar_Result = 255;
        GymLeaderResumeScriptNextFrame();
        return;
    }
    for (i = 0; i < 3; i++)
        names[i] = GetSpeciesName(s->draftSpecies[s->draftRound * 3 + i]);
    GymLeaderMenu(names, 3, TRUE);
}

void GleDraftInfo(void)
{
    struct GleState *s = GymLeaderState();
    u32 index = s->draftRound * 3 + gSpecialVar_0x8007;
    enum Species species;
    if (!IsGymLeaderRun() || s->draftRound >= 3 || gSpecialVar_0x8007 >= 3)
        return;
    species = s->draftSpecies[index];
    StringCopy(gStringVar1, GetSpeciesName(species));
    StringCopy(gStringVar2, gAbilitiesInfo[gSpeciesInfo[species].abilities[s->draftAbilities[index]]].name);
    StringCopy(gStringVar3, sTypeNames[s->gymType]);
}

void GleDraftActions(void)
{
    static const u8 *const names[] = {COMPOUND_STRING("Choose"), COMPOUND_STRING("Summary"), COMPOUND_STRING("Back")};
    GymLeaderMenu(names, 3, FALSE);
}

static void MakeDraftMon(struct Pokemon *mon, u32 index)
{
    struct GleState *s = GymLeaderState();
    u32 stat;
    CreateMonWithIVs(mon, s->draftSpecies[index], 5,
        GleMix(s->seed ^ index ^ 0xDFA7u), OTID_STRUCT_PLAYER_ID, 0);
    GiveMonInitialMoveset(mon);
    for (stat = 0; stat < NUM_STATS; stat++)
    {
        u8 iv = GleMix(s->seed ^ (index * 31) ^ (stat * 257)) % 32;
        SetMonData(mon, MON_DATA_HP_IV + stat, &iv);
    }
    SetMonData(mon, MON_DATA_ABILITY_NUM, &s->draftAbilities[index]);
    CalculateMonStats(mon);
}

static void Task_DraftSummary(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        ResetTasks();
        CleanupOverworldWindowsAndTilemaps();
        ShowPokemonSummaryScreen(SUMMARY_MODE_NORMAL, sDraftPreview, gSpecialVar_0x8007, 2, CB2_ReturnToFieldContinueScript);
    }
}

void GleDraftSummary(void)
{
    struct GleState *s = GymLeaderState();
    u32 i;
    if (!IsGymLeaderRun() || s->draftRound >= 3 || gSpecialVar_0x8007 >= 3)
    {
        GymLeaderResumeScriptNextFrame();
        return;
    }
    for (i = 0; i < 3; i++)
        MakeDraftMon(&sDraftPreview[i], s->draftRound * 3 + i);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_DraftSummary, 10);
}

void GleDraftChoose(void)
{
    struct GleState *s = GymLeaderState();
    enum Species species;
    gSpecialVar_Result = 0;
    if (!IsGymLeaderRun() || s->openingStage != 3 || s->draftRound >= 3
        || gSpecialVar_0x8007 >= 3 || gPartiesCount[B_TRAINER_PLAYER] != s->draftRound)
        return;
    MakeDraftMon(&gParties[B_TRAINER_PLAYER][s->draftRound], s->draftRound * 3 + gSpecialVar_0x8007);
    species = s->draftSpecies[s->draftRound * 3 + gSpecialVar_0x8007];
    GetSetPokedexFlag(gSpeciesInfo[species].natDexNum, FLAG_SET_SEEN);
    GetSetPokedexFlag(gSpeciesInfo[species].natDexNum, FLAG_SET_CAUGHT);
    s->draftRound++;
    gPartiesCount[B_TRAINER_PLAYER] = s->draftRound;
    FlagSet(FLAG_SYS_POKEMON_GET);
    if (s->draftRound == 3)
        s->openingStage = 4;
    gSpecialVar_Result = 1;
}

static void SetTownExit(bool32 league)
{
    static const u16 towns[] = {MAP_SAFFRON_CITY, MAP_CELADON_CITY, MAP_FUCHSIA_CITY};
    static const u16 gyms[] = {MAP_SAFFRON_CITY_GYM, MAP_CELADON_CITY_GYM, MAP_FUCHSIA_CITY_GYM};
    u16 town = league ? MAP_VIRIDIAN_CITY : towns[GymLeaderState()->town];
    if (!league)
    {
        SetDynamicWarpWithCoords(0, MAP_GROUP(town), MAP_NUM(town), -1, 10, 9);
        return;
    }
    u16 gym = league ? MAP_VIRIDIAN_CITY_GYM : gyms[GymLeaderState()->town];
    const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(town), MAP_NUM(town));
    u32 i;
    for (i = 0; i < map->events->warpCount; i++)
    {
        const struct WarpEvent *door = &map->events->warps[i];
        if (door->mapGroup == MAP_GROUP(gym) && door->mapNum == MAP_NUM(gym))
        {
            SetDynamicWarpWithCoords(0, MAP_GROUP(town), MAP_NUM(town), -1, door->x, door->y + 1);
            return;
        }
    }
}

void GleWarpHome(void)
{
    SetTownExit(FALSE);
    SetWarpDestination(MAP_GROUP(MAP_GLE_GYM_INTERIOR), MAP_NUM(MAP_GLE_GYM_INTERIOR), -1, 6, 7);
    DoWarp();
}

void GleWarpLeague(void)
{
    SetTownExit(TRUE);
    SetWarpDestination(MAP_GROUP(MAP_GLE_LEAGUE_HALL), MAP_NUM(MAP_GLE_LEAGUE_HALL), -1, 6, 7);
    DoWarp();
}

void GleFinishOpening(void)
{
    if (IsGymLeaderRun() && GymLeaderState()->openingStage == 4 && gPartiesCount[B_TRAINER_PLAYER] == 3)
    {
        struct GleState *s = GymLeaderState();
        u32 i;
        s->openingStage = 5;
        s->registeredCount = 3;
        for (i = 0; i < 3; i++)
        {
            s->registeredPersonality[i] = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_PERSONALITY);
            s->registeredTrainerId[i] = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_OT_ID);
        }
        FlagSet(FLAG_SYS_POKEDEX_GET);
        FlagSet(FLAG_SYS_B_DASH);
        GymLeaderUnlockTravel();
    }
}

void GymLeaderRedirectWarp(s8 *group, s8 *number, s8 *warp, s8 *x, s8 *y)
{
    static const u16 gyms[] = {MAP_SAFFRON_CITY_GYM, MAP_CELADON_CITY_GYM, MAP_FUCHSIA_CITY_GYM};
    struct GleState *s = GymLeaderState();
    bool32 league;
    u16 destination;
    if (GymLeaderTownWarp(group, number, warp, x, y))
        return;
    if (!IsGymLeaderRun() || s->openingStage == 0)
        return;
    destination = (u8)*number | ((u8)*group << 8);
    league = destination == MAP_VIRIDIAN_CITY_GYM;
    if (!league && destination != gyms[s->town])
        return;
    SetTownExit(league);
    destination = league ? MAP_GLE_LEAGUE_HALL : MAP_GLE_GYM_INTERIOR;
    *group = MAP_GROUP(destination);
    *number = MAP_NUM(destination);
    *warp = -1;
    *x = 6;
    *y = 13;
}

void GleGymPrepare(void)
{
    if (IsGymLeaderRun() && GymLeaderState()->openingStage >= 4)
        FlagSet(FLAG_TEMP_1);
    else
        FlagClear(FLAG_TEMP_1);
}

void GymLeaderGuideMenu(void)
{
    static const u8 *const names[] = {
        COMPOUND_STRING("Defend GYM"), COMPOUND_STRING("Register team"),
        COMPOUND_STRING("GYM status"), COMPOUND_STRING("Heal team"),
        COMPOUND_STRING("League Inspection"), COMPOUND_STRING("Visit GLA"), COMPOUND_STRING("Back")
    };
    GymLeaderMenu(names, ARRAY_COUNT(names), FALSE);
}

void GymLeaderGuideStatus(void)
{
    struct GleState *s = GymLeaderState();
    ConvertIntToDecimalStringN(gStringVar1, s->rank, STR_CONV_MODE_LEFT_ALIGN, 1);
    ConvertIntToDecimalStringN(gStringVar2, GymLeaderLevelCap(), STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar3, s->registeredCount, STR_CONV_MODE_LEFT_ALIGN, 1);
    gSpecialVar_0x8004 = s->queueCount;
}

void GymLeaderInspectionInfo(void)
{
    static const u8 *const names[] = {COMPOUND_STRING("LORELEI"), COMPOUND_STRING("BRUNO"), COMPOUND_STRING("AGATHA"), COMPOUND_STRING("LANCE")};
    struct GleState *s = GymLeaderState();
    gSpecialVar_Result = 0;
    if (!IsGymLeaderRun() || s->pendingInspection < 1 || s->pendingInspection > 4)
        return;
    StringCopy(gStringVar1, names[s->pendingInspection - 1]);
    StringCopy(gStringVar2, s->honor >= 100 ? COMPOUND_STRING("Your good standing is noted.") : s->honor <= -100 ? COMPOUND_STRING("Your methods raise concerns.") : COMPOUND_STRING("Let's assess your leadership."));
    gSpecialVar_Result = 1;
}

void GymLeaderRankInfo(void)
{
    struct GleState *s = GymLeaderState();
    ConvertIntToDecimalStringN(gStringVar1, s->rankXp, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar2, GleXpNeeded(s), STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar3, GymLeaderPartyLimit(), STR_CONV_MODE_LEFT_ALIGN, 1);
}

// Travel permissions are independent of earned badges and League titles. Bit 0
// of milestoneRewardMask records this one-time setup; rank rewards use bits 1-7.
void GymLeaderUnlockTravel(void)
{
    static const u16 flags[] = {
        FLAG_HIDE_CERULEAN_RIVAL, FLAG_HIDE_SS_ANNE_RIVAL,
        FLAG_HIDE_SILPH_RIVAL, FLAG_HIDE_ROUTE_22_RIVAL, FLAG_HIDE_TOWER_RIVAL,
        FLAG_HIDE_CERULEAN_CAVE_GUARD, FLAG_GOT_SS_TICKET, FLAG_GOT_BICYCLE,
        FLAG_HIDE_PEWTER_CITY_GYM_GUIDE, FLAG_HIDE_SAFFRON_ROCKETS, FLAG_HIDE_SILPH_ROCKETS
    };
    static const u16 scenes[][2] = {
        {VAR_MAP_SCENE_VIRIDIAN_CITY_OLD_MAN,2},
        {VAR_MAP_SCENE_VIRIDIAN_CITY_GYM_DOOR,1},
        {VAR_MAP_SCENE_CERULEAN_CITY_RIVAL,1},
        {VAR_MAP_SCENE_ROUTE22,4},
        {VAR_MAP_SCENE_PALLET_TOWN_OAK,3},
        {VAR_MAP_SCENE_VIRIDIAN_CITY_MART,1},
        {VAR_MAP_SCENE_POKEMON_TOWER_2F,1},
        {VAR_MAP_SCENE_POKEMON_TOWER_6F,1},
        {VAR_MAP_SCENE_SILPH_CO_7F,1},
        {VAR_MAP_SCENE_S_S_ANNE_2F_CORRIDOR,1},
        {VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB,4},
        {VAR_MAP_SCENE_ROUTE5_ROUTE6_ROUTE7_ROUTE8_GATES,1},
        {VAR_MAP_SCENE_ROUTE23,8}
    };
    static const u16 equipment[] = {
        ITEM_BICYCLE,ITEM_SS_TICKET,ITEM_POKE_FLUTE,ITEM_SILPH_SCOPE,ITEM_SECRET_KEY,ITEM_CARD_KEY,
        ITEM_HM01,ITEM_HM02,ITEM_HM03,ITEM_HM04,ITEM_HM05,ITEM_HM06,ITEM_HM07,ITEM_HM08
    };
    struct GleState *s=GymLeaderState();u32 i;
    if(!IsGymLeaderRun() || s->openingStage!=5)return;
    // The modern engine treats Escape Rope as reusable field equipment.
    // Also provision existing V1 saves whose travel permit predates this item.
    if(!CheckBagHasItem(ITEM_ESCAPE_ROPE,1) && !AddBagItem(ITEM_ESCAPE_ROPE,1))return;
    if(s->milestoneRewardMask&1)return;
    for(i=0;i<ARRAY_COUNT(equipment);i++)
        if(!CheckBagHasItem(equipment[i],1) && !AddBagItem(equipment[i],1))return;
    for(i=0;i<ARRAY_COUNT(flags);i++)FlagSet(flags[i]);
    for(i=0;i<ARRAY_COUNT(scenes);i++)VarSet(scenes[i][0],scenes[i][1]);
    FlagClear(FLAG_HIDE_SAFFRON_CIVILIANS);
    VarSet(VAR_CHAOS_POKERIDER_UNLOCKED,1);
    s->milestoneRewardMask|=1;
}

static void Task_GleResumeScript(u8 taskId)
{
    DestroyTask(taskId);
    ScriptContext_Enable();
}
void GymLeaderResumeScriptNextFrame(void)
{
    CreateTask(Task_GleResumeScript,0);
}
