#include "gym_leader_state.h"
#include <string.h>
#include <limits.h>

const uint8_t gGleRankCaps[8] = {15, 25, 35, 45, 55, 65, 80, 100};
const uint8_t gGleRankTeamSizes[8] = {3, 3, 4, 4, 5, 5, 6, 6};
static const uint16_t sRankXp[8] = {100, 180, 280, 400, 560, 760, 1000, 0};
static const uint8_t sInspectionBefore[9] = {0, 0, 0, 1, 0, 2, 0, 3, 4};
static const uint16_t sBuildingCost[GLE_BUILDING_COUNT] = {0, 1500, 1200, 600, 1800, 1600, 2500, 3000, 3200, 2800, 500, 2200};
static const uint8_t sBuildingSize[GLE_BUILDING_COUNT] = {0, 1, 1, 1, 1, 2, 2, 3, 2, 2, 1, 2};
static uint16_t Add16(uint16_t a, uint32_t b) { return b > UINT16_MAX - (uint32_t)a ? UINT16_MAX : a + b; }
static uint32_t Add32(uint32_t a, uint32_t b) { return b > UINT32_MAX - a ? UINT32_MAX : a + b; }
static int Shiny(uint8_t k) { return k == GLE_SHINY || k == GLE_SHINY_ELITE; }
static int GetBit(const uint8_t *p, uint16_t n) { return (p[n / 8] >> (n % 8)) & 1; }
static void SetBit(uint8_t *p, uint16_t n) { p[n / 8] |= 1u << (n % 8); }
uint32_t GleMix(uint32_t x) { x = (x ^ (x >> 16)) * 0x7feb352du; x = (x ^ (x >> 15)) * 0x846ca68bu; return x ^ (x >> 16); }
int GleValid(const struct GleState *s) { return s && s->magic == GLE_MAGIC && s->version == GLE_VERSION && s->rank >= 1 && s->rank <= 8 && s->town < 3 && s->queueCount <= GLE_QUEUE && s->voucherCount <= GLE_VOUCHERS && s->registeredCount <= 6; }
void GleInit(struct GleState *s, uint32_t seed) {
    unsigned i;
    memset(s, 0, sizeof(*s));
    s->magic = GLE_MAGIC; s->version = GLE_VERSION; s->seed = seed;
    s->rank = s->rankFloor = s->highestRank = 1; s->gymType = GLE_NONE; s->interiorPlot = s->interiorTown = GLE_NONE; s->reputation = 100;
    for (i = 0; i < GLE_PLOTS; i++) s->plots[i].size = i < 12 ? 1 : i < 20 ? 2 : 3;
    for (i = 0; i < 3; i++) GleEnqueue(s, 0);
}
int GleChooseIdentity(struct GleState *s, uint8_t town, uint8_t type, uint8_t rivalType) {
    if (!GleValid(s) || town >= 3 || type >= 18 || rivalType >= 18 || s->gymType != GLE_NONE) return GLE_INVALID;
    s->town = town; s->rivalTown = (town + 1 + GleMix(s->seed) % 2) % 3;
    s->gymType = type; s->rivalType = rivalType; return GLE_OK;
}
uint8_t GleCap(const struct GleState *s) { return GleValid(s) ? gGleRankCaps[s->rank - 1] : 0; }
uint8_t GleTeamSize(const struct GleState *s) { return GleValid(s) ? gGleRankTeamSizes[s->rank - 1] : 0; }
uint32_t GleXpNeeded(const struct GleState *s) { return GleValid(s) ? sRankXp[s->rank - 1] : 0; }
static void Promote(struct GleState *s) {
    unsigned i;
    s->rankXp -= sRankXp[s->rank - 1]; s->rank++;
    if (s->rank > s->highestRank) s->highestRank = s->rank;
    if (!sInspectionBefore[s->rank]) s->rankFloor = s->rank;
    s->pendingInspection = 0; s->lossStreak = 0;
    for (i = 0; i < GLE_PROJECTS; i++) if (i % 7 + 2 <= s->rank) s->projectUnlocked[i] = 1;
}
int GleAddRankXp(struct GleState *s, uint32_t xp) {
    if (!GleValid(s)) return GLE_INVALID;
    if (s->rank == 8) return GLE_OK;
    s->rankXp = Add32(s->rankXp, xp);
    while (s->rank < 8 && s->rankXp >= sRankXp[s->rank - 1]) {
        if (sInspectionBefore[s->rank + 1]) { s->pendingInspection = sInspectionBefore[s->rank + 1]; return GLE_INSPECTION_NEEDED; }
        Promote(s);
    }
    return GLE_OK;
}
int GleInspectionResult(struct GleState *s, int won) {
    if (!GleValid(s) || !s->pendingInspection || s->rank == 8 || s->rankXp < sRankXp[s->rank - 1]) return GLE_NOT_READY;
    if (!won) return GLE_OK; /* inspection failure is not an official defense */
    Promote(s); return GleAddRankXp(s, 0);
}
int GleEnqueue(struct GleState *s, uint8_t priority) {
    if (!GleValid(s) || priority > 2) return GLE_INVALID;
    if (s->queueCount == GLE_QUEUE) { if (priority > s->pendingImportant) s->pendingImportant = priority; return GLE_NO_ROOM; }
    s->queue[s->queueCount++] = priority; return GLE_OK;
}
int GleTakeChallenger(struct GleState *s) {
    uint8_t p;
    if (!GleValid(s) || !s->queueCount) return -1;
    p = s->queue[0]; memmove(s->queue, s->queue + 1, --s->queueCount);
    if (s->pendingImportant) { GleEnqueue(s, s->pendingImportant); s->pendingImportant = 0; }
    return p;
}
void GleDefenseResult(struct GleState *s, int won) {
    if (!GleValid(s)) return;
    if (won) { s->defenses = Add16(s->defenses, 1); s->lossStreak = 0; s->reputation = Add16(s->reputation, 3); GleAddRankXp(s, 25); }
    else {
        s->losses = Add16(s->losses, 1); if (s->lossStreak < 255) s->lossStreak++;
        if (s->reputation) s->reputation--;
        if (s->lossStreak >= 5 && s->rank > s->rankFloor) {
            s->rank--; s->rankXp = 0; s->pendingInspection = 0; s->lossStreak = 0;
        }
    }
}
static void Recount(struct GleState *s) {
    unsigned i; s->capacity = 0;
    for (i = 0; i < GLE_PLOTS; i++) {
        const struct GlePlot *p = &s->plots[i];
        if (p->building == GLE_HOUSE) s->capacity += 12 * p->tier;
        if (p->building == GLE_HOTEL) s->capacity += 20 * p->tier;
    }
}
void GleActivity(struct GleState *s, uint32_t *money) {
    unsigned i, revenue = 0;
    if (!GleValid(s) || !money) return;
    s->activity++;
    if (++s->queueClock >= 6) { GleEnqueue(s, GleMix(s->seed ^ s->activity) % 25 == 0 ? 2 : 0); s->queueClock = 0; }
    if (s->activity % 12) return;
    Recount(s);
    if (s->reputation >= 100 && s->population < s->capacity) s->population++;
    else if (s->reputation < 60 && s->population) s->population--;
    for (i = 0; i < GLE_PLOTS; i++) if (s->plots[i].building >= GLE_HOTEL) revenue += 4 * s->plots[i].tier;
    revenue += s->population / 2;
    *money = *money >= GLE_MAX_MONEY || revenue > GLE_MAX_MONEY - *money ? GLE_MAX_MONEY : *money + revenue;
    for (i = 0; i < GLE_RECRUITS; i++) {
        struct GleRecruit *r = &s->recruits[i];
        if (!r->id || r->away || !r->active) continue;
        if (s->reputation < 60 && s->lossStreak >= 3 && !r->questBond) {
            if (r->warnings < 3) r->warnings++;
            else if (r->loyalty < 60) { r->away = 1; r->active = 0; }
        } else r->warnings = 0;
    }
}
uint32_t GleBuildCost(uint8_t b, uint8_t tier) { return b && b < GLE_BUILDING_COUNT && tier >= 1 && tier <= 3 ? sBuildingCost[b] * (uint32_t)tier * tier : UINT32_MAX; }
int GleConstruct(struct GleState *s, uint8_t slot, uint8_t b, uint32_t *money) {
    unsigned i; uint8_t tier; uint32_t cost; struct GlePlot *p;
    if (!GleValid(s) || !money || slot >= GLE_PLOTS || !b || b >= GLE_BUILDING_COUNT) return GLE_INVALID;
    p = &s->plots[slot];
    if (p->building && p->building != b) return GLE_INVALID;
    tier = p->tier + 1;
    if (tier > 3 || p->size < sBuildingSize[b]) return GLE_NO_ROOM;
    if (tier > s->rank) return GLE_NOT_READY;
    if (b == GLE_CENTER || b == GLE_MART || b == GLE_LAB) {
        if (p->tier) return GLE_NO_ROOM;
        for (i = 0; i < GLE_PLOTS; i++) if (s->plots[i].building == b) return GLE_DUPLICATE;
    }
    if (tier > 1 && s->population < (tier - 1) * 8) return GLE_NOT_READY;
    cost = GleBuildCost(b, tier);
    if (*money < cost) return GLE_NOT_ENOUGH_MONEY;
    *money -= cost; p->building = b; p->tier = tier; p->type = s->gymType;
    s->reputation = Add16(s->reputation, b == GLE_MARKET ? 0 : 2);
    Recount(s); return GLE_OK;
}
int GleRecruitTrainer(struct GleState *s, uint16_t id, uint8_t potential, uint8_t loyalty) {
    unsigned i, freeSlot = GLE_RECRUITS;
    if (!GleValid(s) || !id || potential < 1 || potential > 5 || loyalty > 100) return GLE_INVALID;
    for (i = 0; i < GLE_RECRUITS; i++) { if (s->recruits[i].id == id) return GLE_DUPLICATE; if (!s->recruits[i].id && freeSlot == GLE_RECRUITS) freeSlot = i; }
    if (freeSlot == GLE_RECRUITS) return GLE_NO_ROOM;
    s->recruits[freeSlot].id = id; s->recruits[freeSlot].potential = potential; s->recruits[freeSlot].loyalty = loyalty; return GLE_OK;
}
int GleAssignTrainer(struct GleState *s, uint8_t slot, int active) {
    unsigned i, count = 0;
    if (!GleValid(s) || slot >= GLE_RECRUITS || !s->recruits[slot].id || s->recruits[slot].away) return GLE_INVALID;
    for (i = 0; i < GLE_RECRUITS; i++) count += !!s->recruits[i].active;
    if (active && !s->recruits[slot].active && count >= (unsigned)((s->rank + 1) / 2 + (s->rank == 8))) return GLE_NO_ROOM;
    s->recruits[slot].active = !!active; return GLE_OK;
}
int GleResearchDiscover(struct GleState *s, uint8_t p, uint8_t stage) {
    if (!GleValid(s) || p >= GLE_PROJECTS || !stage) return GLE_INVALID;
    if (stage > s->projectStage[p]) s->projectStage[p] = stage; /* early clues persist while locked */
    return GLE_OK;
}
int GleUniqueCapture(struct GleState *s, uint8_t allocation) {
    if (!GleValid(s)) return GLE_INVALID;
    if (GetBit(s->uniqueObtained, allocation)) return GLE_DUPLICATE;
    SetBit(s->uniqueObtained, allocation); return GLE_OK;
}
int GleAwardVoucher(struct GleState *s, uint8_t kind, uint8_t tier) {
    struct GleVoucher *v;
    if (!GleValid(s) || kind >= GLE_VOUCHER_KINDS || tier < 1 || tier > 8) return GLE_INVALID;
    if (s->voucherCount >= GLE_VOUCHERS) return GLE_NO_ROOM;
    v = &s->vouchers[s->voucherCount++]; memset(v, 0, sizeof(*v));
    v->kind = kind; v->tier = tier; v->seed = GleMix(s->seed ^ ++s->serial); return GLE_OK;
}
int GleSpeciesClaimed(const struct GleState *s, uint16_t species, uint8_t kind) {
    if (!GleValid(s) || !species || species >= GLE_SPECIES_LIMIT || kind >= GLE_VOUCHER_KINDS) return 1;
    return GetBit(Shiny(kind) ? s->shinyClaims : s->normalClaims, species);
}
int GleViewVoucher(struct GleState *s, uint8_t slot, const uint16_t *pool, uint16_t size) {
    uint32_t i; struct GleVoucher *v; unsigned count = 0;
    if (!GleValid(s) || slot >= s->voucherCount || !pool || !size) return GLE_INVALID;
    v = &s->vouchers[slot]; if (v->viewed) return GLE_OK;
    /* Walk each input once; duplicate pool entries and depleted pools cannot hang. */
    for (i = 0; i < size && count < 3; i++) {
        uint16_t species = pool[(GleMix(v->seed) % size + i) % size]; unsigned j;
        unsigned other, held = 0;
        if (GleSpeciesClaimed(s, species, v->kind)) continue;
        // Two opened vouchers must never promise the same nonrepeatable claim.
        for (other = 0; other < s->voucherCount; other++) {
            const struct GleVoucher *pending = &s->vouchers[other];
            if (other == slot || !pending->viewed || Shiny(pending->kind) != Shiny(v->kind)) continue;
            for (j = 0; j < pending->count; j++) if (pending->species[j] == species) held = 1;
        }
        if (held) continue;
        for (j = 0; j < count && v->species[j] != species; j++);
        if (j == count) v->species[count++] = species;
    }
    if (!count) return GLE_NOT_READY;
    v->count = count; v->viewed = 1; return GLE_OK;
}
int GleClaimVoucher(struct GleState *s, uint8_t slot, uint8_t choice, uint16_t *out) {
    struct GleVoucher *v; uint16_t species;
    if (!GleValid(s) || slot >= s->voucherCount || !out) return GLE_INVALID;
    v = &s->vouchers[slot];
    if (!v->viewed || choice >= v->count) return GLE_NOT_READY;
    species = v->species[choice];
    if (GleSpeciesClaimed(s, species, v->kind)) return GLE_DUPLICATE;
    SetBit(Shiny(v->kind) ? s->shinyClaims : s->normalClaims, species);
    *out = species; memmove(v, v + 1, (--s->voucherCount - slot) * sizeof(*v));
    memset(&s->vouchers[s->voucherCount], 0, sizeof(*v)); return GLE_OK;
}
