#include "global.h"
#include "main.h"
#include "battle.h"
#include "battle_controllers.h"
#include "wild_encounter.h"
#include "dexnav.h"
#include <stddef.h>
struct DexNavGUI
{
    MainCallback savedCallback;
    u8 state;
    u8 cursorSpriteId;
    u8 waterCount;
    u8 waterPage;
    u8 waterIconIds[WATER_DISPLAY_CAPACITY];
    enum Species landSpecies[NUM_LAND_MONS_ENCOUNTER_SLOTS];
    enum Species waterSpecies[WATER_ENCOUNTER_CAPACITY];
    u8 waterMethods[WATER_ENCOUNTER_CAPACITY];
    u8 cursorRow;
    u8 cursorCol;
    u8 environment;
    u8 potential;
    u8 typeIconSpriteIds[2];
    u8 starSpriteIds[3];
};


const unsigned layout[]={
offsetof(struct SaveBlock1,daycare.stepCounter),
offsetof(struct SaveBlock3,randomizerEnabled),
offsetof(struct SaveBlock3,worldSeed),
offsetof(struct DexNavGUI,landSpecies),
offsetof(struct DexNavGUI,waterSpecies),
offsetof(struct DexNavGUI,waterMethods),
offsetof(struct DexNavGUI,waterCount),
NUM_LAND_MONS_ENCOUNTER_SLOTS,
WATER_ENCOUNTER_CAPACITY,
sizeof(enum Species),
MON_DATA_IS_EGG,
MON_DATA_FRIENDSHIP,
MON_DATA_SPECIES,
MON_DATA_NICKNAME,
offsetof(struct SaveBlock1,money),
MON_DATA_HP,
MON_DATA_CHAOS_STARTER_ABILITY,
offsetof(struct BattleResources,bufferA),
offsetof(struct ChooseMoveStruct,moves),
MOVE_EARTHQUAKE,
MOVE_EMBER,
MOVE_SHADOW_BALL,
MOVE_TACKLE,
MOVE_MACH_PUNCH,
ABILITY_EARTH_EATER,
ABILITY_WELL_BAKED_BODY,
ABILITY_PURIFYING_SALT,
ABILITY_TERA_SHELL,
ABILITY_WONDER_GUARD,
ABILITY_MINDS_EYE,
ABILITY_MOLD_BREAKER,
TYPE_NORMAL,
TYPE_GHOST,
TYPE_MYSTERY,
ITEM_ABILITY_SHIELD
};
