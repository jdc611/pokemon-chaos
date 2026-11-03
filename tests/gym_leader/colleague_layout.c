#include "global.h"
#include "constants/battle_move_resolution.h"
#include "constants/flags.h"
#include "data.h"
#include "pokemon.h"
#include "constants/maps.h"
#include "constants/battle_ai.h"
#include <stddef.h>
const unsigned gleColleagueLayout[]={
 offsetof(struct GleState,colleagueWins),offsetof(struct GleState,colleaguePendingVouchers),sizeof(struct GleState),GLE_VERSION,
 MAP_PEWTER_CITY_GYM,MAP_CERULEAN_CITY_GYM,MAP_VERMILION_CITY_GYM,MAP_CELADON_CITY_GYM,MAP_FUCHSIA_CITY_GYM,MAP_SAFFRON_CITY_GYM,MAP_CINNABAR_ISLAND_GYM,
 TYPE_ROCK,TYPE_WATER,TYPE_ELECTRIC,TYPE_GRASS,TYPE_POISON,TYPE_PSYCHIC,TYPE_FIRE,TYPE_ICE,TYPE_FIGHTING,TYPE_GHOST,TYPE_DRAGON,
 SPECIES_TORTERRA,MOVE_GIGA_DRAIN,offsetof(struct TrainerMon,species),sizeof(struct TrainerMon),offsetof(struct Trainer,trainerPic),offsetof(struct GleVoucher,tier),OBEYS,FLAG_TEMP_2,
};
