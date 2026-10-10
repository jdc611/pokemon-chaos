#include "global.h"
#include "pokemon.h"
#include "battle_setup.h"
#include "constants/trainers.h"
#include <stddef.h>
const unsigned layout[] = {
    sizeof(struct BattlePokemon), offsetof(struct BattlePokemon, hp),
    offsetof(struct BattlePokemon, ability), offsetof(struct BattlePokemon, types),
    offsetof(struct BattlePokemon, status1), MON_DATA_EXP, MOVE_SWIFT,
    TYPE_NORMAL, ABILITY_NONE, TRAINER_CHAOS_JESSIE_MOON,
    TRAINER_CHAOS_JAMES_MOON,
    offsetof(struct BattlePokemon, speed), offsetof(struct BattlePokemon, spAttack),
};
