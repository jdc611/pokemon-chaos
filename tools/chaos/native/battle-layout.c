#include "global.h"
#include "battle.h"
#include "pokemon.h"
#include "constants/region_map_sections.h"
#include <stddef.h>
const unsigned layout[]={sizeof(struct BattlePokemon),offsetof(struct BattlePokemon,moves),offsetof(struct BattlePokemon,pp),offsetof(struct BattlePokemon,hp),offsetof(struct BattlePokemon,maxHP),offsetof(struct BattlePokemon,status1),offsetof(struct MapHeader,regionMapSectionId),MAPSEC_ROUTE_1,MAPSEC_VIRIDIAN_CITY,MAPSEC_PEWTER_CITY,offsetof(struct FieldTimer,terrainTimer)};
