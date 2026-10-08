#include "global.h"
#include "pokemon.h"
#include "battle.h"
#include "item.h"
#include <stddef.h>
const unsigned layout[]={sizeof(struct Evolution),offsetof(struct Evolution,params),sizeof(struct EvolutionParam),NUM_SPECIES,EVOLUTIONS_END,IF_MIN_FRIENDSHIP,IF_TRADE_PARTNER_SPECIES,IF_REGION,EVO_TRADE,EVO_SCRIPT_TRIGGER,EVO_ITEM,MON_DATA_ABILITY_NUM,MON_DATA_HP_IV,MON_DATA_HP_EV,MON_DATA_HELD_ITEM,MON_DATA_LEVEL,MON_DATA_IS_EGG,MON_DATA_EXP,ITEM_LINKING_CORD,ITEM_METAL_COAT,ITEM_RARE_CANDY,ITEM_ORAN_BERRY,ITEM_LEFTOVERS,ITEM_HP_UP,ITEM_PROTEIN,offsetof(struct BattleStruct,partyState),offsetof(struct BattleStruct,itemLost),sizeof(struct PartyState),sizeof(((struct BattleStruct *)0)->itemLost[0][0]),MON_DATA_CHAOS_STARTER_ABILITY,offsetof(struct SaveBlock3,minimalGrindingMode),offsetof(struct SaveBlock1,money),ITEMS_COUNT,offsetof(struct SpeciesInfo,evolutions),sizeof(struct SpeciesInfo),CONDITIONS_END,offsetof(struct SaveBlock3,runDifficulty)};
