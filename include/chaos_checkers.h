#ifndef GUARD_CHAOS_CHECKERS_H
#define GUARD_CHAOS_CHECKERS_H
// Independent rules engine: English draughts, short kings, mandatory captures.
// A complete capture chain is one move; crowning ends that turn.
#ifdef CHAOS_CHECKERS_HOST
#include <stdint.h>
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;
#else
#include "global.h"
#endif
#define CHAOS_CHECKERS_MAX_MOVES 96
#define CHAOS_CHECKERS_EMPTY 0
#define CHAOS_CHECKERS_PLAYER 1
#define CHAOS_CHECKERS_CPU 2
#define CHAOS_CHECKERS_KING 4
struct ChaosCheckersBoard { u8 squares[32]; u8 turn; u16 quietTurns; };
struct ChaosCheckersMove { u8 path[13]; u8 length; u8 captures; };
void ChaosCheckersInit(struct ChaosCheckersBoard *board);
u32 ChaosCheckersMoves(const struct ChaosCheckersBoard *board, struct ChaosCheckersMove *moves);
void ChaosCheckersApply(struct ChaosCheckersBoard *board, const struct ChaosCheckersMove *move);
s32 ChaosCheckersChoose(const struct ChaosCheckersBoard *board, u32 difficulty, u32 randomValue, struct ChaosCheckersMove *move);
s32 ChaosCheckersSquare(s32 row, s32 column);
u32 ChaosCheckersColumn(u32 square);
#endif
