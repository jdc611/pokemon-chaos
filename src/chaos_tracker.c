#include "global.h"
#include "chaos_tracker.h"
#include "chaos_v2.h"
#include "ironmon.h"
#include "battle.h"
#include "battle_main.h"
#include "bg.h"
#include "gpu_regs.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "move.h"
#include "palette.h"
#include "pokemon.h"
#include "string_util.h"
#include "text.h"
#include "window.h"
#include "constants/rgb.h"
#include "event_data.h"
#include "constants/flags.h"

// Borrow only one unused window slot for CPU text rendering. No BG manager,
// tile allocation, existing window, task, sprite or battle resource is reset.
#define TRACKER_TILES (1 + 28 * 18)
#define TRACKER_PIXELS (28 * 2 * TILE_SIZE_4BPP)
struct TrackerOverlay
{
    u16 charBackup[TRACKER_TILES * TILE_SIZE_4BPP / 2];
    u16 mapBackup[BG_SCREEN_SIZE / 2];
    ALIGNED(4) u8 pixels[TRACKER_PIXELS];
    u16 unfaded[16], faded[16];
    u16 registers[7];
    struct Window unusedWindow;
    MainCallback callback1, callback2;
    IntrCallback vblank, hblank;
    u8 window, page, party, mainState;
    bool8 released, closing, disabledPrinters;
};
static EWRAM_DATA struct TrackerOverlay *sTracker;
static const u8 sRegisterOffsets[] = {
    REG_OFFSET_DISPCNT, REG_OFFSET_BG0CNT, REG_OFFSET_BG0HOFS,
    REG_OFFSET_BG0VOFS, REG_OFFSET_BLDCNT, REG_OFFSET_BLDALPHA, REG_OFFSET_BLDY
};
static const u16 sPalette[16] = {
    RGB(29,30,31), RGB(29,30,31), RGB(3,5,9), RGB(21,23,25),
    RGB(27,6,7), RGB(8,16,26), RGB(21,25,29), RGB(31,31,31),
    RGB(9,14,20), RGB(17,21,26)
};
static const u8 sInk[] = {1,2,3}, sWhite[] = {4,7,4};
static const u8 sBand[] = {6,2,9};
static const u8 sRed[] = {1,4,3}, sBlue[] = {1,5,3};

static void Print(u32 x, u32 y, const u8 *text, const u8 *colors)
{
    // Render a two-tile-high strip; no second full-screen buffer fits in battle.
    void *dest=(void *)(BG_CHAR_ADDR(2)+TILE_SIZE_4BPP*(1+(y/8)*28));
    CpuCopy16(dest,sTracker->pixels,TRACKER_PIXELS);
    AddTextPrinterParameterized4(sTracker->window,FONT_SMALL,x,0,0,0,colors,TEXT_SKIP_DRAW,text);
    CpuCopy16(sTracker->pixels,dest,TRACKER_PIXELS);
}
static void Number(u32 x,u32 y,u32 number,const u8 *colors)
{
    u8 text[12];
    ConvertIntToDecimalStringN(text,number,STR_CONV_MODE_LEFT_ALIGN,6);
    Print(x,y,text,colors);
}
static struct Pokemon *SelectedMon(void)
{
    return &gParties[B_TRAINER_PLAYER][sTracker->party];
}
static void Stat(u32 x,u32 y,const u8 *name,u32 field,enum Stat stat)
{
    struct Pokemon *mon=SelectedMon();
    u32 nature=GetNature(mon);
    const u8 *colors=sInk;
    if(gNaturesInfo[nature].statUp!=gNaturesInfo[nature].statDown)
    {
        if(gNaturesInfo[nature].statUp==stat)colors=sRed;
        else if(gNaturesInfo[nature].statDown==stat)colors=sBlue;
    }
    Print(x,y,name,colors);Number(x+63,y,GetMonData(mon,field),colors);
}
static void Draw(void)
{
    struct Pokemon *mon=SelectedMon();
    enum Species species=GetMonData(mon,MON_DATA_SPECIES);
    u8 text[128];
    CpuFill16(0x1111,(void *)BG_CHAR_ADDR(2),TRACKER_TILES*TILE_SIZE_4BPP);
    CpuFill16(0x4444,(void *)(BG_CHAR_ADDR(2)+32),28*2*32);
    Print(6,3,COMPOUND_STRING("CHAOS TRACKER"),sWhite);
    Number(180,3,sTracker->page+1,sWhite);Print(192,3,COMPOUND_STRING("/ 3"),sWhite);
    CpuFill16(0x6666,(void *)(BG_CHAR_ADDR(2)+32*(1+3*28)),28*2*32);
    GetMonData(mon,MON_DATA_NICKNAME,text);Print(6,24,text,sBand);
    Print(143,24,COMPOUND_STRING("Lv."),sBand);Number(164,24,GetMonData(mon,MON_DATA_LEVEL),sBand);
    if(sTracker->page==0)
    {
        Print(6,42,gNaturesInfo[GetNature(mon)].name,sInk);
        
        u32 rating=ChaosCurrentStageRating(mon);
        Print(117,42,COMPOUND_STRING("CHAOS"),sInk);
        u8 *end=ConvertIntToDecimalStringN(text,rating/10,STR_CONV_MODE_LEFT_ALIGN,2);
        *end++=CHAR_PERIOD;end=ConvertIntToDecimalStringN(end,rating%10,STR_CONV_MODE_LEFT_ALIGN,1);
        StringCopy(end,COMPOUND_STRING(" / 10"));Print(155,42,text,sInk);
        u32 badges=0;for(u32 i=0;i<8;i++)if(FlagGet(FLAG_BADGE01_GET+i))badges++;
        Print(117,58,COMPOUND_STRING("BADGES"),sInk);Number(171,58,badges,sInk);
        Print(6,58,COMPOUND_STRING("HP"),sInk);
        Number(35,58,GetMonData(mon,MON_DATA_HP),sInk);
        Print(63,58,COMPOUND_STRING("/"),sInk);
        Number(72,58,GetMonData(mon,MON_DATA_MAX_HP),sInk);
        Stat(6,75,COMPOUND_STRING("ATTACK"),MON_DATA_ATK,STAT_ATK);
        Stat(117,75,COMPOUND_STRING("SP. ATK"),MON_DATA_SPATK,STAT_SPATK);
        Stat(6,92,COMPOUND_STRING("DEFENSE"),MON_DATA_DEF,STAT_DEF);
        Stat(117,92,COMPOUND_STRING("SP. DEF"),MON_DATA_SPDEF,STAT_SPDEF);
        Stat(6,109,COMPOUND_STRING("SPEED"),MON_DATA_SPEED,STAT_SPEED);
        Print(117,109,COMPOUND_STRING("Red + / Blue -"),sInk);
    }
    else if(sTracker->page==1)
    {
        for(u32 i=0;i<MAX_MON_MOVES;i++)
        {
            enum Move move=GetMonData(mon,MON_DATA_MOVE1+i);
            u32 y=40+i*24;

            Print(6,y,move?GetMoveName(move):COMPOUND_STRING("--"),sInk);
            if(move)
            {
                Number(159,y,GetMonData(mon,MON_DATA_PP1+i),sInk);
                Print(178,y,COMPOUND_STRING("/"),sInk);
                Number(187,y,CalculatePPWithBonus(move,GetMonData(mon,MON_DATA_PP_BONUSES),i),sInk);
            }
        }
    }
    else
    {
        enum Ability ability=GetMonAbility(mon);
        enum Type type1=GetSpeciesType(species,0),type2=GetSpeciesType(species,1);
        if(gMain.inBattle)
            for(u32 battler=0;battler<gBattlersCount;battler++)
                if(GetBattlerSide(battler)==B_SIDE_PLAYER&&gBattlerPartyIndexes[battler]==sTracker->party)
                {
                    ability=gBattleMons[battler].ability;
                    type1=gBattleMons[battler].types[0];type2=gBattleMons[battler].types[1];
                    break;
                }
        Print(6,42,gSpeciesInfo[species].speciesName,sInk);
        Print(117,42,gTypesInfo[type1].name,sBlue);
        if(type2!=type1)Print(174,42,gTypesInfo[type2].name,sBlue);
        Print(6,59,gAbilitiesInfo[ability].name,sBlue);
        StringCopy(text,gAbilitiesInfo[ability].description);
        WrapFontIdToFit(text,text+StringLength(text),FONT_SMALL,210);
        u8 *line=text;
        u32 y=72;
        for(u8 *cursor=text;;cursor++)
        {
            if(*cursor==CHAR_NEWLINE || *cursor==EOS)
            {
                u8 end=*cursor;*cursor=EOS;Print(6,y,line,sInk);
                if(end==EOS)break;
                *cursor=end;line=cursor+1;y+=16;
                if(y>=128)break;
            }
        }

    }
    CpuFill16(0x8888,(void *)(BG_CHAR_ADDR(2)+32*(1+16*28)),28*2*32);
    static const u8 footer[]={8,7,8};
    Print(6,130,COMPOUND_STRING("L/R PAGE   UP/DOWN PARTY   B CLOSE"),footer);
    // Direct copies target the borrowed blocks, never the live BG configs.

}

static void VBlank(void) { TransferPlttBuffer(); }
static void Restore(void)
{
    struct TrackerOverlay *overlay=sTracker;
    SetVBlankCallback(NULL);SetHBlankCallback(NULL);
    SetGpuReg_ForcedBlank(REG_OFFSET_DISPCNT,DISPCNT_FORCED_BLANK);
    CpuCopy16(overlay->charBackup,(void *)BG_CHAR_ADDR(2),sizeof(overlay->charBackup));
    CpuCopy16(overlay->mapBackup,(void *)BG_SCREEN_ADDR(31),BG_SCREEN_SIZE);
    CpuCopy16(overlay->unfaded,&gPlttBufferUnfaded[0xF0],32);
    CpuCopy16(overlay->faded,&gPlttBufferFaded[0xF0],32);
    gWindows[overlay->window]=overlay->unusedWindow;
    gDisableTextPrinters=overlay->disabledPrinters;
    for(u32 i=0;i<ARRAY_COUNT(sRegisterOffsets);i++)SetGpuReg(sRegisterOffsets[i],overlay->registers[i]);
    SetMainCallback2(overlay->callback2);gMain.callback1=overlay->callback1;
    gMain.state=overlay->mainState;
    SetHBlankCallback(overlay->hblank);SetVBlankCallback(overlay->vblank);
    gMain.newKeys=gMain.newKeysRaw=gMain.newAndRepeatedKeys=0;
    sTracker=NULL;Free(overlay);
}
static void Input(void)
{
    if(!sTracker->released)
    {
        if(gMain.heldKeysRaw==0)sTracker->released=TRUE;
        return;
    }
    if(sTracker->closing)
    {
        if(gMain.heldKeysRaw==0)Restore();
        return;
    }
    if(gMain.newKeysRaw&B_BUTTON){sTracker->closing=TRUE;return;}
    if(gMain.newKeysRaw&(L_BUTTON|R_BUTTON))
    {
        sTracker->page=(sTracker->page+((gMain.newKeysRaw&R_BUTTON)?1:2))%3;
        Draw();
    }
    else if(gMain.newKeysRaw&(DPAD_UP|DPAD_DOWN))
    {
        for(u32 count=0;count<PARTY_SIZE;count++)
        {
            sTracker->party=(sTracker->party+((gMain.newKeysRaw&DPAD_DOWN)?1:PARTY_SIZE-1))%PARTY_SIZE;
            if(GetMonData(SelectedMon(),MON_DATA_SPECIES)&&!GetMonData(SelectedMon(),MON_DATA_IS_EGG))break;
        }
        Draw();
    }
}

bool32 ChaosTrackerIsOpen(void) { return sTracker!=NULL; }
bool32 ChaosTrackerTryOpen(void)
{
    if(sTracker||gPaletteFade.active||IsDma3ManagerBusyWithBgCopy()
     ||(gMain.inBattle&&(gBattleTypeFlags&(BATTLE_TYPE_LINK|BATTLE_TYPE_RECORDED))))return FALSE;
    for(u32 i=0;i<WINDOWS_MAX;i++)if(IsTextPrinterActiveOnWindow(i))return FALSE;
    u32 party=0;
    for(;party<PARTY_SIZE;party++)
        if(GetMonData(&gParties[B_TRAINER_PLAYER][party],MON_DATA_SPECIES)
         &&!GetMonData(&gParties[B_TRAINER_PLAYER][party],MON_DATA_IS_EGG))break;
    if(party==PARTY_SIZE)return FALSE;
    u32 win=0;
    for(;win<WINDOWS_MAX;win++)if(gWindows[win].window.bg==0xFF)break;
    if(win==WINDOWS_MAX)return FALSE;
    struct TrackerOverlay *overlay=AllocZeroedUnchecked(sizeof(*overlay));
    if(!overlay)return FALSE;
    overlay->callback1=gMain.callback1;overlay->callback2=gMain.callback2;
    overlay->mainState=gMain.state;
    overlay->vblank=gMain.vblankCallback;overlay->hblank=gMain.hblankCallback;
    overlay->disabledPrinters=gDisableTextPrinters;
    overlay->window=win;overlay->party=party;overlay->unusedWindow=gWindows[win];
    for(u32 i=0;i<ARRAY_COUNT(sRegisterOffsets);i++)overlay->registers[i]=GetGpuReg(sRegisterOffsets[i]);
    SetVBlankCallback(NULL);SetHBlankCallback(NULL);
    CpuCopy16((void *)BG_CHAR_ADDR(2),overlay->charBackup,sizeof(overlay->charBackup));
    CpuCopy16((void *)BG_SCREEN_ADDR(31),overlay->mapBackup,BG_SCREEN_SIZE);
    CpuCopy16(&gPlttBufferUnfaded[0xF0],overlay->unfaded,32);
    CpuCopy16(&gPlttBufferFaded[0xF0],overlay->faded,32);
    SetGpuReg_ForcedBlank(REG_OFFSET_DISPCNT,DISPCNT_FORCED_BLANK);
    gWindows[win].window=(struct WindowTemplate){0,1,1,28,2,15,1};
    gWindows[win].tileData=overlay->pixels;
    sTracker=overlay;
    CpuFill16(0x1111,(void *)BG_CHAR_ADDR(2),TILE_SIZE_4BPP);
    u16 *map=(u16 *)BG_SCREEN_ADDR(31);
    for(u32 i=0;i<32*32;i++)map[i]=0xF000;
    for(u32 y=0;y<18;y++)for(u32 x=0;x<28;x++)map[(y+1)*32+x+1]=0xF000|((y*28+x)+1);
    LoadPalette(sPalette,0xF0,sizeof(sPalette));
    SetGpuReg(REG_OFFSET_BG0CNT,BGCNT_CHARBASE(2)|BGCNT_SCREENBASE(31));
    SetGpuReg(REG_OFFSET_BG0HOFS,0);SetGpuReg(REG_OFFSET_BG0VOFS,0);
    SetGpuReg(REG_OFFSET_BLDCNT,0);SetGpuReg(REG_OFFSET_BLDALPHA,0);SetGpuReg(REG_OFFSET_BLDY,0);
    Draw();gMain.callback1=NULL;SetMainCallback2(Input);
    SetVBlankCallback(VBlank);SetGpuReg(REG_OFFSET_DISPCNT,DISPCNT_BG0_ON);
    return TRUE;
}
bool32 ChaosTrackerShortcut(void)
{
    const u32 chord=L_BUTTON|SELECT_BUTTON;
    if((gMain.heldKeysRaw&chord)!=chord||!(gMain.newKeysRaw&chord))return FALSE;
    // Consume the chord even if access is safely blocked. In L=A mode its L
    // component must never fall through and select a move/menu action.
    ChaosTrackerTryOpen();
    return TRUE;
}
