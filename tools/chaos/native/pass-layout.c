#include "global.h"
#include "pokemon.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/trainers.h"
#include <stddef.h>
const unsigned layout[]={offsetof(struct SaveBlock1,rivalName),sizeof(struct SaveBlock3),offsetof(struct SaveBlock3,recordsMagic),offsetof(struct SaveBlock3,encounterSpecies),offsetof(struct SaveBlock3,encounterFailed),offsetof(struct SaveBlock3,runCounters),offsetof(struct SaveBlock3,leagueComplete),sizeof(struct Pokemon),MON_DATA_SPECIES,MON_DATA_HP,MON_DATA_MAX_HP,MON_DATA_LEVEL,MON_DATA_EXP,MON_DATA_PERSONALITY,ITEM_POKE_BALL,ITEM_STEELIXITE,ITEM_GYARADOSITE,ITEM_MANECTITE,ITEM_BEEDRILLITE,ITEM_ALAKAZITE,ITEM_GARCHOMPITE,MOVE_TACKLE,MOVE_TWISTER,TRAINER_LEADER_BROCK,TRAINER_CHAOS_JESSIE_MOON,TRAINER_CHAOS_JAMES_MOON};
