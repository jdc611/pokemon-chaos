#include "gym_leader_state.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void announce(struct GleState *s,uint32_t *money){s->activity=s->competitions.nextAt;GleCompetitionActivity(s,money);assert(s->competitions.stage==1);}
static void start(struct GleState *s,uint32_t *money){s->activity=s->competitions.closesAt;GleCompetitionActivity(s,money);}
static void check(const struct GleEventRecord *r){unsigned seen=0,i,a,b,w;for(i=0;i<8;i++){a=GleEventEntrant(r,i);assert(a<=8&&!(seen&(1u<<a)));seen|=1u<<a;}for(i=0;i<7;i++){a=i<4?GleEventEntrant(r,2*i):GleEventWinner(r,2*(i-4));b=i<4?GleEventEntrant(r,2*i+1):GleEventWinner(r,2*(i-4)+1);w=GleEventWinner(r,i);assert(w==a||w==b);}}
int main(void){
 struct GleState s,copy;uint32_t money=100000,before;unsigned i,j;int op;
 assert(sizeof(s)<=3200);GleInit(&s,1234);assert(GleChooseIdentity(&s,0,4,1)==GLE_OK);
 announce(&s,&money);assert(!GleCompetitionEligible(&s));assert(GleCompetitionJoin(&s)==GLE_NOT_READY);s.rank=2;
 assert(GleCompetitionJoin(&s)==GLE_OK);assert(GleCompetitionJoin(&s)==GLE_DUPLICATE);before=money;
 assert(GleCompetitionBet(&s,0,500,&money)==GLE_OK&&money==before-500);assert(GleCompetitionBet(&s,1,500,&money)==GLE_NOT_READY);
 copy=s;start(&s,&money);assert(s.competitions.stage==2);i=0;while((op=GleCompetitionOpponent(&s))>=0){assert(op>0&&op<=8);assert(GleCompetitionResult(&s,1,&money)==GLE_OK);i++;}assert(i==3&&s.regionalWinner&&s.competitions.count==1&&money==before-500+4000+4000);check(&s.competitions.history[0]);
 assert(!memcmp(&copy.competitions.current.entrants,&s.competitions.history[0].entrants,4));before=money;assert(GleCompetitionResult(&s,1,&money)==GLE_NOT_READY&&money==before);
 announce(&s,&money);start(&s,&money);assert(s.competitions.count==2);check(&s.competitions.history[1]);
 announce(&s,&money);assert(s.competitions.current.kindRank&1);assert(GleCompetitionJoin(&s)==GLE_NOT_READY);s.rank=6;assert(GleCompetitionJoin(&s)==GLE_OK);start(&s,&money);while(GleCompetitionOpponent(&s)>=0)assert(GleCompetitionResult(&s,1,&money)==GLE_OK);
 assert(s.competitions.stage==3&&s.competitions.count==2&&!s.championshipWins);assert(GleCompetitionFinalize(&s,&money)==GLE_OK);assert(s.champion&&s.championshipWins==1&&s.competitions.archiveCount==1);check(&s.competitions.history[2]);assert(GleCompetitionFinalize(&s,&money)==GLE_NOT_READY);
 /* Deterministic unattended events, reigning player competes only if entered. */
 for(i=0;i<90;i++){announce(&s,&money);copy=s;before=money;start(&s,&money);start(&copy,&before);assert(!memcmp(&s.competitions,&copy.competitions,sizeof(s.competitions)));if(s.competitions.stage==3){assert(GleEventWinner(&s.competitions.current,6)!=0);assert(GleCompetitionFinalize(&s,&money)==GLE_OK);}check(&s.competitions.history[s.competitions.count-1]);}
 assert(s.competitions.archiveCount==31);announce(&s,&money);assert(GleCompetitionJoin(&s)==GLE_OK);start(&s,&money);assert(GleCompetitionOpponent(&s)>0);assert(GleCompetitionResult(&s,0,&money)==GLE_OK);assert(s.competitions.stage==0);check(&s.competitions.history[s.competitions.count-1]);
 /* Fill remaining archive without rotating any prior bracket. */
 for(j=0;j<4&&s.competitions.archiveCount<32;j++){announce(&s,&money);start(&s,&money);if(s.competitions.stage==3)GleCompetitionFinalize(&s,&money);}assert(s.competitions.archiveCount==32);copy=s;
 for(j=0;j<10;j++){s.activity+=1000;GleCompetitionActivity(&s,&money);if(s.competitions.stage==1)start(&s,&money);}assert(!memcmp(copy.competitions.history,s.competitions.history,copy.competitions.count*sizeof(struct GleEventRecord)));
 /* Betting freezes entrants; overflow and invalid identities cannot debit. */
 GleInit(&s,78);s.rank=6;announce(&s,&money);before=money;assert(GleCompetitionBet(&s,15,500,&money)==GLE_INVALID&&money==before);assert(GleCompetitionBet(&s,1,5001,&money)==GLE_NOT_READY);assert(GleCompetitionBet(&s,1,500,&money)==GLE_OK);assert(GleCompetitionJoin(&s)==GLE_NOT_READY);money=GLE_MAX_MONEY;start(&s,&money);assert(money<=GLE_MAX_MONEY);
 printf("PASS competition brackets, eligibility, all three player rounds, elimination, independent NPC events, deterministic reload, wagers, one-time rewards, pending championship archival, permanent capacity; state=%zu\n",sizeof(s));
}
