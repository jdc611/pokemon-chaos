#include "global.h"
#include "pokemon.h"
#include <stddef.h>
const unsigned gleRocketLayout[]={
 offsetof(struct GleState,rocket),sizeof(struct GleState),GLE_VERSION,
 offsetof(struct GleRocketState,nextAt),offsetof(struct GleRocketState,serial),offsetof(struct GleRocketState,cleared),offsetof(struct GleRocketState,losses),offsetof(struct GleRocketState,stolen),offsetof(struct GleRocketState,recovered),
 offsetof(struct GleRocketState,stage),offsetof(struct GleRocketState,kind),offsetof(struct GleRocketState,remaining),offsetof(struct GleRocketState,staffHelp),offsetof(struct GleRocketState,heist),offsetof(struct GleRocketState,bossDefeated),
 offsetof(struct GleState,colleagueWins),offsetof(struct GleState,colleaguePendingVouchers),SPECIES_CHARIZARD,MOVE_FLAMETHROWER,SPECIES_TORTERRA,MOVE_GIGA_DRAIN,
};
