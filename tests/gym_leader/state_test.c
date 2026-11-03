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
 {
  struct GleState staff;uint32_t funds=0;unsigned matches,active;
  GleInit(&staff,91);assert(GleChooseIdentity(&staff,0,4,1)==GLE_OK);
  assert(GleRecruitTrainer(&staff,14,3,80)==GLE_OK);
  for(i=0;i<60;i++)GleActivity(&staff,&funds);
  assert(staff.recruits[0].wins+staff.recruits[0].losses==0);
  assert(GleAssignTrainer(&staff,0,1)==GLE_OK);
  for(i=0;i<120;i++)GleActivity(&staff,&funds);
  matches=staff.recruits[0].wins+staff.recruits[0].losses;
  assert(matches>0 && staff.queueCount>=1 && staff.rank==1 && !staff.rankXp && !staff.defenses && !staff.losses);
  staff.queue[0]=2;staff.queueCount=2;
  for(i=0;i<60;i++)GleActivity(&staff,&funds);
  assert(staff.queue[0]==2 && staff.recruits[0].wins+staff.recruits[0].losses==matches);
  staff.rank=8;staff.rankFloor=6;
  for(i=1;i<5;i++)assert(GleRecruitTrainer(&staff,14+i,3,80)==GLE_OK);
  for(i=0;i<5;i++)assert(GleAssignTrainer(&staff,i,1)==GLE_OK);
  for(i=0;i<5;i++)GleDefenseResult(&staff,0);
  active=0;for(i=0;i<10;i++)active+=staff.recruits[i].active;
  assert(staff.rank==7 && active==4 && staff.recruits[4].id && !staff.recruits[4].active);
  GleInit(&staff,91);assert(GleChooseIdentity(&staff,0,4,1)==GLE_OK);
  assert(GleRecruitTrainer(&staff,14,1,40)==GLE_OK && GleAssignTrainer(&staff,0,1)==GLE_OK);
  staff.reputation=40;staff.lossStreak=3;
  for(i=0;i<36;i++)GleActivity(&staff,&funds);
  assert(staff.recruits[0].warnings==3 && !staff.recruits[0].away);
  for(i=0;i<12;i++)GleActivity(&staff,&funds);
  assert(staff.recruits[0].away && !staff.recruits[0].active);
 }
 {
  struct GleState solo,team;uint32_t funds=0,seed;
  for(seed=0;seed<1000;seed++)if(GleMix(seed^6u^15u)%100>=42 && GleMix(seed^6u^14u)%100<42)break;
  assert(seed<1000);GleInit(&solo,seed);GleChooseIdentity(&solo,0,4,1);solo.rank=3;solo.activity=5;solo.queueClock=5;
  team=solo;assert(GleRecruitTrainer(&solo,15,1,80)==GLE_OK && GleAssignTrainer(&solo,0,1)==GLE_OK);
  assert(GleRecruitTrainer(&team,14,1,80)==GLE_OK && GleRecruitTrainer(&team,15,1,80)==GLE_OK);
  assert(GleAssignTrainer(&team,0,1)==GLE_OK && GleAssignTrainer(&team,1,1)==GLE_OK);
  GleActivity(&solo,&funds);GleActivity(&team,&funds);
  assert(solo.recruits[0].losses==1 && team.recruits[1].losses==1 && team.recruits[0].wins==1);
  assert(team.queueCount+1==solo.queueCount && !team.rankXp && !solo.rankXp);
 }
 {
  struct GleState residents,copy;uint32_t funds=0,old;
  GleInit(&residents,91);GleChooseIdentity(&residents,0,1,2);
  copy=residents;assert(GleResolveResidentTask(&residents,0,0,&funds)==GLE_NOT_READY && !funds && !memcmp(&copy,&residents,sizeof(copy)));
  residents.plots[0].building=GLE_CENTER;residents.plots[0].tier=1;
  assert(GleResolveResidentTask(&residents,0,0,&funds)==GLE_OK && funds==500 && residents.honor==10);
  copy=residents;old=funds;
  assert(GleResolveResidentTask(&residents,0,0,&funds)==GLE_DUPLICATE && funds==old && !memcmp(&copy,&residents,sizeof(copy)));
  assert(GleResolveResidentTask(&residents,8,0,&funds)==GLE_INVALID);
  assert(GleResolveResidentTask(&residents,1,1,&funds)==GLE_INVALID);
  for(i=4;i<8;i++)assert(GleResolveResidentTask(&residents,i,0,&funds)==GLE_OK);
  assert(residents.honor==170 && residents.reputation==125);
  assert(!memcmp(residents.uniqueObtained,copy.uniqueObtained,25));
  GleInit(&residents,92);GleChooseIdentity(&residents,1,1,2);
  for(i=4;i<8;i++)assert(GleResolveResidentTask(&residents,i,1,&funds)==GLE_OK);
  assert(residents.honor==-240 && residents.reputation==88);
 }
 printf("GLE state scenarios passed; state size %zu bytes\n",sizeof(s));
}
