#ifndef GUARD_GYM_LEADER_ROCKET_H
#define GUARD_GYM_LEADER_ROCKET_H
struct Trainer;
struct TrainerMon;
bool32 GymLeaderRocketBattleAvailable(void);
bool32 GymLeaderPrepareRocketRoster(struct Trainer *,struct TrainerMon *);
u32 GymLeaderPrepareTownDefense(void);
#endif
