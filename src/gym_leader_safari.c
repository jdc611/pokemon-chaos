#include "global.h"
#include "gym_leader.h"
#include "gym_leader_script.h"
#include "event_data.h"
#include "malloc.h"
#include "money.h"
#include "pokemon.h"
#include "script.h"
#include "string_util.h"
#include "constants/maps.h"

static EWRAM_DATA u16 *sSafariPool = NULL;
static EWRAM_DATA u16 sSafariPoolCount = 0;
static u32 SafariSlots(void)
{
    u32 tier=GymLeaderState()->landmarkTier;
    return tier==1?3:tier==2?4:tier==3?6:0;
}
static bool32 Owner(void)
{
    return IsGymLeaderRun() && GymLeaderState()->town==2 && SafariSlots();
}
void GleSafariOwner(void) { gSpecialVar_Result=Owner(); }
static void SafariName(u8 *out,u16 species)
{
    const struct SpeciesInfo *info=&gSpeciesInfo[species];
    StringCopy(out,GetSpeciesName(species));
    if(info->isAlolanForm)StringAppend(out,COMPOUND_STRING(" (Alola)"));
    else if(info->isGalarianForm)StringAppend(out,COMPOUND_STRING(" (Galar)"));
    else if(info->isHisuianForm)StringAppend(out,COMPOUND_STRING(" (Hisui)"));
    else if(info->isPaldeanForm)StringAppend(out,COMPOUND_STRING(" (Paldea)"));
}
void GleSafariSlots(void)
{
    u8 labels[6][36];const u8 *names[6];u32 i;
    if(!Owner()){gSpecialVar_Result=255;GymLeaderResumeScriptNextFrame();return;}
    for(i=0;i<SafariSlots();i++)
    {
        u8 *end=ConvertIntToDecimalStringN(labels[i],i+1,STR_CONV_MODE_LEFT_ALIGN,1);
        end=StringCopy(end,COMPOUND_STRING(": "));
        if(GymLeaderState()->safariSpecies[i])SafariName(end,GymLeaderState()->safariSpecies[i]);
        else StringCopy(end,COMPOUND_STRING("Native wildlife"));
        names[i]=labels[i];
    }
    GymLeaderMenu(names,SafariSlots(),FALSE);
}
void GleSafariSpeciesMenu(void)
{
    const u8 **names;u8 (*labels)[32];u32 i;
    if(sSafariPool){Free(sSafariPool);sSafariPool=NULL;}
    sSafariPoolCount=0;
    if(!Owner() || gSpecialVar_Result>=18)goto failed;
    sSafariPool=Alloc(GLE_SPECIES_LIMIT*sizeof(u16));
    if(!sSafariPool)goto failed;
    sSafariPoolCount=GymLeaderBuildSpeciesPool(gSpecialVar_Result,GymLeaderState()->rank,GLE_STANDARD,FALSE,sSafariPool,GLE_SPECIES_LIMIT);
    if(!sSafariPoolCount || sSafariPoolCount>GLE_SPECIES_LIMIT)goto failed;
    names=Alloc(sSafariPoolCount*sizeof(*names));
    if(!names)goto failed;
    labels=Alloc(sSafariPoolCount*sizeof(*labels));
    if(!labels){Free(names);goto failed;}
    for(i=0;i<sSafariPoolCount;i++){SafariName(labels[i],sSafariPool[i]);names[i]=labels[i];}
    GymLeaderMenu(names,sSafariPoolCount,FALSE);
    Free(labels);Free(names);return;
failed:
    if(sSafariPool){Free(sSafariPool);sSafariPool=NULL;}
    sSafariPoolCount=0;gSpecialVar_Result=255;GymLeaderResumeScriptNextFrame();
}
static u32 SafariCost(u16 species)
{
    const struct SpeciesInfo *info=&gSpeciesInfo[species];
    u32 bst=info->baseHP+info->baseAttack+info->baseDefense+info->baseSpeed+info->baseSpAttack+info->baseSpDefense;
    return bst*4+(bst>=500?3000:bst>=400?1000:0);
}
void GleSafariPrice(void)
{
    u32 index=gSpecialVar_Result;u16 species;
    gSpecialVar_Result=0;
    if(index!=127 && index!=255 && sSafariPool)
    {
        if(index>127)index--;
        if(index<sSafariPoolCount)
        {
            species=sSafariPool[index];
            gSpecialVar_0x8006=species;
            SafariName(gStringVar1,species);
            ConvertIntToDecimalStringN(gStringVar2,SafariCost(species),STR_CONV_MODE_LEFT_ALIGN,6);
            gSpecialVar_Result=1;
        }
    }
    if(sSafariPool)Free(sSafariPool);
    sSafariPool=NULL;sSafariPoolCount=0;
}
void GleSafariConfigure(void)
{
    struct GleState *s=GymLeaderState();u32 i,slot=gSpecialVar_0x8008,cost;
    u16 species=gSpecialVar_0x8006;
    gSpecialVar_Result=GLE_INVALID;
    if(!Owner() || slot>=SafariSlots() || !species || species>=NUM_SPECIES || GymLeaderSpeciesReserved(species))return;
    for(i=0;i<6;i++)if(s->safariSpecies[i]==species){gSpecialVar_Result=GLE_DUPLICATE;return;}
    cost=SafariCost(species);
    if(!IsEnoughMoney(&gSaveBlock1Ptr->money,cost)){gSpecialVar_Result=GLE_NOT_ENOUGH_MONEY;return;}
    RemoveMoney(&gSaveBlock1Ptr->money,cost);
    s->safariSpecies[slot]=species;
    gSpecialVar_Result=GLE_OK;
}
u16 GymLeaderSafariSpecies(u32 slot)
{
    u16 map=(u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8);
    if(!Owner() || (map!=MAP_SAFARI_ZONE_CENTER && map!=MAP_SAFARI_ZONE_EAST && map!=MAP_SAFARI_ZONE_WEST && map!=MAP_SAFARI_ZONE_NORTH))return SPECIES_NONE;
    return GymLeaderState()->safariSpecies[slot%SafariSlots()];
}
