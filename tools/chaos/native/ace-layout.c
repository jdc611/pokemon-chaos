#include "global.h"
#include "custom_build.h"
#include "data.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "constants/opponents_frlg.h"
#include "constants/moves.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/battle_ai.h"
const unsigned layout[]={offsetof(struct OaksDraftState,customVersion),sizeof(struct OaksDraftState),sizeof(struct SaveBlock1),sizeof(struct SaveBlock2),sizeof(struct SaveBlock3),MON_DATA_IS_SHINY,
TRAINER_LEADER_BROCK,TRAINER_LEADER_MISTY,TRAINER_LEADER_LT_SURGE,TRAINER_LEADER_ERIKA,TRAINER_LEADER_SABRINA,TRAINER_LEADER_KOGA,TRAINER_LEADER_BLAINE,TRAINER_LEADER_GIOVANNI,
TRAINER_ELITE_FOUR_LORELEI,TRAINER_ELITE_FOUR_BRUNO,TRAINER_ELITE_FOUR_AGATHA,TRAINER_ELITE_FOUR_LANCE,TRAINER_CHAOS_COLE_SILPH,TRAINER_CHAOS_VESPER_SILPH,TRAINER_CHAOS_JESSIE_MOON,TRAINER_CHAOS_JAMES_SILPH,
sizeof(struct Trainer),offsetof(struct Trainer,aiFlags),MOVE_REFLECT,MOVE_LIGHT_SCREEN,MOVE_PROTECT,MOVE_TAILWIND,ABILITY_SAND_STREAM,ABILITY_DRIZZLE,ABILITY_ELECTRIC_SURGE,ABILITY_DROUGHT,ABILITY_PSYCHIC_SURGE,ITEM_ORAN_BERRY,ITEM_SITRUS_BERRY,MON_DATA_CHAOS_STARTER_ABILITY,sizeof(struct SpeciesInfo),offsetof(struct SpeciesInfo,baseHP),offsetof(struct SpeciesInfo,baseAttack),offsetof(struct SpeciesInfo,baseDefense),offsetof(struct SpeciesInfo,baseSpeed),offsetof(struct SpeciesInfo,baseSpAttack),offsetof(struct SpeciesInfo,baseSpDefense),SPECIES_BEEDRILL,SPECIES_BEEDRILL_MEGA,ABILITY_ADAPTABILITY};
const unsigned long long masks[]={AI_FLAG_SMART_SWITCHING|AI_FLAG_CHECK_VIABILITY|AI_FLAG_TRY_TO_2HKO|AI_FLAG_HP_AWARE|AI_FLAG_PP_STALL_PREVENTION,AI_FLAG_OMNISCIENT|AI_FLAG_MOVE_OMNISCIENCE|AI_FLAG_ITEM_OMNISCIENCE|AI_FLAG_ABILITY_OMNISCIENCE|AI_FLAG_KNOW_OPPONENT_PARTY|AI_FLAG_PREDICT_MOVE|AI_FLAG_PREDICT_SWITCH|AI_FLAG_PREDICT_INCOMING_MON};
