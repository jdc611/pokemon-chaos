#include "global.h"
#include "gym_leader_state.h"
#include <stddef.h>
const unsigned stationLayout[]={offsetof(struct GleState,recruits),sizeof(struct GleRecruit),offsetof(struct GleRecruit,active),offsetof(struct GleRecruit,away),offsetof(struct ObjectEventTemplate,x),offsetof(struct ObjectEventTemplate,y)};
