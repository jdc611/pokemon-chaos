#include "global.h"
#include "pokemon.h"
#include "battle.h"
#include <stddef.h>
const unsigned gleDoublesLayout[]={SPECIES_TORTERRA,MOVE_GIGA_DRAIN,BATTLE_TYPE_DOUBLE,offsetof(struct GleState,rivalFriendlyMask),offsetof(struct GleState,rivalFriendlyWins),offsetof(struct GleState,rivalFriendlyLosses),offsetof(struct GleVoucher,tier),offsetof(struct GleVoucher,kind),sizeof(struct GleState),GLE_VERSION,offsetof(struct GleState,personalBonds),offsetof(struct GleState,rivalStarter),offsetof(struct GleState,rivalType),sizeof(gBattlerPartyIndexes[0])};
