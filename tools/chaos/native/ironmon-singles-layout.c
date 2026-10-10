#include "global.h"
#include "battle_setup.h"
#include "constants/trainers.h"
#include <stddef.h>
const unsigned singlesLayout[] = {
 sizeof(TrainerBattleParameter), offsetof(TrainerBattleParameter,params.opponentA),
 offsetof(TrainerBattleParameter,params.opponentB), offsetof(TrainerBattleParameter,params.defeatTextA),
 offsetof(TrainerBattleParameter,params.defeatTextB), TRAINER_LEADER_KOGA,
 TRAINER_CHAOS_JESSIE_SILPH, TRAINER_CHAOS_JAMES_SILPH,
};
