#include "global.h"
#include "pokemon.h"
#include <stddef.h>
const unsigned gleBondsLayout[]={offsetof(struct GleState,personalBonds),sizeof(struct GleState),GLE_VERSION,offsetof(struct GleState,recruits),sizeof(struct GleRecruit),offsetof(struct GleRecruit,questBond),offsetof(struct GleRecruit,loyalty),offsetof(struct GleRecruit,warnings),offsetof(struct GleRecruit,potential),offsetof(struct GleRecruit,away),TRAINER_BUG_CATCHER_RICK,offsetof(struct GleState,rocket.cleared),GLE_DOJO,GLE_HOUSE,offsetof(struct GleRecruit,active),offsetof(struct GleRecruit,wins),offsetof(struct GleRecruit,losses),offsetof(struct GleState,reputation),offsetof(struct GleState,honor),offsetof(struct GleState,version),GLE_PLOTS};
