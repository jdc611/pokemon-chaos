#include "global.h"
#include "item.h"
#include "item_menu.h"
#include "wild_encounter.h"
#include "constants/flags.h"
#include "data.h"
#include "random.h"
#include "trainer_util.h"
#include "constants/vars.h"
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
 offsetof(struct GleState, landmarkTier), offsetof(struct GleState, voucherCount),
 offsetof(struct GleState, vouchers), sizeof(struct GleVoucher), offsetof(struct GleState, milestoneRewardMask),
 MON_DATA_POKEBALL, MON_DATA_IS_SHINY, offsetof(struct GleState, highestRank),
 MAP_SILPH_CO_1F, MAP_CELADON_CITY_GAME_CORNER, MAP_FUCHSIA_CITY_SAFARI_ZONE_ENTRANCE,
 TOTAL_BOXES_COUNT, IN_BOX_COUNT, offsetof(struct GleState, version), offsetof(struct GleVoucher, level),
 FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
 FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_BADGE07_GET, FLAG_BADGE08_GET,
 FLAG_SYS_GAME_CLEAR, VAR_CHAOS_POKERIDER_UNLOCKED, VAR_MAP_SCENE_ROUTE23,
 VAR_MAP_SCENE_VIRIDIAN_CITY_OLD_MAN, MAP_ROUTE22_NORTH_ENTRANCE, MAP_ROUTE23,
 MAP_ROUTE5_SOUTH_ENTRANCE, VAR_MAP_SCENE_ROUTE5_ROUTE6_ROUTE7_ROUTE8_GATES,
 ITEM_HM_SURF, ITEM_BICYCLE, TRAINER_LEADER_BLAINE, sizeof(struct TrainerGenerator),
 GLE_VERSION, sizeof(struct Trainer), offsetof(struct Trainer, party), DIFFICULTY_NORMAL, TRAINERS_COUNT,
 offsetof(struct GleState, safariSpecies), MAP_SAFARI_ZONE_CENTER,
 sizeof(struct WildPokemonHeader), offsetof(struct WildPokemonHeader, encounterTypes), sizeof(struct WildEncounterTypes),
 MAP_SEAFOAM_ISLANDS_B4F, MAP_POWER_PLANT, MAP_MT_EMBER_SUMMIT, MAP_CERULEAN_CAVE_B1F,
 offsetof(struct GleState, uniqueObtained), ITEM_MASTER_BALL, sizeof(struct ObjectEventTemplate), offsetof(struct ObjectEventTemplate, graphicsId),
 sizeof(struct BagPocket), offsetof(struct BagPosition, pocket), POCKET_POKE_BALLS, offsetof(struct BagPosition, cursorPosition), offsetof(struct BagPosition, scrollPosition), sizeof(struct ItemSlot), BAG_POKEBALLS_COUNT,
 MAP_SEAFOAM_ISLANDS_1F, MAP_POWER_PLANT, MAP_MT_EMBER_EXTERIOR, MAP_CERULEAN_CAVE_1F, MAP_ROUTE20,
 SPECIES_SILVALLY_NORMAL, SPECIES_SILVALLY_FIRE, SPECIES_ARCEUS, SPECIES_ARCEUS_FIRE, ITEM_FIRE_MEMORY, ITEM_FLAME_PLATE,
 offsetof(struct GleState, honor), offsetof(struct GleState, projectStage), offsetof(struct GleState, projectUnlocked),
 ITEM_POTION, ITEM_EXP_SHARE, ITEM_ABILITY_PATCH, SPECIES_TYPE_NULL, SPECIES_GLE_AWAKENED_SLAKING, SPECIES_SLAKING,
};
