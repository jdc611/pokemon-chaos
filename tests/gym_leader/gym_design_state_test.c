#include "gym_leader_state.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){struct GleState s,copy;uint32_t m=100000,before;GleInit(&s,42);assert(sizeof(s)==3200);assert(GleChooseIdentity(&s,0,4,1)==GLE_OK);copy=s;
assert(GleGymDesignPurchase(&s,0,1,&m)==GLE_NOT_READY&&m==100000&&!memcmp(&s,&copy,sizeof(s)));
assert(GleGymDesignPurchase(&s,3,0,&m)==GLE_NOT_READY);s.highestRank=3;m=0;copy=s;assert(GleGymDesignPurchase(&s,3,0,&m)==GLE_NOT_ENOUGH_MONEY&&!memcmp(&s,&copy,sizeof(s)));
m=100000;assert(GleGymDesignPurchase(&s,3,0,&m)==GLE_OK&&m==95000&&s.gymDesign.tier==1);assert(GleGymDesignPurchase(&s,0,1,&m)==GLE_OK&&s.gymDesign.layout==1);assert(GleGymDesignPurchase(&s,1,1,&m)==GLE_OK&&s.gymDesign.puzzle==1);
before=m;assert(GleGymDesignPurchase(&s,1,1,&m)==GLE_DUPLICATE&&m==before);assert(GleGymDesignPurchase(&s,0,2,&m)==GLE_NOT_READY&&m==before);
s.honor=100;assert(GleGymDesignPrice(&s,2,3)==675);assert(GleGymDesignPurchase(&s,2,3,&m)==GLE_OK&&s.gymDesign.theme==3&&s.gymDesign.layout==1&&s.gymDesign.puzzle==1);
s.highestRank=7;assert(GleGymDesignPurchase(&s,3,0,&m)==GLE_OK&&s.gymDesign.tier==2);assert(GleGymDesignPurchase(&s,3,0,&m)==GLE_OK&&s.gymDesign.tier==3);before=m;assert(GleGymDesignPurchase(&s,3,0,&m)==GLE_DUPLICATE&&m==before);
assert(GleGymDesignPurchase(&s,0,3,&m)==GLE_OK&&GleGymDesignPurchase(&s,1,3,&m)==GLE_OK);assert(GleGymDesignPurchase(&s,1,0,&m)==GLE_OK&&s.gymDesign.puzzle==0&&s.gymDesign.layout==3&&s.gymDesign.theme==3&&s.gymDesign.tier==3);
copy=s;before=m;assert(GleGymDesignPurchase(&s,2,6,&m)==GLE_INVALID&&m==before&&!memcmp(&s,&copy,sizeof(s)));
puts("PASS gym design qualification, demotion retention, atomic fees, duplicates, independent settings, honor discount and exact save budget");return 0;}
