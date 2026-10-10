#ifndef GUARD_CHAOS_OBSERVATIONS_H
#define GUARD_CHAOS_OBSERVATIONS_H

// Append-only save extension. Never evict facts or borrow occupied PC boxes.
#define CHAOS_OBSERVATION_CAPACITY 256
#define CHAOS_OBS_SEEN 0
#define CHAOS_OBS_MOVE 0x1000
#define CHAOS_OBS_ABILITY 0x2000
struct __attribute__((packed, aligned(2))) ChaosObservation
{
    u16 species;
    u16 fact;
    u16 count;
};
struct ChaosObservationJournal
{
    u32 magic;
    u32 seed;
    u16 count;
    u8 version;
    bool8 full;
    struct ChaosObservation facts[CHAOS_OBSERVATION_CAPACITY];
};

const struct ChaosObservationJournal *ChaosObservationsRead(void);
void ChaosObserveOpponents(void);
void ChaosObserveMove(u32 battler, u32 move);
void ChaosObserveAbility(u32 battler, u32 ability);
#endif
