#include "global.h"
#include "pokemon.h"
#include "constants/maps.h"
#include <stddef.h>
const unsigned gleNewsLayout[]={offsetof(struct GleState,championshipWins),offsetof(struct GleCompetitionState,champion),offsetof(struct GleState,competitions),offsetof(struct GleState,reputation),offsetof(struct GleState,honor),offsetof(struct GleState,rocket.stage),TRAINER_BUG_CATCHER_RICK,offsetof(struct ObjectEventTemplate,script),offsetof(struct ObjectEventTemplate,x),offsetof(struct ObjectEventTemplate,y),MAP_PALLET_TOWN,MAP_VIRIDIAN_CITY,MAP_PEWTER_CITY,MAP_CERULEAN_CITY,MAP_VERMILION_CITY,MAP_LAVENDER_TOWN,MAP_SAFFRON_CITY,MAP_CELADON_CITY,MAP_FUCHSIA_CITY,MAP_CINNABAR_ISLAND,offsetof(struct GleState,rivalTown)};
