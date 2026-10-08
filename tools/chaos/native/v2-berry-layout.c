#include "global.h"
#include "battle.h"
#include "constants/battle_script_commands.h"
const unsigned layout[]={offsetof(struct BattlePokemon,item),offsetof(struct BattlePokemon,hp),offsetof(struct BattlePokemon,maxHP),BS_ATTACKER,HOLD_EFFECT_RESTORE_HP};
