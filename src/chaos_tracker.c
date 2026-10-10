#include "global.h"
#include "chaos_tracker.h"
#include "chaos_observations.h"
#include "pokemon_storage_system.h"
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
#include "overworld.h"
#include "constants/flags.h"
#include "constants/game_stat.h"

#define TRACKER_PAGES 7

// Borrow only one unused window slot for CPU text rendering. No BG manager,
// tile allocation, existing window, task, sprite or battle resource is reset.
#define TRACKER_TILES (1 + 28 * 18)
#define TRACKER_PIXELS (28 * 2 * TILE_SIZE_4BPP)
struct TrackerOverlay
{
    u16 mapBackup[BG_SCREEN_SIZE / 2];
    ALIGNED(4) u8 pixels[TRACKER_PIXELS];
    u16 unfaded[16], faded[16];
    u16 registers[7];
    u16 observedSpecies, factPage;
    u8 retiredPage;
    struct Window unusedWindow;
    MainCallback callback1, callback2;
    IntrCallback vblank, hblank;
    u8 window, page, party, mainState;
    bool8 released, closing, disabledPrinters;
    u16 charBackup[];
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

// Lossless halfword runs and literal spans. Size first, then encode into a
// bounded allocation; never assume graphics compress enough to fit the heap.
static u32 PackTiles(const u16 *source,u16 *dest,u32 capacity)
{
    const u32 words=TRACKER_TILES*TILE_SIZE_4BPP/2;
    u32 in=0,out=0;
    while(in<words)
    {
        u32 run=1;
        while(in+run<words && source[in+run]==source[in])run++;
        if(run>=3)
        {
            if(dest)
            {
                if(out+2>capacity)return 0;
                dest[out]=0x8000|run;dest[out+1]=source[in];
            }
            out+=2;in+=run;
        }
        else
        {
            u32 start=in;
            in+=run;
            while(in<words)
            {
                run=1;
                while(in+run<words && source[in+run]==source[in])run++;
                if(run>=3)break;
                in+=run;
            }
            u32 count=in-start;
            if(dest)
            {
                if(out+1+count>capacity)return 0;
                dest[out]=count;
                for(u32 i=0;i<count;i++)dest[out+1+i]=source[start+i];
            }
            out+=1+count;
        }
    }
    return out;
}
static void RestoreTiles(const u16 *source)
{
    u16 *dest=(u16 *)BG_CHAR_ADDR(2);
    u32 out=0;
    while(out<TRACKER_TILES*TILE_SIZE_4BPP/2)
    {
        u16 header=*source++;
        u32 count=header&0x7FFF;
        if(header&0x8000)
        {
            u16 value=*source++;
            while(count--)dest[out++]=value;
        }
        else
            while(count--)dest[out++]=*source++;
    }
}

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
static struct BattlePokemon *ActiveMon(void)
{
    if (gMain.inBattle)
        for (u32 battler=0;battler<gBattlersCount;battler++)
            if (GetBattlerParty(battler)==gParties[B_TRAINER_PLAYER]
             && gBattlerPartyIndexes[battler]==sTracker->party)
                return &gBattleMons[battler];
    return NULL;
}
static void Stat(u32 x,u32 y,const u8 *name,u32 field,enum Stat stat)
{
    struct Pokemon *mon=SelectedMon();
    u32 nature=GetNature(mon);
    const u8 *colors=sInk;
    struct BattlePokemon *active=ActiveMon();
    if((!active || !active->volatiles.transformed) && gNaturesInfo[nature].statUp!=gNaturesInfo[nature].statDown)
    {
        if(gNaturesInfo[nature].statUp==stat)colors=sRed;
        else if(gNaturesInfo[nature].statDown==stat)colors=sBlue;
    }
    u32 value=GetMonData(mon,field);
    if(active)
        switch(field)
        {
        case MON_DATA_ATK:value=active->attack;break;
        case MON_DATA_DEF:value=active->defense;break;
        case MON_DATA_SPATK:value=active->spAttack;break;
        case MON_DATA_SPDEF:value=active->spDefense;break;
        case MON_DATA_SPEED:value=active->speed;break;
        }
    Print(x,y,name,colors);Number(x+63,y,value,colors);
    if(active && active->statStages[stat]!=DEFAULT_STAT_STAGE)
    {
        s32 stage=active->statStages[stat]-DEFAULT_STAT_STAGE;
        u8 text[8];
        u8 *end=StringCopy(text,stage>0?COMPOUND_STRING("+"):COMPOUND_STRING("-"));
        ConvertIntToDecimalStringN(end,stage>0?stage:-stage,STR_CONV_MODE_LEFT_ALIGN,1);
        Print(x+91,y,text,sInk);
    }
}
static void CycleObservedSpecies(bool32 forward)
{
    const struct ChaosObservationJournal *journal=ChaosObservationsRead();
    if(!journal)return;
    u32 current=0;
    for(u32 i=0;i<journal->count;i++)
        if(journal->facts[i].fact==CHAOS_OBS_SEEN && journal->facts[i].species==sTracker->observedSpecies)
        {current=i;break;}
    for(u32 n=0;n<journal->count;n++)
    {
        current=(current+(forward?1:journal->count-1))%journal->count;
        if(journal->facts[current].fact==CHAOS_OBS_SEEN)
        {
            sTracker->observedSpecies=journal->facts[current].species;
            sTracker->factPage=0;return;
        }
    }
}
static void DrawObservations(void)
{
    const struct ChaosObservationJournal *journal=ChaosObservationsRead();
    Print(6,24,COMPOUND_STRING("OBSERVED IDENTITIES"),sBand);
    if(!journal || journal->count==0)
    {
        Print(6,48,COMPOUND_STRING("No opponents recorded yet."),sInk);
        Print(6,72,COMPOUND_STRING("Only visible battle events are saved."),sInk);
        return;
    }
    if(!sTracker->observedSpecies)
        for(u32 i=0;i<journal->count;i++)
            if(journal->facts[i].fact==CHAOS_OBS_SEEN)
            {sTracker->observedSpecies=journal->facts[i].species;break;}
    Print(6,40,gSpeciesInfo[sTracker->observedSpecies].speciesName,sBlue);
    Print(117,40,sTracker->page==3?COMPOUND_STRING("MOVES / USES SEEN"):COMPOUND_STRING("ABILITIES SHOWN"),sInk);
    u32 type=sTracker->page==3?CHAOS_OBS_MOVE:CHAOS_OBS_ABILITY;
    u32 count=0,shown=0;
    for(u32 i=0;i<journal->count;i++)
    {
        const struct ChaosObservation *record=&journal->facts[i];
        if(record->species!=sTracker->observedSpecies || (record->fact&0xF000)!=type)continue;
        if(count++<sTracker->factPage*4 || shown==4)continue;
        u32 value=record->fact&0xFFF, y=56+16*shown++;
        Print(6,y,type==CHAOS_OBS_MOVE?GetMoveName(value):gAbilitiesInfo[value].name,sInk);
        if(type==CHAOS_OBS_MOVE)Number(180,y,record->count,sInk);
    }
    if(!shown)Print(6,56,COMPOUND_STRING("Not observed"),sInk);
    Print(6,112,journal->full?COMPOUND_STRING("Journal full: earlier facts preserved."):COMPOUND_STRING("A: MORE   Uses seen; PP unknown."),sInk);
}
static void DrawProgress(void)
{
    Print(6,24,COMPOUND_STRING("RUN PROGRESS"),sBand);
    Print(6,42,COMPOUND_STRING("MODE"),sInk);
    Print(80,42,IsIronmonRun()?(IsIronmonHardcore()?COMPOUND_STRING("IronMON Hardcore"):COMPOUND_STRING("IronMON Normal")):COMPOUND_STRING("Chaos"),sBlue);
    Print(6,58,COMPOUND_STRING("SEED"),sInk);
    u8 text[16];
    ConvertIntToDecimalStringN(text,gSaveBlock3Ptr->worldSeed,STR_CONV_MODE_LEFT_ALIGN,10);
    Print(80,58,text,sInk);
    Print(6,74,COMPOUND_STRING("PLAY TIME"),sInk);
    u8 *end=ConvertIntToDecimalStringN(text,gSaveBlock2Ptr->playTimeHours,STR_CONV_MODE_LEFT_ALIGN,3);
    *end++=CHAR_COLON;
    ConvertIntToDecimalStringN(end,gSaveBlock2Ptr->playTimeMinutes,STR_CONV_MODE_LEADING_ZEROS,2);
    Print(80,74,text,sInk);
    Print(6,90,IsIronmonRun()?COMPOUND_STRING("TRAINERS WON"):COMPOUND_STRING("TRAINER BATTLES"),sInk);
    Number(139,90,IsIronmonRun()?gSaveBlock3Ptr->ironmon.trainersDefeated:GetGameStat(GAME_STAT_TRAINER_BATTLES),sInk);
    Print(6,106,COMPOUND_STRING("BADGES"),sInk);
    u32 badges=0;
    for(u32 i=0;i<8;i++)if(FlagGet(FLAG_BADGE01_GET+i))badges++;
    Number(80,106,badges,sInk);
    if(IsIronmonRun())
    {
        Print(110,106,COMPOUND_STRING("RETIRED"),sInk);
        Number(170,106,gSaveBlock3Ptr->ironmon.retiredCount,sInk);
    }
}
static void DrawRetired(void)
{
    Print(6,24,COMPOUND_STRING("RETIRED HISTORY"),sBand);
    if(!IsIronmonRun())
    {
        Print(6,48,COMPOUND_STRING("Retirement applies to IronMON."),sInk);
        Print(6,72,COMPOUND_STRING("Your Chaos party remains available."),sInk);
        return;
    }
    u32 total=gSaveBlock3Ptr->ironmon.retiredCount,count=min(total,IN_BOX_COUNT);
    if(!count)
    {
        Print(6,48,COMPOUND_STRING("No retired Pokemon yet."),sInk);
        return;
    }
    Print(6,40,COMPOUND_STRING("READ ONLY"),sBlue);
    Print(155,40,COMPOUND_STRING("A: MORE"),sInk);
    if(total>IN_BOX_COUNT)Print(83,40,COMPOUND_STRING("LAST 30"),sInk);
    for(u32 row=0;row<4;row++)
    {
        u32 index=sTracker->retiredPage*4+row;
        if(index>=count)break;
        u32 slot=(total-1-index)%IN_BOX_COUNT;
        u8 name[POKEMON_NAME_LENGTH+1];
        GetAndCopyBoxMonDataAt(IRONMON_RETIRED_BOX,slot,MON_DATA_NICKNAME,name);
        Print(6,56+row*16,name,sInk);
        enum Species species=GetBoxMonDataAt(IRONMON_RETIRED_BOX,slot,MON_DATA_SPECIES);
        if(species<NUM_SPECIES)Print(110,56+row*16,gSpeciesInfo[species].speciesName,sInk);
    }
}
static void Draw(void)
{
    struct Pokemon *mon=SelectedMon();
    struct BattlePokemon *active=ActiveMon();
    enum Species species=active?active->species:GetMonData(mon,MON_DATA_SPECIES);
    u8 text[128];
    CpuFill16(0x1111,(void *)BG_CHAR_ADDR(2),TRACKER_TILES*TILE_SIZE_4BPP);
    CpuFill16(0x4444,(void *)(BG_CHAR_ADDR(2)+32),28*2*32);
    Print(6,3,COMPOUND_STRING("CHAOS TRACKER"),sWhite);
    Number(180,3,sTracker->page+1,sWhite);Print(192,3,COMPOUND_STRING("/ 7"),sWhite);
    CpuFill16(0x6666,(void *)(BG_CHAR_ADDR(2)+32*(1+3*28)),28*2*32);
    if(sTracker->page<3)
    {
        GetMonData(mon,MON_DATA_NICKNAME,text);Print(6,24,text,sBand);
        Print(143,24,COMPOUND_STRING("Lv."),sBand);Number(164,24,GetMonData(mon,MON_DATA_LEVEL),sBand);
    }
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
        Number(35,58,active?active->hp:GetMonData(mon,MON_DATA_HP),sInk);
        Print(63,58,COMPOUND_STRING("/"),sInk);
        Number(72,58,active?active->maxHP:GetMonData(mon,MON_DATA_MAX_HP),sInk);
        Stat(6,75,COMPOUND_STRING("ATTACK"),MON_DATA_ATK,STAT_ATK);
        Stat(117,75,COMPOUND_STRING("SP. ATK"),MON_DATA_SPATK,STAT_SPATK);
        Stat(6,92,COMPOUND_STRING("DEFENSE"),MON_DATA_DEF,STAT_DEF);
        Stat(117,92,COMPOUND_STRING("SP. DEF"),MON_DATA_SPDEF,STAT_SPDEF);
        Stat(6,109,COMPOUND_STRING("SPEED"),MON_DATA_SPEED,STAT_SPEED);
        Print(117,109,active && active->volatiles.transformed?COMPOUND_STRING("Copied stats"):COMPOUND_STRING("Red + / Blue -"),sInk);
    }
    else if(sTracker->page==1)
    {
        for(u32 i=0;i<MAX_MON_MOVES;i++)
        {
            enum Move move=active?active->moves[i]:GetMonData(mon,MON_DATA_MOVE1+i);
            u32 y=40+i*24;

            Print(6,y,move?GetMoveName(move):COMPOUND_STRING("--"),sInk);
            if(move)
            {
                Number(159,y,active?active->pp[i]:GetMonData(mon,MON_DATA_PP1+i),sInk);
                Print(178,y,COMPOUND_STRING("/"),sInk);
                Number(187,y,active && active->volatiles.transformed?min(5,GetMovePP(move)):CalculatePPWithBonus(move,active?active->ppBonuses:GetMonData(mon,MON_DATA_PP_BONUSES),i),sInk);
            }
        }
    }
    else if(sTracker->page==2)
    {
        enum Ability ability=GetMonAbility(mon);
        enum Type type1=GetSpeciesType(species,0),type2=GetSpeciesType(species,1);
        if(active)
        {
            ability=active->ability;
            type1=active->types[0];type2=active->types[1];
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
    else if(sTracker->page<5)DrawObservations();
    else if(sTracker->page==5)DrawProgress();
    else DrawRetired();
    CpuFill16(0x8888,(void *)(BG_CHAR_ADDR(2)+32*(1+16*28)),28*2*32);
    static const u8 footer[]={8,7,8};
    Print(6,130,sTracker->page<3?COMPOUND_STRING("L/R PAGE   UP/DOWN PARTY   B CLOSE"):sTracker->page<5?COMPOUND_STRING("L/R PAGE   UP/DOWN FOE   B CLOSE"):COMPOUND_STRING("L/R PAGE     B CLOSE"),footer);
    // Direct copies target the borrowed blocks, never the live BG configs.

}

static void VBlank(void) { TransferPlttBuffer(); }
static void Restore(void)
{
    struct TrackerOverlay *overlay=sTracker;
    SetVBlankCallback(NULL);SetHBlankCallback(NULL);
    SetGpuReg_ForcedBlank(REG_OFFSET_DISPCNT,DISPCNT_FORCED_BLANK);
    RestoreTiles(overlay->charBackup);
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
        sTracker->page=(sTracker->page+((gMain.newKeysRaw&R_BUTTON)?1:TRACKER_PAGES-1))%TRACKER_PAGES;
        sTracker->factPage=0;
        Draw();
    }
    else if(gMain.newKeysRaw&(DPAD_UP|DPAD_DOWN))
    {
        if(sTracker->page>=3 && sTracker->page<5)
        {
            CycleObservedSpecies((gMain.newKeysRaw&DPAD_DOWN)!=0);
            Draw();return;
        }
        if(sTracker->page>=5)return;
        for(u32 count=0;count<PARTY_SIZE;count++)
        {
            sTracker->party=(sTracker->party+((gMain.newKeysRaw&DPAD_DOWN)?1:PARTY_SIZE-1))%PARTY_SIZE;
            if(GetMonData(SelectedMon(),MON_DATA_SPECIES)&&!GetMonData(SelectedMon(),MON_DATA_IS_EGG))break;
        }
        Draw();
    }
    else if(sTracker->page>=3 && sTracker->page<5 && (gMain.newKeysRaw&A_BUTTON))
    {
        const struct ChaosObservationJournal *journal=ChaosObservationsRead();
        u32 count=0,type=sTracker->page==3?CHAOS_OBS_MOVE:CHAOS_OBS_ABILITY;
        if(journal)
            for(u32 i=0;i<journal->count;i++)
                if(journal->facts[i].species==sTracker->observedSpecies && (journal->facts[i].fact&0xF000)==type)count++;
        sTracker->factPage=(sTracker->factPage+1)*4<count?sTracker->factPage+1:0;
        Draw();
    }
    else if(sTracker->page==6 && (gMain.newKeysRaw&A_BUTTON) && IsIronmonRun())
    {
        u32 count=min(gSaveBlock3Ptr->ironmon.retiredCount,IN_BOX_COUNT);
        sTracker->retiredPage=(sTracker->retiredPage+1)*4<count?sTracker->retiredPage+1:0;
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
    u32 packedWords=PackTiles((const u16 *)BG_CHAR_ADDR(2),NULL,0);
    struct TrackerOverlay *overlay=AllocZeroedUnchecked(sizeof(*overlay)+packedWords*sizeof(u16));
    if(!overlay)return FALSE;
    overlay->callback1=gMain.callback1;overlay->callback2=gMain.callback2;
    overlay->mainState=gMain.state;
    overlay->vblank=gMain.vblankCallback;overlay->hblank=gMain.hblankCallback;
    overlay->disabledPrinters=gDisableTextPrinters;
    overlay->window=win;overlay->party=party;overlay->unusedWindow=gWindows[win];
    for(u32 i=0;i<ARRAY_COUNT(sRegisterOffsets);i++)overlay->registers[i]=GetGpuReg(sRegisterOffsets[i]);
    SetVBlankCallback(NULL);SetHBlankCallback(NULL);
    if(!PackTiles((const u16 *)BG_CHAR_ADDR(2),overlay->charBackup,packedWords))
    {
        SetHBlankCallback(overlay->hblank);SetVBlankCallback(overlay->vblank);
        Free(overlay);return FALSE;
    }
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
