#ifndef GUARD_CHAOS_ABILITIES_H
#define GUARD_CHAOS_ABILITIES_H
#include "pokemon.h"
bool32 ChaosAbilityIsFallback(u32 battler, enum Ability ability);
bool32 ChaosAbilitySwitchOut(u32 battler);
bool32 ChaosAbilitySwitchIn(u32 battler);
bool32 ChaosAbilityEndTurn(u32 battler);
bool32 ChaosAbilityMoveEnd(u32 battler);
bool32 ChaosAbilityBond(u32 battler);
bool32 ChaosAbilityBeforeMove(u32 battler, enum Move move);
u32 ChaosAbilityStat(u32 battler, enum Ability ability, enum Stat stat, u32 value);
s32 ChaosAbilityDamage(u32 attacker, u32 defender, enum Move move, s32 damage);
void ChaosAbilityCommitHit(u32 defender, enum Move move);
u32 ChaosAbilityHealing(u32 battler, u32 amount);
const u8 *ChaosAbilityMarker(u32 battler);
void ChaosAbilityBattleEnd(void);
void ChaosAbilityBeforeStats(struct Pokemon *mon);
void ChaosAbilityAfterStats(struct Pokemon *mon);
void ChaosAbilityPrepareCapture(u32 battler);
#endif
