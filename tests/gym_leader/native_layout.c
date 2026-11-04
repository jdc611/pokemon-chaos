#include "global.h"
#include "item.h"
#include "item_menu.h"
#include "wild_encounter.h"
#include "move_relearner.h"
#include "constants/flags.h"
#include "data.h"
#include "random.h"
#include "trainer_util.h"
#include "constants/vars.h"
#include "pokemon.h"
#include "battle.h"
#include "battle_setup.h"
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
 MAP_SEAFOAM_ISLANDS_B3F, MAP_POWER_PLANT, MAP_POKEMON_MANSION_3F, MAP_ROCK_TUNNEL_B1F, MAP_ROUTE17, MAP_VIRIDIAN_FOREST,
 SPECIES_ARTICUNO, SPECIES_ZAPDOS, SPECIES_MOLTRES, SPECIES_ARTICUNO_GALAR, SPECIES_ZAPDOS_GALAR, SPECIES_MOLTRES_GALAR,
 offsetof(struct MapHeader, events), offsetof(struct MapEvents, objectEvents),
 MOVE_ICE_PUNCH, SPECIES_ELECTABUZZ, MOVE_RELEARNER_TUTOR_MOVES, MOVE_RELEARNER_LEVEL_UP_MOVES, MOVE_RELEARNER_TM_MOVES,
 offsetof(struct GleState, tutorLessons), MOVE_THUNDERBOLT, ITEM_TM_THUNDERBOLT,
 offsetof(struct GleState,recruits),sizeof(struct GleRecruit),offsetof(struct GleRecruit,id),offsetof(struct GleRecruit,wins),
 offsetof(struct GleRecruit,losses),offsetof(struct GleRecruit,potential),offsetof(struct GleRecruit,loyalty),offsetof(struct GleRecruit,warnings),
 offsetof(struct GleRecruit,active),offsetof(struct GleRecruit,away),TRAINER_BUG_CATCHER_RICK,SPECIES_SNOVER,MOVE_ICY_WIND,MOVE_POWDER_SNOW,MAP_GLE_RIVAL_GYM,SPECIES_CHARMANDER,MOVE_EMBER,TRAINER_BOSS_GIOVANNI,TRAINER_LEADER_ERIKA,
 offsetof(TrainerBattleParameter,params.opponentA),offsetof(TrainerBattleParameter,params.opponentB),SPECIES_ARCANINE,
 offsetof(struct GleState,rivalStarter),offsetof(struct GleState,rivalFriendlyWins),offsetof(struct GleState,rivalFriendlyLosses),offsetof(struct GleState,rivalFriendlyMask),
 offsetof(struct SpeciesInfo,types),sizeof(struct SpeciesInfo),offsetof(struct GleState,rivalType),
 MAP_INDIGO_PLATEAU_POKEMON_CENTER_1F,MAP_POKEMON_LEAGUE_LORELEIS_ROOM,MAP_POKEMON_LEAGUE_BRUNOS_ROOM,MAP_POKEMON_LEAGUE_AGATHAS_ROOM,MAP_POKEMON_LEAGUE_LANCES_ROOM,MAP_POKEMON_LEAGUE_CHAMPIONS_ROOM,MAP_POKEMON_LEAGUE_HALL_OF_FAME,FLAG_IS_CHAMPION,
 SPECIES_WO_CHIEN,SPECIES_CHIEN_PAO,SPECIES_TING_LU,SPECIES_CHI_YU,SPECIES_TAPU_KOKO,SPECIES_TAPU_LELE,SPECIES_TAPU_BULU,SPECIES_TAPU_FINI,SPECIES_UXIE,SPECIES_MESPRIT,SPECIES_AZELF,SPECIES_OKIDOGI,SPECIES_MUNKIDORI,SPECIES_FEZANDIPITI,SPECIES_ZARUDE,SPECIES_CYCLIZAR,SPECIES_KORAIDON,SPECIES_MIRAIDON,SPECIES_COBALION,SPECIES_TERRAKION,SPECIES_VIRIZION,SPECIES_KELDEO,SPECIES_RAIKOU,SPECIES_ENTEI,SPECIES_SUICUNE,SPECIES_RAGING_BOLT,SPECIES_GOUGING_FIRE,SPECIES_WALKING_WAKE,SPECIES_DIALGA,SPECIES_PALKIA,SPECIES_GIRATINA,
 MAP_VIRIDIAN_FOREST,MAP_SEAFOAM_ISLANDS_B2F,MAP_ROCK_TUNNEL_1F,MAP_POKEMON_MANSION_B1F,
 MAP_ROUTE10,MAP_ROUTE25,MAP_SAFARI_ZONE_WEST,MAP_ROUTE19,MAP_ROUTE22,MAP_SAFARI_ZONE_EAST,MAP_ROUTE12,MAP_ROUTE15,MAP_ROUTE8,MAP_ROUTE18,
 MON_DATA_CHAOS_STARTER_ABILITY,ABILITY_CLEAR_BODY,
 MAP_SEVEN_ISLAND_TANOBY_RUINS,
 MAP_SEVEN_ISLAND_TANOBY_RUINS_MONEAN_CHAMBER,MAP_SEVEN_ISLAND_TANOBY_RUINS_LIPTOO_CHAMBER,MAP_SEVEN_ISLAND_TANOBY_RUINS_WEEPTH_CHAMBER,
 MAP_SEVEN_ISLAND_TANOBY_RUINS_DILFORD_CHAMBER,MAP_SEVEN_ISLAND_TANOBY_RUINS_SCUFIB_CHAMBER,MAP_SEVEN_ISLAND_TANOBY_RUINS_RIXY_CHAMBER,
 SPECIES_REGIROCK,SPECIES_REGICE,SPECIES_REGISTEEL,SPECIES_REGIELEKI,SPECIES_REGIDRAGO,SPECIES_REGIGIGAS,
 offsetof(struct GleState,highestRank),
};
