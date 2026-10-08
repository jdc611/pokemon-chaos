#ifndef GUARD_CHAOS_V2_H
#define GUARD_CHAOS_V2_H
#include "pokemon.h"
bool32 ChaosTryCareMilestone(s16 x, s16 y);
void ChaosCareNextPage(void);
void ChaosBuyMysteryEgg(void);
void ChaosOpenAbilityMenu(void);
u32 ChaosCurrentStageRating(struct Pokemon *mon);
void ChaosFormatEvolution(const struct Evolution *evo, u8 *dest);
#endif
