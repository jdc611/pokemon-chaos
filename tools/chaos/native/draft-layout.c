#include "global.h"
#include "pokemon.h"
#include "run_settings.h"
#include "constants/trainers.h"
#include "constants/items.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "constants/moves.h"
#include "constants/maps.h"
const unsigned layout[]={
 sizeof(struct SaveBlock3),offsetof(struct SaveBlock3,draft),offsetof(struct SaveBlock3,worldSeed),offsetof(struct SaveBlock3,starterMode),
 offsetof(struct SaveBlock3,bstMode),offsetof(struct SaveBlock3,abilityMode),offsetof(struct SaveBlock3,movesetMode),offsetof(struct SaveBlock3,minimalGrindingMode),
 sizeof(struct Pokemon),MON_DATA_SPECIES,MON_DATA_LEVEL,MON_DATA_HP,MON_DATA_NICKNAME,MON_DATA_DRAFT_MEMBER,MON_DATA_DRAFT_REROLLED,MON_DATA_CHAOS_STARTER_ABILITY,
 MON_DATA_PERSONALITY,MON_DATA_HP_IV,MON_DATA_MOVE1,MON_DATA_MAX_HP,MON_DATA_HELD_ITEM,
 TRAINER_RIVAL_OAKS_LAB_SQUIRTLE,TRAINER_RIVAL_ROUTE22_EARLY_SQUIRTLE,TRAINER_CHAMPION_FIRST_SQUIRTLE,
 ITEM_TM01,ITEM_TM100,ITEM_HM_CUT,FLAG_BADGE02_GET,VAR_CHAOS_NUZLOCKE,
 offsetof(struct SaveBlock1,rivalName),RUN_STARTER_DRAFT,MAP_GROUP(MAP_OAKS_DRAFT_ROOM),MAP_NUM(MAP_OAKS_DRAFT_ROOM),
 MAP_GROUP(MAP_PALLET_TOWN_PROFESSOR_OAKS_LAB),MAP_NUM(MAP_PALLET_TOWN_PROFESSOR_OAKS_LAB),
 offsetof(struct OaksDraftState,round),offsetof(struct OaksDraftState,picks),offsetof(struct OaksDraftState,rival),offsetof(struct OaksDraftState,gymTms),
 VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB,MON_DATA_HP_EV,FLAG_SYS_POKEMON_GET,
};
