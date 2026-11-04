#ifndef GUARD_GYM_LEADER_RECRUIT_H
#define GUARD_GYM_LEADER_RECRUIT_H
struct MapHeader;
struct GleState;
void GymLeaderMigratePersonalBonds(struct GleState *s);
u16 GymLeaderCelebrationStaffGraphics(void);
struct Trainer;
struct TrainerMon;
void GymLeaderRecruitResetOffer(void);
bool32 GymLeaderRecruitOffer(u16 trainer);
bool32 GymLeaderRecruitPostBattleOffer(void);
void GymLeaderConfigureStaffMap(struct MapHeader *header);
bool32 GymLeaderPrepareStaffRoster(u32 slot, struct Trainer *trainer, struct TrainerMon *entries, bool32 preview);
#endif
