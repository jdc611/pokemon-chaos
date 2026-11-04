#include "gym_leader_state.h"
#include <string.h>
#include <limits.h>
static uint32_t Later(uint32_t n,uint32_t delta){return n>UINT32_MAX-delta?UINT32_MAX:n+delta;}
static void Pay(uint32_t *money,uint32_t n){*money=*money>=GLE_MAX_MONEY || n>GLE_MAX_MONEY-*money?GLE_MAX_MONEY:*money+n;}
uint8_t GleEventEntrant(const struct GleEventRecord *r,uint8_t n){return n<8?(r->entrants>>(n*4))&15:15;}
uint8_t GleEventWinner(const struct GleEventRecord *r,uint8_t n){return n<7?(r->winners>>(n*4))&15:15;}
static void Winner(struct GleEventRecord *r,unsigned n,unsigned id){r->winners=(r->winners&~(15u<<(n*4)))|(id<<(n*4));}
static unsigned Side(const struct GleEventRecord *r,unsigned match,unsigned side){return match<4?GleEventEntrant(r,match*2+side):GleEventWinner(r,(match-4)*2+side);}
void GleCompetitionInit(struct GleState *s){memset(&s->competitions,0,sizeof(s->competitions));s->competitions.nextAt=Later(s->activity,160);s->competitions.champion=GLE_NONE;s->competitions.betOn=GLE_NONE;}
int GleCompetitionEligible(const struct GleState *s){return GleValid(s) && (s->competitions.current.kindRank&1? s->rank>=6 && s->regionalWinner:s->rank>=2);}
static int Pending(const struct GleCompetitionState *c){unsigned i;for(i=0;i<7;i++)if(GleEventWinner(&c->current,i)==15)return i;return -1;}
int GleCompetitionOpponent(const struct GleState *s){int m;if(!GleValid(s)||s->competitions.stage!=2||!s->competitions.joined)return -1;m=Pending(&s->competitions);if(m<0)return -1;unsigned a=Side(&s->competitions.current,m,0),b=Side(&s->competitions.current,m,1);return a==0?(int)b:b==0?(int)a:-1;}
static void Finish(struct GleState *s,uint32_t *money){
    struct GleCompetitionState *c=&s->competitions;unsigned winner=GleEventWinner(&c->current,6),kind=c->current.kindRank&1;
    if(c->count>=GLE_EVENT_HISTORY)return;
    c->current.completedAt=s->activity>65535?65535:s->activity;
    c->history[c->count++]=c->current;
    if(c->wager && c->betOn==winner)Pay(money,c->wager*c->betOdds);
    if(kind){c->champion=winner;c->archiveCount++;s->champion=winner==0;if(winner==0&&s->championshipWins<65535)s->championshipWins++;if(winner==1&&s->rivalChampionshipWins<65535)s->rivalChampionshipWins++;}
    if(winner==0){if(!kind)s->regionalWinner=1;
        if(kind){unsigned k,all=1;for(k=209;k<217;k++)if(!(s->uniqueObtained[k/8]&(1u<<(k%8))))all=0;if(all)Pay(money,5000);}
        Pay(money,(kind?5000:2000)*s->rank);s->reputation=s->reputation>65515?65535:s->reputation+20;}
    c->stage=0;c->joined=0;c->wager=0;c->betOn=GLE_NONE;c->nextAt=Later(s->activity,140+GleMix(s->seed^c->count)%101);
}
static void Advance(struct GleState *s,uint32_t *money){
    struct GleCompetitionState *c=&s->competitions;int m;while((m=Pending(c))>=0){unsigned a=Side(&c->current,m,0),b=Side(&c->current,m,1);if(c->joined&&(a==0||b==0))return;Winner(&c->current,m,(GleMix(s->seed^((c->count+1)*0x91e10da5u)^m)&1)?a:b);}
    if(c->current.kindRank&1)c->stage=3;else Finish(s,money);
}
void GleCompetitionActivity(struct GleState *s,uint32_t *money){
    struct GleCompetitionState *c=&s->competitions;unsigned ids[8],i,j,t,kind;
    if(!GleValid(s)||!money)return;
    if(c->stage==1 && s->activity>=c->closesAt){c->stage=2;Advance(s,money);return;}
    if(c->stage||c->count>=GLE_EVENT_HISTORY||s->activity<c->nextAt)return;
    /* Every third event is Kanto, including before player qualification. */
    kind=c->count%3==2;
    /* Never overwrite history or a winning team when archive capacity fills. */
    if(kind&&c->archiveCount>=32)return;
    for(i=0;i<8;i++)ids[i]=i+1;

    for(i=7;i>0;i--){j=GleMix(s->seed^((c->count+1)<<8)^i)%(i+1);t=ids[i];ids[i]=ids[j];ids[j]=t;}
    memset(&c->current,0,sizeof(c->current));c->current.winners=0xffffffffu;
    for(i=0;i<8;i++)c->current.entrants|=ids[i]<<(i*4);
    /* A reigning player is invited, but absent participation is simulated. */
    c->current.kindRank=kind|((s->rank-1)<<1);c->current.host=kind?3:GleMix(s->seed^c->count)%3;
    c->stage=1;c->joined=0;c->wager=0;c->betOn=GLE_NONE;c->closesAt=Later(s->activity,64);
}
int GleCompetitionJoin(struct GleState *s){
    struct GleCompetitionState *c=&s->competitions;unsigned i,replace=8,id;
    if(!GleValid(s)||c->stage!=1||!GleCompetitionEligible(s))return GLE_NOT_READY;
    if(c->joined)return GLE_DUPLICATE;
    /* Bets freeze the entrant list. Joining must precede betting. */
    if(c->wager)return GLE_NOT_READY;
    for(i=0;i<8;i++){id=GleEventEntrant(&c->current,i);if(id==0){replace=i;break;}if(id!=1&&id!=c->champion)replace=i;}
    if(replace>=8)return GLE_INVALID;
    c->current.entrants&=~(15u<<(replace*4));c->joined=1;return GLE_OK;
}
int GleCompetitionBet(struct GleState *s,uint8_t entrant,uint32_t amount,uint32_t *money){
    struct GleCompetitionState *c=&s->competitions;unsigned i;
    if(!GleValid(s)||!money||c->stage!=1||c->wager||!amount||amount>5000)return GLE_NOT_READY;
    for(i=0;i<8;i++)if(GleEventEntrant(&c->current,i)==entrant)break;
    if(i==8)return GLE_INVALID;
    if(*money<amount)return GLE_NOT_ENOUGH_MONEY;
    c->wager=amount;c->betOn=entrant;c->betOdds=8;*money-=amount;return GLE_OK;
}
int GleCompetitionResult(struct GleState *s,int won,uint32_t *money){
    int opponent=GleCompetitionOpponent(s),m;if(!money||opponent<0)return GLE_NOT_READY;
    m=Pending(&s->competitions);Winner(&s->competitions.current,m,won?0:opponent);Advance(s,money);return GLE_OK;
}

int GleCompetitionFinalize(struct GleState *s,uint32_t *money){if(!GleValid(s)||!money||s->competitions.stage!=3||Pending(&s->competitions)>=0)return GLE_NOT_READY;Finish(s,money);return GLE_OK;}
