#include "global.h"
#include "pokemon.h"
#include "battle.h"
#include <stddef.h>
const unsigned gleHostingLayout[]={offsetof(struct GleState,hosting),sizeof(struct GleState),GLE_VERSION,offsetof(struct GleHostingState,nextAt),offsetof(struct GleHostingState,event),offsetof(struct GleHostingState,stage),offsetof(struct GleHostingState,format),offsetof(struct GleEventRecord,host),offsetof(struct GleEventRecord,kindRank),offsetof(struct GleState,reputation),offsetof(struct GleState,honor),offsetof(struct GleState,rocket.nextAt),SPECIES_TORTERRA,MOVE_GIGA_DRAIN,BATTLE_TYPE_DOUBLE,sizeof(gBattlerPartyIndexes[0]),offsetof(struct GleState,version),GLE_DOJO,GLE_HOTEL,offsetof(struct GleState,championshipWins),GLE_PLOTS};
