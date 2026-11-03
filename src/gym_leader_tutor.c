#include "global.h"
#include "gym_leader.h"
#include "gym_leader_tutor.h"
#include "gym_leader_script.h"
#include "event_data.h"
#include "money.h"
#include "move.h"
#include "overworld.h"
#include "pokemon.h"
#include "script.h"
#include "string_util.h"
#include "constants/flags.h"
#include "constants/maps.h"

// Thirty stable move-license bits in the pointer-free GLE save state.
struct GleTutor {u16 move;u8 bit,rank;};
static const struct GleTutor sTutors[]={
    {MOVE_BODY_SLAM,0,3},
    {MOVE_COUNTER,1,3},
    {MOVE_DEFENSE_CURL,2,2},
    {MOVE_DOUBLE_EDGE,3,4},
    {MOVE_DREAM_EATER,4,3},
    {MOVE_DYNAMIC_PUNCH,5,4},
    {MOVE_ENDURE,6,3},
    {MOVE_EXPLOSION,7,5},
    {MOVE_FIRE_PUNCH,8,3},
    {MOVE_FURY_CUTTER,9,2},
    {MOVE_ICE_PUNCH,10,3},
    {MOVE_ICY_WIND,11,3},
    {MOVE_MEGA_KICK,12,4},
    {MOVE_MEGA_PUNCH,13,3},
    {MOVE_METRONOME,14,3},
    {MOVE_MIMIC,15,3},
    {MOVE_MUD_SLAP,16,2},
    {MOVE_PSYCH_UP,17,3},
    {MOVE_ROCK_SLIDE,18,4},
    {MOVE_ROLLOUT,19,2},
    {MOVE_SEISMIC_TOSS,20,3},
    {MOVE_SLEEP_TALK,21,3},
    {MOVE_SNORE,22,2},
    {MOVE_SOFT_BOILED,23,4},
    {MOVE_SUBSTITUTE,24,4},
    {MOVE_SWAGGER,25,3},
    {MOVE_SWIFT,26,2},
    {MOVE_SWORDS_DANCE,27,4},
    {MOVE_THUNDER_PUNCH,28,3},
    {MOVE_THUNDER_WAVE,29,3},
};
static EWRAM_DATA u8 sTutorOffers[30]={0};
static EWRAM_DATA u8 sTutorCount=0;
static EWRAM_DATA u8 sTutorIndex=0;
bool32 GymLeaderTutorUnlocked(u16 move)
{
    u32 i;
    if(!IsGymLeaderRun())return TRUE;
    for(i=0;i<ARRAY_COUNT(sTutors);i++)if(sTutors[i].move==move)return !!(GymLeaderState()->tutorLessons&(1u<<sTutors[i].bit));
    return FALSE;
}
static bool32 PartyCompatible(u16 move)
{
    u32 i;
    for(i=0;i<PARTY_SIZE;i++)
    {
        enum Species species=GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_SPECIES);
        if(species && !GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_IS_EGG)
            && CanLearnTeachableMove(species,move))return TRUE;
    }
    return FALSE;
}
void GleTutorMenu(void)
{
    const u8 *names[31];u32 i;sTutorCount=0;
    if(IsGymLeaderRun())for(i=0;i<ARRAY_COUNT(sTutors);i++)
        if((GymLeaderState()->rank>=sTutors[i].rank || GymLeaderTutorUnlocked(sTutors[i].move)) && PartyCompatible(sTutors[i].move))
        {
            sTutorOffers[sTutorCount]=i;
            names[sTutorCount++]=GetMoveName(sTutors[i].move);
        }
    if(!sTutorCount){gSpecialVar_Result=255;GymLeaderResumeScriptNextFrame();return;}
    names[sTutorCount]=COMPOUND_STRING("Back");GymLeaderMenu(names,sTutorCount+1,FALSE);
}
static u32 LessonCost(void)
{
    u32 cost=5000*sTutors[sTutorIndex].rank;
    return GymLeaderState()->honor>=100?cost*9/10:cost;
}
void GleTutorInfo(void)
{
    u32 choice=gSpecialVar_Result;gSpecialVar_Result=2;
    if(!IsGymLeaderRun() || choice>=sTutorCount)return;
    sTutorIndex=sTutorOffers[choice];
    gSpecialVar_0x8005=sTutors[sTutorIndex].move;
    StringCopy(gStringVar1,GetMoveName(sTutors[sTutorIndex].move));
    ConvertIntToDecimalStringN(gStringVar2,LessonCost(),STR_CONV_MODE_LEFT_ALIGN,5);
    gSpecialVar_Result=GymLeaderTutorUnlocked(sTutors[sTutorIndex].move);
}
void GleTutorPurchase(void)
{
    const struct GleTutor *t=&sTutors[sTutorIndex];u32 cost;
    gSpecialVar_Result=GLE_NOT_READY;
    if(!IsGymLeaderRun() || gSaveBlock1Ptr->location.mapGroup!=MAP_GROUP(MAP_GLE_LEAGUE_HALL)
        || gSaveBlock1Ptr->location.mapNum!=MAP_NUM(MAP_GLE_LEAGUE_HALL) || !PartyCompatible(t->move))return;
    gSpecialVar_0x8005=t->move;
    if(GymLeaderState()->tutorLessons&(1u<<t->bit)){gSpecialVar_Result=GLE_OK;return;}
    if(GymLeaderState()->rank<t->rank)return;
    cost=LessonCost();
    if(!IsEnoughMoney(&gSaveBlock1Ptr->money,cost)){gSpecialVar_Result=GLE_NOT_ENOUGH_MONEY;return;}
    RemoveMoney(&gSaveBlock1Ptr->money,cost);GymLeaderState()->tutorLessons|=1u<<t->bit;gSpecialVar_Result=GLE_OK;
}
