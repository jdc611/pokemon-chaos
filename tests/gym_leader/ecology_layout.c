#include "global.h"
#include "pokemon.h"
#include "wild_encounter.h"
#include "constants/maps.h"
#include <stddef.h>
const unsigned gleEcologyLayout[]={offsetof(struct SaveBlock3,randomizerEnabled),sizeof(struct SpeciesInfo),offsetof(struct SpeciesInfo,types),offsetof(struct SpeciesInfo,baseHP),MAP_ROUTE1,MAP_VIRIDIAN_FOREST,MAP_ROCK_TUNNEL_1F,MAP_ROUTE19,MAP_SEAFOAM_ISLANDS_B4F,MAP_POWER_PLANT,MAP_POKEMON_MANSION_B1F,MAP_MT_EMBER_SUMMIT,TYPE_WATER,TYPE_GRASS,TYPE_BUG,TYPE_POISON,TYPE_FAIRY,TYPE_ROCK,TYPE_GROUND,TYPE_STEEL,TYPE_DARK,TYPE_GHOST,TYPE_FIGHTING,TYPE_ICE,TYPE_ELECTRIC,TYPE_FIRE,sizeof(struct WildPokemonInfo),offsetof(struct WildPokemonInfo,wildPokemon),offsetof(struct GleState,magic),offsetof(struct SpeciesInfo,baseAttack),offsetof(struct SpeciesInfo,baseDefense),offsetof(struct SpeciesInfo,baseSpAttack),offsetof(struct SpeciesInfo,baseSpDefense),offsetof(struct SpeciesInfo,baseSpeed),offsetof(struct SaveBlock3,dexNavChain)};
