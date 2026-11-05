#include "global.h"
#include "pokemon.h"
#include "constants/maps.h"
#include <stddef.h>
const unsigned gleGymDesignLayout[]={offsetof(struct GleState,gymDesign),sizeof(struct GleState),GLE_VERSION,offsetof(struct GleGymDesign,layout),offsetof(struct GleGymDesign,puzzle),offsetof(struct GleGymDesign,theme),offsetof(struct GleGymDesign,tier),offsetof(struct GleState,version),offsetof(struct GleState,honor),offsetof(struct GleState,hosting),offsetof(struct GleState,regionalWinner)};
