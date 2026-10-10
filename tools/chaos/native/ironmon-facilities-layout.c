#include "global.h"
#include "battle.h"
#include "trainer_tower.h"
#include "constants/battle_frontier.h"
#include <stddef.h>
const unsigned layout[] = {offsetof(struct BattleScripting, specialTrainerBattleType), SPECIAL_BATTLE_EREADER, BATTLE_TYPE_TRAINER,
sizeof(struct TrainerTowerState),offsetof(struct TrainerTowerState,data)+offsetof(struct EReaderTrainerTowerSet,floors),sizeof(struct TrainerTowerFloor),offsetof(struct TrainerTowerFloor,challengeType),offsetof(struct TrainerTowerFloor,trainers)+offsetof(struct TrainerTowerTrainer,mons),offsetof(struct SaveBlock1,towerChallengeId)};
