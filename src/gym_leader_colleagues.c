#include "global.h"
#include "gym_leader_news.h"
#include "gym_leader.h"
#include "gym_leader_colleagues.h"
#include "gym_leader_competitions_native.h"
#include "battle.h"
#include "event_data.h"
#include "malloc.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "string_util.h"
#include "constants/maps.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/battle_ai.h"

// The saved indices are append-only: seven Kanto colleagues, then Elite Four.
static const u8 sCatalogTypes[]={12,2,3,4,7,10,1,5,6,13,14};
static const u8 *const sNames[]={COMPOUND_STRING("BROCK"),COMPOUND_STRING("MISTY"),COMPOUND_STRING("LT. SURGE"),COMPOUND_STRING("ERIKA"),COMPOUND_STRING("KOGA"),COMPOUND_STRING("SABRINA"),COMPOUND_STRING("BLAINE"),COMPOUND_STRING("LORELEI"),COMPOUND_STRING("BRUNO"),COMPOUND_STRING("AGATHA"),COMPOUND_STRING("LANCE")};
static const u16 sPictures[]={TRAINER_PIC_LEADER_BROCK_FRLG,TRAINER_PIC_LEADER_MISTY_FRLG,TRAINER_PIC_LEADER_LT_SURGE_FRLG,TRAINER_PIC_LEADER_ERIKA_FRLG,TRAINER_PIC_LEADER_KOGA_FRLG,TRAINER_PIC_LEADER_SABRINA_FRLG,TRAINER_PIC_LEADER_BLAINE_FRLG,TRAINER_PIC_ELITE_FOUR_LORELEI_FRLG,TRAINER_PIC_ELITE_FOUR_BRUNO_FRLG,TRAINER_PIC_ELITE_FOUR_AGATHA_FRLG,TRAINER_PIC_ELITE_FOUR_LANCE_FRLG};
static const u16 sGyms[]={MAP_PEWTER_CITY_GYM,MAP_CERULEAN_CITY_GYM,MAP_VERMILION_CITY_GYM,MAP_CELADON_CITY_GYM,MAP_FUCHSIA_CITY_GYM,MAP_SAFFRON_CITY_GYM,MAP_CINNABAR_ISLAND_GYM};
extern const u8 PewterCity_Gym_EventScript_Brock[],CeruleanCity_Gym_EventScript_Misty[],VermilionCity_Gym_EventScript_LtSurge[],CeladonCity_Gym_EventScript_Erika[],FuchsiaCity_Gym_EventScript_Koga[],SaffronCity_Gym_EventScript_Sabrina[],CinnabarIsland_Gym_EventScript_Blaine[];
static const u8 *const sOriginalScripts[]={PewterCity_Gym_EventScript_Brock,CeruleanCity_Gym_EventScript_Misty,VermilionCity_Gym_EventScript_LtSurge,CeladonCity_Gym_EventScript_Erika,FuchsiaCity_Gym_EventScript_Koga,SaffronCity_Gym_EventScript_Sabrina,CinnabarIsland_Gym_EventScript_Blaine};
extern const u8 GleChampion_NewsNpc[],GleHosting_Desk[];
extern const u8 GleColleague_Leader[],GleColleague_LeagueDesk[],GleCompetition_Desk[],GleCompetition_Trophy[],GleFinalOak_Desk[],GleRivalDouble_Desk[],PalletTown_ProfessorOaksLab_EventScript_ProfOak[];
static EWRAM_DATA u8 sColleagueId=0;
static u16 CurrentMap(void){return (u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8);}
static u32 Displaced(u32 town){return town==0?5:town==1?3:4;}
bool32 GymLeaderConfigureColleagues(struct MapEvents *events,struct ObjectEventTemplate *objects,u32 capacity)
{
    u32 i,j,count=events->objectEventCount;bool32 changed=FALSE;u16 map=CurrentMap();
    if(!IsGymLeaderRun())return FALSE;
    for(i=0;i<count;i++){const u8 *replacement=GymLeaderChampionNpcScript(objects[i].script);if(replacement!=objects[i].script){objects[i].script=replacement;changed=TRUE;}}
    for(i=0;i<7;i++)if(map==sGyms[i])
        for(j=0;j<count;j++)if(objects[j].script==sOriginalScripts[i])
        {objects[j].script=GleColleague_Leader;changed=TRUE;}
    if(map==MAP_GLE_LEAGUE_HALL)
        for(i=0;i<2 && count<capacity;i++)
        {
            u32 town=i?GymLeaderState()->rivalTown:GymLeaderState()->town;
            u16 graphics=town==0?OBJ_EVENT_GFX_SABRINA:town==1?OBJ_EVENT_GFX_ERIKA:OBJ_EVENT_GFX_KOGA;
            objects[count++]=(struct ObjectEventTemplate){.localId=230+i,.graphicsId=graphics,
                .x=3+i*6,.y=5,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleColleague_Leader};changed=TRUE;
        }
    if(map==MAP_INDIGO_PLATEAU_POKEMON_CENTER_1F && count<capacity)
    {
        objects[count++]=(struct ObjectEventTemplate){.localId=232,.graphicsId=OBJ_EVENT_GFX_COOLTRAINER_M,
            .x=2,.y=12,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleColleague_LeagueDesk};changed=TRUE;
    }
    if(map==MAP_GLE_LEAGUE_HALL && count<capacity){objects[count++]=(struct ObjectEventTemplate){.localId=233,.graphicsId=OBJ_EVENT_GFX_GENTLEMAN,.x=9,.y=8,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleCompetition_Desk};changed=TRUE;}
    if(map==MAP_GLE_GYM_INTERIOR && GymLeaderState()->openingStage==5)for(i=0;i<3&&count<capacity;i++)if(GymLeaderTrophyAt(i)){objects[count++]=(struct ObjectEventTemplate){.localId=236+i,.graphicsId=OBJ_EVENT_GFX_TRICK_HOUSE_STATUE,.x=2+i*4,.y=3,.elevation=3,.movementType=MOVEMENT_TYPE_NONE,.script=GleCompetition_Trophy};changed=TRUE;}
    if(map==MAP_PALLET_TOWN_PROFESSOR_OAKS_LAB && GymLeaderState()->openingStage==5)for(i=0;i<count;i++)if(objects[i].script==PalletTown_ProfessorOaksLab_EventScript_ProfOak){objects[i].script=GleFinalOak_Desk;objects[i].flagId=0;objects[i].x=6;objects[i].y=3;changed=TRUE;}
    if(map==MAP_GLE_LEAGUE_HALL && count<capacity){objects[count++]=(struct ObjectEventTemplate){.localId=234,.graphicsId=OBJ_EVENT_GFX_BLUE,.x=2,.y=8,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleRivalDouble_Desk};changed=TRUE;}
    if(map==MAP_GLE_LEAGUE_HALL && count<capacity){objects[count++]=(struct ObjectEventTemplate){.localId=235,.graphicsId=OBJ_EVENT_GFX_GENTLEMAN_FRLG,.x=7,.y=8,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleChampion_NewsNpc};changed=TRUE;}
    if(map==MAP_GLE_LEAGUE_HALL&&count<capacity){objects[count++]=(struct ObjectEventTemplate){.localId=236,.graphicsId=OBJ_EVENT_GFX_GENTLEMAN_FRLG,.x=6,.y=5,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleHosting_Desk};changed=TRUE;}
    events->objectEventCount=count;events->objectEvents=objects;return changed;
}
u32 GymLeaderColleagueSelected(void){return sColleagueId;}
bool32 GymLeaderColleagueAvailable(void)
{
    return IsGymLeaderRun() && sColleagueId<11 && !(GymLeaderState()->colleagueWins[sColleagueId]&(1u<<(GymLeaderState()->rank-1)));
}
static void CollectPending(u32 id)
{
    struct GleState *s=GymLeaderState();u32 rank;
    if(id<7 || id>=11)return;
    for(rank=1;rank<=8;rank++)
        if(s->colleaguePendingVouchers[id-7]&(1u<<(rank-1)))
        {
            if(GleAwardVoucher(s,GLE_STANDARD,rank)!=GLE_OK)return;
            s->colleaguePendingVouchers[id-7]&=~(1u<<(rank-1));
        }
}
void GleColleagueInfo(void)
{
    u32 i;u16 map=CurrentMap();gSpecialVar_Result=2;
    if(!IsGymLeaderRun())return;
    if(map==MAP_GLE_LEAGUE_HALL)
    {
        if(gSpecialVar_LastTalked<230 || gSpecialVar_LastTalked>231)return;
        sColleagueId=Displaced(gSpecialVar_LastTalked==230?GymLeaderState()->town:GymLeaderState()->rivalTown);
    }
    else if(map!=MAP_INDIGO_PLATEAU_POKEMON_CENTER_1F)
    {
        for(i=0;i<7;i++)if(map==sGyms[i])break;
        if(i>=7)return;sColleagueId=i;
    }
    if(sColleagueId>=11)return;
    CollectPending(sColleagueId);
    StringCopy(gStringVar1,sNames[sColleagueId]);
    ConvertIntToDecimalStringN(gStringVar2,GymLeaderLevelCap(),STR_CONV_MODE_LEFT_ALIGN,3);
    ConvertIntToDecimalStringN(gStringVar3,GymLeaderPartyLimit(),STR_CONV_MODE_LEFT_ALIGN,1);
    gSpecialVar_Result=GymLeaderColleagueAvailable()?0:1;
}
void GleColleagueLeagueMenu(void)
{
    const u8 *names[5];u32 i;for(i=0;i<4;i++)names[i]=sNames[7+i];names[4]=COMPOUND_STRING("Back");GymLeaderMenu(names,5,FALSE);
}
void GleColleagueLeagueSelect(void)
{
    u32 choice=gSpecialVar_Result;gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || CurrentMap()!=MAP_INDIGO_PLATEAU_POKEMON_CENTER_1F || choice>=4)return;
    sColleagueId=7+choice;gSpecialVar_Result=1;
}
static enum Species Ace(u32 id,u32 level)
{
    switch(id)
    {
    case 0:return SPECIES_ONIX;
    case 1:return level>=35?SPECIES_STARMIE:SPECIES_STARYU;
    case 2:return level>=35?SPECIES_RAICHU:SPECIES_PIKACHU;
    case 3:return level>=35?SPECIES_VILEPLUME:level>=21?SPECIES_GLOOM:SPECIES_ODDISH;
    case 4:return level>=31?SPECIES_VENOMOTH:SPECIES_VENONAT;
    case 5:return level>=36?SPECIES_ALAKAZAM:level>=16?SPECIES_KADABRA:SPECIES_ABRA;
    case 6:return level>=35?SPECIES_ARCANINE:SPECIES_GROWLITHE;
    case 7:return level>=30?SPECIES_JYNX:SPECIES_SMOOCHUM;
    case 8:return level>=40?SPECIES_MACHAMP:level>=28?SPECIES_MACHOKE:SPECIES_MACHOP;
    case 9:return level>=40?SPECIES_GENGAR:level>=25?SPECIES_HAUNTER:SPECIES_GASTLY;
    default:return level>=55?SPECIES_DRAGONITE:level>=30?SPECIES_DRAGONAIR:SPECIES_DRATINI;
    }
}
bool32 GymLeaderPrepareColleagueRoster(struct Trainer *trainer,struct TrainerMon *entries)
{
    struct GleState *s=GymLeaderState();u32 id=sColleagueId,level,count,seed,i,j,attempt;u16 *pool;
    if(!IsGymLeaderRun() || id>=11)return FALSE;
    pool=Alloc(GLE_SPECIES_LIMIT*sizeof(*pool));if(pool==NULL)return FALSE;
    count=GymLeaderBuildSpeciesPool(sCatalogTypes[id],s->rank,GLE_STANDARD,FALSE,pool,GLE_SPECIES_LIMIT);
    if(!count || count>GLE_SPECIES_LIMIT){Free(pool);return FALSE;}
    memset(trainer,0,sizeof(*trainer));memset(entries,0,PARTY_SIZE*sizeof(*entries));
    trainer->trainerClass=id<7?TRAINER_CLASS_LEADER_FRLG:TRAINER_CLASS_ELITE_FOUR;
    trainer->trainerPic=sPictures[id];trainer->gender=id==1 || id==3 || id==5 || id==7 || id==9;
    StringCopy(trainer->trainerName,sNames[id]);trainer->partySize=GymLeaderPartyLimit();trainer->party=entries;
    trainer->aiFlags=AI_FLAG_CHECK_BAD_MOVE|AI_FLAG_CHECK_VIABILITY|AI_FLAG_TRY_TO_FAINT;
    seed=GleMix(s->seed^((id+1)<<24)^0xC011EAu);level=GymLeaderLevelCap();ZeroEnemyPartyMons();
    for(i=0;i<trainer->partySize;i++)
    {
        enum Species species=Ace(id,level);struct Pokemon *mon=&gParties[B_TRAINER_OPPONENT_A][i];u8 ability=GleMix(seed^i)&1;
        if(i)for(attempt=0;attempt<count;attempt++)
        {
            species=pool[(GleMix(seed^i)%count+attempt)%count];
            for(j=0;j<i;j++)if(entries[j].species==species)break;
            if(j==i)break;
        }
        CreateMonWithIVs(mon,species,level,GleMix(seed^i^0xACEu),OTID_STRUCT_PRESET(seed),16+s->rank);
        if(gSpeciesInfo[species].abilities[ability]==ABILITY_NONE)ability=0;
        SetMonData(mon,MON_DATA_ABILITY_NUM,&ability);GiveMonInitialMoveset(mon);
        if(i==0 && s->rank>=4){u16 item=ITEM_SITRUS_BERRY;SetMonData(mon,MON_DATA_HELD_ITEM,&item);}
        entries[i].species=species;entries[i].lvl=level;
    }
    gPartiesCount[B_TRAINER_OPPONENT_A]=trainer->partySize;Free(pool);return TRUE;
}
void GymLeaderSettleColleague(bool32 won)
{
    struct GleState *s=GymLeaderState();u32 id=sColleagueId,bit=1u<<(s->rank-1);
    if(!IsGymLeaderRun() || id>=11 || !won || (s->colleagueWins[id]&bit))return;
    s->colleagueWins[id]|=bit;AddMoney(&gSaveBlock1Ptr->money,(id<7?400:600)*s->rank);
    s->reputation=min(65535,s->reputation+2);
    if(id>=7){s->colleaguePendingVouchers[id-7]|=bit;CollectPending(id);}
}

bool32 GymLeaderPrepareEventColleague(u32 id,struct Trainer *trainer,struct TrainerMon *entries){u32 old=sColleagueId;bool32 ok;if(id>=7)return FALSE;sColleagueId=id;ok=GymLeaderPrepareColleagueRoster(trainer,entries);sColleagueId=old;return ok;}
