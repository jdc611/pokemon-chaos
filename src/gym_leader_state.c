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
    s->rocket.nextAt=120;
    s->winds.selected=GLE_NONE;
    GleCompetitionInit(s);
    GleHostingInit(s);
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
            {unsigned i,n=0,limit=(s->rank+1)/2+(s->rank==8);
             for(i=0;i<GLE_RECRUITS;i++)if(s->recruits[i].active && !s->recruits[i].away)
                 if(++n>limit)s->recruits[i].active=0;}
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
static void FilterStaffChallenger(struct GleState *s) {
    unsigned i,start=s->activity/6%GLE_RECRUITS;
    if(s->queueCount<2 || s->queue[0]!=0)return;
    for(i=0;i<GLE_RECRUITS;i++) {
        struct GleRecruit *r=&s->recruits[(start+i)%GLE_RECRUITS];unsigned chance;
        if(!r->id || !r->active || r->away)continue;
        chance=35+7*r->potential+2*s->gymDesign.puzzle+(r->wins+r->losses>20?5:0);
        if(GleMix(s->seed^s->activity^r->id)%100<chance) {
            r->wins=Add16(r->wins,1);if(r->loyalty<100)r->loyalty++;
            memmove(s->queue,s->queue+1,--s->queueCount);return;
        } else {r->losses=Add16(r->losses,1);if(r->loyalty)r->loyalty--;}
    }
}
void GleActivity(struct GleState *s, uint32_t *money) {
    unsigned i, revenue = 0;
    if (!GleValid(s) || !money) return;
    s->activity++;
    GleRocketActivity(s);
    GleCompetitionActivity(s,money);
    if (++s->queueClock >= 6) { GleEnqueue(s, GleMix(s->seed ^ s->activity) % 25 == 0 ? 2 : 0); s->queueClock = 0; FilterStaffChallenger(s); }
    if (s->activity % 12) return;
    Recount(s);
    if (s->reputation >= 100 && s->population < s->capacity) s->population++;
    else if (s->reputation < 60 && s->population) s->population--;
    for (i = 0; i < GLE_PLOTS; i++) if (s->plots[i].building >= GLE_HOTEL) revenue += 4 * s->plots[i].tier;
    revenue += s->population / 2;
    if(s->rocket.stage)revenue/=2;
    *money = *money >= GLE_MAX_MONEY || revenue > GLE_MAX_MONEY - *money ? GLE_MAX_MONEY : *money + revenue;
    for (i = 0; i < GLE_RECRUITS; i++) {
        struct GleRecruit *r = &s->recruits[i];
        if (!r->id || r->away || !r->active) continue;
        if (s->reputation < 60 && s->lossStreak >= 3 && !r->questBond) {
            if (r->warnings < 3) r->warnings++;
            else if (r->loyalty < 60) { r->away = 1+GleMix(s->seed^r->id)%2; r->active = 0; }
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
    *money -= cost;
    if (!p->tier) p->type = s->gymType;
    p->building = b; p->tier = tier;
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
    if (!active) s->recruits[slot].active = 0;
    else if (!s->recruits[slot].active) s->recruits[slot].active = 1;
    return GLE_OK;
}
/* station 0 selects automatic placement; 1..5 request a safe station. */
int GlePositionTrainer(struct GleState *s, uint8_t slot, uint8_t station) {
    unsigned i;
    if (!GleValid(s) || slot >= GLE_RECRUITS || station > 5 ||
        !s->recruits[slot].id || !s->recruits[slot].active || s->recruits[slot].away) return GLE_INVALID;
    if (station) for (i = 0; i < GLE_RECRUITS; i++)
        if (i != slot && s->recruits[i].id && !s->recruits[i].away && s->recruits[i].active == station + 1) return GLE_NO_ROOM;
    s->recruits[slot].active = station + 1;
    return GLE_OK;
}
uint8_t GleTrainerStation(const struct GleState *s, uint8_t slot) {
    unsigned i, place, reserved = 0, used = 0;
    if (!GleValid(s) || slot >= GLE_RECRUITS) return GLE_NONE;
    for (i = 0; i < GLE_RECRUITS; i++) {
        const struct GleRecruit *r = &s->recruits[i];
        if (r->id && !r->away && r->active >= 2 && r->active <= 6) reserved |= 1u << (r->active - 2);
    }
    for (i = 0; i <= slot; i++) {
        const struct GleRecruit *r = &s->recruits[i];
        if (!r->id || r->away || !r->active) { if (i == slot) return GLE_NONE; continue; }
        place = r->active >= 2 && r->active <= 6 ? r->active - 2 : 5;
        if (place == 5 || (used & (1u << place))) {
            for (place = 0; place < 5 && ((used | reserved) & (1u << place)); place++);
            if (place == 5) for (place = 0; place < 5 && (used & (1u << place)); place++);
        }
        if (i == slot) return place < 5 ? place : GLE_NONE;
        if (place < 5) used |= 1u << place;
    }
    return GLE_NONE;
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

/* Eight finite resident stories use their own reserved ledger bits, 200–207.
 * Research reserves bits 95–199. No save layout changes or story flags. */
int GleResidentTaskReady(const struct GleState *s, uint8_t request)
{
    static const uint8_t buildings[4]={GLE_CENTER,GLE_HABITAT,GLE_RESTAURANT,GLE_MUSEUM};
    unsigned i;
    if(!GleValid(s) || request>=8)return GLE_INVALID;
    if(GetBit(s->uniqueObtained,200+request))return GLE_DUPLICATE;
    if(request>=4)return GLE_OK;
    for(i=0;i<GLE_PLOTS;i++)if(s->plots[i].building==buildings[request] && s->plots[i].tier)return GLE_OK;
    return GLE_NOT_READY;
}
int GleResolveResidentTask(struct GleState *s, uint8_t request, uint8_t choice, uint32_t *money)
{
    static const uint16_t honestRewards[8]={500,1000,800,1200,400,400,1000,0};
    static const uint16_t shadyRewards[4]={1500,800,5000,2000};
    static const int8_t shadyHonor[4]={-60,-50,-80,-50};
    int status,honor;
    if(!money || request>=8 || choice>1 || (request<4 && choice))return GLE_INVALID;
    status=GleResidentTaskReady(s,request);if(status!=GLE_OK)return status;
    *money=Add32(*money,choice?shadyRewards[request-4]:honestRewards[request]);
    if(*money>GLE_MAX_MONEY)*money=GLE_MAX_MONEY;
    honor=s->honor+(choice?shadyHonor[request-4]:request<4?10:40);
    s->honor=honor<INT16_MIN?INT16_MIN:honor>INT16_MAX?INT16_MAX:honor;
    if(choice)s->reputation=s->reputation>3?s->reputation-3:0;
    else s->reputation=Add16(s->reputation,5);
    SetBit(s->uniqueObtained,200+request);return GLE_OK;
}

void GleRocketActivity(struct GleState *s)
{
    unsigned i,buildings=0,habitat=0,museum=0,power=0,base,help;uint32_t roll;
    if(!GleValid(s))return;
    if(s->rocket.stage)
    {
        if(s->activity%24==0 && s->reputation)s->reputation--;
        return;
    }
    if(s->rank<2 || s->population<6 || s->activity<s->rocket.nextAt)return;
    for(i=0;i<GLE_PLOTS;i++)if(s->plots[i].building)
    {buildings++;habitat|=s->plots[i].building==GLE_HABITAT;museum|=s->plots[i].building==GLE_MUSEUM;}
    if(buildings<2)return;
    s->rocket.serial=Add16(s->rocket.serial,1);roll=GleMix(s->seed^s->rocket.serial^0x524F434Bu);
    s->rocket.kind=museum && roll%3==0?2:habitat && roll%2==0?1:0;
    if(!s->rocket.bossDefeated && s->rank>=7 && s->rocket.cleared>=3 && s->rocket.stolen)s->rocket.kind=3;
    s->rocket.heist=s->rocket.kind==2 && GleMix(roll^0x48454953u)%4==0;
    if(s->rocket.heist)s->rocket.stolen=Add16(s->rocket.stolen,1);
    for(i=0;i<GLE_RECRUITS;i++)if(s->recruits[i].id && s->recruits[i].active && !s->recruits[i].away)power+=s->recruits[i].potential;
    base=1+s->rank/3+(s->population>=20)+(s->population>=40)+(s->rocket.kind==3);
    if(base>5)base=5;
    help=power>=15?2:power>=7?1:0;
    if(help>=base)help=base-1;
    s->rocket.staffHelp=help;s->rocket.remaining=base-help;s->rocket.stage=1;
}
int GleRocketBattleResult(struct GleState *s,int won)
{
    if(!GleValid(s) || s->rocket.stage!=1 || !s->rocket.remaining)return GLE_NOT_READY;
    if(!won){s->rocket.losses=Add16(s->rocket.losses,1);return GLE_OK;}
    if(!--s->rocket.remaining)s->rocket.stage=2;
    return GLE_OK;
}
int GleResolveRocket(struct GleState *s,uint8_t choice,uint32_t *money)
{
    int honor;
    if(!GleValid(s) || !money || choice>1)return GLE_INVALID;
    if(s->rocket.stage!=2)return GLE_NOT_READY;
    *money=Add32(*money,(choice?1500u:1000u)*s->rank);if(*money>GLE_MAX_MONEY)*money=GLE_MAX_MONEY;
    honor=s->honor+(choice?-30:20);s->honor=honor<INT16_MIN?INT16_MIN:honor>INT16_MAX?INT16_MAX:honor;
    if(choice)s->reputation=s->reputation>5?s->reputation-5:0;else s->reputation=Add16(s->reputation,10);
    if(s->rocket.kind==3 && !choice)
    {
        s->rocket.recovered=Add16(s->rocket.recovered,s->rocket.stolen);s->rocket.stolen=0;s->rocket.bossDefeated=1;
    }
    s->rocket.cleared=Add16(s->rocket.cleared,1);s->rocket.stage=0;s->rocket.remaining=0;
    s->rocket.nextAt=Add32(s->activity,80+GleMix(s->seed^s->rocket.serial)%80);
    return GLE_OK;
}

uint32_t GleGymDesignPrice(const struct GleState *s,uint8_t kind,uint8_t choice)
{
 static const uint32_t upgrade[]={5000,15000,30000};uint32_t n;
 if(!s||kind>3)return 0;
 if(kind==3){if(s->gymDesign.tier>=3)return 0;n=upgrade[s->gymDesign.tier];}
 else {if(choice>=(kind==2?6:4))return 0;n=kind==0?1000:kind==1?1500:750;}
 return s->honor>=100?n*9/10:n;
}
int GleGymDesignPurchase(struct GleState *s,uint8_t kind,uint8_t choice,uint32_t *money)
{
 uint8_t *field;uint32_t cost;
 if(!GleValid(s)||!money||kind>3||choice>=(kind==2?6:4))return GLE_INVALID;
 field=kind==0?&s->gymDesign.layout:kind==1?&s->gymDesign.puzzle:kind==2?&s->gymDesign.theme:&s->gymDesign.tier;
 if(kind==3){if(*field>=3)return GLE_DUPLICATE;if((s->rank>s->highestRank?s->rank:s->highestRank)<3+2*(*field))return GLE_NOT_READY;choice=*field+1;}
 else {if(*field==choice)return GLE_DUPLICATE;if(kind<2&&choice>s->gymDesign.tier)return GLE_NOT_READY;}
 cost=GleGymDesignPrice(s,kind,choice);if(*money<cost)return GLE_NOT_ENOUGH_MONEY;
 *money-=cost;*field=choice;return GLE_OK;
}
