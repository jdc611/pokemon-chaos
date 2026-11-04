#ifndef GUARD_GYM_LEADER_STATE_H
#define GUARD_GYM_LEADER_STATE_H
/* Pointer-free, fixed-width persistence. This core is also compiled on the host
 * to exercise long progression histories independently of GBA presentation. */
#include <stdint.h>
#include "gym_leader_competitions.h"
#define GLE_MAGIC 0x474C4531u
#define GLE_VERSION 13
#define GLE_PLOTS 24
#define GLE_RECRUITS 10
#define GLE_QUEUE 5
#define GLE_PROJECTS 16
#define GLE_VOUCHERS 24
#define GLE_SPECIES_LIMIT 2048
#define GLE_NONE 255
#define GLE_MAX_MONEY 999999u

enum GleVoucherKind { GLE_STANDARD, GLE_RARE, GLE_ELITE, GLE_SHINY, GLE_SHINY_ELITE, GLE_VOUCHER_KINDS };
enum GleBuilding { GLE_EMPTY, GLE_CENTER, GLE_MART, GLE_HOUSE, GLE_HOTEL, GLE_RESTAURANT, GLE_DOJO, GLE_MUSEUM, GLE_LAB, GLE_HABITAT, GLE_PARK, GLE_MARKET, GLE_BUILDING_COUNT };
enum GleResult { GLE_OK, GLE_INVALID, GLE_NO_ROOM, GLE_NOT_READY, GLE_NOT_ENOUGH_MONEY, GLE_DUPLICATE, GLE_INSPECTION_NEEDED };
struct GlePlot { uint8_t building, tier, size, type; };
struct GleRecruit { uint16_t id, wins, losses; uint8_t potential, loyalty, warnings, active, away, questBond; };
struct GleVoucher { uint16_t species[3]; uint8_t level; uint32_t seed; uint8_t kind, tier, count, viewed; };
struct GleRocketState {
    uint32_t nextAt;
    uint16_t serial, cleared, losses, stolen, recovered;
    uint8_t stage, kind, remaining, staffHelp, heist, bossDefeated;
};
struct GleWind {
    uint32_t ivs, personality, status;
    uint16_t hp, maxHp;
    uint8_t mapGroup, mapNum, level, flags;
};
struct GleWindsState {struct GleWind mon[4];uint8_t selected;uint8_t reserved[3];};
struct GleState {
    uint32_t magic, seed, activity, rankXp, serial;
    uint16_t version, population, capacity, reputation, defenses, losses;
    int16_t honor;
    uint8_t rank, rankFloor, pendingInspection, town, rivalTown, gymType, rivalType, draftRound;
    uint16_t signatureMove, draftSpecies[9];
    uint8_t draftAbilities[9];
    uint8_t queue[GLE_QUEUE], queueCount, pendingImportant, queueClock, lossStreak;
    struct GlePlot plots[GLE_PLOTS];
    struct GleRecruit recruits[GLE_RECRUITS];
    uint8_t projectStage[GLE_PROJECTS], projectUnlocked[GLE_PROJECTS];
    uint8_t uniqueObtained[32];
    uint8_t normalClaims[GLE_SPECIES_LIMIT / 8], shinyClaims[GLE_SPECIES_LIMIT / 8];
    struct GleVoucher vouchers[GLE_VOUCHERS];
    uint8_t voucherCount, regionalWinner, champion, highestRank;
    uint16_t championshipWins, rivalChampionshipWins;
    uint8_t townName[13];
    uint8_t openingStage;
    uint8_t registeredCount;
    uint32_t registeredPersonality[6], registeredTrainerId[6];
    uint8_t interiorPlot, interiorTown, landmarkTier, milestoneRewardMask;
    uint16_t safariSpecies[6];
    uint32_t tutorLessons;
    uint16_t rivalStarter, rivalFriendlyWins, rivalFriendlyLosses;
    uint8_t rivalFriendlyMask;
    _Alignas(4) uint8_t colleagueWins[11];
    uint8_t colleaguePendingVouchers[4];
    _Alignas(4) struct GleRocketState rocket;
    struct GleCompetitionState competitions;
    struct GleWindsState winds;
    uint8_t personalBonds[32];
};
extern const uint8_t gGleRankCaps[8], gGleRankTeamSizes[8];
void GleInit(struct GleState *, uint32_t seed);
int GleValid(const struct GleState *);
uint32_t GleMix(uint32_t);
int GleChooseIdentity(struct GleState *, uint8_t town, uint8_t type, uint8_t rivalType);
uint8_t GleCap(const struct GleState *);
uint8_t GleTeamSize(const struct GleState *);
uint32_t GleXpNeeded(const struct GleState *);
int GleAddRankXp(struct GleState *, uint32_t xp);
int GleInspectionResult(struct GleState *, int won);
void GleDefenseResult(struct GleState *, int won);
void GleActivity(struct GleState *, uint32_t *money);
int GleEnqueue(struct GleState *, uint8_t priority);
int GleTakeChallenger(struct GleState *);
uint32_t GleBuildCost(uint8_t building, uint8_t tier);
int GleConstruct(struct GleState *, uint8_t plot, uint8_t building, uint32_t *money);
int GleRecruitTrainer(struct GleState *, uint16_t id, uint8_t potential, uint8_t loyalty);
int GleAssignTrainer(struct GleState *, uint8_t recruit, int active);
void GleRocketActivity(struct GleState *);
int GleRocketBattleResult(struct GleState *, int won);
int GleResolveRocket(struct GleState *, uint8_t choice, uint32_t *money);
int GleResidentTaskReady(const struct GleState *, uint8_t request);
int GleResolveResidentTask(struct GleState *, uint8_t request, uint8_t choice, uint32_t *money);
int GleResearchDiscover(struct GleState *, uint8_t project, uint8_t stage);
int GleUniqueCapture(struct GleState *, uint8_t allocation);
int GleAwardVoucher(struct GleState *, uint8_t kind, uint8_t tier);
int GleViewVoucher(struct GleState *, uint8_t slot, const uint16_t *pool, uint16_t poolSize);
int GleClaimVoucher(struct GleState *, uint8_t slot, uint8_t choice, uint16_t *species);
int GleSpeciesClaimed(const struct GleState *, uint16_t species, uint8_t kind);
#endif
