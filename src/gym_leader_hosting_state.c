#include "gym_leader_state.h"
#include <string.h>
#include <limits.h>
static uint32_t Later(uint32_t n,uint32_t delta){return n>UINT32_MAX-delta?UINT32_MAX:n+delta;}
static unsigned Facility(const struct GleState *s,unsigned building){unsigned i,n=0;for(i=0;i<GLE_PLOTS;i++)if(s->plots[i].building==building)n+=s->plots[i].tier;return n;}
static unsigned Best(const struct GleState *s,unsigned building){unsigned i,n=0;for(i=0;i<GLE_PLOTS;i++)if(s->plots[i].building==building&&s->plots[i].tier>n)n=s->plots[i].tier;return n;}
static unsigned Fee(unsigned format){static const unsigned fees[]={3000,5000,8000,15000};return format<4?fees[format]:UINT32_MAX;}
static unsigned PlayerScore(const struct GleState *s){return s->population*2+s->reputation/10+12*(Facility(s,GLE_DOJO)+Facility(s,GLE_HOTEL)+Facility(s,GLE_RESTAURANT)+Facility(s,GLE_PARK)+s->landmarkTier);}
static unsigned RivalScore(const struct GleState *s){unsigned activity=s->activity/12;return 24+(activity<140?activity:140)+10*s->rivalChampionshipWins;}
static unsigned HostByte(unsigned town,unsigned format,unsigned flag){return town|((format==GLE_HOST_SHOWCASE?1:format==GLE_HOST_DEFENSE?2:format==GLE_HOST_KANTO?3:0)<<2)|flag;}
uint8_t GleEventVenue(const struct GleEventRecord *r){return r->host&(GLE_HOST_PLAYER|GLE_HOST_RIVAL)?r->host&3:3;}
uint8_t GleEventFormat(const struct GleEventRecord *r){unsigned f=(r->host>>2)&3;if(r->kindRank&1)return GLE_HOST_KANTO;return r->host&(GLE_HOST_PLAYER|GLE_HOST_RIVAL)?f==1?GLE_HOST_SHOWCASE:f==2?GLE_HOST_DEFENSE:GLE_HOST_REGIONAL:GLE_HOST_REGIONAL;}
void GleHostingInit(struct GleState *s){memset(&s->hosting,0,sizeof(s->hosting));s->hosting.nextAt=Later(s->activity,180);}
int GleHostingEligible(const struct GleState *s,uint8_t format)
{
 static const uint8_t rank[]={2,3,4,6},people[]={9,12,18,24};static const uint16_t rep[]={60,80,120,180};unsigned dojo,hotel;
 if(!GleValid(s)||format>=4||s->rocket.stage||s->rank<rank[format]||s->population<people[format]||s->reputation<rep[format])return 0;
 dojo=Best(s,GLE_DOJO);hotel=Best(s,GLE_HOTEL);
 if(format==0)return dojo||hotel||Facility(s,GLE_PARK)||Facility(s,GLE_RESTAURANT);
 if(format==1)return dojo>=1;
 if(format==2)return dojo>=1&&hotel>=1;
 return dojo>=2&&hotel>=2&&s->landmarkTier>=2;
}
uint32_t GleHostingFee(const struct GleState *s,uint8_t f){uint32_t fee=Fee(f);return fee==UINT32_MAX?fee:s->honor>=100?fee*9/10:fee;}
uint32_t GleHostingIncome(const struct GleState *s,uint8_t f)
{
 uint32_t n,cap;if(f>=4)return 0;n=(60*s->population+15*s->reputation+1000*Facility(s,GLE_HOTEL)+700*Facility(s,GLE_RESTAURANT))*(f==3?3:f==2?2:1);cap=2*Fee(f);if(n>cap)n=cap;if(s->rocket.stage)n/=2;return n;
}
static void RivalHost(struct GleState *s){unsigned f=s->competitions.current.kindRank&1?3:s->competitions.count%3==0?0:s->competitions.count%3==1?1:2;s->competitions.current.host=HostByte(s->rivalTown,f,GLE_HOST_RIVAL);}
void GleHostingAnnouncement(struct GleState *s)
{
 struct GleHostingState *h=&s->hosting;unsigned f=s->competitions.current.kindRank&1?3:0;if(h->stage==GLE_HOST_BOOKED)return;
 h->stage=0;h->event=s->competitions.count+1;
 if(s->activity>=h->nextAt&&GleHostingEligible(s,f)&&(GleMix(s->seed^h->event^0x484F5354u)&1))h->stage=GLE_HOST_INVITED;
 else if((GleMix(s->seed^h->event^0x52495648u)&3)==0)RivalHost(s);
}
void GleHostingStart(struct GleState *s){if(s->hosting.stage==GLE_HOST_INVITED){s->hosting.stage=0;s->hosting.nextAt=Later(s->activity,160);if(!(s->competitions.current.host&GLE_HOST_PLAYER))RivalHost(s);}}
int GleHostingBook(struct GleState *s,uint8_t f,int invitation,uint32_t *money)
{
 struct GleHostingState *h=&s->hosting;struct GleCompetitionState *c=&s->competitions;uint32_t fee;
 if(!GleValid(s)||!money||c->stage!=1||f>=4||((c->current.kindRank&1)!=(f==3))||!GleHostingEligible(s,f))return GLE_NOT_READY;
 if(h->stage==GLE_HOST_BOOKED)return GLE_DUPLICATE;
 if(invitation&&(h->stage!=GLE_HOST_INVITED||h->event!=c->count+1))return GLE_NOT_READY;
 fee=GleHostingFee(s,f);if(*money<fee)return GLE_NOT_ENOUGH_MONEY;
 if(h->stage!=GLE_HOST_INVITED&&PlayerScore(s)<RivalScore(s)){RivalHost(s);h->event=c->count+1;h->nextAt=Later(s->activity,120);return GLE_INVALID;}
 *money-=fee;h->stage=GLE_HOST_BOOKED;h->format=f;h->event=c->count+1;c->current.host=HostByte(s->town,f,GLE_HOST_PLAYER);return GLE_OK;
}
int GleHostingDecline(struct GleState *s){if(!GleValid(s)||s->hosting.stage!=GLE_HOST_INVITED||s->competitions.stage!=1||s->hosting.event!=s->competitions.count+1)return GLE_NOT_READY;s->hosting.stage=0;s->hosting.nextAt=Later(s->activity,120);return GLE_OK;}
void GleHostingFinish(struct GleState *s,uint32_t *money)
{
 struct GleHostingState *h=&s->hosting;unsigned n;if(!money||!s->competitions.count||h->event!=s->competitions.count)return;
 if(h->stage==GLE_HOST_BOOKED&&(s->competitions.history[h->event-1].host&GLE_HOST_PLAYER)){n=GleHostingIncome(s,h->format);*money=*money>=GLE_MAX_MONEY||n>GLE_MAX_MONEY-*money?GLE_MAX_MONEY:*money+n;n=6+2*h->format;s->reputation=s->reputation>65535-n?65535:s->reputation+n;}
 h->stage=0;h->nextAt=Later(s->activity,160);
}
