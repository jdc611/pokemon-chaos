#include "global.h"
#include "ironmon.h"
#include "main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "overworld.h"
#include "bg.h"
#include "gpu_regs.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "string_util.h"
#include "event_data.h"
#include "save.h"
#include "constants/rgb.h"
#include "constants/flags.h"

static const struct BgTemplate sBg[] = {
    {.bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31, .screenSize = 0, .priority = 0}
};
static const struct WindowTemplate sWindows[] = {
    {.bg = 0, .tilemapLeft = 1, .tilemapTop = 1, .width = 28, .height = 18, .paletteNum = 15, .baseBlock = 1},
    DUMMY_WIN_TEMPLATE
};
static const u16 sColors[16] = {RGB(29,30,31), RGB(3,5,9), RGB(20,23,26), RGB(27,6,7), RGB(9,15,23), RGB(31,31,31),RGB(29,30,31)};
static EWRAM_DATA u16 sTilemap[32 * 32];
static EWRAM_DATA bool8 sKeysReleased;
static const u8 sInk[] = {6,1,2}, sWhite[] = {4,5,1}, sTitle[] = {3,5,1};

static void Print(u32 x, u32 y, const u8 *text, const u8 *colors)
{
    AddTextPrinterParameterized3(0, FONT_SMALL, x, y, colors, TEXT_SKIP_DRAW, text);
}
static void Number(u32 x,u32 y,u32 value)
{
    u8 buffer[16];
    ConvertIntToDecimalStringN(buffer,value,STR_CONV_MODE_LEFT_ALIGN,10);
    Print(x,y,buffer,sInk);
}
static void VBlank(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}
static void RunOverInput(void)
{
    UpdatePaletteFade();
    if (!gMain.heldKeys) sKeysReleased = TRUE;
    if (sKeysReleased && JOY_NEW(A_BUTTON | B_BUTTON)) DoSoftReset();
}
void CB2_IronmonRunOver(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    SetMainCallback1(NULL);
    ResetVramOamAndBgCntRegs();
    ResetPaletteFade();
    DeactivateAllTextPrinters();
    ResetTasks();
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeAllWindowBuffers();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0,sBg,ARRAY_COUNT(sBg));
    ResetAllBgsCoordinates();
    memset(sTilemap,0,sizeof(sTilemap));
    SetBgTilemapBuffer(0,sTilemap);
    InitWindows(sWindows);
    SetDefaultFontsPointer();
    LoadPalette(sColors,15*16,sizeof(sColors));
    FillWindowPixelBuffer(0,PIXEL_FILL(6));
    FillWindowPixelRect(0,PIXEL_FILL(3),0,0,224,23);
    FillWindowPixelRect(0,PIXEL_FILL(4),0,25,224,17);
    Print(74,4,COMPOUND_STRING("RUN OVER"),sTitle);
    Print(8,27,IsIronmonHardcore()?COMPOUND_STRING("IRONMON HARDCORE"):COMPOUND_STRING("IRONMON NORMAL"),sWhite);
    Print(8,48,COMPOUND_STRING("SEED"),sInk);Number(76,48,gSaveBlock3Ptr->ironmon.seed);
    u8 nickname[POKEMON_NAME_LENGTH+1];
    GetMonData(&gParties[B_TRAINER_PLAYER][0],MON_DATA_NICKNAME,nickname);
    Print(8,64,nickname,sInk);
    Print(118,64,COMPOUND_STRING("LEVEL"),sInk);Number(169,64,GetMonData(&gParties[B_TRAINER_PLAYER][0],MON_DATA_LEVEL));
    u32 badges=0;
    for(u32 i=0;i<8;i++) if(FlagGet(FLAG_BADGE01_GET+i))badges++;
    Print(8,80,COMPOUND_STRING("BADGES"),sInk);Number(76,80,badges);
    Print(112,80,COMPOUND_STRING("RETIRED"),sInk);Number(175,80,gSaveBlock3Ptr->ironmon.retiredCount);
    Print(8,96,COMPOUND_STRING("TRAINERS"),sInk);Number(76,96,gSaveBlock3Ptr->ironmon.trainersDefeated);
    Print(112,96,COMPOUND_STRING("TIME"),sInk);Number(153,96,gSaveBlock2Ptr->playTimeHours);
    Print(171,96,COMPOUND_STRING(":"),sInk);Number(180,96,gSaveBlock2Ptr->playTimeMinutes);
    // Save the terminal state without deleting the run or its Pokémon.
    u32 status=TrySavingData(SAVE_NORMAL);
    Print(8,115,status==SAVE_STATUS_OK?COMPOUND_STRING("Ended run saved."):COMPOUND_STRING("Save failed. Ended state is in memory."),sInk);
    Print(40,132,COMPOUND_STRING("A / B: RETURN TO TITLE"),sInk);
    PutWindowTilemap(0);
    CopyWindowToVram(0,COPYWIN_FULL);
    CopyBgTilemapBufferToVram(0);
    SetGpuReg(REG_OFFSET_DISPCNT,DISPCNT_MODE_0);
    ShowBg(0);
    SetGpuReg(REG_OFFSET_BLDCNT,0);
    SetGpuReg(REG_OFFSET_BLDY,0);
    sKeysReleased=FALSE;
    SetVBlankCallback(VBlank);
    SetMainCallback2(RunOverInput);
}
