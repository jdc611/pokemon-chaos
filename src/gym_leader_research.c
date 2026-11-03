#include "global.h"
#include "gym_leader.h"
#include "gym_leader_research.h"
#include "gym_leader_script.h"
#include "battle.h"
#include "battle_setup.h"
#include "event_data.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "script.h"
#include "string_util.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/maps.h"

#define WINGS_PROJECT 12
#define WINGS_FIRST_CAPTURE 73
#define WINGS_LOCAL_ID 250
struct GleMigration {u16 species,map;u8 x,y,rank;const u8 *location;};
static const struct GleMigration sMigrations[]={
    {SPECIES_ARTICUNO,MAP_SEAFOAM_ISLANDS_B3F,14,18,3,COMPOUND_STRING("SEAFOAM B3F")},
    {SPECIES_ZAPDOS,MAP_POWER_PLANT,41,24,4,COMPOUND_STRING("POWER PLANT")},
    {SPECIES_MOLTRES,MAP_POKEMON_MANSION_3F,30,7,5,COMPOUND_STRING("MANSION 3F")},
    {SPECIES_ARTICUNO_GALAR,MAP_ROCK_TUNNEL_B1F,39,27,6,COMPOUND_STRING("ROCK TUNNEL B1F")},
    {SPECIES_ZAPDOS_GALAR,MAP_ROUTE17,17,93,7,COMPOUND_STRING("CYCLING ROAD")},
    {SPECIES_MOLTRES_GALAR,MAP_VIRIDIAN_FOREST,43,7,8,COMPOUND_STRING("VIRIDIAN FOREST")}
};
static const u8 *const sMigrationNames[]={COMPOUND_STRING("ARTICUNO"),COMPOUND_STRING("ZAPDOS"),COMPOUND_STRING("MOLTRES"),
    COMPOUND_STRING("GALARIAN ARTICUNO"),COMPOUND_STRING("GALARIAN ZAPDOS"),COMPOUND_STRING("GALARIAN MOLTRES")};
static EWRAM_DATA struct MapEvents sResearchEvents={0};
static EWRAM_DATA struct ObjectEventTemplate sResearchObjects[32]={0};
static EWRAM_DATA u8 sWingsSlot=0;
extern const u8 GleWings_Approach[];
static u16 CurrentMap(void){return (u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8);}
static u32 CurrentMigration(void)
{
    u32 i;for(i=0;i<ARRAY_COUNT(sMigrations);i++)if(sMigrations[i].map==CurrentMap())return i;
    return GLE_NONE;
}
static bool32 Captured(u32 slot)
{
    u32 bit=WINGS_FIRST_CAPTURE+slot;
    return slot<ARRAY_COUNT(sMigrations) && (GymLeaderState()->uniqueObtained[bit/8]&(1u<<(bit%8)));
}
static u32 CaptureCount(void)
{
    u32 i,count=0;for(i=0;i<ARRAY_COUNT(sMigrations);i++)count+=!!Captured(i);
    return count;
}
void GymLeaderConfigureResearchMap(struct MapHeader *header)
{
    struct GleState *s=GymLeaderState();u32 slot=CurrentMigration(),count=header->events->objectEventCount;
    const struct GleMigration *m;
    if(!IsGymLeaderRun() || slot>=ARRAY_COUNT(sMigrations) || !s->projectUnlocked[WINGS_PROJECT]
        || s->rank<sMigrations[slot].rank || Captured(slot) || count>=ARRAY_COUNT(sResearchObjects))return;
    m=&sMigrations[slot];sResearchEvents=*header->events;
    memcpy(sResearchObjects,header->events->objectEvents,count*sizeof(*sResearchObjects));
    sResearchObjects[count]=(struct ObjectEventTemplate){.localId=WINGS_LOCAL_ID,.graphicsId=OBJ_EVENT_MON|m->species,
        .x=m->x,.y=m->y,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleWings_Approach};
    sResearchEvents.objectEvents=sResearchObjects;sResearchEvents.objectEventCount=count+1;header->events=&sResearchEvents;
}
void GleWingsNotebook(void)
{
    struct GleState *s=GymLeaderState();gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || s->rank<3)return;
    s->projectUnlocked[WINGS_PROJECT]=1;
    s->projectStage[WINGS_PROJECT]=CaptureCount();
    ConvertIntToDecimalStringN(gStringVar1,s->projectStage[WINGS_PROJECT],STR_CONV_MODE_LEFT_ALIGN,1);
    gSpecialVar_Result=1;
}
void GleWingsMenu(void)
{
    const u8 *names[7];u32 i;
    for(i=0;i<ARRAY_COUNT(sMigrations);i++)names[i]=sMigrationNames[i];
    names[6]=COMPOUND_STRING("Back");GymLeaderMenu(names,ARRAY_COUNT(names),FALSE);
}
void GleWingsReport(void)
{
    u32 slot=gSpecialVar_Result;gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || slot>=ARRAY_COUNT(sMigrations))return;
    StringCopy(gStringVar1,sMigrationNames[slot]);
    StringCopy(gStringVar2,sMigrations[slot].location);
    ConvertIntToDecimalStringN(gStringVar3,sMigrations[slot].rank,STR_CONV_MODE_LEFT_ALIGN,1);
    gSpecialVar_Result=Captured(slot);
}
void GleWingsInfo(void)
{
    u32 slot=CurrentMigration();gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || slot>=ARRAY_COUNT(sMigrations))return;
    sWingsSlot=slot;
    StringCopy(gStringVar1,sMigrationNames[slot]);
    ConvertIntToDecimalStringN(gStringVar2,GymLeaderLevelCap(),STR_CONV_MODE_LEFT_ALIGN,3);
    ConvertIntToDecimalStringN(gStringVar3,sMigrations[slot].rank,STR_CONV_MODE_LEFT_ALIGN,1);
    if(Captured(slot)){gSpecialVar_Result=2;return;}
    if(GymLeaderState()->projectUnlocked[WINGS_PROJECT] && GymLeaderState()->rank>=sMigrations[slot].rank)gSpecialVar_Result=1;
}
u32 GymLeaderPrepareWingsBattle(void)
{
    u32 slot=sWingsSlot;
    if(!IsGymLeaderRun() || slot>=ARRAY_COUNT(sMigrations) || CurrentMap()!=sMigrations[slot].map
        || !GymLeaderState()->projectUnlocked[WINGS_PROJECT] || GymLeaderState()->rank<sMigrations[slot].rank || Captured(slot))return 0;
    ZeroEnemyPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][0],sMigrations[slot].species,GymLeaderLevelCap(),
        Random32(),OTID_STRUCT_PLAYER_ID,USE_RANDOM_IVS);
    GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][0]);gPartiesCount[B_TRAINER_OPPONENT_A]=1;
    return 1;
}
void GleWingsStart(void)
{
    if(!GymLeaderPrepareWingsBattle()){gSpecialVar_Result=0;GymLeaderResumeScriptNextFrame();return;}
    BattleSetup_StartLegendaryBattle();
}
void GleWingsFinish(void)
{
    struct GleState *s=GymLeaderState();gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || sWingsSlot>=ARRAY_COUNT(sMigrations) || CurrentMap()!=sMigrations[sWingsSlot].map || gBattleOutcome!=B_OUTCOME_CAUGHT)return;
    if(GleUniqueCapture(s,WINGS_FIRST_CAPTURE+sWingsSlot)!=GLE_OK)return;
    s->projectStage[WINGS_PROJECT]=CaptureCount();gSpecialVar_Result=1;
}
