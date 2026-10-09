#include "global.h"
#include "chaos_moves.h"
#include "move.h"
#include "run_settings.h"
#include "constants/moves.h"

// Callers consume these tables locally. Separate caches keep simultaneous level
// and egg table reads independent; nothing is stored in the save or consumes RNG.
#define LEARNSET_CACHE_SLOTS 4
#define LEARNSET_CAPACITY 64
struct LevelCache
{
    const struct LevelUpMove *source;
    u32 seed;
    enum Species species;
    struct LevelUpMove moves[LEARNSET_CAPACITY + 1];
};
struct EggCache
{
    const u16 *source;
    u32 seed;
    enum Species species;
    u16 moves[LEARNSET_CAPACITY + 1];
};
static EWRAM_DATA struct LevelCache sLevelCache[LEARNSET_CACHE_SLOTS] = {0};
static EWRAM_DATA struct EggCache sEggCache[LEARNSET_CACHE_SLOTS] = {0};
static EWRAM_DATA u8 sNextLevelCache = 0;
static EWRAM_DATA u8 sNextEggCache = 0;

static bool32 RandomMovesetsEnabled(void)
{
    return gSaveBlock3Ptr != NULL && gSaveBlock3Ptr->randomizerEnabled
        && gSaveBlock3Ptr->movesetMode == RUN_MOVESETS_RANDOM;
}

static u32 Gcd(u32 a, u32 b)
{
    while (b != 0)
    {
        u32 remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

static void GetPermutation(enum Species species, u32 *multiplier, u32 *offset)
{
    u32 hash = gSaveBlock3Ptr->worldSeed ^ ((u32)species * 0x9E3779B9u);
    hash = (hash ^ (hash >> 16)) * 0x7FEB352Du;
    hash = (hash ^ (hash >> 15)) * 0x846CA68Bu;
    hash ^= hash >> 16;
    *multiplier = 1 + hash % (MOVES_COUNT - 2);
    while (Gcd(*multiplier, MOVES_COUNT - 1) != 1)
        *multiplier = *multiplier % (MOVES_COUNT - 2) + 1;
    *offset = 1 + (hash >> 16) % (MOVES_COUNT - 2);
}

// Cycle walking restricts a permutation to ordinary usable moves. It preserves
// duplicate entries and cannot introduce new duplicates into an egg move list.
static enum Move RandomizedMove(enum Move move, u32 multiplier, u32 offset)
{
    if (move == MOVE_NONE || move >= MOVES_COUNT || move == MOVE_STRUGGLE || GetMovePP(move) == 0)
        return move;
    do
    {
        move = 1 + (multiplier * (move - 1) + offset) % (MOVES_COUNT - 1);
    } while (move == MOVE_STRUGGLE || GetMovePP(move) == 0);
    return move;
}

const struct LevelUpMove *ChaosGetLevelUpLearnset(enum Species species, const struct LevelUpMove *original)
{
    u32 i, count, multiplier, offset;
    struct LevelCache *cache;
    if (!RandomMovesetsEnabled())
        return original;
    for (i = 0; i < LEARNSET_CACHE_SLOTS; i++)
        if (sLevelCache[i].source == original && sLevelCache[i].species == species && sLevelCache[i].seed == gSaveBlock3Ptr->worldSeed)
            return sLevelCache[i].moves;
    for (count = 0; original[count].move != LEVEL_UP_MOVE_END; count++)
        if (count == LEARNSET_CAPACITY)
            return original; // Checked against all current species tables in QA.
    cache = &sLevelCache[sNextLevelCache++ % LEARNSET_CACHE_SLOTS];
    cache->source = original;
    cache->species = species;
    cache->seed = gSaveBlock3Ptr->worldSeed;
    GetPermutation(species, &multiplier, &offset);
    for (i = 0; i < count; i++)
    {
        cache->moves[i] = original[i];
        cache->moves[i].move = RandomizedMove(original[i].move, multiplier, offset);
    }
    cache->moves[count] = original[count];
    return cache->moves;
}

const u16 *ChaosGetEggLearnset(enum Species species, const u16 *original)
{
    u32 i, count, multiplier, offset;
    struct EggCache *cache;
    if (!RandomMovesetsEnabled())
        return original;
    for (i = 0; i < LEARNSET_CACHE_SLOTS; i++)
        if (sEggCache[i].source == original && sEggCache[i].species == species && sEggCache[i].seed == gSaveBlock3Ptr->worldSeed)
            return sEggCache[i].moves;
    for (count = 0; original[count] != MOVE_UNAVAILABLE; count++)
        if (count == LEARNSET_CAPACITY)
            return original;
    cache = &sEggCache[sNextEggCache++ % LEARNSET_CACHE_SLOTS];
    cache->source = original;
    cache->species = species;
    cache->seed = gSaveBlock3Ptr->worldSeed;
    GetPermutation(species, &multiplier, &offset);
    for (i = 0; i < count; i++)
        cache->moves[i] = RandomizedMove(original[i], multiplier, offset);
    cache->moves[count] = MOVE_UNAVAILABLE;
    return cache->moves;
}
