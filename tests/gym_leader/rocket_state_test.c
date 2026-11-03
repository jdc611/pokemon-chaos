#include "gym_leader_state.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static void Setup(struct GleState *s,unsigned seed) {
 GleInit(s,seed);GleChooseIdentity(s,0,4,1);s->rank=5;s->population=24;s->capacity=24;s->activity=120;
 s->plots[0].building=GLE_HOUSE;s->plots[0].tier=2;
 s->plots[1].building=GLE_MUSEUM;s->plots[1].tier=1;s->rocket.nextAt=0;
}
int main(void) {
 struct GleState s,a,b,saved;unsigned i,seed;uint32_t funds=0,other=0;
 Setup(&s,13);s.rank=1;GleRocketActivity(&s);assert(!s.rocket.stage);
 s.rank=2;s.population=5;GleRocketActivity(&s);assert(!s.rocket.stage);
 s.population=6;s.rocket.nextAt=121;GleRocketActivity(&s);assert(!s.rocket.stage);
 s.rocket.nextAt=0;GleRocketActivity(&s);assert(s.rocket.stage==1 && s.rocket.remaining);
 saved=s;assert(GleRocketBattleResult(&s,0)==GLE_OK && s.rocket.losses==1);
 assert(s.rocket.remaining==saved.rocket.remaining && s.rank==saved.rank && s.rankXp==saved.rankXp && s.queueCount==saved.queueCount && s.losses==saved.losses);
 for(seed=0;seed<10000;seed++){Setup(&s,seed);GleRocketActivity(&s);if(s.rocket.heist)break;}
 assert(seed<10000 && s.rocket.kind==2 && s.rocket.stolen==1);
 saved=s;
 while(s.rocket.stage==1)assert(GleRocketBattleResult(&s,1)==GLE_OK);
 assert(s.rocket.stage==2 && s.rocket.stolen==1);
 assert(GleResolveRocket(&s,0,&funds)==GLE_OK && funds==5000 && s.honor==20 && !s.rocket.stage && s.rocket.stolen==1);
 assert(!memcmp(saved.plots,s.plots,sizeof(s.plots)));
 assert(GleResolveRocket(&s,0,&funds)==GLE_NOT_READY && funds==5000);
 assert(s.rocket.nextAt>=s.activity+80 && s.rocket.nextAt<s.activity+160);
 Setup(&a,17);a.rank=8;a.population=40;b=a;
 for(i=0;i<3;i++){a.recruits[i].id=b.recruits[i].id=i+1;a.recruits[i].potential=b.recruits[i].potential=5;a.recruits[i].active=1;}
 GleRocketActivity(&a);GleRocketActivity(&b);
 assert(a.rocket.staffHelp==2 && b.rocket.staffHelp==0 && a.rocket.remaining+2==b.rocket.remaining && a.rocket.remaining>=1);
 Setup(&a,29);a.activity=11;a.rocket.nextAt=UINT32_MAX;a.plots[1].building=GLE_HOTEL;b=a;b.rocket.stage=1;b.rocket.remaining=2;
 funds=other=0;GleActivity(&a,&funds);GleActivity(&b,&other);assert(funds>0 && other==funds/2);
 Setup(&s,33);s.rank=7;s.rocket.cleared=3;s.rocket.stolen=2;GleRocketActivity(&s);
 assert(s.rocket.kind==3 && s.rocket.stage==1);
 while(s.rocket.stage==1)GleRocketBattleResult(&s,1);
 saved=s;funds=0;assert(GleResolveRocket(&s,0,&funds)==GLE_OK && s.rocket.bossDefeated && s.rocket.recovered==2 && !s.rocket.stolen && funds==7000);
 funds=0;assert(GleResolveRocket(&saved,1,&funds)==GLE_OK && !saved.rocket.bossDefeated && saved.rocket.stolen==2 && saved.honor==-30 && funds==10500);
 puts("Rocket core: rank/population/activity gates, persistent losses, successful heists, active staff, income pressure, anti-farm settlement and Giovanni recovery passed");
}
