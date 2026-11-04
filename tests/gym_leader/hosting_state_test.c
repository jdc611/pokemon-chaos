#include "gym_leader_state.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
static void setup(struct GleState *s,unsigned seed){GleInit(s,seed);assert(GleChooseIdentity(s,0,4,1)==GLE_OK);s->rank=2;s->population=9;s->reputation=60;s->plots[0].building=GLE_PARK;s->plots[0].tier=1;s->rocket.nextAt=UINT32_MAX;}
static void announce(struct GleState *s,uint32_t *m){s->activity=s->competitions.nextAt;GleCompetitionActivity(s,m);assert(s->competitions.stage==1);}
static void start(struct GleState *s,uint32_t *m){s->activity=s->competitions.closesAt;GleCompetitionActivity(s,m);}
int main(void){
 struct GleState s,copy;uint32_t money,before,income;unsigned seed,host;
 assert(sizeof(s)==3196&&sizeof(s.hosting)==8);
 setup(&s,1);money=3000;announce(&s,&money);s.hosting.stage=0;
 assert(GleHostingEligible(&s,0)&&!GleHostingEligible(&s,1));copy=s;money=100;
 assert(GleHostingBook(&s,0,0,&money)==GLE_NOT_ENOUGH_MONEY&&money==100&&!memcmp(&s,&copy,sizeof(s)));
 money=3000;assert(GleHostingBook(&s,0,0,&money)==GLE_INVALID&&money==3000);host=s.competitions.current.host;
 assert((host&GLE_HOST_RIVAL)&&GleHostingBook(&s,0,0,&money)==GLE_INVALID&&s.competitions.current.host==host);
 s.reputation=70;assert(GleHostingBook(&s,0,0,&money)==GLE_OK&&money==0);
 assert(GleEventVenue(&s.competitions.current)==s.town&&GleEventFormat(&s.competitions.current)==0);
 assert(GleHostingBook(&s,0,0,&money)==GLE_DUPLICATE&&money==0);
 income=GleHostingIncome(&s,0);assert(income<3000);copy=s;start(&s,&money);
 assert(s.competitions.count==1&&s.hosting.stage==0&&money==income&&s.reputation==76);
 before=money;GleHostingFinish(&s,&money);assert(money==before);money=0;start(&copy,&money);assert(money==before&&!memcmp(&s,&copy,sizeof(s)));
 for(seed=0;seed<100;seed++){setup(&s,seed);s.population=12;s.reputation=100;s.competitions.nextAt=200;money=10000;announce(&s,&money);if(s.hosting.stage==GLE_HOST_INVITED)break;}
 assert(seed<100);copy=s;before=money;assert(GleHostingDecline(&s)==GLE_OK&&money==before&&s.reputation==copy.reputation&&s.honor==copy.honor);
 assert(GleHostingDecline(&s)==GLE_NOT_READY&&GleHostingBook(&s,0,1,&money)==GLE_NOT_READY);
 assert(GleHostingBook(&s,0,0,&money)==GLE_OK);s=copy;money=0;
 assert(GleHostingBook(&s,0,1,&money)==GLE_NOT_ENOUGH_MONEY&&s.hosting.stage==GLE_HOST_INVITED);
 money=3000;assert(GleHostingBook(&s,0,1,&money)==GLE_OK&&money==0);
 s=copy;money=10000;start(&s,&money);assert(s.hosting.stage==0&&(s.competitions.history[0].host&GLE_HOST_RIVAL)&&money==10000);
 setup(&s,7);s.rank=6;s.population=24;s.reputation=180;s.landmarkTier=2;
 s.plots[1].building=GLE_DOJO;s.plots[1].tier=1;s.plots[2].building=GLE_DOJO;s.plots[2].tier=1;s.plots[3].building=GLE_HOTEL;s.plots[3].tier=2;
 assert(!GleHostingEligible(&s,3));s.plots[1].tier=2;assert(GleHostingEligible(&s,3));s.rocket.stage=1;assert(!GleHostingEligible(&s,3));s.rocket.stage=0;
 money=100000;s.competitions.count=2;announce(&s,&money);s.hosting.stage=GLE_HOST_INVITED;
 assert(GleHostingBook(&s,2,1,&money)==GLE_NOT_READY);s.honor=100;assert(GleHostingFee(&s,3)==13500);
 assert(GleHostingBook(&s,3,1,&money)==GLE_OK&&money==86500&&GleEventFormat(&s.competitions.current)==3);
 income=GleHostingIncome(&s,3);assert(income<=30000);s.rocket.stage=1;assert(GleHostingIncome(&s,3)==income/2);s.rocket.stage=0;
 start(&s,&money);assert(s.competitions.stage==3&&s.hosting.stage==GLE_HOST_BOOKED);
 assert(GleCompetitionFinalize(&s,&money)==GLE_OK&&money==86500+income&&s.hosting.stage==0);
 before=money;assert(GleCompetitionFinalize(&s,&money)==GLE_NOT_READY&&money==before);
 setup(&s,9);s.rank=3;s.population=12;s.reputation=65535;s.plots[1].building=GLE_DOJO;s.plots[1].tier=1;money=GLE_MAX_MONEY;announce(&s,&money);
 assert(GleHostingBook(&s,1,0,&money)==GLE_OK&&GleEventFormat(&s.competitions.current)==1);money=GLE_MAX_MONEY-1;start(&s,&money);
 assert(money==GLE_MAX_MONEY&&s.reputation==65535&&GleEventFormat(&s.competitions.history[0])==1);
 s.activity=UINT32_MAX-1;GleHostingInit(&s);assert(s.hosting.nextAt==UINT32_MAX);
 puts("PASS hosting qualification, rival bids, invitations/declines, atomic fees, financial risk, unattended revenue, formats, single settlement, saturation and persistence");return 0;
}
