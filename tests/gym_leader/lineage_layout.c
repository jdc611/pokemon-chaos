#include "global.h"
#include "pokemon.h"
#include <stddef.h>
const unsigned gleLineageLayout[]={SPECIES_COSMOG,SPECIES_COSMOEM,SPECIES_POIPOLE,SPECIES_MELTAN,SPECIES_KUBFU,SPECIES_SHAYMIN,offsetof(struct Evolution,targetSpecies),sizeof(struct Evolution),offsetof(struct Evolution,method),EVOLUTIONS_END,EVO_MODE_NORMAL,CHECK_EVO,offsetof(struct GleState,highestRank),GLE_PLOTS};
