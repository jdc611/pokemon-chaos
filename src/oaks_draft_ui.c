#include "global.h"
#include "custom_build.h"
#include "oaks_draft.h"
#include "main.h"
#include "malloc.h"
#include "bg.h"
#include "window.h"
#include "gpu_regs.h"
#include "palette.h"
#include "sprite.h"
#include "menu.h"
#include "menu_helpers.h"
#include "text_window.h"
#include "task.h"
#include "string_util.h"
#include "pokemon_summary_screen.h"
#include "overworld.h"
#include "script.h"
#include "event_data.h"
#include "data.h"
#include "move.h"
#include "battle_main.h"
#include "battle_util.h"
#include "constants/rgb.h"
#include "constants/pokemon.h"
#include "constants/abilities.h"

// State lives on the heap: Summary may reset tasks/BGs but never owns these mons.
struct DraftScreen {
    u16 tilemap[1024];
    struct Pokemon candidates[6];
    u8 mode, cursor, confirmation, page;
    u16 giftSpecies, giftItem;
};
static EWRAM_DATA struct DraftScreen *sDraftScreen;
static const u8 sColors[] = {TEXT_COLOR_WHITE,TEXT_COLOR_DARK_GRAY,TEXT_COLOR_LIGHT_GRAY};
static void InitDraftScreen(void);
static void FailedOpen(u8 task) { gSpecialVar_Result=MON_CANT_GIVE;ScriptContext_Enable();DestroyTask(task); }
static void QueueFailure(void) { CreateTask(FailedOpen,0); }
static void Print(const u8 *text,u32 x,u32 y) { AddTextPrinterParameterized4(0,FONT_SMALL,x,y,0,0,sColors,TEXT_SKIP_DRAW,text); }
static void Number(u32 value,u32 x,u32 y) { u8 text[12];ConvertIntToDecimalStringN(text,value,STR_CONV_MODE_LEFT_ALIGN,4);Print(text,x,y); }
static void Main(void) { RunTasks();AnimateSprites();BuildOamBuffer();UpdatePaletteFade(); }
static void VBlank(void) { LoadOam();ProcessSpriteCopyRequests();TransferPlttBuffer(); }
static void Close(void)
{
    FreeAllWindowBuffers();UnsetBgTilemapBuffer(0);Free(sDraftScreen);sDraftScreen=NULL;
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}
static void ReturnFromSummary(void)
{
    sDraftScreen->cursor = min(gLastViewedMonIndex,sDraftScreen->mode == 0 ? 2 : 5);
    SetMainCallback2(InitDraftScreen);
}
static void PrintMon(struct Pokemon *mon,u32 y,bool32 selected)
{
    u8 name[POKEMON_NAME_LENGTH+1];GetMonData(mon,MON_DATA_NICKNAME,name);
    Print(selected?COMPOUND_STRING("> "):COMPOUND_STRING("  "),1,y);Print(name,13,y);
    Print(GetSpeciesName(GetMonData(mon,MON_DATA_SPECIES)),102,y);
}
static void DrawAnalysis(void)
{
    u32 physical=0,special=0,balanced=0,evolves=0,coverage=0;
    u8 weaknesses[NUMBER_OF_MON_TYPES]={0};
    for(u32 i=0;i<6;i++)
    {
        struct Pokemon *mon = &sDraftScreen->candidates[i];
        if(!GetMonData(mon,MON_DATA_SPECIES))continue;
        u32 atk=GetMonData(mon,MON_DATA_ATK),spa=GetMonData(mon,MON_DATA_SPATK);
        if(atk*5>spa*6)physical++;else if(spa*5>atk*6)special++;else balanced++;
        const struct Evolution *evo=gSpeciesInfo[GetMonData(mon,MON_DATA_SPECIES)].evolutions;
        if(evo && evo[0].method!=EVOLUTIONS_END)evolves++;
        for(u32 j=0;j<4;j++){enum Move move=GetMonData(mon,MON_DATA_MOVE1+j);if(move && GetMovePower(move))coverage|=1u<<GetMoveType(move);}
        for(u32 type=0;type<NUMBER_OF_MON_TYPES;type++)if(GetOverworldTypeEffectiveness(mon,type)>UQ_4_12(1.0))weaknesses[type]++;
    }
    Print(COMPOUND_STRING("PHYSICAL / SPECIAL / BALANCED"),3,22);
    Number(physical,20,35);Number(special,90,35);Number(balanced,174,35);
    Print(COMPOUND_STRING("ATTACK TYPES IN CURRENT MOVES"),3,49);
    u32 col=0;
    for(u32 type=0;type<NUMBER_OF_MON_TYPES;type++)if(coverage&(1u<<type)){Print(gTypesInfo[type].name,3+(col%3)*72,62+(col/3)*12);col++;if(col==6)break;}
    Print(COMPOUND_STRING("LARGEST SHARED WEAKNESSES"),3,88);
    for(u32 n=0;n<2;n++)
    {
        u32 best=0;for(u32 type=1;type<NUMBER_OF_MON_TYPES;type++)if(weaknesses[type]>weaknesses[best])best=type;
        if(!weaknesses[best])break;Print(gTypesInfo[best].name,3+n*106,101);Number(weaknesses[best],75+n*106,101);weaknesses[best]=0;
    }
    Print(COMPOUND_STRING("EVOLUTION PATHS REMAINING:"),3,114);Number(evolves,185,114);
    Print(physical>special+2?COMPOUND_STRING("Consider special attack coverage."):special>physical+2?COMPOUND_STRING("Consider physical attack coverage."):COMPOUND_STRING("A mixed team: develop your coverage."),3,127);
}
static void Draw(void)
{
    FillWindowPixelBuffer(0,PIXEL_FILL(TEXT_COLOR_WHITE));
    if(sDraftScreen->confirmation)
    {
        Print(sDraftScreen->mode==0?COMPOUND_STRING("Give this companion a new home?"):sDraftScreen->mode==2?COMPOUND_STRING("Trade this companion permanently?"):COMPOUND_STRING("Spend 2000 Coins on one reroll?"),3,8);
        PrintMon(&sDraftScreen->candidates[sDraftScreen->cursor],30,FALSE);
        Print(sDraftScreen->mode==0?COMPOUND_STRING("Your choice cannot be changed."):sDraftScreen->mode==2?COMPOUND_STRING("Your old friend cannot be reclaimed.\nThe new friend keeps this level."):COMPOUND_STRING("The new ability is hidden.\nOne purchase per Pokemon."),3,53);
        if(sDraftScreen->mode==2) {
            u8 name[16],note[160];OaksDraftPreviewGift(sDraftScreen->giftSpecies,name,note);
            Print(name,3,82);Print(COMPOUND_STRING("LEVEL"),96,82);
            Number(GetMonData(&sDraftScreen->candidates[sDraftScreen->cursor],MON_DATA_LEVEL),138,82);
            Print(note,3,96);
        }
        Print(COMPOUND_STRING("A: CONFIRM       B: GO BACK"),3,sDraftScreen->mode==2?128:109);
    }
    else if(sDraftScreen->mode==0)
    {
        Print(COMPOUND_STRING("OAK'S DRAFT - CRATE"),3,1);Number(gSaveBlock3Ptr->draft.round+1,155,1);
        for(u32 i=0;i<3;i++)PrintMon(&sDraftScreen->candidates[i],18+i*15,sDraftScreen->cursor==i);
        struct Pokemon *mon=&sDraftScreen->candidates[sDraftScreen->cursor];
        enum Species species=GetMonData(mon,MON_DATA_SPECIES);
        Print(gTypesInfo[GetSpeciesType(species,0)].name,3,66);Print(gTypesInfo[GetSpeciesType(species,1)].name,78,66);
        Print(gAbilitiesInfo[GetMonAbility(mon)].name,3,80);
        u8 note[160];OaksDraftFormatBackground(mon,note);Print(note,3,95);
        Print(COMPOUND_STRING("A: SUMMARY  SELECT: PICK  B: EXIT"),3,128);
    }
    else if(sDraftScreen->mode==1)
    {
        Print(sDraftScreen->page>=2?COMPOUND_STRING("CURRENT TEAM REPORT"):COMPOUND_STRING("ORIGINAL DRAFT REPORT"),3,1);
        if(sDraftScreen->page&1)DrawAnalysis();
        else
        {
            for(u32 i=0;i<6;i++)if(GetMonData(&sDraftScreen->candidates[i],MON_DATA_SPECIES))PrintMon(&sDraftScreen->candidates[i],18+i*16,sDraftScreen->cursor==i);
            Print(COMPOUND_STRING("A: SUMMARY   L/R: REPORT PAGES"),3,119);
        }
        if(!(sDraftScreen->page&1))Print(COMPOUND_STRING("B: CLOSE"),3,132);
    }
    else
    {
        Print(sDraftScreen->mode==2?COMPOUND_STRING("ONE FRIEND FOR ONE FRIEND"):COMPOUND_STRING("MYSTERY ABILITY - 2000 COINS"),3,1);
        if(sDraftScreen->mode==2) {
            u8 name[16],note[160];OaksDraftPreviewGift(sDraftScreen->giftSpecies,name,note);
            Print(GetSpeciesName(sDraftScreen->giftSpecies),3,16);Print(name,112,16);
        }
        for(u32 i=0;i<gPartiesCount[B_TRAINER_PLAYER];i++) {
            PrintMon(&sDraftScreen->candidates[i],33+i*14,sDraftScreen->cursor==i);
            if(sDraftScreen->mode==3 && GetMonData(&sDraftScreen->candidates[i],MON_DATA_DRAFT_REROLLED))Print(COMPOUND_STRING("USED"),191,33+i*14);
        }
        Print(COMPOUND_STRING("SELECT: CHOOSE    B: CANCEL"),3,128);
    }
    PutWindowTilemap(0);CopyWindowToVram(0,COPYWIN_FULL);
}
static void RefreshReport(void)
{
    memset(sDraftScreen->candidates,0,sizeof(sDraftScreen->candidates));
    if(sDraftScreen->page>=2)memcpy(sDraftScreen->candidates,gParties[B_TRAINER_PLAYER],sizeof(sDraftScreen->candidates));
    else for(u32 i=0;i<gSaveBlock3Ptr->draft.round;i++)OaksDraftCreateCandidate(&sDraftScreen->candidates[i],i*3+gSaveBlock3Ptr->draft.picks[i],5);
}
static void Task_Input(u8 task)
{
    if(gPaletteFade.active)return;
    if(sDraftScreen->confirmation)
    {
        if(JOY_NEW(B_BUTTON)){sDraftScreen->confirmation=FALSE;Draw();return;}
        if(!JOY_NEW(A_BUTTON))return;
        bool32 ok=FALSE;
        if(sDraftScreen->mode==0)ok=OaksDraftCommit(sDraftScreen->cursor);
        else if(sDraftScreen->mode==2)ok=OaksDraftGiftTrade(sDraftScreen->cursor,sDraftScreen->giftSpecies,sDraftScreen->giftItem);
        else ok=OaksDraftRerollAbility(sDraftScreen->cursor);
        gSpecialVar_Result=ok?MON_GIVEN_TO_PARTY:MON_CANT_GIVE;
        DestroyTask(task);Close();return;
    }
    if(JOY_NEW(B_BUTTON)){gSpecialVar_Result=MON_CANT_GIVE;DestroyTask(task);Close();return;}
    u32 count=sDraftScreen->mode==0?3:sDraftScreen->mode==1?6:gPartiesCount[B_TRAINER_PLAYER];
    if(JOY_NEW(DPAD_UP))sDraftScreen->cursor=(sDraftScreen->cursor+count-1)%count;
    else if(JOY_NEW(DPAD_DOWN))sDraftScreen->cursor=(sDraftScreen->cursor+1)%count;
    else if(sDraftScreen->mode==1 && JOY_NEW(L_BUTTON|R_BUTTON)){sDraftScreen->page=(sDraftScreen->page+(JOY_NEW(L_BUTTON)?3:1))%4;RefreshReport();}
    else if(JOY_NEW(A_BUTTON) && sDraftScreen->mode<=1 && GetMonData(&sDraftScreen->candidates[sDraftScreen->cursor],MON_DATA_SPECIES))
    {
        FreeAllWindowBuffers();UnsetBgTilemapBuffer(0);DestroyTask(task);
        ShowPokemonSummaryScreen(SUMMARY_MODE_LOCK_MOVES,sDraftScreen->candidates,sDraftScreen->cursor,count-1,ReturnFromSummary);return;
    }
    else if(JOY_NEW(SELECT_BUTTON) && sDraftScreen->mode!=1)sDraftScreen->confirmation=TRUE;
    else return;
    Draw();
}
static void InitDraftScreen(void)
{
    static const struct BgTemplate bgs[]={{.bg=0,.charBaseIndex=0,.mapBaseIndex=31,.priority=1}};
    static const struct WindowTemplate windows[]={{.bg=0,.tilemapLeft=1,.tilemapTop=1,.width=28,.height=18,.paletteNum=15,.baseBlock=1},DUMMY_WIN_TEMPLATE};
    SetVBlankCallback(NULL);ResetVramOamAndBgCntRegs();ResetTasks();ResetSpriteData();FreeAllSpritePalettes();
    ResetBgsAndClearDma3BusyFlags(0);InitBgsFromTemplates(0,bgs,1);SetBgTilemapBuffer(0,sDraftScreen->tilemap);
    InitWindows(windows);DeactivateAllTextPrinters();LoadPalette(GetTextWindowPalette(0),BG_PLTT_ID(15),PLTT_SIZE_4BPP);
    Draw();SetGpuReg(REG_OFFSET_DISPCNT,DISPCNT_OBJ_ON|DISPCNT_OBJ_1D_MAP);ShowBg(0);ResetPaletteFade();
    BeginNormalPaletteFade(PALETTES_ALL,0,16,0,RGB_BLACK);SetVBlankCallback(VBlank);CreateTask(Task_Input,0);SetMainCallback2(Main);
}
static bool32 Open(u32 mode)
{
    gSpecialVar_Result=MON_CANT_GIVE;
    if(!IsOaksDraft() || sDraftScreen){QueueFailure();return FALSE;}
    sDraftScreen=AllocZeroed(sizeof(*sDraftScreen));if(!sDraftScreen){QueueFailure();return FALSE;}
    sDraftScreen->mode=mode;
    ScriptContext_Stop();SetMainCallback2(InitDraftScreen);return TRUE;
}
void OaksDraftOpenCrate(void)
{
    if(IsCustomBuild()){CustomBuildOpenCrate();return;}
    if(!IsOaksDraft() || gSpecialVar_0x8004 != gSaveBlock3Ptr->draft.round || gSaveBlock3Ptr->draft.round>=6){QueueFailure();return;}
    if(Open(0))for(u32 i=0;i<3;i++)OaksDraftCreateCandidate(&sDraftScreen->candidates[i],gSaveBlock3Ptr->draft.round*3+i,5);
}
void OaksDraftOpenReport(void)
{
    if(IsCustomBuild()){CustomBuildOpenReport();return;}
    if(Open(1))RefreshReport();
}
void OaksDraftOpenGift(void)
{
    if(IsCustomBuild()){CustomBuildOpenGift();return;}
    if(!gPartiesCount[B_TRAINER_PLAYER]){QueueFailure();return;}
    if(Open(2)){sDraftScreen->giftSpecies=gSpecialVar_0x8004;sDraftScreen->giftItem=gSpecialVar_0x8005;memcpy(sDraftScreen->candidates,gParties[B_TRAINER_PLAYER],sizeof(sDraftScreen->candidates));}
}
void OaksDraftOpenAbilityReroll(void)
{
    if(!gPartiesCount[B_TRAINER_PLAYER]){QueueFailure();return;}
    if(Open(3))memcpy(sDraftScreen->candidates,gParties[B_TRAINER_PLAYER],sizeof(sDraftScreen->candidates));
}
