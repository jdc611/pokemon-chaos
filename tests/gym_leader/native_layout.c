#include "global.h"
#include "pokemon.h"
#include "battle.h"
#include <stddef.h>
#include "task.h"
#include "constants/item.h"
#include "pokemon_storage_system.h"
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
 offsetof(struct GleState, registeredCount),
 offsetof(struct GleState, registeredPersonality), offsetof(struct GleState, registeredTrainerId),
 offsetof(struct GleState, defenses), offsetof(struct GleState, losses), offsetof(struct GleState, rankXp),
 offsetof(struct GleState, queueCount), sizeof(struct BoxPokemon), offsetof(struct PokemonStorage, boxes),
 MON_DATA_EXP, MON_DATA_PERSONALITY, MON_DATA_OT_ID, MON_DATA_MOVE1, MON_DATA_HP,
 MON_DATA_HP_EV, MON_DATA_HELD_ITEM,
 ITEM_GYM_SIGNATURE_TM, POCKET_KEY_ITEMS, offsetof(struct GleState, signatureMove), MON_DATA_PP1,
 offsetof(struct GleState, pendingInspection), offsetof(struct GleState, lossStreak),
 offsetof(struct GleState, rankFloor), sizeof(struct BattlePokemon), offsetof(struct BattlePokemon, hp), offsetof(struct BattlePokemon, maxHP),
 offsetof(struct GleState, plots), sizeof(struct GlePlot), offsetof(struct GleState, population), offsetof(struct GleState, capacity),
 offsetof(struct GleState, activity), offsetof(struct GleState, interiorPlot),
 MAP_SAFFRON_CITY, MAP_CELADON_CITY, MAP_FUCHSIA_CITY,
 MAP_GLE_CENTER_INTERIOR, MAP_GLE_MART_INTERIOR, MAP_GLE_BUILDING_INTERIOR,
 offsetof(struct SaveBlock1, money),



};
