#include "global.h"
#include "battle.h"
#include "pokemon.h"
#include <stddef.h>
const unsigned layout[]={offsetof(struct BattlePokemon,types),offsetof(struct BattlePokemon,ability),offsetof(struct BattlePokemon,moves),sizeof(enum Type),sizeof(enum Ability),sizeof(enum Move),offsetof(struct SaveBlock3,minimalGrindingMode)};
