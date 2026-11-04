#ifndef GUARD_GYM_LEADER_COMPETITIONS_H
#define GUARD_GYM_LEADER_COMPETITIONS_H
#include <stdint.h>
#define GLE_EVENT_HISTORY 128
#define GLE_EVENT_EMPTY 15
/* Entrants and seven bracket winners are packed nibbles. IDs: player 0,
 * rival 1, Kanto colleagues 2..8. Historical slots are never rotated. */
struct GleEventRecord {
    uint32_t entrants, winners;
    uint16_t completedAt;
    uint8_t kindRank, host;
};
struct GleCompetitionState {
    uint32_t nextAt, closesAt;
    struct GleEventRecord current;
    uint32_t wager;
    uint16_t count;
    uint8_t stage, joined, betOn, betOdds, champion, archiveCount;
    uint8_t pendingTeam[112];
    struct GleEventRecord history[GLE_EVENT_HISTORY];
};
struct GleState;
void GleCompetitionInit(struct GleState *);
void GleCompetitionActivity(struct GleState *, uint32_t *money);
int GleCompetitionEligible(const struct GleState *);
int GleCompetitionJoin(struct GleState *);
int GleCompetitionBet(struct GleState *,uint8_t entrant,uint32_t amount,uint32_t *money);
int GleCompetitionOpponent(const struct GleState *);
int GleCompetitionFinalize(struct GleState *,uint32_t *money);
int GleCompetitionResult(struct GleState *,int won,uint32_t *money);
uint8_t GleEventEntrant(const struct GleEventRecord *,uint8_t slot);
uint8_t GleEventWinner(const struct GleEventRecord *,uint8_t match);
#endif
