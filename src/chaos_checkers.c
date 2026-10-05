#include "chaos_checkers.h"
#ifdef CHAOS_CHECKERS_HOST
#include <stdlib.h>
#define Alloc malloc
#define Free free
#else
#include "malloc.h"
#endif

s32 ChaosCheckersSquare(s32 row, s32 column)
{
    if (row < 0 || row >= 8 || column < 0 || column >= 8 || ((row + column) & 1) == 0)
        return -1;
    return row * 4 + column / 2;
}

u32 ChaosCheckersColumn(u32 square)
{
    return (square % 4) * 2 + ((square / 4 + 1) & 1);
}

void ChaosCheckersInit(struct ChaosCheckersBoard *board)
{
    for (u32 i = 0; i < 32; i++)
        board->squares[i] = i < 12 ? CHAOS_CHECKERS_CPU : i >= 20 ? CHAOS_CHECKERS_PLAYER : 0;
    board->turn = CHAOS_CHECKERS_PLAYER;
    board->quietTurns = 0;
}

static u32 JumpChains(const struct ChaosCheckersBoard *board, u32 square, const struct ChaosCheckersMove *prefix, struct ChaosCheckersMove *moves, u32 count)
{
    u32 piece = board->squares[square];
    s32 row = square / 4, col = ChaosCheckersColumn(square);
    u32 extended = 0;
    for (s32 dr = -1; dr <= 1; dr += 2)
    {
        if (!(piece & CHAOS_CHECKERS_KING) && dr != (board->turn == CHAOS_CHECKERS_PLAYER ? -1 : 1))
            continue;
        for (s32 dc = -1; dc <= 1; dc += 2)
        {
            s32 middle = ChaosCheckersSquare(row + dr, col + dc);
            s32 landing = ChaosCheckersSquare(row + 2 * dr, col + 2 * dc);
            if (middle < 0 || landing < 0 || board->squares[landing] != 0
             || board->squares[middle] == 0 || (board->squares[middle] & 3) == board->turn)
                continue;
            struct ChaosCheckersBoard next = *board;
            struct ChaosCheckersMove chain = *prefix;
            next.squares[square] = 0;
            next.squares[middle] = 0;
            next.squares[landing] = piece;
            chain.path[chain.length++] = landing;
            chain.captures++;
            extended = 1;
            // American/English checkers: a man reaching the king row stops.
            if (!(piece & CHAOS_CHECKERS_KING) && (landing / 4 == 0 || landing / 4 == 7))
            {
                if (count < CHAOS_CHECKERS_MAX_MOVES)
                    moves[count++] = chain;
            }
            else
                count = JumpChains(&next, landing, &chain, moves, count);
        }
    }
    if (!extended && prefix->captures && count < CHAOS_CHECKERS_MAX_MOVES)
        moves[count++] = *prefix;
    return count;
}

u32 ChaosCheckersMoves(const struct ChaosCheckersBoard *board, struct ChaosCheckersMove *moves)
{
    u32 count = 0;
    for (u32 i = 0; i < 32; i++)
    {
        if ((board->squares[i] & 3) != board->turn)
            continue;
        struct ChaosCheckersMove prefix = {{0}, 1, 0};
        prefix.path[0] = i;
        count = JumpChains(board, i, &prefix, moves, count);
    }
    if (count)
        return count;
    for (u32 i = 0; i < 32; i++)
    {
        u32 piece = board->squares[i];
        if ((piece & 3) != board->turn)
            continue;
        s32 row = i / 4, col = ChaosCheckersColumn(i);
        for (s32 dr = -1; dr <= 1; dr += 2)
        {
            if (!(piece & CHAOS_CHECKERS_KING) && dr != (board->turn == CHAOS_CHECKERS_PLAYER ? -1 : 1))
                continue;
            for (s32 dc = -1; dc <= 1; dc += 2)
            {
                s32 target = ChaosCheckersSquare(row + dr, col + dc);
                if (target >= 0 && board->squares[target] == 0 && count < CHAOS_CHECKERS_MAX_MOVES)
                {
                    struct ChaosCheckersMove step = {{0}, 2, 0};
                    step.path[0] = i;
                    step.path[1] = target;
                    moves[count++] = step;
                }
            }
        }
    }
    return count;
}

void ChaosCheckersApply(struct ChaosCheckersBoard *board, const struct ChaosCheckersMove *move)
{
    u32 from = move->path[0], piece = board->squares[from];
    for (u32 step = 1; step < move->length; step++)
    {
        u32 to = move->path[step];
        if (move->captures)
        {
            s32 row = (from / 4 + to / 4) / 2;
            s32 col = (ChaosCheckersColumn(from) + ChaosCheckersColumn(to)) / 2;
            board->squares[ChaosCheckersSquare(row, col)] = 0;
        }
        board->squares[from] = 0;
        board->squares[to] = piece;
        from = to;
    }
    u32 promoted = !(piece & CHAOS_CHECKERS_KING) && (from / 4 == 0 || from / 4 == 7);
    if (promoted)
        board->squares[from] |= CHAOS_CHECKERS_KING;
    board->quietTurns = move->captures || promoted ? 0 : board->quietTurns + 1;
    board->turn = 3 - board->turn;
}

static s32 Evaluate(const struct ChaosCheckersBoard *board)
{
    s32 score = 0;
    for (u32 i = 0; i < 32; i++)
    {
        u32 piece = board->squares[i];
        if (!piece)
            continue;
        s32 row = i / 4, col = ChaosCheckersColumn(i);
        s32 value = piece & CHAOS_CHECKERS_KING ? 180 : 100 + ((piece & 3) == 2 ? row : 7 - row) * 5;
        // Central squares and guarded home ranks improve positioning.
        if (col > 1 && col < 6 && row > 1 && row < 6)
            value += 8;
        if ((piece & 3) == 2 ? row == 0 : row == 7)
            value += 6;
        score += (piece & 3) == CHAOS_CHECKERS_CPU ? value : -value;
    }
    return score;
}

static s32 Search(const struct ChaosCheckersBoard *board, s32 depth, s32 alpha, s32 beta, u32 *budget, struct ChaosCheckersMove *workspace)
{
    if (!depth || !*budget)
        return Evaluate(board);
    (*budget)--;
    struct ChaosCheckersMove *moves = workspace;
    u32 count = ChaosCheckersMoves(board, moves);
    if (!count)
        return board->turn == CHAOS_CHECKERS_CPU ? -20000 - depth : 20000 + depth;
    if (board->quietTurns >= 80)
        return 0;
    s32 best = board->turn == CHAOS_CHECKERS_CPU ? -30000 : 30000;
    for (u32 i = 0; i < count; i++)
    {
        struct ChaosCheckersBoard next = *board;
        ChaosCheckersApply(&next, &moves[i]);
        s32 value = Search(&next, depth - 1, alpha, beta, budget, workspace + CHAOS_CHECKERS_MAX_MOVES);
        if (board->turn == CHAOS_CHECKERS_CPU)
        {
            if (value > best) best = value;
            if (best > alpha) alpha = best;
        }
        else
        {
            if (value < best) best = value;
            if (best < beta) beta = best;
        }
        if (alpha >= beta || !*budget)
            break;
    }
    return best;
}

s32 ChaosCheckersChoose(const struct ChaosCheckersBoard *board, u32 difficulty, u32 randomValue, struct ChaosCheckersMove *move)
{
    struct ChaosCheckersMove *moves = Alloc(sizeof(*moves) * CHAOS_CHECKERS_MAX_MOVES * 6);
    if (!moves) return -1;
    u32 count = ChaosCheckersMoves(board, moves);
    if (!count)
    {
        Free(moves);
        return 0;
    }
    if (difficulty == 0 && (randomValue & 3) == 0)
    {
        *move = moves[(randomValue >> 2) % count];
        Free(moves);
        return 1;
    }
    // Iterative deepening completes each depth fairly across root alternatives.
    // A deeper incomplete iteration never replaces the last complete result.
    u32 maxDepth = difficulty == 0 ? 1 : difficulty == 1 ? 3 : 5;
    u32 budget = difficulty == 2 ? 6000 : 2000;
    u32 chosen = randomValue % count;
    for (u32 depth = 1; depth <= maxDepth; depth++)
    {
        s32 best = board->turn == CHAOS_CHECKERS_CPU ? -30000 : 30000;
        u32 candidate = chosen;
        for (u32 j = 0; j < count; j++)
        {
            u32 i = (j + randomValue % count) % count;
            struct ChaosCheckersBoard next = *board;
            ChaosCheckersApply(&next, &moves[i]);
            s32 value = Search(&next, depth - 1, -30000, 30000, &budget, moves + CHAOS_CHECKERS_MAX_MOVES);
            if ((board->turn == CHAOS_CHECKERS_CPU && value > best)
             || (board->turn != CHAOS_CHECKERS_CPU && value < best))
            {
                best = value;
                candidate = i;
            }
            if (!budget)
                break;
        }
        if (!budget)
            break;
        chosen = candidate;
    }
    *move = moves[chosen];
    Free(moves);
    return 1;
}
