#include "global.h"
#include "battle.h"
#include "move.h"
#include <stddef.h>
const unsigned campaignLayout[]={sizeof(struct BattlePokemon),offsetof(struct BattlePokemon,moves),offsetof(struct BattlePokemon,pp),offsetof(struct BattlePokemon,types),sizeof(enum Move),sizeof(enum Type),sizeof(struct MoveInfo),NUMBER_OF_MON_TYPES,offsetof(struct BattlePokemon,attack),offsetof(struct BattlePokemon,spAttack),offsetof(struct BattlePokemon,defense),offsetof(struct BattlePokemon,spDefense),offsetof(struct BattleScripting,learnMoveState),offsetof(struct BattleStruct,expGetterMonId)};
