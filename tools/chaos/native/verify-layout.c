#include "global.h"
#include "battle.h"
#include "sprite.h"
#include "constants/battle.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include <stddef.h>
const unsigned layout[]={offsetof(struct BattleStruct,weatherDuration),offsetof(struct FieldTimer,terrain),sizeof(struct Sprite),offsetof(struct Sprite,callback),offsetof(struct Sprite,data),offsetof(struct SaveBlock3,leagueTeam),offsetof(struct SaveBlock3,leagueCounters),offsetof(struct SaveBlock3,leagueSeed),offsetof(struct SaveBlock3,runCounters),offsetof(struct SaveBlock3,leagueComplete),B_WEATHER_RAIN,B_TERRAIN_ELECTRIC,B_TERRAIN_GRASSY,TYPE_WATER,TYPE_GRASS,TYPE_GROUND,TYPE_NORMAL,MOVE_THUNDERBOLT,MOVE_SURF,ITEM_PYROARITE,offsetof(struct SaveBlock3,worldSeed),offsetof(struct SaveBlock3,runDifficulty)};
