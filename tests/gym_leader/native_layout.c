#include "global.h"
#include "pokemon.h"
#include <stddef.h>
#include "task.h"
const unsigned gleLayout[] = {
 offsetof(struct SaveBlock1, gymLeader), sizeof(struct GleState),
 offsetof(struct GleState, rank), offsetof(struct GleState, gymType),
 offsetof(struct SaveBlock1, rivalName), sizeof(struct Pokemon),
 SPECIES_GLASTRIER, SPECIES_GLE_ROYAL_CORVIKNIGHT,
 SPECIES_GLE_REVENANT_ANNIHILAPE, SPECIES_GLE_ETERNAL_HERO_PALAFIN,
 MON_DATA_SPECIES, MON_DATA_LEVEL, MON_DATA_ABILITY_NUM,
 offsetof(struct Pokemon, hp), offsetof(struct Pokemon, maxHP),
 sizeof(struct SaveBlock1), sizeof(((struct SaveBlock1 *)0)->secretBases),
 offsetof(struct GleState, openingStage), offsetof(struct GleState, draftRound),
 sizeof(struct Task), offsetof(struct Task, data),
 offsetof(struct GleState, draftSpecies), offsetof(struct GleState, draftAbilities),
 MAP_GLE_GYM_INTERIOR, MAP_GLE_LEAGUE_HALL,
 offsetof(struct SaveBlock1, location),
};
