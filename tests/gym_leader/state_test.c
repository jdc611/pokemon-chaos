#include "gym_leader_state.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void) {
 struct GleState s, saved; uint32_t money=100000; uint16_t out=0; unsigned i; uint16_t pool[]={1,2,3,4};
 GleInit(&s,17); assert(GleValid(&s)); assert(GleChooseIdentity(&s,1,4,2)==GLE_OK); assert(s.town!=s.rivalTown);
 assert(GleAddRankXp(&s,UINT32_MAX)==GLE_INSPECTION_NEEDED); assert(s.rank==2); assert(GleInspectionResult(&s,0)==GLE_OK && s.rank==2);
 while(s.rank<8) { if(s.pendingInspection) GleInspectionResult(&s,1); else GleAddRankXp(&s,10000); }
 assert(GleCap(&s)==100 && GleTeamSize(&s)==6);
 GleDefenseResult(&s,1); for(i=0;i<5;i++)GleDefenseResult(&s,0); assert(s.rank==7 && s.rankFloor==6); assert(GleAddRankXp(&s,10000)==GLE_INSPECTION_NEEDED);
 while(s.queueCount<5) { GleEnqueue(&s,0); }
 saved=s; assert(GleEnqueue(&s,2)==GLE_NO_ROOM); assert(!memcmp(saved.queue,s.queue,5)); assert(GleTakeChallenger(&s)>=0 && s.queueCount==5 && s.queue[4]==2);
 assert(GleConstruct(&s,0,GLE_HOUSE,&money)==GLE_OK); assert(s.population==0 && s.capacity==12);
 for(i=0;i<120;i++) { GleActivity(&s,&money); }
 assert(s.population>0 && s.population<=s.capacity);
 assert(GleConstruct(&s,1,GLE_CENTER,&money)==GLE_OK); saved=s; {uint32_t old=money;assert(GleConstruct(&s,2,GLE_CENTER,&money)==GLE_DUPLICATE);assert(money==old && !memcmp(&saved,&s,sizeof(s)));}
 {struct GleState themed=s;uint32_t budget=100000;themed.population=24;themed.plots[0].type=17;
 assert(GleConstruct(&themed,0,GLE_HOUSE,&budget)==GLE_OK && themed.plots[0].type==17);}
 assert(GleAwardVoucher(&s,GLE_STANDARD,1)==GLE_OK); assert(GleViewVoucher(&s,0,pool,4)==GLE_OK); saved=s;
 assert(GleViewVoucher(&s,0,pool+1,3)==GLE_OK && !memcmp(&saved,&s,sizeof(s))); assert(GleClaimVoucher(&s,0,0,&out)==GLE_OK); assert(GleSpeciesClaimed(&s,out,GLE_RARE)); assert(!GleSpeciesClaimed(&s,out,GLE_SHINY));
 assert(GleAwardVoucher(&s,GLE_RARE,2)==GLE_OK);
 assert(GleAwardVoucher(&s,GLE_RARE,2)==GLE_OK);
 assert(GleViewVoucher(&s,0,pool,4)==GLE_OK);
 assert(GleViewVoucher(&s,1,pool,4)==GLE_NOT_READY); /* other offer reserves the remaining choices */
 assert(GleClaimVoucher(&s,0,0,&out)==GLE_OK);
 assert(GleViewVoucher(&s,0,pool,4)==GLE_OK); /* released unchosen offers are eligible */
 assert(GleUniqueCapture(&s,71)==GLE_OK && GleUniqueCapture(&s,71)==GLE_DUPLICATE);
 assert(GleResearchDiscover(&s,15,3)==GLE_OK && s.projectStage[15]==3);
 for(i=0;i<10;i++) { assert(GleRecruitTrainer(&s,i+1,3,40)==GLE_OK); }
 assert(GleRecruitTrainer(&s,20,3,40)==GLE_NO_ROOM);
 saved=s; memcpy(&s,&saved,sizeof(s)); assert(GleValid(&s));
 printf("GLE state scenarios passed; state size %zu bytes\n",sizeof(s));
}
