#include "global.h"
#include "gym_leader.h"
#include "gym_leader_news.h"
#include "gym_leader_script.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "string_util.h"
#include "data.h"
#include "constants/flags.h"
#include "constants/maps.h"
#include "constants/vars.h"

extern const u8 GleChampion_NewsNpc[];
extern const u8 PalletTown_EventScript_FatMan[],ViridianCity_EventScript_Boy[],PewterCity_EventScript_Lass[],CeruleanCity_EventScript_LittleBoy[],VermilionCity_EventScript_OldMan1[],LavenderTown_EventScript_Boy[],SaffronCity_EventScript_Man[],CeladonCity_EventScript_LittleGirl[],FuchsiaCity_EventScript_OldMan[],CinnabarIsland_EventScript_Woman[];
extern void GleRocketJournal(void);
static const u16 sTowns[]={MAP_SAFFRON_CITY,MAP_CELADON_CITY,MAP_FUCHSIA_CITY};
static const u8 *const sTownNames[]={COMPOUND_STRING("SAFFRON"),COMPOUND_STRING("CELADON"),COMPOUND_STRING("FUCHSIA")};
static const u8 *const sWinners[]={COMPOUND_STRING("YOU"),COMPOUND_STRING("RIVAL"),COMPOUND_STRING("BROCK"),COMPOUND_STRING("MISTY"),COMPOUND_STRING("LT. SURGE"),COMPOUND_STRING("ERIKA"),COMPOUND_STRING("KOGA"),COMPOUND_STRING("SABRINA"),COMPOUND_STRING("BLAINE")};
static const u8 *Winner(u32 id){return id==0?gSaveBlock2Ptr->playerName:id<ARRAY_COUNT(sWinners)?sWinners[id]:COMPOUND_STRING("NOT YET CROWNED");}
static const u8 *Standing(void){s16 h=GymLeaderState()->honor;return h>=100?COMPOUND_STRING("Honorable"):h<=-100?COMPOUND_STRING("Notorious"):COMPOUND_STRING("Neutral");}
static bool32 Home(void){struct GleState *s=GymLeaderState();return IsGymLeaderRun()&&s->town<3&&((u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8))==sTowns[s->town];}
// Six free ledger bits store the last celebrated title count. The permanent
// championship archive can hold only 32 teams, so this cannot overflow in play.
u32 GymLeaderCelebratedTitles(void){u32 i,n=0;for(i=0;i<6;i++){u32 bit=247+i;if(GymLeaderState()->uniqueObtained[bit/8]&(1u<<(bit%8)))n|=1u<<i;}return n;}
bool32 GymLeaderCelebrationPending(void){return Home()&&!GymLeaderState()->rocket.stage&&min(63,GymLeaderState()->championshipWins)>GymLeaderCelebratedTitles();}
void GleTownArrival(void){VarSet(VAR_TEMP_F,GymLeaderCelebrationPending());}
void GleCelebrationInfo(void)
{
    struct GleState *s=GymLeaderState();gSpecialVar_Result=0;if(!GymLeaderCelebrationPending())return;
    StringCopy(gStringVar1,s->townName);ConvertIntToDecimalStringN(gStringVar2,s->championshipWins,STR_CONV_MODE_LEFT_ALIGN,2);
    if(!GymLeaderCelebratedTitles()){
        StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("Welcome home, Kanto Champion!\n{STR_VAR_1} has gathered to\lcelebrate your Gym's achievement.\pOur trainers and townspeople\nbuilt this home together. Today,\lKanto knows our town's name!\pYour Kanto titles: {STR_VAR_2}.\nThese championship displays\lwill remember this achievement."));gSpecialVar_Result=1;
    }else{
        StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("Another title for our hometown!\nYour Kanto titles: {STR_VAR_2}.\p{STR_VAR_1} welcomes its\nchampion home. Congratulations\lto you and your partners!"));gSpecialVar_Result=2;
    }
}
void GleCelebrationStaffText(void)
{
    struct GleState *s=GymLeaderState();u32 i;
    for(i=0;i<GLE_RECRUITS;i++)if(s->recruits[i].id&&s->recruits[i].active&&!s->recruits[i].away){
        StringCopy(gStringVar1,GetTrainerNameFromId(s->recruits[i].id));
        StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("{STR_VAR_1}: All that training\npaid off! Our Gym's partners\lshare this championship with you.\pWe're proud to defend this Gym\nand help the next generation\lof challengers grow."));return;
    }
    StringCopy(gStringVar4,COMPOUND_STRING("GYM GUIDE: Your partners earned\nthis alongside you. What a day\lfor our Gym and our hometown!"));
}
void GleCelebrationFinish(void)
{
    struct GleState *s=GymLeaderState();u32 i,n=min(63,s->championshipWins);
    if(!GymLeaderCelebrationPending())return;
    for(i=0;i<6;i++){u32 bit=247+i,mask=1u<<(bit%8);if(n&(1u<<i))s->uniqueObtained[bit/8]|=mask;else s->uniqueObtained[bit/8]&=~mask;}
    FlagSet(FLAG_TEMP_1E);FlagSet(FLAG_TEMP_1F);
    RemoveObjectEventByLocalIdAndMap(245,gSaveBlock1Ptr->location.mapNum,gSaveBlock1Ptr->location.mapGroup);
    RemoveObjectEventByLocalIdAndMap(246,gSaveBlock1Ptr->location.mapNum,gSaveBlock1Ptr->location.mapGroup);
}
const u8 *GymLeaderChampionNpcScript(const u8 *original)
{
    static const u8 *const civilians[]={PalletTown_EventScript_FatMan,ViridianCity_EventScript_Boy,PewterCity_EventScript_Lass,CeruleanCity_EventScript_LittleBoy,VermilionCity_EventScript_OldMan1,LavenderTown_EventScript_Boy,SaffronCity_EventScript_Man,CeladonCity_EventScript_LittleGirl,FuchsiaCity_EventScript_OldMan,CinnabarIsland_EventScript_Woman};
    u32 i;if(!IsGymLeaderRun()||GymLeaderState()->openingStage!=5)return original;
    for(i=0;i<ARRAY_COUNT(civilians);i++)if(original==civilians[i])return GleChampion_NewsNpc;
    return original;
}
void GleRecognitionInfo(void)
{
    struct GleState *s=GymLeaderState();gSpecialVar_Result=0;
    StringCopy(gStringVar4,COMPOUND_STRING("The Kanto Gazette follows our\nGym Leaders, towns and competitions.\lYou can read the latest news here."));
    if(!IsGymLeaderRun()||!s->championshipWins)return;
    StringCopy(gStringVar4,s->competitions.champion==0?COMPOUND_STRING("Our reigning Kanto Champion!\nYour Gym's name is known\lthroughout Kanto."):COMPOUND_STRING("A former Kanto Champion!\nYour place in League history\lwill always be remembered."));
    StringAppend(gStringVar4,s->honor>=100?COMPOUND_STRING("\pPeople admire your leadership\nand the care you show your town."):s->honor<=-100?COMPOUND_STRING("\pYour victories bring fame,\nbut your methods make some\lpeople wary."):COMPOUND_STRING("\pYour achievements have put\nyour hometown on the map."));gSpecialVar_Result=1;
}
void GleNewsMenu(void){static const u8 *const names[]={COMPOUND_STRING("Championship desk"),COMPOUND_STRING("Regional results"),COMPOUND_STRING("Your hometown"),COMPOUND_STRING("Rival's career"),COMPOUND_STRING("Rocket bulletin"),COMPOUND_STRING("Back")};GymLeaderMenu(names,ARRAY_COUNT(names),FALSE);}
void GleNewsPage(void)
{
    struct GleState *s=GymLeaderState();struct GleCompetitionState *c=&s->competitions;u32 page=gSpecialVar_Result,i;
    if(page==4){GleRocketJournal();return;}
    if(page==0){
        StringCopy(gStringVar1,Winner(c->champion));ConvertIntToDecimalStringN(gStringVar2,s->championshipWins,STR_CONV_MODE_LEFT_ALIGN,2);ConvertIntToDecimalStringN(gStringVar3,s->rivalChampionshipWins,STR_CONV_MODE_LEFT_ALIGN,2);
        StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("KANTO GAZETTE / CHAMPIONSHIPS\nReigning champion: {STR_VAR_1}\pYour retained Kanto titles: {STR_VAR_2}\nRival's retained titles: {STR_VAR_3}\pA later loss cannot erase a title.\nThe GLA keeps permanent brackets\land winning-team records."));
    }else if(page==1){
        for(i=c->count;i>0;i--)if(!(c->history[i-1].kindRank&1))break;
        if(!i)StringCopy(gStringVar4,COMPOUND_STRING("REGIONAL DESK\nNo completed Regional Cup yet.\pWatch the GLA event notice.\nCompetitions continue while\lyou explore Kanto."));
        else{ConvertIntToDecimalStringN(gStringVar1,i,STR_CONV_MODE_LEFT_ALIGN,3);StringCopy(gStringVar2,Winner(GleEventWinner(&c->history[i-1],6)));StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("REGIONAL DESK / EVENT {STR_VAR_1}\nWinner: {STR_VAR_2}\pThe complete eight-entrant bracket\nis preserved at the GLA event desk."));}
    }else if(page==2){
        StringCopy(gStringVar1,s->townName);ConvertIntToDecimalStringN(gStringVar2,s->population,STR_CONV_MODE_LEFT_ALIGN,3);StringCopy(gStringVar3,Standing());StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("TOWN DESK / {STR_VAR_1}\nPopulation: {STR_VAR_2}\lLeadership: {STR_VAR_3}\pHomes, businesses and habitats\nshape the town's daily life.\lIts Gym gives it a place in Kanto."));
        {u8 number[8];ConvertIntToDecimalStringN(number,s->reputation,STR_CONV_MODE_LEFT_ALIGN,5);StringAppend(gStringVar4,COMPOUND_STRING("\pTown reputation: "));StringAppend(gStringVar4,number);StringAppend(gStringVar4,COMPOUND_STRING(".\nKanto follows your leadership."));}
    }else if(page==3){
        StringCopy(gStringVar1,sTownNames[s->rivalTown<3?s->rivalTown:0]);ConvertIntToDecimalStringN(gStringVar2,s->rivalChampionshipWins,STR_CONV_MODE_LEFT_ALIGN,2);StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("RIVAL DESK / {STR_VAR_1}\nRetained Kanto titles: {STR_VAR_2}\pYour rival runs an independent Gym\nand enters competitions even\lwhen you do not take part."));if(c->champion==1)StringAppend(gStringVar4,COMPOUND_STRING("\pYour rival currently holds\nthe Kanto Championship."));
    }
}
