#ifndef GUARD_GLE_COMPETITIONS_NATIVE_H
#define GUARD_GLE_COMPETITIONS_NATIVE_H
bool32 GymLeaderChampionTMReusable(u16 item);
u32 GymLeaderTrophyAt(u32 slot);
struct Trainer;struct TrainerMon;
bool32 GymLeaderPrepareCompetitionRoster(struct Trainer *,struct TrainerMon *);
void GymLeaderSettleCompetition(bool32 won);
void GymLeaderCompetitionArchive(void);
void GymLeaderCaptureCompetitionTeam(void);
u32 GymLeaderPrepareCompetition(void);
bool32 GymLeaderCompetitionAtVenue(void);
#endif
