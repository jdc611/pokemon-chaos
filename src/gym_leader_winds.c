#include "global.h"
#include "gym_leader.h"
#include "gym_leader_winds.h"
#include "battle.h"
#include "event_data.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "region_map.h"
#include "roamer.h"
#include "string_util.h"
#include "wild_encounter_ow.h"
#include "constants/maps.h"

// One native encounter slot is multiplexed over four independent persistent
// signals. Changing the tracked signal never rerolls an initialized Pokémon.
static const u16 sSpecies[]={SPECIES_TORNADUS,SPECIES_THUNDURUS,SPECIES_LANDORUS,SPECIES_ENAMORUS};
static const u16 sWindRoutes[]={MAP_ROUTE1,MAP_ROUTE2,MAP_ROUTE3,MAP_ROUTE4,MAP_ROUTE5,MAP_ROUTE6,MAP_ROUTE7,MAP_ROUTE8,MAP_ROUTE9,MAP_ROUTE10,MAP_ROUTE11,MAP_ROUTE12,MAP_ROUTE13,MAP_ROUTE14,MAP_ROUTE15,MAP_ROUTE16,MAP_ROUTE17,MAP_ROUTE18,MAP_ROUTE21_NORTH,MAP_ROUTE22,MAP_ROUTE24,MAP_ROUTE25};
#define WIND_STATE(w) ((w)->flags&3)
#define WIND_SHINY 4
static struct GleWind *Selected(void){struct GleWindsState *w=&GymLeaderState()->winds;return w->selected<4?&w->mon[w->selected]:NULL;}
bool32 GymLeaderWindSpecies(u16 species){u32 i;for(i=0;i<4;i++)if(sSpecies[i]==species)return TRUE;return FALSE;}
static void Repack(void)
{
    struct GleWind *w=Selected();struct Roamer *r=&gSaveBlock1Ptr->roamer[0];u32 index=GymLeaderState()->winds.selected;
    memset(r,0,sizeof(*r));if(!w||index>=4)return;
    r->species=sSpecies[index];r->ivs=w->ivs;r->personality=w->personality;r->hp=w->hp;r->level=w->level;
    r->statusA=w->status;r->statusB=w->status>>8;r->shiny=!!(w->flags&WIND_SHINY);r->active=WIND_STATE(w)==1;
}
static void Move(struct GleWind *w)
{
    u32 next,i;if(WIND_STATE(w)!=1)return;
    next=Random()%ARRAY_COUNT(sWindRoutes);
    for(i=0;i<ARRAY_COUNT(sWindRoutes);i++){
        u16 map=sWindRoutes[(next+i)%ARRAY_COUNT(sWindRoutes)];
        if(w->mapGroup!=MAP_GROUP(map)||w->mapNum!=MAP_NUM(map)){w->mapGroup=MAP_GROUP(map);w->mapNum=MAP_NUM(map);return;}
    }
}
void GymLeaderMoveWinds(void)
{
    struct GleWindsState *s;u32 i;u8 group,num;
    if(!IsGymLeaderRun())return;s=&GymLeaderState()->winds;
    group=gSaveBlock1Ptr->location.mapGroup;num=gSaveBlock1Ptr->location.mapNum;
    // Reserved metadata caches the last actual map transition. Loading the
    // same map twice, including Continue, must not reroll saved locations.
    if(s->reserved[2]&&s->reserved[0]==group&&s->reserved[1]==num){Repack();return;}
    s->reserved[0]=group;s->reserved[1]=num;s->reserved[2]=1;
    for(i=0;i<4;i++){
        struct GleWind *w=&s->mon[i];
        // Arrival leaves an encounter opportunity. Other signals sometimes
        // move on a transition; fleeing a battle always moves that wind.
        if(WIND_STATE(w)==1&&(w->mapGroup!=group||w->mapNum!=num)&&Random()%4==0)Move(w);
    }
    Repack();
}
bool32 GymLeaderWindMove(u32 index){struct GleWind *w;if(!IsGymLeaderRun())return FALSE;w=Selected();if(index==0&&w)Move(w);Repack();return TRUE;}
bool32 GymLeaderWindAt(u8 group,u8 num){struct GleWind *w=Selected();return w&&WIND_STATE(w)==1&&group==w->mapGroup&&num==w->mapNum;}
void GymLeaderWindLocation(u8 *group,u8 *num){struct GleWind *w=Selected();*group=w?w->mapGroup:MAP_GROUP(MAP_UNDEFINED);*num=w?w->mapNum:MAP_NUM(MAP_UNDEFINED);}
bool32 GymLeaderCreateWind(void)
{
    struct GleWind *w=Selected();struct Pokemon *mon=&gParties[B_TRAINER_OPPONENT_A][0];u16 maxHp,hp;u8 shiny;u32 status;
    if(!w||WIND_STATE(w)!=1)return FALSE;
    ZeroEnemyPartyMons();CreateMonWithIVsPersonality(mon,sSpecies[GymLeaderState()->winds.selected],GymLeaderLevelCap(),w->ivs,w->personality);GiveMonInitialMoveset(mon);
    shiny=!!(w->flags&WIND_SHINY);SetMonData(mon,MON_DATA_IS_SHINY,&shiny);maxHp=GetMonData(mon,MON_DATA_MAX_HP);
    hp=w->maxHp?min(maxHp,max(1,((u32)w->hp*maxHp+w->maxHp-1)/w->maxHp)):maxHp;status=w->status;
    SetMonData(mon,MON_DATA_HP,&hp);SetMonData(mon,MON_DATA_STATUS,&status);w->hp=hp;w->maxHp=maxHp;w->level=GymLeaderLevelCap();
    gPartiesCount[B_TRAINER_OPPONENT_A]=1;Repack();return TRUE;
}
void GymLeaderUpdateWind(struct Pokemon *mon)
{
    struct GleState *s=GymLeaderState();struct GleWind *w=Selected();u32 outcome=gBattleOutcome&0x7f;if(!w)return;
    w->hp=GetMonData(mon,MON_DATA_HP);w->maxHp=GetMonData(mon,MON_DATA_MAX_HP);w->status=GetMonData(mon,MON_DATA_STATUS);w->level=GetMonData(mon,MON_DATA_LEVEL);
    if(outcome==B_OUTCOME_CAUGHT){w->flags=(w->flags&WIND_SHINY)|3;GleUniqueCapture(s,126+s->winds.selected);}
    else if(!w->hp||outcome==B_OUTCOME_WON||outcome==B_OUTCOME_DREW)w->flags=(w->flags&WIND_SHINY)|2;
    else Move(w);
    Repack();
}
bool32 GymLeaderWindKeepActive(void){struct GleWind *w=Selected();return gBattleOutcome==B_OUTCOME_PLAYER_TELEPORTED&&w&&WIND_STATE(w)==1;}
static bool32 Track(u32 index)
{
    struct GleState *s=GymLeaderState();struct GleWind *w;struct Roamer *r=&gSaveBlock1Ptr->roamer[0];struct Pokemon *mon=&gParties[B_TRAINER_OPPONENT_A][0];
    if(index>=4||!(s->uniqueObtained[(196+index)/8]&(1u<<((196+index)%8)))||!(max(s->rank,s->highestRank)>=3+index))return FALSE;
    w=&s->winds.mon[index];if(WIND_STATE(w)==3||s->uniqueObtained[(126+index)/8]&(1u<<((126+index)%8)))return FALSE;
    if(!WIND_STATE(w)){
        r->active=FALSE;if(!TryAddRoamer(sSpecies[index],GymLeaderLevelCap()))return FALSE;
        w->ivs=r->ivs;w->personality=r->personality;w->hp=r->hp;w->maxHp=GetMonData(mon,MON_DATA_MAX_HP);w->level=r->level;
        w->flags=1|(r->shiny?WIND_SHINY:0);Move(w);
    }else if(WIND_STATE(w)==2){w->hp=w->maxHp;w->status=0;w->flags=(w->flags&WIND_SHINY)|1;Move(w);}
    if(s->winds.selected!=index)DespawnTrackedRoamerOWE();
    s->winds.selected=index;Repack();return TRUE;
}
void GleWindsTrack(void){extern u32 GymLeaderFieldSelected(void);u32 slot=GymLeaderFieldSelected();gSpecialVar_Result=IsGymLeaderRun()&&slot>=31&&slot<35&&Track(slot-31);}
void GleWindsCanTrack(void){extern u32 GymLeaderFieldSelected(void);u32 slot=GymLeaderFieldSelected(),bit=165+slot;gSpecialVar_Result=IsGymLeaderRun()&&slot>=31&&slot<35&&(GymLeaderState()->uniqueObtained[bit/8]&(1u<<(bit%8)))&&WIND_STATE(&GymLeaderState()->winds.mon[slot-31])!=3;}
bool32 GymLeaderWindDescription(u32 index,u8 *dest)
{
    struct GleWind *w;u8 name[MAP_NAME_LENGTH+1];const struct MapHeader *map;if(!IsGymLeaderRun()||index>=4)return FALSE;w=&GymLeaderState()->winds.mon[index];if(!WIND_STATE(w))return FALSE;
    StringCopy(dest,GetSpeciesName(sSpecies[index]));StringAppend(dest,COMPOUND_STRING(": "));
    if(WIND_STATE(w)==3)StringAppend(dest,COMPOUND_STRING("CAUGHT"));
    else if(WIND_STATE(w)==2)StringAppend(dest,COMPOUND_STRING("RESTING"));
    else{map=Overworld_GetMapHeaderByGroupAndId(w->mapGroup,w->mapNum);GetMapName(name,map->regionMapSectionId,0);StringAppend(dest,name);}
    return TRUE;
}

bool32 GymLeaderWindCaption(u8 *dest){return IsGymLeaderRun()&&GymLeaderWindDescription(GymLeaderState()->winds.selected,dest);}
