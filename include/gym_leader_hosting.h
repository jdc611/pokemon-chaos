#ifndef GUARD_GYM_LEADER_HOSTING_H
#define GUARD_GYM_LEADER_HOSTING_H
#include <stdint.h>
enum GleHostFormat {GLE_HOST_SHOWCASE,GLE_HOST_DEFENSE,GLE_HOST_REGIONAL,GLE_HOST_KANTO};
enum GleHostStage {GLE_HOST_NONE,GLE_HOST_INVITED,GLE_HOST_BOOKED};
#define GLE_HOST_PLAYER 16
#define GLE_HOST_RIVAL 32
struct GleHostingState {uint32_t nextAt;uint16_t event;uint8_t stage,format;};
struct GleState;struct GleEventRecord;
void GleHostingInit(struct GleState *);
int GleHostingEligible(const struct GleState *,uint8_t);
uint32_t GleHostingFee(const struct GleState *,uint8_t);
uint32_t GleHostingIncome(const struct GleState *,uint8_t);
int GleHostingBook(struct GleState *,uint8_t,int,uint32_t *);
int GleHostingDecline(struct GleState *);
void GleHostingAnnouncement(struct GleState *);
void GleHostingStart(struct GleState *);
void GleHostingFinish(struct GleState *,uint32_t *);
uint8_t GleEventFormat(const struct GleEventRecord *);
uint8_t GleEventVenue(const struct GleEventRecord *);
int GymLeaderHostingTown(void);
#endif
