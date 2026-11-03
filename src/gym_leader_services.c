#include "global.h"
#include "gym_leader.h"
#include "gym_leader_preview.h"
#include "gym_leader_script.h"
#include "data.h"
#include "event_data.h"
#include "field_screen_effect.h"
#include "main.h"
#include "malloc.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "script.h"
#include "string_util.h"
#include "task.h"
#include "constants/pokemon.h"
#include "constants/maps.h"
#include "constants/rgb.h"

static EWRAM_DATA u8 sVoucherSlot = 0;
static const u8 *const sVoucherNames[] = {
    COMPOUND_STRING("Standard"), COMPOUND_STRING("Rare"), COMPOUND_STRING("Elite"),
    COMPOUND_STRING("Shiny"), COMPOUND_STRING("Shiny Elite")
};
void GleServicesMenu(void)
{
    static const u8 *const names[] = {COMPOUND_STRING("Voucher desk"),COMPOUND_STRING("Milestone rewards"),COMPOUND_STRING("Training room"),COMPOUND_STRING("Return home"),COMPOUND_STRING("Back")};
    GymLeaderMenu(names,ARRAY_COUNT(names),FALSE);
}
void GleMilestoneRewards(void)
{
    static const u8 kinds[] = {GLE_STANDARD,GLE_STANDARD,GLE_RARE,GLE_STANDARD,GLE_ELITE,GLE_SHINY,GLE_ELITE,GLE_SHINY_ELITE};
    struct GleState *s=GymLeaderState();
    u32 rank,count=0;
    for(rank=2;rank<=s->highestRank;rank++)
        if(!(s->milestoneRewardMask&(1u<<(rank-1))))
        {
            if(GleAwardVoucher(s,kinds[rank-1],rank)!=GLE_OK)break;
            s->milestoneRewardMask|=1u<<(rank-1);
            count++;
        }
    ConvertIntToDecimalStringN(gStringVar1,count,STR_CONV_MODE_LEFT_ALIGN,2);
    gSpecialVar_Result=count;
}
void GleVoucherMenu(void)
{
    struct GleState *s=GymLeaderState();
    u8 labels[GLE_VOUCHERS][32];
    const u8 *names[GLE_VOUCHERS];
    u32 i;
    if(gSaveBlock1Ptr->location.mapGroup!=MAP_GROUP(MAP_GLE_LEAGUE_HALL)
        || gSaveBlock1Ptr->location.mapNum!=MAP_NUM(MAP_GLE_LEAGUE_HALL) || !s->voucherCount){gSpecialVar_Result=255;GymLeaderResumeScriptNextFrame();return;}
    for(i=0;i<s->voucherCount;i++)
    {
        u8 *end=StringCopy(labels[i],sVoucherNames[s->vouchers[i].kind]);
        end=StringCopy(end,COMPOUND_STRING(" / Tier "));
        ConvertIntToDecimalStringN(end,s->vouchers[i].tier,STR_CONV_MODE_LEFT_ALIGN,1);
        names[i]=labels[i];
    }
    GymLeaderMenu(names,s->voucherCount,FALSE);
}
static struct GleVoucher *CurrentVoucher(void)
{
    struct GleState *s=GymLeaderState();
    return sVoucherSlot<s->voucherCount?&s->vouchers[sVoucherSlot]:NULL;
}
static void MakeVoucherMon(struct Pokemon *mon,u32 choice)
{
    const struct GleVoucher *v=CurrentVoucher();
    u32 seed=GleMix(v->seed^(choice*0x7331u)),stat;
    u8 level=v->level;
    u8 ability=(seed>>16)&1;
    u8 shiny=v->kind==GLE_SHINY || v->kind==GLE_SHINY_ELITE || GleMix(seed^0x5191u)%4096==0;
    enum PokeBall ball=BALL_GLA;
    enum Species species=v->species[choice];
    if(!gSpeciesInfo[species].abilities[ability])ability=0;
    if(v->tier>=6 && (v->kind==GLE_ELITE || v->kind==GLE_SHINY_ELITE)
        && gSpeciesInfo[species].abilities[2] && GleMix(seed^0xAB11u)%4==0)ability=2;
    CreateMonWithIVs(mon,species,level,seed,OTID_STRUCT_PLAYER_ID,0);
    GiveMonInitialMoveset(mon);
    for(stat=0;stat<NUM_STATS;stat++)
    {
        u8 iv=GleMix(seed^(stat*257))%32;
        SetMonData(mon,MON_DATA_HP_IV+stat,&iv);
    }
    SetMonData(mon,MON_DATA_ABILITY_NUM,&ability);
    SetMonData(mon,MON_DATA_IS_SHINY,&shiny);
    SetMonData(mon,MON_DATA_POKEBALL,&ball);
    CalculateMonStats(mon);
}
void GleVoucherOpen(void)
{
    struct GleState *s=GymLeaderState();
    u16 *pool;
    u32 count,i,slot=gSpecialVar_Result;
    gSpecialVar_Result=GLE_INVALID;
    if(slot>=s->voucherCount)return;
    sVoucherSlot=slot;
    if(!s->vouchers[slot].viewed)
    {
        pool=Alloc(GLE_SPECIES_LIMIT*sizeof(u16));
        if(pool==NULL)return;
        count=GymLeaderBuildSpeciesPool(s->gymType,s->vouchers[slot].tier,s->vouchers[slot].kind,FALSE,pool,GLE_SPECIES_LIMIT);
        if(count && count<=GLE_SPECIES_LIMIT)gSpecialVar_Result=GleViewVoucher(s,slot,pool,count);
        else gSpecialVar_Result=GLE_NOT_READY;
        Free(pool);
        if(gSpecialVar_Result!=GLE_OK)return;
    }
    if(!s->vouchers[slot].level)s->vouchers[slot].level=max(5,GymLeaderLevelCap()-3);
    for(i=0;i<s->vouchers[slot].count;i++)MakeVoucherMon(&gGlePreviewParty[i],i);
    gSpecialVar_Result=GLE_OK;
}
void GleVoucherChoices(void)
{
    const struct GleVoucher *v=CurrentVoucher();
    const u8 *names[3];u32 i;
    if(v==NULL || !v->viewed){gSpecialVar_Result=255;GymLeaderResumeScriptNextFrame();return;}
    for(i=0;i<v->count;i++)names[i]=GetSpeciesName(v->species[i]);
    GymLeaderMenu(names,v->count,FALSE);
}
void GleVoucherInfo(void)
{
    const struct GleVoucher *v=CurrentVoucher();
    u32 choice=gSpecialVar_0x8007;
    if(v==NULL || choice>=v->count)return;
    StringCopy(gStringVar1,GetSpeciesName(v->species[choice]));
    StringCopy(gStringVar2,gAbilitiesInfo[GetMonAbility(&gGlePreviewParty[choice])].name);
    ConvertIntToDecimalStringN(gStringVar3,GetMonData(&gGlePreviewParty[choice],MON_DATA_LEVEL),STR_CONV_MODE_LEFT_ALIGN,3);
}
static void Task_VoucherSummary(u8 taskId)
{
    if(!gPaletteFade.active)
    {
        const struct GleVoucher *v=CurrentVoucher();
        ResetTasks();CleanupOverworldWindowsAndTilemaps();
        ShowPokemonSummaryScreen(SUMMARY_MODE_NORMAL,gGlePreviewParty,gSpecialVar_0x8007,v->count-1,CB2_ReturnToFieldContinueScript);
    }
}
void GleVoucherSummary(void)
{
    const struct GleVoucher *v=CurrentVoucher();
    if(v==NULL || gSpecialVar_0x8007>=v->count){GymLeaderResumeScriptNextFrame();return;}
    BeginNormalPaletteFade(PALETTES_ALL,0,0,16,RGB_BLACK);
    CreateTask(Task_VoucherSummary,10);
}
void GleVoucherClaim(void)
{
    const struct GleVoucher *v=CurrentVoucher();
    u32 choice=gSpecialVar_0x8007,result;u16 species;
    gSpecialVar_Result=GLE_INVALID;
    if(v==NULL || !v->viewed || choice>=v->count)return;
    if(GleSpeciesClaimed(GymLeaderState(),v->species[choice],v->kind)){gSpecialVar_Result=GLE_DUPLICATE;return;}
    if(IsPlayerPartyAndPokemonStorageFull()){gSpecialVar_Result=GLE_NO_ROOM;return;}
    result=GiveScriptedMonToPlayer(&gGlePreviewParty[choice],PARTY_SIZE);
    if(result==MON_CANT_GIVE){gSpecialVar_Result=GLE_NO_ROOM;return;}
    // The checks above make the remaining state transaction infallible. No
    // voucher is consumed for a full party/PC or for browsing the desk.
    gSpecialVar_Result=GleClaimVoucher(GymLeaderState(),sVoucherSlot,choice,&species);
    StringCopy(gStringVar1,GetSpeciesName(species));
    sVoucherSlot=GLE_NONE;
}

static const u8 *const sResearchNames[GLE_PROJECTS] = {
    COMPOUND_STRING("Type: Null"),COMPOUND_STRING("Paradox of Evolution"),COMPOUND_STRING("Time and Space"),COMPOUND_STRING("Ancient Titans"),
    COMPOUND_STRING("Ancient Awakening"),COMPOUND_STRING("Four Winds"),COMPOUND_STRING("Swords of Justice"),COMPOUND_STRING("Cursed Treasures"),
    COMPOUND_STRING("Lakes of the Mind"),COMPOUND_STRING("Guardian Shrines"),COMPOUND_STRING("Echoes of the Past"),COMPOUND_STRING("Jungle Guardian"),
    COMPOUND_STRING("Wings of Kanto"),COMPOUND_STRING("Toxic Pact"),COMPOUND_STRING("Slaking research"),COMPOUND_STRING("Form research")
};
void GleResearchMenu(void) { GymLeaderMenu(sResearchNames,GLE_PROJECTS,FALSE); }
void GleResearchInfo(void)
{
    struct GleState *s=GymLeaderState();u32 project=gSpecialVar_Result;
    if(project>=GLE_PROJECTS)return;
    StringCopy(gStringVar1,sResearchNames[project]);
    ConvertIntToDecimalStringN(gStringVar2,s->projectStage[project],STR_CONV_MODE_LEFT_ALIGN,3);
    StringCopy(gStringVar3,s->projectUnlocked[project]?COMPOUND_STRING("ACTIVE"):COMPOUND_STRING("AWAITING FIELD EVIDENCE"));
}
