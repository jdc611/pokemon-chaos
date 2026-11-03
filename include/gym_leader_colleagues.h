#ifndef GUARD_GYM_LEADER_COLLEAGUES_H
#define GUARD_GYM_LEADER_COLLEAGUES_H
struct MapEvents;
struct ObjectEventTemplate;
struct Trainer;
struct TrainerMon;
bool32 GymLeaderConfigureColleagues(struct MapEvents *,struct ObjectEventTemplate *,u32 capacity);
bool32 GymLeaderPrepareColleagueRoster(struct Trainer *,struct TrainerMon *);
u32 GymLeaderColleagueSelected(void);
bool32 GymLeaderColleagueAvailable(void);
void GymLeaderSettleColleague(bool32 won);
u32 GymLeaderPrepareColleagueFriendly(void);
#endif
