#ifndef GUARD_GLE_FINALE_H
#define GUARD_GLE_FINALE_H
struct Trainer;struct TrainerMon;
bool32 GymLeaderFinalOakEligible(void);
bool32 GymLeaderFinalOakComplete(void);
bool32 GymLeaderPrepareFinalOakRoster(struct Trainer *,struct TrainerMon *);
void GymLeaderSettleFinalOak(bool32 won);
u32 GymLeaderPrepareFinalOak(void);
#endif
