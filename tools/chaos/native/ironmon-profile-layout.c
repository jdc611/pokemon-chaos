#include "global.h"
#include "pokemon.h"
#include "run_settings.h"
#include "ironmon.h"
#include "battle_gimmick.h"
#include "constants/opponents_frlg.h"
#include <stddef.h>
const unsigned profile_layout[] = {
    offsetof(struct SaveBlock3, starterMode), offsetof(struct SaveBlock3, bstMode),
    RUN_STARTER_CHOOSE, RUN_STARTER_RANDOM, RUN_BST_SHUFFLE, RUN_BST_RANDOM,
    MON_DATA_DYNAMAX_LEVEL, BLOCK_AI_DYNAMAX,
    TRAINER_RIVAL_OAKS_LAB_SQUIRTLE, TRAINER_RIVAL_OAKS_LAB_BULBASAUR,
    TRAINER_RIVAL_OAKS_LAB_CHARMANDER, GIMMICK_DYNAMAX,
    IRONMON_LEGACY_STATE_MAGIC, IRONMON_STATE_MAGIC, offsetof(struct IronmonRunState, bstMode),
};
