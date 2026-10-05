#include "global.h"
#include "bg.h"
#include "coins.h"
#include "event_data.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "constants/field_weather.h"
#include "gpu_regs.h"
#include "item.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "overworld.h"
#include "palette.h"
#include "random.h"
#include "rtc.h"
#include "script.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "chaos_checkers.h"
#include "constants/coins.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/rgb.h"

#define ARCADE_SAVE_MAGIC 0x43415231
void ChaosArcadeEnsureSave(void)
{
    if (gSaveBlock3Ptr->arcadeMagic != ARCADE_SAVE_MAGIC)
    {
        gSaveBlock3Ptr->arcadeMagic = ARCADE_SAVE_MAGIC;
        gSaveBlock3Ptr->arcadeRefillAt = 0;
        gSaveBlock3Ptr->arcadeClockSeen = 0;
        gSaveBlock3Ptr->arcadeDailyAt = 0;
        gSaveBlock3Ptr->arcadeCosmeticsOwned[0] = 0;
        gSaveBlock3Ptr->arcadeCosmeticsOwned[1] = 0;
        gSaveBlock3Ptr->arcadeDecorations = 0;
        gSaveBlock3Ptr->arcadeWallpaper = 0;
        gSaveBlock3Ptr->arcadeRiderTheme = 0;
        gSaveBlock3Ptr->arcadeRanchTheme = 0;
        gSaveBlock3Ptr->arcadeOutfit = 0;
    }
}

// Read the cartridge/emulator clock directly, never the game's Time Changer.
// Zero denotes an unavailable RTC; it grants no repeatable fallback bonus.
static u32 ArcadeRealTime(void)
{
    struct SiiRtcInfo rtc;
    RtcGetRawInfo(&rtc);
    if (RtcCheckInfo(&rtc) & RTC_ERR_FLAG_MASK)
        return 0;
    u32 month = ConvertBcdToBinary(rtc.month), day = ConvertBcdToBinary(rtc.day);
    if (month < 1 || month > 12 || day < 1 || day > 31)
        return 0;
    return 1 + RtcGetDayCount(&rtc) * 86400 + ConvertBcdToBinary(rtc.hour) * 3600
        + ConvertBcdToBinary(rtc.minute) * 60 + ConvertBcdToBinary(rtc.second);
}

// Result: 0 awarded, 1 waiting, 2 no RTC, 3 full, 4 no case/badge.
void ChaosArcadeRefill(void)
{
    ChaosArcadeEnsureSave();
    gSpecialVar_Result = 4;
    if (!FlagGet(FLAG_BADGE04_GET) || !CheckBagHasItem(ITEM_COIN_CASE, 1))
        return;
    u32 now = ArcadeRealTime();
    gSpecialVar_Result = 2;
    if (!now)
        return;
    u32 seen = gSaveBlock3Ptr->arcadeClockSeen;
    // Backwards changes and large forward jumps restart a two-hour window.
    // They never create accumulated grants or a permanent future lockout.
    if (seen && (now < seen || now - seen > 86400))
        gSaveBlock3Ptr->arcadeRefillAt = now;
    gSaveBlock3Ptr->arcadeClockSeen = now;
    u32 last = gSaveBlock3Ptr->arcadeRefillAt;
    gSpecialVar_Result = 1;
    if (last && now - last < 7200)
    {
        ConvertIntToDecimalStringN(gStringVar1, (7200 - (now - last) + 59) / 60, STR_CONV_MODE_LEFT_ALIGN, 3);
        return;
    }
    gSpecialVar_Result = 3;
    if (GetCoins() > MAX_COINS - 100)
        return;
    AddCoins(100);
    gSaveBlock3Ptr->arcadeRefillAt = now;
    gSpecialVar_Result = 0;
}

struct ArcadeCheckersUi
{
    struct ChaosCheckersBoard board;
    struct ChaosCheckersMove moves[CHAOS_CHECKERS_MAX_MOVES];
    struct ChaosCheckersMove selected;
    u32 count;
    u16 tilemap[1024];
    u16 reward;
    u8 cursor, difficulty, phase, delay;
};
static EWRAM_DATA struct ArcadeCheckersUi *sCheckers;
static const struct BgTemplate sArcadeBg[] = {{.bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31, .screenSize = 0, .paletteMode = 0, .priority = 0}};
static const struct WindowTemplate sArcadeWindows[] = {{.bg = 0, .tilemapLeft = 1, .tilemapTop = 1, .width = 28, .height = 18, .paletteNum = 15, .baseBlock = 1}, DUMMY_WIN_TEMPLATE};
static const u16 sArcadePalette[16] = {RGB(3,5,10), RGB(30,30,31), RGB(3,4,8), RGB(17,18,21), RGB(10,13,19), RGB(24,27,30), RGB(5,15,30), RGB(30,9,9), RGB(31,25,3), RGB(8,29,14)};
static const u8 sTextEasy[] = _("EASY");
static const u8 sTextNormal[] = _("NORMAL");
static const u8 sTextHard[] = _("HARD");
static const u8 *const sDifficulty[] = {sTextEasy, sTextNormal, sTextHard};

static void ArcadePrint(const u8 *text, u32 x, u32 y)
{
    static const u8 colors[] = {1, 2, 3};
    AddTextPrinterParameterized4(0, FONT_SMALL, x, y, 0, 0, colors, TEXT_SKIP_DRAW, text);
}

static void DrawPiece(u32 x, u32 y, u32 piece)
{
    static const u8 widths[] = {4, 8, 10, 10, 10, 10, 10, 10, 8, 4};
    FillWindowPixelRect(0, 3, x + 4, y + 12, 7, 1);
    for (u32 row = 0; row < 10; row++)
    {
        u32 width = widths[row];
        FillWindowPixelRect(0, 2, x + (14 - width) / 2, y + 2 + row, width, 1);
        if (row > 0 && row < 9)
            FillWindowPixelRect(0, row < 4 ? ((piece & 3) == 1 ? 6 : 7) : 1,
                x + (14 - width) / 2 + 1, y + 2 + row, width - 2, 1);
    }
    FillWindowPixelRect(0, 2, x + 2, y + 6, 10, 2);
    FillWindowPixelRect(0, 2, x + 5, y + 5, 4, 4);
    FillWindowPixelRect(0, 1, x + 6, y + 6, 2, 2);
    if (piece & CHAOS_CHECKERS_KING)
    {
        FillWindowPixelRect(0, 8, x + 3, y + 2, 8, 2);
        FillWindowPixelRect(0, 8, x + 3, y, 2, 4);
        FillWindowPixelRect(0, 8, x + 6, y, 2, 4);
        FillWindowPixelRect(0, 8, x + 9, y, 2, 4);
    }
}

static bool32 IsNextLanding(u32 square)
{
    u32 length = sCheckers->selected.length;
    if (!length) return FALSE;
    for (u32 i = 0; i < sCheckers->count; i++)
    {
        const struct ChaosCheckersMove *move = &sCheckers->moves[i];
        if (length >= move->length || move->path[length] != square) continue;
        u32 j = 0;
        while (j < length && move->path[j] == sCheckers->selected.path[j]) j++;
        if (j == length) return TRUE;
    }
    return FALSE;
}

static void DrawCheckers(void)
{
    FillWindowPixelBuffer(0, PIXEL_FILL(1));
    FillWindowPixelRect(0, 4, 0, 0, 224, 18);
    static const u8 headerColors[] = {4, 1, 2};
    AddTextPrinterParameterized4(0, FONT_SMALL, 5, 1, 0, 0, headerColors, TEXT_SKIP_DRAW, COMPOUND_STRING("CHAOS CHECKERS"));
    u8 coins[16];
    StringCopy(coins, COMPOUND_STRING("COINS "));
    ConvertIntToDecimalStringN(coins + 6, GetCoins(), STR_CONV_MODE_RIGHT_ALIGN, 4);
    AddTextPrinterParameterized4(0, FONT_SMALL, 160, 1, 0, 0, headerColors, TEXT_SKIP_DRAW, coins);
    if (sCheckers->phase == 0)
    {
        ArcadePrint(sDifficulty[sCheckers->difficulty], 4, 21);
        ArcadePrint(COMPOUND_STRING("LEFT / RIGHT: Difficulty   A: Play\nB: Leave\nCapture when possible; finish the chain.\nGold kings move and capture both ways.\nWins: 450 / 900 / 1500 COINS\nDraws: 50 / 100 / 150 COINS"), 4, 38);
    }
    else if (sCheckers->phase == 6)
        ArcadePrint(COMPOUND_STRING("Forfeit this game?\nA: Forfeit   B: Keep playing\n\nForfeit earns no COINS."), 4, 40);
    else if (sCheckers->phase >= 3)
    {
        ArcadePrint(sCheckers->phase == 3 ? COMPOUND_STRING("You won!") : sCheckers->phase == 4 ? COMPOUND_STRING("The arcade won.") : sCheckers->phase == 7 ? COMPOUND_STRING("Not enough memory to continue.") : COMPOUND_STRING("Draw game."), 4, 38);
        ConvertIntToDecimalStringN(gStringVar1, sCheckers->reward, STR_CONV_MODE_LEFT_ALIGN, 4);
        StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("Awarded {STR_VAR_1} COINS.\nA: Play again   B: Leave"));
        ArcadePrint(gStringVar4, 4, 62);
    }
    else
    {
        struct ChaosCheckersBoard preview = sCheckers->board;
        if (sCheckers->selected.length > 1)
        {
            ChaosCheckersApply(&preview, &sCheckers->selected);
            preview.turn = sCheckers->board.turn;
        }
        FillWindowPixelRect(0, 2, 2, 22, 116, 116);
        for (u32 row = 0; row < 8; row++)
            for (u32 col = 0; col < 8; col++)
            {
                s32 sq = ChaosCheckersSquare(row, col);
                u32 x = 4 + col * 14, y = 24 + row * 14;
                FillWindowPixelRect(0, sq < 0 ? 5 : 4, x, y, 14, 14);
                if (sq >= 0 && preview.squares[sq]) DrawPiece(x, y, preview.squares[sq]);
                u32 color = sq >= 0 && sq == sCheckers->cursor ? 8 : sq >= 0 && IsNextLanding(sq) ? 9 : 0;
                if (color)
                {
                    FillWindowPixelRect(0, color, x, y, 14, 1);
                    FillWindowPixelRect(0, color, x, y + 13, 14, 1);
                    FillWindowPixelRect(0, color, x, y, 1, 14);
                    FillWindowPixelRect(0, color, x + 13, y, 1, 14);
                }
            }
        ArcadePrint(sDifficulty[sCheckers->difficulty], 124, 23);
        ArcadePrint(COMPOUND_STRING("BLUE: YOU\nRED: ARCADE"), 124, 40);
        ArcadePrint(sCheckers->phase == 2 ? COMPOUND_STRING("Thinking...") : sCheckers->selected.length ? COMPOUND_STRING("Green: land here\nA: Move\nB: Clear choice") : COMPOUND_STRING("Choose a piece\nA: Select\nB: Forfeit"), 124, 74);
        if (sCheckers->count && sCheckers->moves[0].captures)
            ArcadePrint(COMPOUND_STRING("Must capture!"), 124, 121);
    }
    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
}

static void EndCheckers(u32 result)
{
    static const u16 wins[] = {450, 900, 1500};
    static const u16 draws[] = {50, 100, 150};
    u32 wanted = result == 3 ? wins[sCheckers->difficulty] : result == 5 ? draws[sCheckers->difficulty] : 0;
    sCheckers->reward = min(wanted, MAX_COINS - GetCoins());
    if (sCheckers->reward) AddCoins(sCheckers->reward);
    sCheckers->phase = result;
    sCheckers->selected.length = 0;
    DrawCheckers();
}

static void RefreshCheckersTurn(void)
{
    sCheckers->selected.length = 0;
    sCheckers->count = ChaosCheckersMoves(&sCheckers->board, sCheckers->moves);
    if (!sCheckers->count)
        EndCheckers(sCheckers->board.turn == CHAOS_CHECKERS_CPU ? 3 : 4);
    else if (sCheckers->board.quietTurns >= 80)
        EndCheckers(5);
    else
    {
        sCheckers->phase = sCheckers->board.turn == CHAOS_CHECKERS_CPU ? 2 : 1;
        sCheckers->delay = 30;
        DrawCheckers();
    }
}

static void LeaveCheckers(u8 taskId)
{
    FreeAllWindowBuffers();
    UnsetBgTilemapBuffer(0);
    Free(sCheckers);
    sCheckers = NULL;
    DestroyTask(taskId);
    SetMainCallback2(CB2_ReturnToFieldContinueScript);
}

static void Task_Checkers(u8 taskId)
{
    if (sCheckers->phase == 2)
    {
        if (sCheckers->delay) { sCheckers->delay--; return; }
        struct ChaosCheckersMove chosen;
        s32 result = ChaosCheckersChoose(&sCheckers->board, sCheckers->difficulty, Random(), &chosen);
        if (result < 0)
            EndCheckers(7);
        else if (result == 0)
            EndCheckers(3);
        else
        {
            ChaosCheckersApply(&sCheckers->board, &chosen);
            RefreshCheckersTurn();
        }
        return;
    }
    if (JOY_NEW(B_BUTTON))
    {
        if (sCheckers->phase == 1 && sCheckers->selected.length)
        {
            sCheckers->selected.length = 0;
            DrawCheckers();
        }
        else if (sCheckers->phase == 1)
        {
            sCheckers->phase = 6;
            DrawCheckers();
        }
        else if (sCheckers->phase == 6)
        {
            sCheckers->phase = 1;
            DrawCheckers();
        }
        else LeaveCheckers(taskId);
        return;
    }
    if (sCheckers->phase == 6)
    {
        if (JOY_NEW(A_BUTTON)) LeaveCheckers(taskId);
        return;
    }
    if (sCheckers->phase == 0)
    {
        if (JOY_NEW(DPAD_LEFT)) sCheckers->difficulty = (sCheckers->difficulty + 2) % 3;
        if (JOY_NEW(DPAD_RIGHT)) sCheckers->difficulty = (sCheckers->difficulty + 1) % 3;
        if (JOY_NEW(A_BUTTON))
        {
            ChaosCheckersInit(&sCheckers->board);
            sCheckers->cursor = 20;
            RefreshCheckersTurn();
        }
        else if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT)) DrawCheckers();
        return;
    }
    if (sCheckers->phase >= 3)
    {
        if (JOY_NEW(A_BUTTON)) { sCheckers->phase = 0; DrawCheckers(); }
        return;
    }
    if (JOY_NEW(DPAD_UP)) sCheckers->cursor = (sCheckers->cursor + 28) % 32;
    if (JOY_NEW(DPAD_DOWN)) sCheckers->cursor = (sCheckers->cursor + 4) % 32;
    if (JOY_NEW(DPAD_LEFT)) sCheckers->cursor = (sCheckers->cursor + 31) % 32;
    if (JOY_NEW(DPAD_RIGHT)) sCheckers->cursor = (sCheckers->cursor + 1) % 32;
    if (JOY_NEW(A_BUTTON))
    {
        struct ChaosCheckersMove *selection = &sCheckers->selected;
        for (u32 i = 0; i < sCheckers->count; i++)
        {
            struct ChaosCheckersMove *legal = &sCheckers->moves[i];
            if (legal->path[selection->length] != sCheckers->cursor) continue;
            u32 prefix = 0;
            while (prefix < selection->length && selection->path[prefix] == legal->path[prefix]) prefix++;
            if (prefix != selection->length) continue;
            selection->path[selection->length++] = sCheckers->cursor;
            selection->captures = legal->captures ? selection->length - 1 : 0;
            if (selection->length == legal->length)
            {
                ChaosCheckersApply(&sCheckers->board, legal);
                RefreshCheckersTurn();
            }
            break;
        }
    }
    if (JOY_NEW(DPAD_ANY | A_BUTTON)) DrawCheckers();
}

static void ArcadeMain(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}
static void ArcadeVBlank(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}
static void InitCheckers(void)
{
    SetVBlankCallback(NULL);
    ResetVramOamAndBgCntRegs();
    ResetTasks();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sArcadeBg, ARRAY_COUNT(sArcadeBg));
    SetBgTilemapBuffer(0, sCheckers->tilemap);
    InitWindows(sArcadeWindows);
    DeactivateAllTextPrinters();
    LoadPalette(sArcadePalette, BG_PLTT_ID(15), sizeof(sArcadePalette));
    DrawCheckers();
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(0);
    ResetPaletteFade();
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    SetVBlankCallback(ArcadeVBlank);
    CreateTask(Task_Checkers, 0);
    SetMainCallback2(ArcadeMain);
}
static void Task_LaunchCheckers(u8 taskId)
{
    if (gPaletteFade.active) return;
    DestroyTask(taskId);
    SetMainCallback2(InitCheckers);
}
static void Task_CheckersLaunchFailed(u8 taskId)
{
    ScriptContext_Enable();
    DestroyTask(taskId);
}
void ChaosArcadeCheckers(void)
{
    gSpecialVar_Result = FALSE;
    if (!FlagGet(FLAG_BADGE04_GET) || !CheckBagHasItem(ITEM_COIN_CASE, 1))
    {
        CreateTask(Task_CheckersLaunchFailed, 0);
        return;
    }
    sCheckers = AllocZeroed(sizeof(*sCheckers));
    if (!sCheckers)
    {
        CreateTask(Task_CheckersLaunchFailed, 0);
        return;
    }
    gSpecialVar_Result = TRUE;
    FadeScreen(FADE_TO_BLACK, 0);
    CreateTask(Task_LaunchCheckers, 0);
}
