#ifndef GUARD_IRONMON_H
#define GUARD_IRONMON_H
#include "pokemon.h"
struct Trainer;
struct TrainerMon;
struct TrainerGenerator;
#define IRONMON_STATE_MAGIC 0x494D3031u
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
u32 IronmonTrainerSeed(const struct Trainer *trainer);
void IronmonGenerateTrainerMon(struct Pokemon *mon, const struct TrainerMon *entry, struct TrainerGenerator *trainer);
void IronmonGiveStarter(enum Species species);
u32 IronmonAcceptCapture(struct Pokemon *mon);
u32 IronmonPivotFloor(void);
bool32 IronmonPermitHealing(void);
void IronmonAuthorizeCenterHealing(void);
void IronmonRecordBattleEnd(void);
#endif
