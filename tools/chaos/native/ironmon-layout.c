#include "global.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "battle.h"
#include "run_settings.h"
#include "constants/opponents_frlg.h"
#include <stddef.h>
const unsigned layout[] = {
sizeof(struct SaveBlock3), offsetof(struct SaveBlock3, ironmon),
offsetof(struct SaveBlock3, worldSeed), offsetof(struct SaveBlock3, runDifficulty),
offsetof(struct IronmonRunState, retiredCount), offsetof(struct IronmonRunState, ended),
sizeof(struct Pokemon), sizeof(struct PokemonStorage),
MON_DATA_SPECIES, MON_DATA_LEVEL, MON_DATA_HELD_ITEM, MON_DATA_HP,
MON_DATA_HP_IV, MON_DATA_HP_EV, MON_DATA_ATK_IV, MON_DATA_ATK_EV,
MON_DATA_MOVE1, MON_DATA_PP1, MON_DATA_STATUS, MON_DATA_MAX_HP,
MOVE_DESTINY_BOND, MOVE_TACKLE, ITEM_ORAN_BERRY, SPECIES_BULBASAUR, SPECIES_PIKACHU,
FLAG_BADGE01_GET, FLAG_BADGE02_GET, MON_GIVEN_TO_PARTY, MON_CANT_GIVE,
TRAINER_LEADER_BROCK, offsetof(struct MapHeader, regionMapSectionId),
NUM_SPECIES, offsetof(struct IronmonRunState, mode), offsetof(struct IronmonRunState, seed), offsetof(struct SaveBlock1, rivalName),
};
