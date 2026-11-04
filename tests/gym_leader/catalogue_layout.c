#include "global.h"
#include "pokemon.h"
#include <stddef.h>
const unsigned gleCatalogueLayout[]={offsetof(struct GleState,normalClaims),offsetof(struct GleState,shinyClaims),offsetof(struct GleVoucher,viewed),offsetof(struct GleVoucher,kind),offsetof(struct GleVoucher,count),SPECIES_SQUIRTLE,offsetof(struct GleState,honor),offsetof(struct GleVoucher,species)};
