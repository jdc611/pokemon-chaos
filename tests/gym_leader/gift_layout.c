#include "global.h"
#include "pokemon.h"
#include "constants/maps.h"
#include "constants/flags.h"
#include <stddef.h>
const unsigned gleGiftLayout[]={MAP_CELADON_CITY_CONDOMINIUMS_ROOF_ROOM,SPECIES_EEVEE,SPECIES_PORYGON,SPECIES_GLASTRIER,sizeof(struct PokemonTemplate),offsetof(struct PokemonTemplate,species),offsetof(struct PokemonTemplate,level),sizeof(((struct PokemonTemplate *)0)->species),sizeof(((struct PokemonTemplate *)0)->level),B_SIDE_PLAYER,B_SIDE_OPPONENT,FLAG_HIDE_EEVEE_BALL,offsetof(struct PokemonTemplate,gender),offsetof(struct PokemonTemplate,nature),MON_GENDER_RANDOM,NATURE_RANDOM,offsetof(struct PokemonTemplate,origin),sizeof(((struct PokemonTemplate *)0)->origin),GIFTMON_ORIGIN};
