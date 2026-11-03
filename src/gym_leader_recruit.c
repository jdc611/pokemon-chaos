#include "global.h"
#include "gym_leader.h"
#include "gym_leader_preview.h"
#include "gym_leader_recruit.h"
#include "gym_leader_script.h"
#include "battle_setup.h"
#include "data.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "constants/flags.h"
#include "malloc.h"
#include "money.h"
#include "overworld.h"
#include "main.h"
#include "palette.h"
#include "pokemon_summary_screen.h"
#include "task.h"
#include "field_screen_effect.h"
#include "constants/rgb.h"
#include "constants/characters.h"
#include "pokemon.h"
#include "script.h"
#include "string_util.h"
#include "constants/battle_ai.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/maps.h"
struct GleRecruitSource {u16 trainer,map,graphics;u8 localId,x,y;};
#include "data/gym_leader_recruit_sources.h"
static EWRAM_DATA struct MapEvents sStaffEvents={0};
static EWRAM_DATA struct ObjectEventTemplate sStaffObjects[32]={0};
static EWRAM_DATA u8 sPreviewCount=0;
static EWRAM_DATA u16 sRecruitTrainer=0;
static EWRAM_DATA bool8 sRecruitShown=FALSE;
static EWRAM_DATA u8 sStaffSlot=0;
static EWRAM_DATA u8 sStaffChoices[GLE_RECRUITS]={0};
static EWRAM_DATA u8 sStaffCount=0;
extern const u8 GleStaff_Member[];
static u16 CurrentMap(void){return (u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8);}
static const struct GleRecruitSource *Source(u16 trainer)
{
    u32 i;if(!trainer)return NULL;for(i=0;i<ARRAY_COUNT(sRecruitSources);i++)if(sRecruitSources[i].trainer==trainer)return &sRecruitSources[i];
    return NULL;
}
static s32 Record(u16 trainer)
{
    u32 i;for(i=0;i<GLE_RECRUITS;i++)if(GymLeaderState()->recruits[i].id==trainer)return i;
    return -1;
}
static u32 Potential(u16 trainer){return 1+GleMix(GymLeaderState()->seed^trainer^0x504F54u)%5;}
static bool32 Ace(u16 id)
{
    u32 c=GetTrainerClassFromId(id);
    return c==TRAINER_CLASS_COOLTRAINER || c==TRAINER_CLASS_COOLTRAINER_2 || c==TRAINER_CLASS_COOLTRAINER_FRLG || c==TRAINER_CLASS_COOL_COUPLE_FRLG;
}
static bool32 Eligible(u16 id)
{
    u32 c;
    if(!id || id>=TRAINERS_COUNT || !Source(id))return FALSE;
    c=GetTrainerClassFromId(id);
    if(c==TRAINER_CLASS_LEADER || c==TRAINER_CLASS_LEADER_FRLG || c==TRAINER_CLASS_ELITE_FOUR || c==TRAINER_CLASS_ELITE_FOUR_FRLG
        || c==TRAINER_CLASS_CHAMPION || c==TRAINER_CLASS_CHAMPION_FRLG || c==TRAINER_CLASS_RIVAL
        || c==TRAINER_CLASS_RIVAL_EARLY_FRLG || c==TRAINER_CLASS_RIVAL_LATE_FRLG || c==TRAINER_CLASS_TEAM_ROCKET_FRLG
        || c==TRAINER_CLASS_BOSS_FRLG || c==TRAINER_CLASS_PKMN_PROF_FRLG || c==TRAINER_CLASS_PLAYER_FRLG)return FALSE;
    return GleMix(GymLeaderState()->seed^id^0x454C49u)%5!=0;
}
static bool32 WillJoin(u16 id)
{
    struct GleState *s=GymLeaderState();u32 p=Potential(id),score;
    if(s->honor<=-100 && Ace(id))return FALSE;
    if(s->rank<1+(p>=4)+(p==5))return FALSE;
    score=min(95,35+7*s->rank+min(15,s->reputation/20)+(s->honor>=100?8:0));
    return GleMix(s->seed^id^0x4A4F49u)%100<score;
}
static u32 RosterCount(void)
{
    u32 i,n=0;for(i=0;i<GLE_RECRUITS;i++)n+=GymLeaderState()->recruits[i].id!=0;
    return n;
}
void GymLeaderRecruitResetOffer(void){sRecruitShown=FALSE;}
bool32 GymLeaderRecruitOffer(u16 trainer)
{
    s32 slot;
    if(!IsGymLeaderRun() || GymLeaderState()->openingStage!=5 || sRecruitShown || !trainer || trainer>=TRAINERS_COUNT || !HasTrainerBeenFought(trainer))return FALSE;
    slot=Record(trainer);
    if(slot>=0 && !GymLeaderState()->recruits[slot].away)return FALSE;
    if(!Eligible(trainer))return FALSE;
    sRecruitShown=TRUE;sRecruitTrainer=trainer;return TRUE;
}
bool32 GymLeaderRecruitPostBattleOffer(void)
{
    u16 trainer=(gWhichTrainerToFaceAfterBattle && TRAINER_BATTLE_PARAM.opponentB)?TRAINER_BATTLE_PARAM.opponentB:TRAINER_BATTLE_PARAM.opponentA;
    return GymLeaderRecruitOffer(trainer);
}
void GleRecruitInfo(void)
{
    StringCopy(gStringVar1,GetTrainerNameFromId(sRecruitTrainer));
    ConvertIntToDecimalStringN(gStringVar2,Potential(sRecruitTrainer),STR_CONV_MODE_LEFT_ALIGN,1);
    ConvertIntToDecimalStringN(gStringVar3,500+250*Potential(sRecruitTrainer),STR_CONV_MODE_LEFT_ALIGN,4);
    gSpecialVar_Result=Record(sRecruitTrainer)>=0?2:WillJoin(sRecruitTrainer)?1:0;
}
static u32 ReturnRecruit(u32 slot)
{
    struct GleState *s=GymLeaderState();struct GleRecruit *r;
    if(slot>=GLE_RECRUITS)return GLE_INVALID;
    r=&s->recruits[slot];
    if(!r->id || !r->away)return GLE_INVALID;
    if(s->rank<3+r->potential/2 || s->reputation<100 || (s->honor<=-100 && Ace(r->id))
        || GleMix(s->seed^r->id^0x524554u)%100>=65+3*s->rank)return GLE_NOT_READY;
    r->away=0;r->active=0;r->warnings=0;r->loyalty=max(r->loyalty,60);
    return GLE_OK;
}
void GleRecruitHire(void)
{
    struct GleState *s=GymLeaderState();s32 slot=Record(sRecruitTrainer);u32 p=Potential(sRecruitTrainer),fee=500+250*p;
    gSpecialVar_Result=GLE_NOT_READY;
    if(!IsGymLeaderRun() || !Eligible(sRecruitTrainer) || !HasTrainerBeenFought(sRecruitTrainer))return;
    if(slot>=0){gSpecialVar_Result=ReturnRecruit(slot);return;}
    if(!WillJoin(sRecruitTrainer))return;
    if(RosterCount()>=min(GLE_RECRUITS,s->rank+2)){gSpecialVar_Result=GLE_NO_ROOM;return;}
    if(!IsEnoughMoney(&gSaveBlock1Ptr->money,fee)){gSpecialVar_Result=GLE_NOT_ENOUGH_MONEY;return;}
    gSpecialVar_Result=GleRecruitTrainer(s,sRecruitTrainer,p,55+4*p+GleMix(s->seed^sRecruitTrainer^0x4C4F59u)%20);
    if(gSpecialVar_Result==GLE_OK)RemoveMoney(&gSaveBlock1Ptr->money,fee);
}
void GleStaffMenu(void)
{
    const u8 *names[GLE_RECRUITS+1];u32 i;sStaffCount=0;
    for(i=0;i<GLE_RECRUITS;i++)if(GymLeaderState()->recruits[i].id)
    {sStaffChoices[sStaffCount]=i;names[sStaffCount++]=GetTrainerNameFromId(GymLeaderState()->recruits[i].id);}
    names[sStaffCount]=COMPOUND_STRING("Back");GymLeaderMenu(names,sStaffCount+1,FALSE);
}
void GleStaffSelect(void)
{
    u32 choice=gSpecialVar_Result;gSpecialVar_Result=GLE_INVALID;
    if(choice>=sStaffCount)return;
    sStaffSlot=sStaffChoices[choice];gSpecialVar_0x8007=sStaffSlot;gSpecialVar_Result=GLE_OK;
}
void GleStaffMemberSelect(void)
{
    u32 local=gSpecialVar_LastTalked;gSpecialVar_Result=GLE_INVALID;
    if(local<230 || local>=230+GLE_RECRUITS)return;
    sStaffSlot=local-230;gSpecialVar_0x8007=sStaffSlot;
    if(GymLeaderState()->recruits[sStaffSlot].id)gSpecialVar_Result=GLE_OK;
}
void GleStaffInfo(void)
{
    struct GleRecruit *r=&GymLeaderState()->recruits[sStaffSlot];u32 total=r->wins+r->losses;
    u8 num[12];const u8 *tag=r->away==2?COMPOUND_STRING(" / RIVAL"):r->away?COMPOUND_STRING(" / AWAY"):r->active?COMPOUND_STRING(" / ACTIVE"):COMPOUND_STRING(" / RESERVE");
    StringCopy(gStringVar1,GetTrainerNameFromId(r->id));StringAppend(gStringVar1,tag);
    ConvertIntToDecimalStringN(gStringVar2,r->potential,STR_CONV_MODE_LEFT_ALIGN,1);StringAppend(gStringVar2,COMPOUND_STRING(" stars / Loyalty "));
    ConvertIntToDecimalStringN(num,r->loyalty,STR_CONV_MODE_LEFT_ALIGN,3);StringAppend(gStringVar2,num);
    StringCopy(gStringVar3,COMPOUND_STRING("W "));ConvertIntToDecimalStringN(num,r->wins,STR_CONV_MODE_LEFT_ALIGN,5);StringAppend(gStringVar3,num);
    StringAppend(gStringVar3,COMPOUND_STRING(" L "));ConvertIntToDecimalStringN(num,r->losses,STR_CONV_MODE_LEFT_ALIGN,5);StringAppend(gStringVar3,num);
    StringAppend(gStringVar3,COMPOUND_STRING(" / "));ConvertIntToDecimalStringN(num,total?(100u*r->wins)/total:0,STR_CONV_MODE_LEFT_ALIGN,3);StringAppend(gStringVar3,num);StringAppend(gStringVar3,COMPOUND_STRING("%"));
    gSpecialVar_Result=r->warnings;
}
void GleStaffActions(void)
{
    static const u8 *const own[]={COMPOUND_STRING("Assign to GYM"),COMPOUND_STRING("Keep in reserve"),COMPOUND_STRING("View team"),COMPOUND_STRING("Practice battle"),COMPOUND_STRING("Dismiss"),COMPOUND_STRING("Back")};
    static const u8 *const away[]={COMPOUND_STRING("Ask to return"),COMPOUND_STRING("View team"),COMPOUND_STRING("Dismiss record"),COMPOUND_STRING("Back")};
    if(GymLeaderState()->recruits[sStaffSlot].away)GymLeaderMenu(away,ARRAY_COUNT(away),FALSE);
    else GymLeaderMenu(own,ARRAY_COUNT(own),FALSE);
}
void GleStaffAction(void)
{
    u32 action=gSpecialVar_Result;struct GleRecruit *r=&GymLeaderState()->recruits[sStaffSlot];
    gSpecialVar_Result=GLE_INVALID;gSpecialVar_0x8007=sStaffSlot;
    if(r->away)
    {if(action==0)gSpecialVar_Result=ReturnRecruit(sStaffSlot);else if(action==1)gSpecialVar_Result=22;else if(action==2)gSpecialVar_Result=20;return;}
    if(action<=1)gSpecialVar_Result=GleAssignTrainer(GymLeaderState(),sStaffSlot,action==0);
    else if(action==2)gSpecialVar_Result=22;
    else if(action==3)gSpecialVar_Result=21;
    else if(action==4)gSpecialVar_Result=20;
}
void GleStaffDismiss(void)
{
    // These temporary flags are scoped to the rebuilt town and new Gym maps.
    // Prevent a dismissed visible actor from returning through lazy spawning.
    FlagSet(FLAG_TEMP_2+sStaffSlot);
    RemoveObjectEventByLocalIdAndMap(230+sStaffSlot,gSaveBlock1Ptr->location.mapNum,gSaveBlock1Ptr->location.mapGroup);
    memset(&GymLeaderState()->recruits[sStaffSlot],0,sizeof(struct GleRecruit));
}
void GymLeaderConfigureStaffMap(struct MapHeader *header)
{
    struct GleState *s=GymLeaderState();u16 map=CurrentMap();u32 i,j,count=0,visible=0;bool32 rebuilt=FALSE;
    static const u16 towns[]={MAP_SAFFRON_CITY,MAP_CELADON_CITY,MAP_FUCHSIA_CITY};
    static const u8 xs[]={2,10,2,10,4},ys[]={5,5,8,8,4};
    if(!IsGymLeaderRun() || s->openingStage!=5 || !header->events || header->events->objectEventCount>ARRAY_COUNT(sStaffObjects)-5)return;
    for(i=0;i<3;i++)if(map==towns[i] && (s->town==i || s->rivalTown==i))rebuilt=TRUE;
    sStaffEvents=*header->events;
    for(i=0;i<header->events->objectEventCount;i++)
    {
        bool32 hide=FALSE;const struct ObjectEventTemplate *obj=&header->events->objectEvents[i];
        if(!rebuilt)for(j=0;j<GLE_RECRUITS;j++)
        {
            const struct GleRecruit *r=&s->recruits[j];const struct GleRecruitSource *source=Source(r->id);
            if(source && source->map==map && source->localId==obj->localId && r->away!=1){hide=TRUE;break;}
        }
        if(!hide)sStaffObjects[count++]=*obj;
    }
    for(i=0;i<GLE_RECRUITS && visible<5;i++)
    {
        const struct GleRecruit *r=&s->recruits[i];const struct GleRecruitSource *source=Source(r->id);u8 x,y;
        if(!source)continue;
        if(map==MAP_GLE_GYM_INTERIOR && !r->away && r->active){x=xs[visible];y=ys[visible];}
        else if(map==towns[s->town] && !r->away && !r->active){x=14+3*visible;y=8;}
        else if(map==MAP_GLE_RIVAL_GYM && r->away==2){x=xs[visible];y=ys[visible];}
        else continue;
        FlagClear(FLAG_TEMP_2+i);
        sStaffObjects[count++]=(struct ObjectEventTemplate){.localId=230+i,.flagId=FLAG_TEMP_2+i,.graphicsId=source->graphics,.x=x,.y=y,.elevation=3,
            .movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleStaff_Member};visible++;
    }
    sStaffEvents.objectEventCount=count;sStaffEvents.objectEvents=sStaffObjects;header->events=&sStaffEvents;
}
static enum Species DevelopSpecies(enum Species species,u32 level)
{
    u32 rounds,j;
    for(rounds=0;rounds<5;rounds++)
    {
        const struct Evolution *ev=gSpeciesInfo[species].evolutions;enum Species next=species;
        if(ev)for(j=0;j<64 && ev[j].method!=EVOLUTIONS_END;j++)
            if(ev[j].method==EVO_LEVEL && ev[j].param<=level){next=ev[j].targetSpecies;break;}
        if(next==species)break;species=next;
    }
    return species;
}
bool32 GymLeaderPrepareStaffRoster(u32 slot,struct Trainer *trainer,struct TrainerMon *entries,bool32 preview)
{
    struct GleState *s=GymLeaderState();struct GleRecruit *r;const struct Trainer *native;u16 *pool;u32 type,i,n,tier,seed,rank;enum Species ace;
    static const u8 types[]={TYPE_NORMAL,TYPE_FIRE,TYPE_WATER,TYPE_ELECTRIC,TYPE_GRASS,TYPE_ICE,TYPE_FIGHTING,TYPE_POISON,TYPE_GROUND,
        TYPE_FLYING,TYPE_PSYCHIC,TYPE_BUG,TYPE_ROCK,TYPE_GHOST,TYPE_DRAGON,TYPE_DARK,TYPE_STEEL,TYPE_FAIRY};
    if(!IsGymLeaderRun() || slot>=GLE_RECRUITS || !(r=&s->recruits[slot])->id)return FALSE;
    rank=preview?max(s->rank,s->highestRank):s->rank;
    native=GetTrainerStructFromId(r->id);
    ace=native->partySize?native->party[0].species:SPECIES_NONE;
    if(!ace || ace>=NUM_SPECIES || GymLeaderSpeciesReserved(ace))ace=SPECIES_RATTATA;
    for(type=0;type<18;type++)if(types[type]==gSpeciesInfo[ace].types[0])break;
    if(type==18)type=0;
    pool=Alloc(GLE_SPECIES_LIMIT*sizeof(u16));if(!pool)return FALSE;
    tier=min(8,max(1,(s32)rank+r->potential-3));
    n=GymLeaderBuildSpeciesPool(type,tier,GLE_STANDARD,FALSE,pool,GLE_SPECIES_LIMIT);
    if(!n){Free(pool);return FALSE;}
    *trainer=*native;trainer->party=entries;
    trainer->partySize=min(gGleRankTeamSizes[rank-1],1+(rank-1)/2+(r->potential>=3)+(r->potential==5));
    trainer->aiFlags|=AI_FLAG_CHECK_BAD_MOVE|AI_FLAG_CHECK_VIABILITY|AI_FLAG_TRY_TO_FAINT;
    ZeroEnemyPartyMons();seed=GleMix(s->seed^r->id);
    memset(entries,0,PARTY_SIZE*sizeof(*entries));
    for(i=0;i<trainer->partySize;i++)
    {
        const struct GleRecruitSource *origin=Source(r->id);u16 metLocation=origin?Overworld_GetMapHeaderByGroupAndId(origin->map>>8,origin->map&255)->regionMapSectionId:0;
        u8 otName[PLAYER_NAME_LENGTH+1];u8 level=gGleRankCaps[rank-1];enum Species species=i?pool[GleMix(seed^i)%n]:DevelopSpecies(ace,level);
        CreateMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][i],species,level,GleMix(seed^i^0x535441u),OTID_STRUCT_PRESET(seed),8+4*r->potential);
        StringFill(otName,EOS,PLAYER_NAME_LENGTH);StringCopyN(otName,trainer->trainerName,PLAYER_NAME_LENGTH);otName[PLAYER_NAME_LENGTH]=EOS;
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][i],MON_DATA_OT_NAME,otName);
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][i],MON_DATA_MET_LOCATION,&metLocation);
        if(i==0 && native->partySize){u8 metLevel=min(level,native->party[0].lvl);SetMonData(&gParties[B_TRAINER_OPPONENT_A][i],MON_DATA_MET_LEVEL,&metLevel);}
        GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][i]);entries[i].species=species;entries[i].lvl=level;
    }
    gPartiesCount[B_TRAINER_OPPONENT_A]=trainer->partySize;Free(pool);return TRUE;
}

void GleStaffOfficeInfo(void)
{
    struct GleState *s=GymLeaderState();u32 i,active=0;
    for(i=0;i<GLE_RECRUITS;i++)active+=s->recruits[i].id && s->recruits[i].active && !s->recruits[i].away;
    ConvertIntToDecimalStringN(gStringVar1,RosterCount(),STR_CONV_MODE_LEFT_ALIGN,2);
    ConvertIntToDecimalStringN(gStringVar2,min(GLE_RECRUITS,s->rank+2),STR_CONV_MODE_LEFT_ALIGN,2);
    ConvertIntToDecimalStringN(gStringVar3,active,STR_CONV_MODE_LEFT_ALIGN,1);
    StringAppend(gStringVar3,COMPOUND_STRING(" / "));
    {u8 n[3];ConvertIntToDecimalStringN(n,(s->rank+1)/2+(s->rank==8),STR_CONV_MODE_LEFT_ALIGN,1);StringAppend(gStringVar3,n);}
}
void GleStaffDevelopment(void)
{
    struct GleRecruit *r=&GymLeaderState()->recruits[sStaffSlot];
    ConvertIntToDecimalStringN(gStringVar1,r->wins+r->losses,STR_CONV_MODE_LEFT_ALIGN,6);
    ConvertIntToDecimalStringN(gStringVar2,r->warnings,STR_CONV_MODE_LEFT_ALIGN,1);
    StringCopy(gStringVar3,r->questBond?COMPOUND_STRING("Personal bond"):COMPOUND_STRING("Ordinary contract"));
}
static void Task_StaffSummary(u8 taskId)
{
    if(!gPaletteFade.active)
    {
        DestroyTask(taskId);ResetTasks();CleanupOverworldWindowsAndTilemaps();
        ShowPokemonSummaryScreen(SUMMARY_MODE_NORMAL,gGlePreviewParty,0,sPreviewCount-1,CB2_ReturnToFieldContinueScript);
    }
}
void GleStaffSummary(void)
{
    struct Trainer trainer;struct TrainerMon entries[PARTY_SIZE];u32 i;
    if(!GymLeaderPrepareStaffRoster(sStaffSlot,&trainer,entries,TRUE))
    {GymLeaderResumeScriptNextFrame();return;}
    sPreviewCount=trainer.partySize;
    for(i=0;i<sPreviewCount;i++)gGlePreviewParty[i]=gParties[B_TRAINER_OPPONENT_A][i];
    BeginNormalPaletteFade(PALETTES_ALL,0,0,16,RGB_BLACK);CreateTask(Task_StaffSummary,10);
}
