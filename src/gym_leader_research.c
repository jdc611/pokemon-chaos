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
extern const u8 GleWings_Approach[], GleTitans_Approach[];
#define TITANS_PROJECT 3
#define TITANS_CAPTURE 79
#define TITANS_DISCOVERY 85
#define TITANS_SEAL 90
static const struct GleMigration sTitans[]={
    {SPECIES_REGIROCK,MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER,11,8,4,COMPOUND_STRING("MONEAN CHAMBER")},
    {SPECIES_REGICE,MAP_SEVEN_ISLAND_TANOBY_RUINS_LIPTOO_CHAMBER,11,8,4,COMPOUND_STRING("LIPTOO CHAMBER")},
    {SPECIES_REGISTEEL,MAP_SEVEN_ISLAND_TANOBY_RUINS_WEEPTH_CHAMBER,11,8,4,COMPOUND_STRING("WEEPTH CHAMBER")},
    {SPECIES_REGIELEKI,MAP_SEVEN_ISLAND_TANOBY_RUINS_DILFORD_CHAMBER,11,8,4,COMPOUND_STRING("DILFORD CHAMBER")},
    {SPECIES_REGIDRAGO,MAP_SEVEN_ISLAND_TANOBY_RUINS_SCUFIB_CHAMBER,11,8,4,COMPOUND_STRING("SCUFIB CHAMBER")},
    {SPECIES_REGIGIGAS,MAP_SEVEN_ISLAND_TANOBY_RUINS_RIXY_CHAMBER,11,8,6,COMPOUND_STRING("RIXY CHAMBER")}
};
static EWRAM_DATA u8 sTitanSlot=0;
static bool32 ResearchBit(u32 bit)
{
    return !!(GymLeaderState()->uniqueObtained[bit/8]&(1u<<(bit%8)));
}
static u32 TitanDiscoveries(void)
{
    u32 i,n=0;for(i=0;i<5;i++)n+=ResearchBit(TITANS_DISCOVERY+i);return n;
}
static u32 CurrentTitan(void);
static bool32 TitanRank(u32 slot)
{
    const struct GleState *s=GymLeaderState();
    return slot<ARRAY_COUNT(sTitans) && max(s->rank,s->highestRank)>=sTitans[slot].rank;
}
static bool32 TitanAvailable(u32 slot)
{
    return TitanRank(slot) && (slot<5 || TitanDiscoveries()==5);
}
static u16 CurrentMap(void){return (u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8);}
static u32 CurrentTitan(void)
{
    u32 i;for(i=0;i<ARRAY_COUNT(sTitans);i++)if(sTitans[i].map==CurrentMap())return i;
    return GLE_NONE;
}
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
/* Research access is earned permanently; battle strength still uses the
 * current rank cap. Include current rank for older saves/entry fixtures. */
static bool32 MigrationUnlocked(u32 slot)
{
    const struct GleState *s=GymLeaderState();
    return slot<ARRAY_COUNT(sMigrations) && s->projectUnlocked[WINGS_PROJECT]
        && (s->rank>=sMigrations[slot].rank || s->highestRank>=sMigrations[slot].rank);
}
static u32 CaptureCount(void)
{
    u32 i,count=0;for(i=0;i<ARRAY_COUNT(sMigrations);i++)count+=!!Captured(i);
    return count;
}
void GymLeaderConfigureResearchMap(struct MapHeader *header)
{
    u32 slot,count;const struct GleMigration *m;const u8 *script;u8 localId;
    if(!IsGymLeaderRun() || header->events==NULL)return;
    count=header->events->objectEventCount;
    if(count>=ARRAY_COUNT(sResearchObjects))return;
    slot=CurrentTitan();
    if(slot<ARRAY_COUNT(sTitans))
    {
        struct GleState *s=GymLeaderState();
        if(slot<5)GleUniqueCapture(s,TITANS_DISCOVERY+slot);
        s->projectUnlocked[TITANS_PROJECT]=1;
        s->projectStage[TITANS_PROJECT]=TitanDiscoveries();
        if(ResearchBit(TITANS_CAPTURE+slot))return;
        m=&sTitans[slot];script=GleTitans_Approach;localId=251;
    }
    else
    {
        slot=CurrentMigration();
        if(!MigrationUnlocked(slot) || Captured(slot))return;
        m=&sMigrations[slot];script=GleWings_Approach;localId=WINGS_LOCAL_ID;
    }
    sResearchEvents=*header->events;
    memcpy(sResearchObjects,header->events->objectEvents,count*sizeof(*sResearchObjects));
    sResearchObjects[count]=(struct ObjectEventTemplate){.localId=localId,.graphicsId=OBJ_EVENT_MON|m->species,
        .x=m->x,.y=m->y,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=script};
    sResearchEvents.objectEvents=sResearchObjects;sResearchEvents.objectEventCount=count+1;header->events=&sResearchEvents;
}
void GleWingsNotebook(void)
{
    struct GleState *s=GymLeaderState();gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || (s->rank<3 && s->highestRank<3 && !s->projectUnlocked[WINGS_PROJECT]))return;
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
    if(MigrationUnlocked(slot))gSpecialVar_Result=1;
}
u32 GymLeaderPrepareWingsBattle(void)
{
    u32 slot=sWingsSlot;
    if(!IsGymLeaderRun() || slot>=ARRAY_COUNT(sMigrations) || CurrentMap()!=sMigrations[slot].map
        || !MigrationUnlocked(slot) || Captured(slot))return 0;
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

// Tanoby inscriptions are environmental discoveries, independent of catches.
void GleTitansNotebook(void)
{
    struct GleState *s=GymLeaderState();
    s->projectStage[TITANS_PROJECT]=TitanDiscoveries();
    ConvertIntToDecimalStringN(gStringVar1,TitanDiscoveries(),STR_CONV_MODE_LEFT_ALIGN,1);
}
void GleTitansMenu(void)
{
    const u8 *names[7];u32 i;
    for(i=0;i<6;i++)names[i]=sTitans[i].location;
    names[6]=COMPOUND_STRING("Back");GymLeaderMenu(names,7,FALSE);
}
void GleTitansReport(void)
{
    u32 slot=gSpecialVar_Result;
    if(slot>=6)return;
    StringCopy(gStringVar1,GetSpeciesName(sTitans[slot].species));
    StringCopy(gStringVar2,ResearchBit(TITANS_CAPTURE+slot)?COMPOUND_STRING("CAPTURE RECORDED"):
        slot<5 && ResearchBit(TITANS_DISCOVERY+slot)?COMPOUND_STRING("CHAMBER DISCOVERED"):COMPOUND_STRING("AWAITING EXPLORATION"));
    ConvertIntToDecimalStringN(gStringVar3,sTitans[slot].rank,STR_CONV_MODE_LEFT_ALIGN,1);
}
void GleTitansInfo(void)
{
    u32 slot=CurrentTitan();gSpecialVar_Result=0;sTitanSlot=slot;
    if(!IsGymLeaderRun() || slot>=6)return;
    StringCopy(gStringVar1,GetSpeciesName(sTitans[slot].species));
    ConvertIntToDecimalStringN(gStringVar2,GymLeaderLevelCap(),STR_CONV_MODE_LEFT_ALIGN,3);
    ConvertIntToDecimalStringN(gStringVar3,sTitans[slot].rank,STR_CONV_MODE_LEFT_ALIGN,1);
    if(ResearchBit(TITANS_CAPTURE+slot)){gSpecialVar_Result=2;return;}
    if(!TitanRank(slot))return;
    if(!TitanAvailable(slot)){gSpecialVar_Result=4;return;}
    gSpecialVar_Result=slot<5 && !ResearchBit(TITANS_SEAL+slot)?3:1;
}
static const u8 *const sSealClues[]={
    COMPOUND_STRING("The inscription reads:\nMountains crumble, yet I remain.\lName the titan's enduring body."),
    COMPOUND_STRING("The inscription reads:\nI sleep beneath the frozen sea.\lWhat stills the rushing water?"),
    COMPOUND_STRING("The inscription reads:\nForged and tempered, never rusted.\lName this titan's armor."),
    COMPOUND_STRING("The inscription reads:\nI race through wire, unseen.\lWhat wakes the ancient generator?"),
    COMPOUND_STRING("The inscription reads:\nA dragon's heart fuels my body.\lWhat power does this seal honor?")
};
static const u8 *const sSealAnswers[5][3]={
    {COMPOUND_STRING("Stone"),COMPOUND_STRING("Mist"),COMPOUND_STRING("Leaves")},
    {COMPOUND_STRING("Flame"),COMPOUND_STRING("Ice"),COMPOUND_STRING("Sand")},
    {COMPOUND_STRING("Wood"),COMPOUND_STRING("Glass"),COMPOUND_STRING("Steel")},
    {COMPOUND_STRING("Electricity"),COMPOUND_STRING("Moonlight"),COMPOUND_STRING("Rain")},
    {COMPOUND_STRING("Ocean"),COMPOUND_STRING("Dragon"),COMPOUND_STRING("Forest")}
};
void GleTitansClue(void)
{
    if(sTitanSlot<5)StringCopy(gStringVar4,sSealClues[sTitanSlot]);
}
void GleTitansSealMenu(void)
{
    if(sTitanSlot<5)GymLeaderMenu(sSealAnswers[sTitanSlot],3,FALSE);
    else GymLeaderResumeScriptNextFrame();
}
void GleTitansSolve(void)
{
    static const u8 answers[]={0,1,2,0,1};
    u32 choice=gSpecialVar_Result;gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || sTitanSlot>=5 || CurrentTitan()!=sTitanSlot || !TitanAvailable(sTitanSlot)
        || choice!=answers[sTitanSlot])return;
    GleUniqueCapture(GymLeaderState(),TITANS_SEAL+sTitanSlot);gSpecialVar_Result=1;
}
u32 GymLeaderPrepareTitanBattle(void)
{
    u32 slot=sTitanSlot;
    if(!IsGymLeaderRun() || slot>=6 || CurrentTitan()!=slot || !TitanAvailable(slot)
        || ResearchBit(TITANS_CAPTURE+slot) || (slot<5 && !ResearchBit(TITANS_SEAL+slot)))return 0;
    ZeroEnemyPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][0],sTitans[slot].species,GymLeaderLevelCap(),
        Random32(),OTID_STRUCT_PLAYER_ID,USE_RANDOM_IVS);
    GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][0]);gPartiesCount[B_TRAINER_OPPONENT_A]=1;
    return 1;
}
void GleTitansStart(void)
{
    if(!GymLeaderPrepareTitanBattle()){gSpecialVar_Result=0;GymLeaderResumeScriptNextFrame();return;}
    BattleSetup_StartLegendaryBattle();
}
void GleTitansFinish(void)
{
    gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || sTitanSlot>=6 || CurrentTitan()!=sTitanSlot || gBattleOutcome!=B_OUTCOME_CAUGHT)return;
    if(GleUniqueCapture(GymLeaderState(),TITANS_CAPTURE+sTitanSlot)==GLE_OK)gSpecialVar_Result=1;
}
