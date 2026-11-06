#include "global.h"
#include "battle.h"
#include "move.h"
#include "constants/abilities.h"
#include <stddef.h>
const unsigned moveMeta[] __attribute__((section(".qa_move_meta")))={sizeof(struct MoveInfo),sizeof(uq4_12_t),DAMAGE_CATEGORY_PHYSICAL,DAMAGE_CATEGORY_SPECIAL,TYPE_NONE,TYPE_MYSTERY,TYPE_NORMAL,TYPE_FIRE,TYPE_WATER,TYPE_ELECTRIC,TYPE_GRASS,TYPE_GROUND,TYPE_GHOST,offsetof(struct BattlePokemon,ability),sizeof(enum Ability),ABILITY_SAP_SIPPER,ABILITY_LEVITATE,ABILITY_WATER_ABSORB,ABILITY_VOLT_ABSORB,ABILITY_FLASH_FIRE,ABILITY_DRY_SKIN,ABILITY_STORM_DRAIN,ABILITY_LIGHTNING_ROD,ABILITY_MOTOR_DRIVE,ABILITY_EARTH_EATER,ABILITY_WIND_RIDER,ABILITY_WONDER_GUARD,ABILITY_SOUNDPROOF,ABILITY_MOLD_BREAKER,ABILITY_TERAVOLT,ABILITY_TURBOBLAZE};
const struct MoveInfo moveBits[] __attribute__((section(".qa_move_bits")))={{.power=1},{.type=1},{.category=1},{.accuracy=1},{.strikeCount=1},{.multiHit=1},{.priority=1},{.windMove=1},{.soundMove=1}};
