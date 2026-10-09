#ifndef GUARD_CHAOS_MOVES_H
#define GUARD_CHAOS_MOVES_H
#include "pokemon.h"
const struct LevelUpMove *ChaosGetLevelUpLearnset(enum Species species, const struct LevelUpMove *original);
const u16 *ChaosGetEggLearnset(enum Species species, const u16 *original);
#endif
