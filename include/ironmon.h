#ifndef GUARD_IRONMON_H
#define GUARD_IRONMON_H
#include "pokemon.h"
struct Trainer;
struct TrainerMon;
struct TrainerGenerator;
#define IRONMON_LEGACY_STATE_MAGIC 0x494D3031u
#define IRONMON_STATE_MAGIC 0x494D3032u
#define IRONMON_RETIRED_BOX (TOTAL_BOXES_COUNT - 1)
bool32 IsIronmonRun(void);
bool32 IsIronmonHardcore(void);
bool32 IsIronmonDifficulty(u32 difficulty);
void IronmonInitializeRun(void);
void IronmonEnforcePreset(void);
u32 IronmonMix(u32 value);
bool32 IronmonMoveAllowed(enum Move move);
bool32 IronmonMoveIsStarterAttack(enum Move move);
enum Move IronmonStarterAttack(enum Species species);
enum Item IronmonHeldItem(u32 seed);
void IronmonSetTrainerIdentity(u32 id);
u32 IronmonTrainerPartySize(const struct Trainer *trainer);
u32 IronmonTrainerSeed(const struct Trainer *trainer);
void IronmonGenerateTrainerMon(struct Pokemon *mon, const struct TrainerMon *entry, struct TrainerGenerator *trainer);
void IronmonGenerateFacilityMon(struct Pokemon *mon, const struct BattleTowerPokemon *entry, u32 identity, u32 level);
void IronmonGiveStarter(enum Species species);
void IronmonPrepareWildMon(struct Pokemon *mon);
u32 IronmonAcceptCapture(struct Pokemon *mon);
u32 IronmonPivotFloor(void);
bool32 IronmonPermitHealing(void);
void IronmonAuthorizeCenterHealing(void);
void IronmonRecordBattleEnd(void);
void IronmonMarkFainted(u32 battler);
bool32 IronmonCheckRunOver(void);
void CB2_IronmonRunOver(void);
struct MapHeader;
bool32 IronmonGymTrainersDefeated(u32 section);
bool32 IronmonLeaderLocked(u32 trainer);
void IronmonOnMapLoaded(void);
bool32 IronmonEscapeLocked(void);
bool32 IronmonWarpAllowed(const struct MapHeader *destination);
enum Item IronmonOverworldItem(u32 seed);
void IronmonOnMapTransition(const struct MapHeader *destination);
void IronmonPrepareGymTM(void);
bool32 IronmonPermitItemAward(enum Item item, u32 count);
#endif
