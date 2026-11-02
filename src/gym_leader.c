#include "global.h"
#include "gym_leader.h"
#include "pokemon.h"
#include "event_data.h"
#include "constants/pokemon.h"
#include "data/gym_leader_catalog.h"

struct GleState *GymLeaderState(void)
{
    struct GleState *s;
    if (gSaveBlock1Ptr == NULL)
        return NULL;
    s = &gSaveBlock1Ptr->gymLeader;
    if (s->magic == GLE_MAGIC && s->version >= 2 && s->version < GLE_VERSION)
    {
        if (s->version == 2)
        {
            s->registeredCount = 0;
            memset(s->registeredPersonality, 0, sizeof(s->registeredPersonality));
            memset(s->registeredTrainerId, 0, sizeof(s->registeredTrainerId));
        }
        if (s->version < 4)
        {
            s->interiorPlot = s->interiorTown = GLE_NONE;
            s->landmarkTier = 0;
        }
        {
            u32 i;
            for(i=0;i<GLE_VOUCHERS;i++)s->vouchers[i].level=0;
        }
        s->milestoneRewardMask = 0;
        s->version = GLE_VERSION;
    }
    return s;
}

bool32 IsGymLeaderRun(void)
{
    return IS_FRLG && GleValid(GymLeaderState());
}

void GymLeaderInitialize(void)
{
    if (IS_FRLG && GymLeaderState() != NULL)
        GleInit(GymLeaderState(), gSaveBlock3Ptr->worldSeed);
}

u32 GymLeaderLevelCap(void)
{
    return GleCap(GymLeaderState());
}

u32 GymLeaderPartyLimit(void)
{
    return GleTeamSize(GymLeaderState());
}

enum Species GymLeaderExclusiveSpecies(u32 type, u32 location)
{
    if (type >= GLE_EXCLUSIVE_TYPES || location >= GLE_EXCLUSIVE_PER_TYPE)
        return SPECIES_NONE;
    return sGleExclusive[type][location];
}

bool32 GymLeaderSpeciesReserved(enum Species species)
{
    u32 i, j;
    for (i = 0; i < GLE_EXCLUSIVE_TYPES; i++)
        for (j = 0; j < GLE_EXCLUSIVE_PER_TYPE; j++)
            if (sGleExclusive[i][j] == species)
                return TRUE;
    for (i = 0; i < ARRAY_COUNT(sGleResearchSpecies); i++)
        if (sGleResearchSpecies[i] == species)
            return TRUE;
    return FALSE;
}

bool32 GymLeaderTeamLegal(struct Pokemon *party, u32 count)
{
    struct GleState *state = GymLeaderState();
    u32 i, j, offType = 0;
    enum Type type;
    if (!IsGymLeaderRun() || state->gymType >= GLE_EXCLUSIVE_TYPES
        || count == 0 || count > GymLeaderPartyLimit())
        return FALSE;
    type = sGleTypes[state->gymType];
    for (i = 0; i < count; i++)
    {
        enum Species species = GetMonData(&party[i], MON_DATA_SPECIES);
        if (species == SPECIES_NONE || species >= NUM_SPECIES
            || GetMonData(&party[i], MON_DATA_IS_EGG))
            return FALSE;
        if (gSpeciesInfo[species].types[0] == type || gSpeciesInfo[species].types[1] == type)
            continue;
        if (++offType > 1 || state->signatureMove == MOVE_NONE)
            return FALSE;
        for (j = 0; j < MAX_MON_MOVES; j++)
            if (GetMonData(&party[i], MON_DATA_MOVE1 + j) == state->signatureMove)
                break;
        if (j == MAX_MON_MOVES)
            return FALSE;
    }
    // Levels are preserved across demotion. Official battle adapter must
    // normalize over-cap mons transactionally before calling a battle.
    return TRUE;
}

bool32 GymLeaderOfficialTeamLegal(void)
{
    return GymLeaderTeamLegal(gParties[B_TRAINER_PLAYER], gPartiesCount[B_TRAINER_PLAYER]);
}

void GymLeaderScriptStatus(void)
{
    gSpecialVar_Result = IsGymLeaderRun();
    if (!gSpecialVar_Result)
        return;
    gSpecialVar_0x8004 = GymLeaderState()->rank;
    gSpecialVar_0x8005 = GymLeaderLevelCap();
    gSpecialVar_0x8006 = GymLeaderPartyLimit();
}

void GymLeaderScriptExclusive(void)
{
    struct GleState *s = GymLeaderState();
    gSpecialVar_Result = IsGymLeaderRun()
        ? GymLeaderExclusiveSpecies(s->gymType, gSpecialVar_0x8004) : SPECIES_NONE;
}

void GymLeaderScriptChooseIdentity(void)
{
    gSpecialVar_Result = GleChooseIdentity(GymLeaderState(), gSpecialVar_0x8004,
        gSpecialVar_0x8005, gSpecialVar_0x8006);
}

void GymLeaderScriptActivity(void)
{
    if (IsGymLeaderRun())
    {
        // SaveBlock money is encrypted. Runtime must pass an unencrypted
        // balance through GetMoney/SetMoney rather than write it directly.
        gSpecialVar_Result = GymLeaderState()->queueCount;
    }
}

#include "move.h"
#include "money.h"
#include "data/gym_leader_signature_pools.h"

u32 GymLeaderSignatureOption(u32 type, u32 slot)
{
    struct GleState *s = GymLeaderState();
    if (!IsGymLeaderRun() || type >= 18 || slot >= 3)
        return MOVE_NONE;
    return sGleSignaturePools[type][slot][GleMix(s->seed ^ (type << 8) ^ slot) % 3];
}

static bool32 GlePoolSpeciesAllowed(enum Species species, u32 type)
{
    const struct SpeciesInfo *info = &gSpeciesInfo[species];
    if (species == SPECIES_NONE || species >= SPECIES_GLE_AWAKENED_SLAKING
        || info->baseHP == 0 || info->natDexNum == NATIONAL_DEX_NONE
        || info->isMegaEvolution || info->isPrimalReversion || info->isGigantamax
        || info->isTotem || info->isUltraBurst || info->isTeraForm)
        return FALSE;
    // Battle/cosmetic variants are not separate starter/voucher species.
    // Regional forms remain eligible because their typing can be essential.
    if (species > SPECIES_PECHARUNT && !info->isAlolanForm && !info->isGalarianForm
        && !info->isHisuianForm && !info->isPaldeanForm)
        return FALSE;
    return info->types[0] == sGleTypes[type] || info->types[1] == sGleTypes[type];
}

u32 GymLeaderBuildSpeciesPool(u32 type, u32 tier, u32 kind, bool32 starters, u16 *out, u32 capacity)
{
    u8 evolved[(NUM_SPECIES + 7) / 8];
    u32 species, count = 0;
    u32 maxBst = starters ? 420 : 320 + tier * 45;
    if (type >= 18 || tier < 1 || tier > 8 || kind >= GLE_VOUCHER_KINDS )
        return 0;
    memset(evolved, 0, sizeof(evolved));
    if (starters)
    {
        for (species = 1; species < SPECIES_GLE_AWAKENED_SLAKING; species++)
        {
            const struct Evolution *evo = gSpeciesInfo[species].evolutions;
            u32 j;
            if (evo == NULL)
                continue;
            for (j = 0; j < 64 && evo[j].method != EVOLUTIONS_END; j++)
                if (evo[j].targetSpecies < NUM_SPECIES)
                    evolved[evo[j].targetSpecies / 8] |= 1u << (evo[j].targetSpecies % 8);
        }
    }
    for (species = 1; species < SPECIES_GLE_AWAKENED_SLAKING; species++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species];
        u32 bst;
        bool32 elite = kind == GLE_ELITE || kind == GLE_SHINY_ELITE;
        if (!GlePoolSpeciesAllowed(species, type) || GymLeaderSpeciesReserved(species))
            continue;
        if ((info->isRestrictedLegendary || info->isSubLegendary || info->isMythical
            || info->isUltraBeast || info->isParadox) && (!elite || starters))
            continue;
        if (starters && ((evolved[species / 8] >> (species % 8)) & 1))
            continue;
        // Catalog strength uses species data, independent of other modes'
        // randomized-stat machinery (which can be costly and changes policy).
        bst = info->baseHP + info->baseAttack + info->baseDefense
            + info->baseSpAttack + info->baseSpDefense + info->baseSpeed;
        if (bst > maxBst || (elite && bst < 480) || (kind == GLE_RARE && bst < 350))
            continue;
        if (count < capacity)
            out[count] = species;
        count++;
    }
    return count; // Returns required size; caller must not use truncated count.
}

void GymLeaderAdvanceActivity(void)
{
    u32 money;
    if (!IsGymLeaderRun())
        return;
    money = GetMoney(&gSaveBlock1Ptr->money);
    GleActivity(GymLeaderState(), &money);
    SetMoney(&gSaveBlock1Ptr->money, money);
}


u32 GymLeaderPoolCount(u32 type, u32 tier, u32 kind, bool32 starters)
{
    return GymLeaderBuildSpeciesPool(type, tier, kind, starters, NULL, 0);
}

bool32 GymLeaderSignaturePoolsMatchTypes(void)
{
    u32 type, slot, option;
    for (type = 0; type < 18; type++)
        for (slot = 0; slot < 3; slot++)
            for (option = 0; option < 3; option++)
                if (gMovesInfo[sGleSignaturePools[type][slot][option]].type != sGleTypes[type])
                    return FALSE;
    return TRUE;
}

bool32 GymLeaderCanLearnSignature(enum Species species, enum Move move)
{
    const struct LevelUpMove *learnset;
    u32 i;
    if (species == SPECIES_NONE || species >= NUM_SPECIES || move == MOVE_NONE || move >= MOVES_COUNT)
        return FALSE;
    if (CanLearnTeachableMove(species, move))
        return TRUE;
    learnset = GetSpeciesLevelUpLearnset(species);
    if (learnset != NULL)
        for (i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
            if (learnset[i].move == move)
                return TRUE;
    return FALSE;
}

u32 GymLeaderBuildStarterPool(u32 type, u16 *out, u32 capacity)
{
    return GymLeaderBuildSpeciesPool(type, 1, GLE_STANDARD, TRUE, out, capacity);
}
