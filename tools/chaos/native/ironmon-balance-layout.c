#include "global.h"
#include "pokemon.h"
#include "move.h"
#include "constants/opponents_frlg.h"
#include <stddef.h>
const unsigned layout[]={sizeof(struct SpeciesInfo),offsetof(struct SpeciesInfo,growthRate),sizeof(struct MoveInfo),MON_DATA_EXP,TRAINER_LEADER_MISTY,
    DAMAGE_CATEGORY_STATUS,DAMAGE_CATEGORY_PHYSICAL,DAMAGE_CATEGORY_SPECIAL,MON_DATA_ATK,MON_DATA_SPATK};
