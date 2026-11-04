#include "global.h"
#include "gym_leader.h"
#include "gym_leader_finale.h"
#include "gym_leader_script.h"
#include "event_data.h"
#include "malloc.h"
#include "money.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "pokemon_summary_screen.h"
#include "string_util.h"
#include "task.h"
#include "constants/maps.h"
#include "constants/rgb.h"

static EWRAM_DATA u16 *sCatalogPool=NULL;
static EWRAM_DATA u16 sCatalogCount=0;
static EWRAM_DATA u16 sCatalogSpecies=SPECIES_NONE;
extern struct Pokemon gGlePreviewParty[PARTY_SIZE];
static bool32 AtDesk(void){return GymLeaderFinalOakComplete()&&gSaveBlock1Ptr->location.mapGroup==MAP_GROUP(MAP_GLE_LEAGUE_HALL)&&gSaveBlock1Ptr->location.mapNum==MAP_NUM(MAP_GLE_LEAGUE_HALL);}
static void FreePool(void){TRY_FREE_AND_SET_NULL(sCatalogPool);sCatalogCount=0;}
static bool32 Canonical(u16 species)
{
    const struct SpeciesInfo *info=&gSpeciesInfo[species];
    return GetFormSpeciesId(species,0)==species||info->isAlolanForm||info->isGalarianForm||info->isHisuianForm||info->isPaldeanForm;
}
static bool32 HeldByVoucher(u16 species)
{
    struct GleState *s=GymLeaderState();u32 i,j;
    for(i=0;i<s->voucherCount;i++){
        const struct GleVoucher *v=&s->vouchers[i];
        if(!v->viewed||v->kind==GLE_SHINY||v->kind==GLE_SHINY_ELITE)continue;
        for(j=0;j<v->count;j++)if(v->species[j]==species)return TRUE;
    }
    return FALSE;
}
static u32 Price(u16 species)
{
    const struct SpeciesInfo *i=&gSpeciesInfo[species];u32 bst=i->baseHP+i->baseAttack+i->baseDefense+i->baseSpeed+i->baseSpAttack+i->baseSpDefense;
    u32 cost=i->isRestrictedLegendary||i->isSubLegendary||i->isMythical||i->isUltraBeast||i->isParadox?50000+20*bst:1000+10*bst;
    return GymLeaderState()->honor>=100?cost*9/10:cost;
}
static void SpeciesName(u8 *out,u16 species)
{
    const struct SpeciesInfo *i=&gSpeciesInfo[species];StringCopy(out,GetSpeciesName(species));
    if(i->isAlolanForm)StringAppend(out,COMPOUND_STRING(" (Alola)"));
    else if(i->isGalarianForm)StringAppend(out,COMPOUND_STRING(" (Galar)"));
    else if(i->isHisuianForm)StringAppend(out,COMPOUND_STRING(" (Hisui)"));
    else if(i->isPaldeanForm)StringAppend(out,COMPOUND_STRING(" (Paldea)"));
}
void GleCatalogStatus(void){gSpecialVar_Result=AtDesk();}
void GleCatalogMenu(void)
{
    u16 *temp;u8 seen[GLE_SPECIES_LIMIT/8]={0};u32 type=gSpecialVar_Result,kind,count,j;
    const u8 **names;u8 (*labels)[32];FreePool();sCatalogSpecies=SPECIES_NONE;
    if(!AtDesk()||type>=18)goto failed;
    sCatalogPool=Alloc(GLE_SPECIES_LIMIT*sizeof(u16));temp=Alloc(GLE_SPECIES_LIMIT*sizeof(u16));
    if(!sCatalogPool||!temp){Free(temp);goto failed;}
    for(kind=0;kind<=GLE_ELITE;kind++){
        count=GymLeaderBuildSpeciesPool(type,8,kind,FALSE,temp,GLE_SPECIES_LIMIT);
        if(count>GLE_SPECIES_LIMIT){Free(temp);goto failed;}
        for(j=0;j<count;j++){
            u16 species=temp[j];
            if((seen[species/8]&(1u<<(species%8)))||!Canonical(species)||GleSpeciesClaimed(GymLeaderState(),species,GLE_STANDARD))continue;
            seen[species/8]|=1u<<(species%8);sCatalogPool[sCatalogCount++]=species;
        }
    }
    Free(temp);if(!sCatalogCount)goto failed;
    names=Alloc(sCatalogCount*sizeof(*names));labels=Alloc(sCatalogCount*sizeof(*labels));
    if(!names||!labels){Free(names);Free(labels);goto failed;}
    for(j=0;j<sCatalogCount;j++){SpeciesName(labels[j],sCatalogPool[j]);names[j]=labels[j];}
    GymLeaderMenu(names,sCatalogCount,FALSE);Free(names);Free(labels);return;
failed:
    FreePool();gSpecialVar_Result=255;GymLeaderResumeScriptNextFrame();
}
void GleCatalogQuote(void)
{
    u32 index=gSpecialVar_Result;gSpecialVar_Result=GLE_INVALID;sCatalogSpecies=SPECIES_NONE;
    if(index!=127&&index!=255&&sCatalogPool){
        if(index>255)index-=2;else if(index>127)index--;
        if(index<sCatalogCount&&AtDesk()){
            sCatalogSpecies=sCatalogPool[index];
            SpeciesName(gStringVar1,sCatalogSpecies);ConvertIntToDecimalStringN(gStringVar2,Price(sCatalogSpecies),STR_CONV_MODE_LEFT_ALIGN,6);
            CreateMonWithIVs(&gGlePreviewParty[0],sCatalogSpecies,5,GleMix(GymLeaderState()->seed^sCatalogSpecies^0xCA7A10u),OTID_STRUCT_PLAYER_ID,15);
            {u8 normal=0;SetMonData(&gGlePreviewParty[0],MON_DATA_ABILITY_NUM,&normal);}
            GiveMonInitialMoveset(&gGlePreviewParty[0]);gSpecialVar_Result=GLE_OK;
        }
    }
    FreePool();
}
void GleCatalogActions(void){static const u8 *const names[]={COMPOUND_STRING("Summary"),COMPOUND_STRING("Arrange placement"),COMPOUND_STRING("Back")};GymLeaderMenu(names,ARRAY_COUNT(names),FALSE);}
static void Task_CatalogSummary(u8 taskId)
{
    if(gPaletteFade.active)return;
    ResetTasks();CleanupOverworldWindowsAndTilemaps();ShowPokemonSummaryScreen(SUMMARY_MODE_NORMAL,gGlePreviewParty,0,0,CB2_ReturnToFieldContinueScript);
}
void GleCatalogSummary(void){if(!AtDesk()||!sCatalogSpecies){GymLeaderResumeScriptNextFrame();return;}BeginNormalPaletteFade(PALETTES_ALL,0,0,16,RGB_BLACK);CreateTask(Task_CatalogSummary,10);}
void GleCatalogClaim(void)
{
    struct GleState *s=GymLeaderState();u16 species=sCatalogSpecies;u32 cost,delivered;gSpecialVar_Result=GLE_NOT_READY;
    if(!AtDesk()||!species||species>=GLE_SPECIES_LIMIT||GymLeaderSpeciesReserved(species)||HeldByVoucher(species))return;
    if(GleSpeciesClaimed(s,species,GLE_STANDARD)){gSpecialVar_Result=GLE_DUPLICATE;return;}
    if(IsPlayerPartyAndPokemonStorageFull()){gSpecialVar_Result=GLE_NO_ROOM;return;}
    cost=Price(species);if(!IsEnoughMoney(&gSaveBlock1Ptr->money,cost)){gSpecialVar_Result=GLE_NOT_ENOUGH_MONEY;return;}
    if(GetMonData(&gGlePreviewParty[0],MON_DATA_SPECIES)!=species)return;
    delivered=GiveScriptedMonToPlayer(&gGlePreviewParty[0],PARTY_SIZE);
    if(delivered==MON_CANT_GIVE){gSpecialVar_Result=GLE_NO_ROOM;return;}
    s->normalClaims[species/8]|=1u<<(species%8);RemoveMoney(&gSaveBlock1Ptr->money,cost);gSpecialVar_Result=GLE_OK;
}

void GleCatalogDetails(void)
{
    if(sCatalogSpecies&&sCatalogSpecies<NUM_SPECIES){SpeciesName(gStringVar1,sCatalogSpecies);ConvertIntToDecimalStringN(gStringVar2,Price(sCatalogSpecies),STR_CONV_MODE_LEFT_ALIGN,6);}
}

void GleCatalogCancel(void){FreePool();sCatalogSpecies=SPECIES_NONE;}
