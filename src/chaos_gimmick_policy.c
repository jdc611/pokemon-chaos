#include "global.h"
#include "custom_build.h"
#include "item.h"
#include "move.h"
#include "string_util.h"
#include "text.h"
#include "data.h"
#include "constants/items.h"
#include "constants/hold_effects.h"
#include "constants/moves.h"
#include "constants/abilities.h"
bool32 ChaosRemovedItem(enum Item item)
{
 if(item<=ITEM_NONE || item>=ITEMS_COUNT)return FALSE;
 return GetItemHoldEffect(item)==HOLD_EFFECT_Z_CRYSTAL
 || item==ITEM_Z_POWER_RING || item==ITEM_DYNAMAX_BAND || item==ITEM_DYNAMAX_CANDY
 || item==ITEM_TERA_ORB || item==ITEM_STELLAR_TERA_SHARD
 || (item>=ITEM_BUG_TERA_SHARD && item<=ITEM_WATER_TERA_SHARD)
 || item==ITEM_MAX_MUSHROOMS || item==ITEM_MAX_HONEY || item==ITEM_WISHING_PIECE;
}
bool32 ChaosUsableMove(enum Move move)
{
 return move>MOVE_NONE && move<MOVES_COUNT && GetMovePP(move)>0 && move!=MOVE_STRUGGLE
;
}
bool32 ChaosUsableAbility(enum Ability ability)
{
 return ability>ABILITY_NONE && ability<ABILITIES_COUNT && gAbilitiesInfo[ability].name[0]!=EOS && gAbilitiesInfo[ability].description != NULL

 && ability!=ABILITY_TERA_SHIFT && ability!=ABILITY_TERA_SHELL && ability!=ABILITY_TERAFORM_ZERO;
}
