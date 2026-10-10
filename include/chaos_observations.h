#ifndef GUARD_CHAOS_OBSERVATIONS_H
#define GUARD_CHAOS_OBSERVATIONS_H

// Append-only save extension. Never evict facts or borrow occupied PC boxes.
#define CHAOS_OBSERVATION_CAPACITY 256
#define CHAOS_OBS_SEEN 0
#define CHAOS_OBS_MOVE 0x1000
#define CHAOS_OBS_ABILITY 0x2000
#define CHAOS_OBS_DAMAGE 0x3000
#define CHAOS_OBS_STAT 0x4000
#define CHAOS_OBS_BATTLE 0x5000
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
bool32 ChaosHasObservedSpecies(u32 species);
void ChaosObservationsReset(void);
void ChaosObserveOpponents(void);
void ChaosObserveMove(u32 battler, u32 move);
void ChaosObserveAbility(u32 battler, u32 ability);
void ChaosObserveDamage(u32 attacker, u32 defender, u32 move, u32 damage);
void ChaosObserveStat(u32 battler, u32 stat, u32 stage);
void ChaosObserveBattleEnd(void);
#endif
