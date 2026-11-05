#include "global.h"
#include "gym_leader_state.h"
#include "gym_leader_recruit.h"
#include <stddef.h>
const unsigned stationLayout[]={offsetof(struct GleState,recruits),sizeof(struct GleRecruit),offsetof(struct GleRecruit,active),offsetof(struct GleRecruit,away),offsetof(struct ObjectEventTemplate,x),offsetof(struct ObjectEventTemplate,y),GLE_STAFF_LOCAL_ID,offsetof(struct SaveBlock1,objectEventTemplates),offsetof(struct ObjectEvent,localId),sizeof(struct ObjectEvent),offsetof(struct SaveBlock1,objectEvents)};
