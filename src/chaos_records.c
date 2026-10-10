#include "global.h"
#include "chaos_records.h"
#include "battle.h"
#include "battle_setup.h"
#include "event_data.h"
#include "item.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "pokedex.h"
#include "run_settings.h"
#include "string_util.h"
#include "constants/battle.h"
#include "constants/pokedex.h"
#include "constants/trainers.h"
#include "wild_encounter.h"
#include "overworld.h"
#include "strings.h"
#include "move.h"
#include "caps.h"
#include "text.h"
#include "battle_main.h"
#include "script.h"
#include "script_menu.h"
#include "task.h"
#include "window.h"
#include "menu.h"
#include "field_message_box.h"
#include "main.h"
#include "constants/rgb.h"
#include "constants/region_map_sections.h"

#define RECORDS_MAGIC 0x43525231
void ChaosEnsureRunRecords(void)
{
    if (gSaveBlock3Ptr->recordsMagic == RECORDS_MAGIC) return;
    memset(gSaveBlock3Ptr->encounterSpecies, 0,
        offsetof(struct SaveBlock3, ironmon) - offsetof(struct SaveBlock3, encounterSpecies));
    memcpy(gSaveBlock3Ptr->encounterFailed, gSaveBlock3Ptr->nuzlockeEncounterUsed, 32);
    gSaveBlock3Ptr->recordsMagic = RECORDS_MAGIC;
}

bool8 NuzlockeMapSectionEncounterFailed(u16 section)
{
    ChaosEnsureRunRecords();
    return section < 256 && (gSaveBlock3Ptr->encounterFailed[section >> 3] & (1 << (section & 7)));
}

static void RecordMon(struct ChaosRunMonRecord *record, struct Pokemon *mon)
{
    record->personality = GetMonData(mon, MON_DATA_PERSONALITY);
    record->species = GetMonData(mon, MON_DATA_SPECIES);
    record->item = GetMonData(mon, MON_DATA_HELD_ITEM);
    record->ability = GetMonAbility(mon);
    record->nature = GetNature(mon);
    GetMonData(mon, MON_DATA_NICKNAME, record->nickname);
    for (u32 i = 0; i < 4; i++) record->moves[i] = GetMonData(mon, MON_DATA_MOVE1 + i);
}

void ChaosRecordMove(void)
{
    if (GetBattlerSide(gBattlerAttacker) != B_SIDE_PLAYER
     || gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED | BATTLE_TYPE_FIRST_BATTLE)) return;
    ChaosEnsureRunRecords();
    struct Pokemon *mon = GetBattlerMon(gBattlerAttacker);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY);
    u32 index = 0;
    for (u32 i = 0; i < ARRAY_COUNT(gSaveBlock3Ptr->journeyMons); i++)
    {
        struct ChaosRunMonRecord *record = &gSaveBlock3Ptr->journeyMons[i];
        if (record->species != SPECIES_NONE && record->personality == personality) {index = i; break;}
        if (record->score < gSaveBlock3Ptr->journeyMons[index].score) index = i;
    }
    struct ChaosRunMonRecord *record = &gSaveBlock3Ptr->journeyMons[index];
    if (record->personality != personality) record->score = 0;
    RecordMon(record, mon);
    u32 weight = gBattleTypeFlags & BATTLE_TYPE_TRAINER ? 3 : 1;
    if (gMapHeader.battleType == MAP_BATTLE_SCENE_GYM) weight = 8;
    if (record->score < 0xFFFFFF00) record->score += weight;
    gSaveBlock3Ptr->runCounters[11]++;
    u32 move = gCurrentMove, type = GetMoveType(move);
    if (type < NUMBER_OF_MON_TYPES && gSaveBlock3Ptr->typeRecords[type] < 65535) gSaveBlock3Ptr->typeRecords[type]++;
    bool32 found = FALSE;
    for (u32 i = 0; i < 6; i++) if (gSaveBlock3Ptr->moveRecords[i][0] == move || !gSaveBlock3Ptr->moveRecords[i][1])
    {
        gSaveBlock3Ptr->moveRecords[i][0] = move;
        if (gSaveBlock3Ptr->moveRecords[i][1] < 65535) gSaveBlock3Ptr->moveRecords[i][1]++;
        found = TRUE;
        break;
    }
    if (!found) for (u32 i = 0; i < 6; i++) gSaveBlock3Ptr->moveRecords[i][1]--;
    u32 level = GetMonData(mon, MON_DATA_LEVEL);
    if (level > gSaveBlock3Ptr->runCounters[12]) gSaveBlock3Ptr->runCounters[12] = level;
}

void ChaosRecordBattleEnd(void)
{
    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED | BATTLE_TYPE_FIRST_BATTLE)) return;
    ChaosEnsureRunRecords();
    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        if (gBattleOutcome == B_OUTCOME_WON) gSaveBlock3Ptr->runCounters[0]++;
        else if (gBattleOutcome == B_OUTCOME_LOST || gBattleOutcome == B_OUTCOME_DREW) gSaveBlock3Ptr->runCounters[1]++;
    }
    else
    {
        if (gBattleOutcome == B_OUTCOME_CAUGHT)
        {
            gSaveBlock3Ptr->runCounters[3]++;
        }
    }
    gSaveBlock3Ptr->runCounters[13] += gBattleResults.playerFaintCounter;
    gSaveBlock3Ptr->runCounters[14] += gBattleResults.opponentFaintCounter;
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        u32 level = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL);
        if (level > gSaveBlock3Ptr->runCounters[12]) gSaveBlock3Ptr->runCounters[12] = level;
    }
}

static u32 BadgeCount(void);
static u32 SurvivorCount(void)
{
    u32 count = 0;
    for (u32 i = 0; i < PARTY_SIZE; i++)
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) && GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP)) count++;
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            if (!Nuzlocke_IsGraveBox(box) && GetBoxMonDataAt(box, slot, MON_DATA_SPECIES) && !GetBoxMonDataAt(box, slot, MON_DATA_IS_EGG)) count++;
    return count;
}
static u32 AvailableAreas(void)
{
    u8 sections[32] = {0};
    u32 count = 0;
    for (u32 i = 0; gWildMonHeaders[i].mapGroup != MAP_GROUP(MAP_UNDEFINED); i++)
    {
        const struct WildPokemonHeader *header = &gWildMonHeaders[i];
        const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(header->mapGroup, header->mapNum);
        u32 section = map->regionMapSectionId;
        if (IS_FRLG && section < KANTO_MAPSEC_START) continue;
        if (section >= 256 || sections[section >> 3] & (1 << (section & 7))) continue;
        sections[section >> 3] |= 1 << (section & 7);
        if (!NuzlockeMapSectionEncounterUsed(section)) count++;
    }
    return count;
}
static u32 BestPartner(void)
{
    u32 best = 0;
    for (u32 i = 1; i < ARRAY_COUNT(gSaveBlock3Ptr->journeyMons); i++)
        if (gSaveBlock3Ptr->journeyMons[i].score > gSaveBlock3Ptr->journeyMons[best].score) best = i;
    return best;
}
static u32 MostMove(void)
{
    u32 best = 0;
    for (u32 i = 1; i < 6; i++) if (gSaveBlock3Ptr->moveRecords[i][1] > gSaveBlock3Ptr->moveRecords[best][1]) best = i;
    return gSaveBlock3Ptr->moveRecords[best][0];
}
static u32 MostType(void)
{
    u32 best = 0;
    for (u32 i = 1; i < NUMBER_OF_MON_TYPES; i++) if (gSaveBlock3Ptr->typeRecords[i] > gSaveBlock3Ptr->typeRecords[best]) best = i;
    return best;
}

static void RecordHighestPartyLevel(void)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        u32 level = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL);
        if (level > gSaveBlock3Ptr->runCounters[12]) gSaveBlock3Ptr->runCounters[12] = level;
    }
}

void ChaosSnapshotLeague(void)
{
    ChaosEnsureRunRecords();
    RecordHighestPartyLevel();
    for (u32 i = 0; i < PARTY_SIZE; i++) RecordMon(&gSaveBlock3Ptr->leagueTeam[i], &gParties[B_TRAINER_PLAYER][i]);
    memcpy(gSaveBlock3Ptr->leagueCounters, gSaveBlock3Ptr->runCounters, sizeof(gSaveBlock3Ptr->runCounters));
    gSaveBlock3Ptr->leagueSeed = gSaveBlock3Ptr->worldSeed;
    gSaveBlock3Ptr->leagueHours = gSaveBlock2Ptr->playTimeHours;
    gSaveBlock3Ptr->leagueMinutes = gSaveBlock2Ptr->playTimeMinutes;
    gSaveBlock3Ptr->leagueDifficulty = gSaveBlock3Ptr->runDifficulty;
    gSaveBlock3Ptr->leagueNuzlocke = IsNuzlockeRun();
    gSaveBlock3Ptr->leagueComplete = TRUE;
    gSaveBlock3Ptr->leagueHighestDamage = gSaveBlock3Ptr->highestDamage;
    gSaveBlock3Ptr->leagueSeen = GetNationalPokedexCount(FLAG_GET_SEEN);
    gSaveBlock3Ptr->leagueCaught = GetNationalPokedexCount(FLAG_GET_CAUGHT);
    gSaveBlock3Ptr->leagueBadges = BadgeCount();
    gSaveBlock3Ptr->leagueCap = GetCurrentLevelCap();
    gSaveBlock3Ptr->leagueEzCatch = VarGet(VAR_CHAOS_EZ_CATCH);
    gSaveBlock3Ptr->leagueAvailable = AvailableAreas();
    gSaveBlock3Ptr->leagueSurvivors = SurvivorCount();
    gSaveBlock3Ptr->leagueMostMove = MostMove();
    gSaveBlock3Ptr->leagueMostType = MostType();
    StringCopy(gSaveBlock3Ptr->leagueBestNickname, gSaveBlock3Ptr->journeyMons[BestPartner()].score ? gSaveBlock3Ptr->journeyMons[BestPartner()].nickname : COMPOUND_STRING(""));
    gSaveBlock3Ptr->leagueUsed = gSaveBlock3Ptr->leagueFailed = 0;
    for (u32 i = 0; i < 256; i++) if (NuzlockeMapSectionEncounterUsed(i))
    {
        gSaveBlock3Ptr->leagueUsed++;
        if (NuzlockeMapSectionEncounterFailed(i)) gSaveBlock3Ptr->leagueFailed++;
    }
}

static void AddNumber(const u8 *label, u32 value, bool32 newline)
{
    u8 number[12];
    StringAppend(gStringVar4, label);
    ConvertIntToDecimalStringN(number, value, STR_CONV_MODE_LEFT_ALIGN, 10);
    StringAppend(gStringVar4, number);
    if (newline) StringAppend(gStringVar4, COMPOUND_STRING("\n"));
}

static u32 BadgeCount(void)
{
    static const u16 flags[] = {FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
        FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_BADGE07_GET, FLAG_BADGE08_GET};
    u32 count = 0;
    for (u32 i = 0; i < ARRAY_COUNT(flags); i++) count += FlagGet(flags[i]);
    return count;
}

void ChaosBuildRecordsPage(void)
{
    ChaosEnsureRunRecords();
    RecordHighestPartyLevel();
    bool32 league = gSpecialVar_0x8005 && gSaveBlock3Ptr->leagueComplete;
    u32 *counts = league ? gSaveBlock3Ptr->leagueCounters : gSaveBlock3Ptr->runCounters;
    StringCopy(gStringVar4, COMPOUND_STRING(""));
    switch (gSpecialVar_0x8004)
    {
    case 0:
        AddNumber(league ? COMPOUND_STRING("League time: ") : COMPOUND_STRING("Play time: "), league ? gSaveBlock3Ptr->leagueHours : gSaveBlock2Ptr->playTimeHours, FALSE);
        StringAppend(gStringVar4, COMPOUND_STRING("h "));
        AddNumber(COMPOUND_STRING(""), league ? gSaveBlock3Ptr->leagueMinutes : gSaveBlock2Ptr->playTimeMinutes, FALSE);
        StringAppend(gStringVar4, COMPOUND_STRING("m\n"));
        AddNumber(COMPOUND_STRING("Badges: "), league ? gSaveBlock3Ptr->leagueBadges : BadgeCount(), TRUE);
        AddNumber(COMPOUND_STRING("Seen: "), league ? gSaveBlock3Ptr->leagueSeen : GetNationalPokedexCount(FLAG_GET_SEEN), TRUE);
        AddNumber(COMPOUND_STRING("Caught: "), league ? gSaveBlock3Ptr->leagueCaught : GetNationalPokedexCount(FLAG_GET_CAUGHT), FALSE);
        break;
    case 1:
        AddNumber(COMPOUND_STRING("Trainer wins: "), counts[0], TRUE);
        AddNumber(COMPOUND_STRING("Trainer losses: "), counts[1], TRUE);
        AddNumber(COMPOUND_STRING("Wild mons seen: "), counts[2], TRUE);
        AddNumber(COMPOUND_STRING("Captures: "), counts[3], FALSE);
        break;
    case 2:
        AddNumber(COMPOUND_STRING("Shinies seen: "), counts[5], TRUE);
        AddNumber(COMPOUND_STRING("Shinies caught: "), counts[6], TRUE);
        AddNumber(COMPOUND_STRING("Highest level: "), counts[12], TRUE);
        AddNumber(COMPOUND_STRING("Current cap: "), league ? gSaveBlock3Ptr->leagueCap : GetCurrentLevelCap(), FALSE);
        break;
    case 3:
        AddNumber(COMPOUND_STRING("Foes defeated: "), counts[14], TRUE);
        AddNumber(COMPOUND_STRING("Your fainted mons: "), counts[13], TRUE);
        AddNumber(COMPOUND_STRING("Critical hits: "), counts[8], TRUE);
        AddNumber(COMPOUND_STRING("Super-effective hits: "), counts[9], FALSE);
        break;
    case 4:
    {
        u32 used = 0, failed = 0;
        for (u32 i = 0; i < 256; i++)
            if (NuzlockeMapSectionEncounterUsed(i))
            {
                used++;
                if (NuzlockeMapSectionEncounterFailed(i)) failed++;
            }
        if (league) {used = gSaveBlock3Ptr->leagueUsed; failed = gSaveBlock3Ptr->leagueFailed;}
        AddNumber(COMPOUND_STRING("Areas caught: "), used - failed, TRUE);
        AddNumber(COMPOUND_STRING("Areas failed: "), failed, TRUE);
        AddNumber(COMPOUND_STRING("Dupes skipped: "), counts[4], TRUE);
        AddNumber(COMPOUND_STRING("Nuzlocke deaths: "), counts[7], FALSE);
        break;
    }
    case 5:
        StringAppend(gStringVar4, COMPOUND_STRING("Battle partner: "));
        const u8 *nickname = league ? gSaveBlock3Ptr->leagueBestNickname : gSaveBlock3Ptr->journeyMons[BestPartner()].nickname;
        if (*nickname && *nickname != EOS) StringAppend(gStringVar4, nickname);
        else StringAppend(gStringVar4, COMPOUND_STRING("No data"));
        StringAppend(gStringVar4, COMPOUND_STRING("\n"));
        AddNumber(COMPOUND_STRING("Moves used: "), counts[11], TRUE);
        AddNumber(COMPOUND_STRING("Highest damage: "), league ? gSaveBlock3Ptr->leagueHighestDamage : gSaveBlock3Ptr->highestDamage, TRUE);
        AddNumber(COMPOUND_STRING("Statuses inflicted: "), counts[10], FALSE);
        break;
    case 7:
        StringAppend(gStringVar4, COMPOUND_STRING("Move: "));
        StringAppend(gStringVar4, GetMoveName(league ? gSaveBlock3Ptr->leagueMostMove : MostMove()));
        StringAppend(gStringVar4, COMPOUND_STRING("\nType: "));
        StringAppend(gStringVar4, gTypesInfo[league ? gSaveBlock3Ptr->leagueMostType : MostType()].name);
        StringAppend(gStringVar4, COMPOUND_STRING("\n"));
        AddNumber(COMPOUND_STRING("Areas available: "), league ? gSaveBlock3Ptr->leagueAvailable : AvailableAreas(), TRUE);
        AddNumber(COMPOUND_STRING("Living partners: "), league ? gSaveBlock3Ptr->leagueSurvivors : SurvivorCount(), FALSE);
        break;
    case 8:
    case 9:
        StringAppend(gStringVar4, league ? COMPOUND_STRING("LEAGUE TEAM\n") : COMPOUND_STRING("CURRENT PARTY\n"));
        for (u32 i = (gSpecialVar_0x8004 - 8) * 3; i < (gSpecialVar_0x8004 - 8) * 3 + 3; i++)
        {
            struct ChaosRunMonRecord mon;
            if (league) mon = gSaveBlock3Ptr->leagueTeam[i];
            else RecordMon(&mon, &gParties[B_TRAINER_PLAYER][i]);
            if (mon.species) StringAppend(gStringVar4, gSpeciesInfo[mon.species].speciesName);
            else StringAppend(gStringVar4, COMPOUND_STRING("--"));
            if (i % 3 != 2) StringAppend(gStringVar4, COMPOUND_STRING("\n"));
        }
        break;
    default:
        AddNumber(COMPOUND_STRING("Seed: "), league ? gSaveBlock3Ptr->leagueSeed : gSaveBlock3Ptr->worldSeed, TRUE);
        StringAppend(gStringVar4, (league ? gSaveBlock3Ptr->leagueDifficulty : gSaveBlock3Ptr->runDifficulty) == RUN_DIFFICULTY_HARD
            ? COMPOUND_STRING("AI: HARD\n") : (league ? gSaveBlock3Ptr->leagueDifficulty : gSaveBlock3Ptr->runDifficulty) == RUN_DIFFICULTY_EASY ? COMPOUND_STRING("AI: BASIC\n") : COMPOUND_STRING("AI: NORMAL\n"));
        StringAppend(gStringVar4, (league ? gSaveBlock3Ptr->leagueNuzlocke : IsNuzlockeRun()) ? COMPOUND_STRING("Nuzlocke: ON\n") : COMPOUND_STRING("Nuzlocke: OFF\n"));
        StringAppend(gStringVar4, (league ? gSaveBlock3Ptr->leagueEzCatch : VarGet(VAR_CHAOS_EZ_CATCH)) ? COMPOUND_STRING("EZ Catch: ON") : COMPOUND_STRING("EZ Catch: OFF"));
        break;
    }
}

static void DrawRecordsScreen(u8 windowId)
{
    static const u8 colors[] = {TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};
    SetStandardWindowBorderStyle(windowId, FALSE);
    AddTextPrinterParameterized3(windowId, FONT_NORMAL, 8, 6, colors, TEXT_SKIP_DRAW,
        gSpecialVar_0x8005 ? COMPOUND_STRING("COMPLETED LEAGUE RECORD") : COMPOUND_STRING("YOUR RUN RECORDS"));
    ChaosBuildRecordsPage();
    AddTextPrinterParameterized3(windowId, FONT_NORMAL, 8, 34, colors, TEXT_SKIP_DRAW, gStringVar4);
    AddTextPrinterParameterized3(windowId, FONT_SMALL, 8, 123, colors, TEXT_SKIP_DRAW,
        COMPOUND_STRING("LEFT/RIGHT: Pages   B: Return"));
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void Task_RunRecords(u8 taskId)
{
    if (gTasks[taskId].data[1]) {gTasks[taskId].data[1]--; return;}
    if (JOY_NEW(B_BUTTON))
    {
        ClearToTransparentAndRemoveWindow(gTasks[taskId].data[0]);
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
    else if (JOY_NEW(A_BUTTON | DPAD_RIGHT | DPAD_DOWN))
    {
        gSpecialVar_0x8004 = (gSpecialVar_0x8004 + 1) % 10;
        DrawRecordsScreen(gTasks[taskId].data[0]);
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_UP))
    {
        gSpecialVar_0x8004 = (gSpecialVar_0x8004 + 9) % 10;
        DrawRecordsScreen(gTasks[taskId].data[0]);
    }
}

void ChaosOpenRunRecords(void)
{
    HideFieldMessageBox();
    gSpecialVar_0x8004 = 0;
    u8 taskId = CreateTask(Task_RunRecords, 80);
    gTasks[taskId].data[0] = CreateWindowFromRect(0, 0, 28, 18);
    gTasks[taskId].data[1] = 8;
    DrawRecordsScreen(gTasks[taskId].data[0]);
}

void ChaosBuildLeagueBanner(void)
{
    StringCopy(gStringVar4, COMPOUND_STRING("LEAGUE RUN COMPLETE!\n"));
    AddNumber(COMPOUND_STRING("Wins: "), gSaveBlock3Ptr->leagueCounters[0], TRUE);
    AddNumber(COMPOUND_STRING("Seed: "), gSaveBlock3Ptr->leagueSeed, FALSE);
}

// Superseded by the three automatic V2 milestones. Keep the native symbol
// for old script references; nurses no longer award packages.
void ChaosClaimCarePackage(void) { gSpecialVar_Result = 0; }

// Reward claims remain retryable after a win when the Items pocket is full.
void ChaosClaimGymStone(void)
{
    static const u16 stones[] = {ITEM_STEELIXITE, ITEM_GYARADOSITE, ITEM_MANECTITE,
        ITEM_BEEDRILLITE, ITEM_ALAKAZITE, ITEM_GARCHOMPITE, ITEM_PYROARITE};
    u32 index = gSpecialVar_0x8004;
    ChaosEnsureRunRecords();
    gSpecialVar_Result = 0;
    if (index >= ARRAY_COUNT(stones)) return;
    bool32 eligible = index < 3 ? (VarGet(VAR_CHAOS_REMATCHES) & (1 << index))
        : FlagGet(index == 3 ? FLAG_BADGE05_GET : index == 4 ? FLAG_BADGE06_GET : index == 5 ? FLAG_BADGE08_GET : FLAG_BADGE07_GET);
    if (!eligible || gSaveBlock3Ptr->runCounters[15] & (1 << (index + 8))) return;
    if (!AddBagItem(stones[index], 1)) {gSpecialVar_Result = 2; return;}
    gSaveBlock3Ptr->runCounters[15] |= 1 << (index + 8);
    CopyItemName(stones[index], gStringVar1);
    gSpecialVar_Result = 1;
}
