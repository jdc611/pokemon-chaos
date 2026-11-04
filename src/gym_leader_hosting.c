#include "global.h"
#include "gym_leader.h"
#include "gym_leader_competitions_native.h"
#include "gym_leader_script.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "money.h"
#include "overworld.h"
#include "string_util.h"
#include "constants/maps.h"
static const u16 sTowns[]={MAP_SAFFRON_CITY,MAP_CELADON_CITY,MAP_FUCHSIA_CITY};
static const u8 *const sFormats[]={COMPOUND_STRING("Town showcase"),COMPOUND_STRING("Gym defense doubles"),COMPOUND_STRING("Regional Cup"),COMPOUND_STRING("Kanto Championship")};
static const u8 *const sNames[]={COMPOUND_STRING("SAFFRON"),COMPOUND_STRING("CELADON"),COMPOUND_STRING("FUCHSIA"),COMPOUND_STRING("GLA HEADQUARTERS")};
static EWRAM_DATA u8 sHostingFormat=GLE_NONE;
static u16 Map(void){return (u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8);}
static const u8 *VenueName(unsigned n){return n==GymLeaderState()->town?GymLeaderState()->townName:sNames[n<4?n:3];}
int GymLeaderHostingTown(void){struct GleState *s;unsigned n;if(!IsGymLeaderRun())return -1;s=GymLeaderState();if(s->competitions.stage&&(n=GleEventVenue(&s->competitions.current))<3)return n;if(s->competitions.stage==1&&s->hosting.stage==GLE_HOST_INVITED&&s->hosting.event==s->competitions.count+1)return s->town;return -1;}
bool32 GymLeaderCompetitionAtVenue(void){unsigned n=GleEventVenue(&GymLeaderState()->competitions.current);return n>=3||Map()==sTowns[n];}
void GleHostingRefreshVenue(void)
{
 struct GleState *s=GymLeaderState();unsigned n;gSpecialVar_Result=0;for(n=0;n<3;n++)if(Map()==sTowns[n])break;
 if(!IsGymLeaderRun()||n>=3||(n!=s->town&&n!=s->rivalTown))return;
 RemoveObjectEventByLocalIdAndMap(249,gSaveBlock1Ptr->location.mapNum,gSaveBlock1Ptr->location.mapGroup);GymLeaderConfigureTown(&gMapHeader);
 if(GymLeaderHostingTown()==(s32)n){TrySpawnObjectEvent(249,gSaveBlock1Ptr->location.mapNum,gSaveBlock1Ptr->location.mapGroup);gSpecialVar_Result=1;}
 if(n==s->town&&s->championshipWins){TrySpawnObjectEvent(247,gSaveBlock1Ptr->location.mapNum,gSaveBlock1Ptr->location.mapGroup);TrySpawnObjectEvent(248,gSaveBlock1Ptr->location.mapNum,gSaveBlock1Ptr->location.mapGroup);}
}
void GleHostingInfo(void)
{
 struct GleState *s=GymLeaderState();struct GleCompetitionState *c=&s->competitions;gSpecialVar_Result=3;
 if(!c->stage){StringCopy(gStringVar4,COMPOUND_STRING("No event is announced yet.\nApplications open with the\lGLA's next competition notice."));return;}
 StringCopy(gStringVar1,sFormats[GleEventFormat(&c->current)]);StringCopy(gStringVar2,VenueName(GleEventVenue(&c->current)));
 StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("{STR_VAR_1}\nVenue: {STR_VAR_2}\pHosting and entering are separate.\nYou may host while NPCs compete,\lor enter with your registered team."));
 if(c->stage!=1){StringAppend(gStringVar4,COMPOUND_STRING("\pApplications have closed.\nTravel to the venue for your\lnext registered match."));return;}
 gSpecialVar_Result=0;
 if(s->hosting.stage==GLE_HOST_BOOKED){StringAppend(gStringVar4,COMPOUND_STRING("\pYour booking is confirmed.\nVisitor income arrives after\lthe event. No errands are needed."));gSpecialVar_Result=2;}
 else if(s->hosting.stage==GLE_HOST_INVITED&&s->hosting.event==c->count+1){StringAppend(gStringVar4,COMPOUND_STRING("\pThe GLA has invited your town!\nAccept or decline without\llosing reputation."));gSpecialVar_Result=1;}
}
void GleHostingMenu(void){static const u8 *const n[]={COMPOUND_STRING("Apply to host"),COMPOUND_STRING("Hosting invitation"),COMPOUND_STRING("Travel to event"),COMPOUND_STRING("Back")};GymLeaderMenu(n,4,FALSE);}
void GleHostingFormatMenu(void){static const u8 *const n[]={COMPOUND_STRING("Town showcase"),COMPOUND_STRING("Gym defense doubles"),COMPOUND_STRING("Regional Cup"),COMPOUND_STRING("Back")};static const u8 *const k[]={COMPOUND_STRING("Kanto Championship"),COMPOUND_STRING("Back")};sHostingFormat=GLE_NONE;if(GymLeaderState()->competitions.current.kindRank&1)GymLeaderMenu(k,2,FALSE);else GymLeaderMenu(n,4,FALSE);}
void GleHostingQuote(void)
{
 static const u8 *const req[]={COMPOUND_STRING("Showcase: Rank 2, population 9,\nreputation 60, and a Dojo, Hotel,\lPark or Restaurant."),COMPOUND_STRING("Defense doubles: Rank 3,\npopulation 12, reputation 80\land a Dojo."),COMPOUND_STRING("Regional: Rank 4, population 18,\nreputation 120, a Dojo and Hotel."),COMPOUND_STRING("Kanto: Rank 6, population 24,\nreputation 180, and tier-2 Dojo,\lHotel and landmark facilities.")};
 struct GleState *s=GymLeaderState();unsigned choice=gSpecialVar_Result,kind=s->competitions.current.kindRank&1;gSpecialVar_Result=2;if(choice>=(kind?1:3))return;sHostingFormat=kind?3:choice;gSpecialVar_Result=0;
 if(s->competitions.stage!=1||s->hosting.stage==GLE_HOST_BOOKED){StringCopy(gStringVar4,COMPOUND_STRING("This event cannot be booked now.\nReview its current notice."));return;}
 if(!GleHostingEligible(s,sHostingFormat)){StringCopy(gStringVar4,req[sHostingFormat]);if(s->rocket.stage)StringAppend(gStringVar4,COMPOUND_STRING("\pResolve the Rocket incident\nbefore bringing visitors here."));return;}
 StringCopy(gStringVar1,sFormats[sHostingFormat]);ConvertIntToDecimalStringN(gStringVar2,GleHostingFee(s,sHostingFormat),STR_CONV_MODE_LEFT_ALIGN,6);ConvertIntToDecimalStringN(gStringVar3,GleHostingIncome(s,sHostingFormat),STR_CONV_MODE_LEFT_ALIGN,6);gSpecialVar_Result=1;
}
void GleHostingConfirm(void){u32 money=GetMoney(&gSaveBlock1Ptr->money);gSpecialVar_Result=GleHostingBook(GymLeaderState(),sHostingFormat,gSpecialVar_0x8005,&money);SetMoney(&gSaveBlock1Ptr->money,money);}
void GleHostingDeclineNative(void){gSpecialVar_Result=GleHostingDecline(GymLeaderState());}
void GleHostingTravel(void){struct GleState *s=GymLeaderState();unsigned n=GleEventVenue(&s->competitions.current);u16 map=n<3?sTowns[n]:MAP_GLE_LEAGUE_HALL;gSpecialVar_Result=0;if(!IsGymLeaderRun()||!s->competitions.stage)return;SetWarpDestination(MAP_GROUP(map),MAP_NUM(map),-1,n<3?10:6,n<3?10:13);DoWarp();gSpecialVar_Result=1;}
