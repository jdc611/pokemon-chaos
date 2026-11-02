#ifndef GUARD_GYM_LEADER_H
#define GUARD_GYM_LEADER_H
#include "gym_leader_state.h"
struct Pokemon;
#define GLE_EXCLUSIVE_TYPES 18
#define GLE_EXCLUSIVE_PER_TYPE 4
struct GleEncounter { u16 species; u8 rank, project; };
bool32 IsGymLeaderRun(void);
struct GleState *GymLeaderState(void);
u32 GymLeaderLevelCap(void);
u32 GymLeaderPartyLimit(void);
enum Species GymLeaderExclusiveSpecies(u32 type, u32 location);
bool32 GymLeaderSpeciesReserved(enum Species species);
bool32 GymLeaderOfficialTeamLegal(void);
void GymLeaderInitialize(void);
void GymLeaderScriptStatus(void);
void GymLeaderScriptExclusive(void);
void GymLeaderScriptChooseIdentity(void);
void GymLeaderScriptActivity(void);
u32 GymLeaderSignatureOption(u32 type, u32 slot);
u32 GymLeaderBuildSpeciesPool(u32 type, u32 tier, u32 kind, bool32 starters, u16 *out, u32 capacity);
void GymLeaderAdvanceActivity(void);
u32 GymLeaderPoolCount(u32 type, u32 tier, u32 kind, bool32 starters);
bool32 GymLeaderSignaturePoolsMatchTypes(void);
bool32 GymLeaderCanLearnSignature(enum Species species, enum Move move);
u32 GymLeaderBuildStarterPool(u32 type, u16 *out, u32 capacity);
void GymLeaderRedirectWarp(s8 *group, s8 *number, s8 *warp, s8 *x, s8 *y);
#endif
