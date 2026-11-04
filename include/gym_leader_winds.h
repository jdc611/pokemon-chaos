#ifndef GUARD_GLE_WINDS_H
#define GUARD_GLE_WINDS_H
struct Pokemon;
bool32 GymLeaderWindDescription(u32 index,u8 *dest);
bool32 GymLeaderWindMove(u32 index);
void GymLeaderMoveWinds(void);
bool32 GymLeaderWindAt(u8 group,u8 num);
bool32 GymLeaderCreateWind(void);
void GymLeaderUpdateWind(struct Pokemon *mon);
bool32 GymLeaderWindKeepActive(void);
void GymLeaderWindLocation(u8 *group,u8 *num);
bool32 GymLeaderWindCaption(u8 *dest);
bool32 GymLeaderWindSpecies(u16 species);
#endif
