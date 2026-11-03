#include "global.h"
#include "gym_leader.h"
#include "data.h"
#include "event_data.h"
#include "malloc.h"
#include "pokemon.h"
#include "string_util.h"
#include "constants/battle_ai.h"

static const u8 sTypes[] = {
    TYPE_NORMAL, TYPE_FIRE, TYPE_WATER, TYPE_ELECTRIC, TYPE_GRASS, TYPE_ICE,
    TYPE_FIGHTING, TYPE_POISON, TYPE_GROUND, TYPE_FLYING, TYPE_PSYCHIC, TYPE_BUG,
    TYPE_ROCK, TYPE_GHOST, TYPE_DRAGON, TYPE_DARK, TYPE_STEEL, TYPE_FAIRY
};
static const u8 *const sNames[] = {
    COMPOUND_STRING("NORMAL"), COMPOUND_STRING("FIRE"), COMPOUND_STRING("WATER"),
    COMPOUND_STRING("ELECTRIC"), COMPOUND_STRING("GRASS"), COMPOUND_STRING("ICE"),
    COMPOUND_STRING("FIGHTING"), COMPOUND_STRING("POISON"), COMPOUND_STRING("GROUND"),
    COMPOUND_STRING("FLYING"), COMPOUND_STRING("PSYCHIC"), COMPOUND_STRING("BUG"),
    COMPOUND_STRING("ROCK"), COMPOUND_STRING("GHOST"), COMPOUND_STRING("DRAGON"),
    COMPOUND_STRING("DARK"), COMPOUND_STRING("STEEL"), COMPOUND_STRING("FAIRY")
};

static bool32 MatchesType(enum Species species, u32 type)
{
    return species > SPECIES_NONE && species < NUM_SPECIES
        && (gSpeciesInfo[species].types[0] == sTypes[type]
            || gSpeciesInfo[species].types[1] == sTypes[type]);
}

// Keep the original starter's identity. A level evolution is permitted only
// when it still qualifies for this Gym (e.g. Water Surskit stays Surskit).
static enum Species EvolveStarter(enum Species species, u32 level, u32 type)
{
    u32 round, j;
    for (round = 0; round < 5; round++)
    {
        const struct Evolution *ev = gSpeciesInfo[species].evolutions;
        enum Species next = species;
        if (ev != NULL)
            for (j = 0; j < 64 && ev[j].method != EVOLUTIONS_END; j++)
                if (ev[j].method == EVO_LEVEL && ev[j].param <= level
                    && MatchesType(ev[j].targetSpecies, type))
                {
                    next = ev[j].targetSpecies;
                    break;
                }
        if (next == species)
            break;
        species = next;
    }
    return species;
}

bool32 GymLeaderPrepareRivalRoster(struct Trainer *trainer, struct TrainerMon *entries)
{
    struct GleState *s = GymLeaderState();
    u16 *pool;
    u32 count, i, j, attempt, seed, level;
    if (!IsGymLeaderRun() || s->rivalType >= ARRAY_COUNT(sTypes))
        return FALSE;
    pool = Alloc(GLE_SPECIES_LIMIT * sizeof(*pool));
    if (pool == NULL)
        return FALSE;
    seed = GleMix(s->seed ^ 0x52495641u);
    if (s->rivalStarter == SPECIES_NONE)
    {
        count = GymLeaderBuildStarterPool(s->rivalType, pool, GLE_SPECIES_LIMIT);
        if (!count || count > GLE_SPECIES_LIMIT)
            goto fail;
        s->rivalStarter = pool[seed % count];
    }
    if (!MatchesType(s->rivalStarter, s->rivalType) || GymLeaderSpeciesReserved(s->rivalStarter))
        goto fail;
    count = GymLeaderBuildSpeciesPool(s->rivalType, s->rank, GLE_STANDARD,
        FALSE, pool, GLE_SPECIES_LIMIT);
    if (!count || count > GLE_SPECIES_LIMIT)
        goto fail;
    memset(trainer, 0, sizeof(*trainer));
    memset(entries, 0, PARTY_SIZE * sizeof(*entries));
    trainer->trainerClass = TRAINER_CLASS_RIVAL_LATE_FRLG;
    trainer->trainerPic = TRAINER_PIC_RIVAL_LATE_FRLG;
    StringCopy(trainer->trainerName, gSaveBlock1Ptr->rivalName);
    trainer->partySize = GymLeaderPartyLimit();
    trainer->party = entries;
    trainer->aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT;
    level = GymLeaderLevelCap();
    ZeroEnemyPartyMons();
    for (i = 0; i < trainer->partySize; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][i];
        u8 ability = GleMix(seed ^ i) & 1;
        enum Species species = EvolveStarter(s->rivalStarter, level, s->rivalType);
        // Remaining team slots may develop with rank, but never duplicate a
        // species when the current type catalog provides enough distinct ones.
        if (i != 0)
            for (attempt = 0; attempt < count; attempt++)
            {
                species = pool[(GleMix(seed ^ i) % count + attempt) % count];
                for (j = 0; j < i; j++)
                    if (entries[j].species == species)
                        break;
                if (j == i)
                    break;
            }
        CreateMonWithIVs(mon, species, level, GleMix(seed ^ i ^ 0x414345u),
            OTID_STRUCT_PRESET(seed), 16 + s->rank);
        if (gSpeciesInfo[species].abilities[ability] == ABILITY_NONE)
            ability = 0;
        SetMonData(mon, MON_DATA_ABILITY_NUM, &ability);
        SetMonData(mon, MON_DATA_OT_NAME, gSaveBlock1Ptr->rivalName);
        if (i == 0)
        {
            u8 metLevel = 5;
            SetMonData(mon, MON_DATA_MET_LEVEL, &metLevel);
        }
        GiveMonInitialMoveset(mon);
        entries[i].species = species;
        entries[i].lvl = level;
    }
    gPartiesCount[B_TRAINER_OPPONENT_A] = trainer->partySize;
    Free(pool);
    return TRUE;
fail:
    Free(pool);
    return FALSE;
}

void GleRivalInfo(void)
{
    struct GleState *s = GymLeaderState();
    gSpecialVar_Result = 2;
    if (!IsGymLeaderRun() || s->rivalType >= ARRAY_COUNT(sNames))
        return;
    StringCopy(gStringVar1, sNames[s->rivalType]);
    ConvertIntToDecimalStringN(gStringVar2, GymLeaderLevelCap(), STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar3, GymLeaderPartyLimit(), STR_CONV_MODE_LEFT_ALIGN, 1);
    gSpecialVar_Result = (s->rivalFriendlyMask >> (s->rank - 1)) & 1;
}
