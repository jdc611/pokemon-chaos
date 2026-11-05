#include "gym_leader_state.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void unique(const struct GleState *s) {
    unsigned i,mask=0;
    for(i=0;i<GLE_RECRUITS;i++) {
        uint8_t p=GleTrainerStation(s,i);
        if(p==GLE_NONE)continue;
        assert(p<5 && !(mask&(1u<<p))); mask|=1u<<p;
    }
}
int main(void) {
    struct GleState s,copy; unsigned i,j;
    GleInit(&s,42); s.rank=s.highestRank=8;
    assert(sizeof(s)==3200 && GLE_VERSION==15);
    for(i=0;i<5;i++){assert(GleRecruitTrainer(&s,i+1,3,80)==GLE_OK);assert(GleAssignTrainer(&s,i,1)==GLE_OK);}
    for(i=0;i<5;i++)assert(GleTrainerStation(&s,i)==i);
    assert(GlePositionTrainer(&s,4,1)==GLE_OK && GleTrainerStation(&s,4)==0);
    assert(GleTrainerStation(&s,0)==1);unique(&s);
    copy=s;assert(GlePositionTrainer(&s,0,1)==GLE_NO_ROOM && !memcmp(&s,&copy,sizeof(s)));
    assert(GleAssignTrainer(&s,4,1)==GLE_OK && s.recruits[4].active==2);
    for(i=0;i<5;i++){assert(GlePositionTrainer(&s,4,0)==GLE_OK);assert(GlePositionTrainer(&s,4,i+1)==GLE_OK);unique(&s);}
    for(i=0;i<5;i++)assert(GlePositionTrainer(&s,i,0)==GLE_OK);
    for(i=0;i<5;i++)assert(GlePositionTrainer(&s,i,5-i)==GLE_OK);
    for(i=0;i<5;i++)assert(GleTrainerStation(&s,i)==4-i);
    copy=s;copy.lossStreak=4;GleDefenseResult(&copy,0);
    assert(copy.rank==7 && !copy.recruits[4].active);
    for(i=0;i<4;i++)assert(copy.recruits[i].active==6-i);
    unique(&copy);
    /* Malformed duplicate/unknown encodings must still render without overlap. */
    for(i=0;i<5;i++)for(j=0;j<256;j++){copy=s;copy.recruits[i].active=j;unique(&copy);}
    s.recruits[4].away=2;assert(GleTrainerStation(&s,4)==GLE_NONE);
    copy=s;assert(GlePositionTrainer(&s,4,0)==GLE_INVALID&&!memcmp(&s,&copy,sizeof(s)));
    assert(GlePositionTrainer(&s,0,1)==GLE_OK);unique(&s);
    s.recruits[4].away=0;unique(&s); /* Defensive fallback for duplicate return. */
    assert(GleAssignTrainer(&s,4,0)==GLE_OK && !s.recruits[4].active);
    assert(GleAssignTrainer(&s,4,1)==GLE_OK && s.recruits[4].active==1);
    copy=s;assert(GlePositionTrainer(&s,0,6)==GLE_INVALID&&!memcmp(&s,&copy,sizeof(s)));
    assert(GlePositionTrainer(&s,GLE_RECRUITS,0)==GLE_INVALID);
    memset(&s.recruits[0],0,sizeof(s.recruits[0]));assert(GleTrainerStation(&s,0)==GLE_NONE);unique(&s);
    GleInit(&s,42);s.rank=s.highestRank=8;
    assert(GleRecruitTrainer(&s,1,3,20)==GLE_OK);
    assert(GleAssignTrainer(&s,0,1)==GLE_OK && GlePositionTrainer(&s,0,5)==GLE_OK);
    s.recruits[0].warnings=3;s.reputation=50;s.lossStreak=3;s.activity=11;
    {uint32_t money=0;GleActivity(&s,&money);}
    assert(s.recruits[0].away && !s.recruits[0].active && GleTrainerStation(&s,0)==GLE_NONE);
    puts("PASS station reservations, atomic conflict rejection, automatic fill, assignment retention, departure, malformed fallback and unchanged save budget");
}
