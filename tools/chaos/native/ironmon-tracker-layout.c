#include "global.h"
#include "battle.h"
#include "pokemon.h"
#include "main.h"
#include <stddef.h>
const unsigned layout[] = {sizeof(struct BattleStruct),sizeof(struct BattlePokemon),offsetof(struct Main,state),offsetof(struct BattlePokemon,attack),offsetof(struct BattlePokemon,moves),offsetof(struct BattlePokemon,pp)};
