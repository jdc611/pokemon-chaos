#include "global.h"
#include "pokemon.h"
#include "constants/maps.h"
#include <stddef.h>
const unsigned gleCompetitionLayout[]={offsetof(struct GleState,competitions),sizeof(struct GleState),GLE_VERSION,
offsetof(struct GleCompetitionState,nextAt),offsetof(struct GleCompetitionState,closesAt),offsetof(struct GleCompetitionState,current),offsetof(struct GleCompetitionState,wager),offsetof(struct GleCompetitionState,count),offsetof(struct GleCompetitionState,stage),offsetof(struct GleCompetitionState,joined),offsetof(struct GleCompetitionState,betOn),offsetof(struct GleCompetitionState,champion),offsetof(struct GleCompetitionState,archiveCount),offsetof(struct GleCompetitionState,pendingTeam),offsetof(struct GleCompetitionState,history),SPECIES_TORTERRA,MOVE_GIGA_DRAIN,MAP_GLE_GYM_INTERIOR,ITEM_TM_EARTHQUAKE,offsetof(struct GleEventRecord,host)};
