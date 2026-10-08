#include "global.h"
#include "move.h"
#include "item.h"
#define PAIR(move) ITEM_TM_##move,MOVE_##move,
const unsigned v2tms[]={sizeof(struct MoveInfo),offsetof(struct MoveInfo,description),FOREACH_TM(PAIR)};
