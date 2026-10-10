#include "global.h"
#include "chaos_observations.h"
#include "pokemon_storage_system.h"
#include "battle.h"
#include "main.h"
#include <stddef.h>
const unsigned layout[]={
    offsetof(struct PokemonStorage,observations),sizeof(struct PokemonStorage),
    sizeof(struct ChaosObservationJournal),sizeof(struct ChaosObservation),
    offsetof(struct ChaosObservationJournal,facts),0,
    sizeof(struct BattlePokemon),offsetof(struct BattlePokemon,species),
    offsetof(struct BattlePokemon,moves),offsetof(struct BattlePokemon,ability),
    offsetof(struct BattleStruct,illusion),sizeof(struct Illusion),
    offsetof(struct Illusion,state),offsetof(struct Illusion,mon),
    ILLUSION_ON,ABILITY_SOUNDPROOF,ABILITY_ILLUSION,
};
