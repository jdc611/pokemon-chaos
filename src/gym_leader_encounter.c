#include "global.h"
#include "gym_leader.h"
#include "gym_leader_script.h"
#include "gym_leader_finale.h"
#include "battle.h"
#include "battle_setup.h"
#include "event_data.h"
#include "pokemon.h"
#include "overworld.h"
#include "field_screen_effect.h"
#include "random.h"
#include "script.h"
#include "string_util.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/maps.h"

static const u16 sEncounterMaps[4]={MAP_SEAFOAM_ISLANDS_B4F,MAP_POWER_PLANT,MAP_MT_EMBER_SUMMIT,MAP_CERULEAN_CAVE_B1F};
static const u16 sHideFlags[4]={FLAG_HIDE_ARTICUNO,FLAG_HIDE_ZAPDOS,FLAG_HIDE_MOLTRES,FLAG_HIDE_MEWTWO};
static const u16 sFoughtFlags[4]={FLAG_FOUGHT_ARTICUNO,FLAG_FOUGHT_ZAPDOS,FLAG_FOUGHT_MOLTRES,FLAG_FOUGHT_MEWTWO};
static const u8 sRanks[4]={3,4,6,8};
static const u8 sLocalIds[4]={3,6,1,3};
static EWRAM_DATA u8 sExclusiveSlot=0;
static EWRAM_DATA u8 sExclusiveType=0;
static EWRAM_DATA struct MapEvents sExclusiveEvents={0};
static EWRAM_DATA struct ObjectEventTemplate sExclusiveObjects[12]={0};
static u16 CurrentMap(void) { return (u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8); }
static bool32 Obtained(u32 type,u32 slot)
{
    u32 bit=type*4+slot;
    return (GymLeaderState()->uniqueObtained[bit/8]>>(bit%8))&1;
}
void GymLeaderConfigureExclusiveMap(struct MapHeader *header)
{
    u32 slot,i;u16 species;
    if(!IsGymLeaderRun() || GymLeaderState()->gymType>=GLE_EXCLUSIVE_TYPES)return;
    for(slot=0;slot<4;slot++)if(CurrentMap()==sEncounterMaps[slot])break;
    if(slot==4 || header->events->objectEventCount>ARRAY_COUNT(sExclusiveObjects))return;
    sExclusiveEvents=*header->events;
    memcpy(sExclusiveObjects,header->events->objectEvents,header->events->objectEventCount*sizeof(*sExclusiveObjects));
    species=GymLeaderExclusiveSpecies(GymLeaderState()->gymType,slot);
    for(i=0;i<header->events->objectEventCount;i++)
        if(sExclusiveObjects[i].localId==sLocalIds[slot])sExclusiveObjects[i].graphicsId=GymLeaderFinalOakComplete()?OBJ_EVENT_GFX_SCIENTIST_2:OBJ_EVENT_MON|species;
    sExclusiveEvents.objectEvents=sExclusiveObjects;header->events=&sExclusiveEvents;
    if(!GymLeaderFinalOakComplete() && Obtained(GymLeaderState()->gymType,slot)){FlagSet(sHideFlags[slot]);FlagSet(sFoughtFlags[slot]);}
    else {FlagClear(sHideFlags[slot]);FlagClear(sFoughtFlags[slot]);}
}
static void ExclusiveInfo(u32 slot,u32 type)
{
    gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || GymLeaderState()->gymType>=GLE_EXCLUSIVE_TYPES || type>=18 || slot>=4 || CurrentMap()!=sEncounterMaps[slot])return;
    sExclusiveSlot=slot;sExclusiveType=type;
    StringCopy(gStringVar1,GetSpeciesName(GymLeaderExclusiveSpecies(type,slot)));
    ConvertIntToDecimalStringN(gStringVar2,GymLeaderLevelCap(),STR_CONV_MODE_LEFT_ALIGN,3);
    ConvertIntToDecimalStringN(gStringVar3,sRanks[slot],STR_CONV_MODE_LEFT_ALIGN,1);
    gSpecialVar_Result=Obtained(type,slot)?2:!GymLeaderFinalOakComplete() && GymLeaderState()->rank<sRanks[slot]?0:1;
}
u32 GymLeaderPrepareExclusiveBattle(void)
{
    enum Species species;
    if(!IsGymLeaderRun() || GymLeaderState()->gymType>=GLE_EXCLUSIVE_TYPES || sExclusiveSlot>=4 || CurrentMap()!=sEncounterMaps[sExclusiveSlot]
        || sExclusiveType>=18 || (sExclusiveType!=GymLeaderState()->gymType && !GymLeaderFinalOakComplete())
        || Obtained(sExclusiveType,sExclusiveSlot) || (!GymLeaderFinalOakComplete() && GymLeaderState()->rank<sRanks[sExclusiveSlot]))return 0;
    species=GymLeaderExclusiveSpecies(sExclusiveType,sExclusiveSlot);
    ZeroEnemyPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][0],species,GymLeaderLevelCap(),
        Random32(),OTID_STRUCT_PLAYER_ID,USE_RANDOM_IVS);
    GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][0]);
    gPartiesCount[B_TRAINER_OPPONENT_A]=1;
    return 1;
}
void GleExclusiveStart(void)
{
    if(!GymLeaderPrepareExclusiveBattle()){gSpecialVar_Result=0;GymLeaderResumeScriptNextFrame();return;}
    BattleSetup_StartLegendaryBattle();
}
void GleExclusiveFinish(void)
{
    gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || GymLeaderState()->gymType>=GLE_EXCLUSIVE_TYPES || sExclusiveSlot>=4 || sExclusiveType>=18 || (sExclusiveType!=GymLeaderState()->gymType && !GymLeaderFinalOakComplete()) || CurrentMap()!=sEncounterMaps[sExclusiveSlot])return;
    if((gBattleOutcome&0x7F)==B_OUTCOME_CAUGHT)
    {
        GleUniqueCapture(GymLeaderState(),sExclusiveType*4+sExclusiveSlot);
        if(!GymLeaderFinalOakComplete()){FlagSet(sHideFlags[sExclusiveSlot]);FlagSet(sFoughtFlags[sExclusiveSlot]);}
        gSpecialVar_Result=1;
    }
}

void GleExclusiveRefresh(void)
{
    gSpecialVar_0x8004=sExclusiveSlot;
    ExclusiveInfo(sExclusiveSlot,sExclusiveType);
}
void GleExclusiveInfo(void){ExclusiveInfo(gSpecialVar_0x8004,GymLeaderState()->gymType);}
void GleExclusivePostgame(void){gSpecialVar_Result=GymLeaderFinalOakComplete();}
void GleExclusiveSelect(void)
{
    u32 type=gSpecialVar_Result;gSpecialVar_Result=0;
    if(GymLeaderFinalOakComplete() && type<18)ExclusiveInfo(gSpecialVar_0x8004,type);
}
void GleExpeditionMenu(void)
{
    static const u8 *const names[]={COMPOUND_STRING("Seafoam Islands"),COMPOUND_STRING("Power Plant"),COMPOUND_STRING("Mt. Ember"),COMPOUND_STRING("Cerulean Cave"),COMPOUND_STRING("Tanoby Ruins"),COMPOUND_STRING("Back")};
    GymLeaderMenu(names,ARRAY_COUNT(names),FALSE);
}
void GleExpeditionTravel(void)
{
    static const u16 maps[]={MAP_SEAFOAM_ISLANDS_1F,MAP_POWER_PLANT,MAP_MT_EMBER_EXTERIOR,MAP_CERULEAN_CAVE_1F,MAP_SEVEN_ISLAND_TANOBY_RUINS};
    static const u8 positions[][2]={{6,20},{4,38},{28,47},{33,20},{35,6}};
    u32 slot=gSpecialVar_Result,rank=slot==4?4:slot<4?sRanks[slot]:0;
    if(!IsGymLeaderRun() || slot>=5){gSpecialVar_Result=0;return;}
    ConvertIntToDecimalStringN(gStringVar3,rank,STR_CONV_MODE_LEFT_ALIGN,1);
    if(max(GymLeaderState()->rank,GymLeaderState()->highestRank)<rank){gSpecialVar_Result=0;return;}
    SetWarpDestination(MAP_GROUP(maps[slot]),MAP_NUM(maps[slot]),-1,positions[slot][0],positions[slot][1]);
    DoWarp();gSpecialVar_Result=1;
}
