#include "global.h"
#include "oaks_draft.h"
#include "ironmon.h"
#include "run_settings.h"
#include "event_data.h"
#include "random.h"
#include "data.h"
#include "item.h"
#include "move.h"
#include "string_util.h"
#include "script_pokemon_util.h"
#include "pokemon_storage_system.h"
#include "pokedex.h"
#include "coins.h"
#include "fieldmap.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/trainers.h"
#include "constants/trainer_types.h"
#include "constants/pokemon.h"
#include "constants/maps.h"
#include "constants/flags.h"
#include "constants/event_objects.h"
#include "battle_setup.h"
#include "battle.h"
#include "chaos_v2.h"
#include "caps.h"
#include "text.h"
#include "party_menu.h"
#include "event_object_movement.h"
#include "malloc.h"

STATIC_ASSERT(sizeof(struct OaksDraftState) <= sizeof(struct IronmonRunState), DraftSaveTailFits);
static bool8 sGymAward;
static bool8 sPrebuilt;
static u8 sHasPre[(NUM_SPECIES+7)/8];
static u16 sGenerated[18];
static u32 sGeneratedSeed;
static bool8 sGeneratedValid;
bool32 OaksDraftAwardingGymTM(void) { return sGymAward; }
static const u8 *const sNames[] = {
    COMPOUND_STRING("PATCH"), COMPOUND_STRING("EMBER"), COMPOUND_STRING("PUDDLE"), COMPOUND_STRING("BISCUIT"),
    COMPOUND_STRING("SCOUT"), COMPOUND_STRING("MILO"), COMPOUND_STRING("BUTTON"), COMPOUND_STRING("NOVA"),
    COMPOUND_STRING("WALLY"), COMPOUND_STRING("PEPPER"), COMPOUND_STRING("DOT"), COMPOUND_STRING("BEAN"),
    COMPOUND_STRING("ROOKIE"), COMPOUND_STRING("SAGE"), COMPOUND_STRING("WIDGET"), COMPOUND_STRING("POPPY"),
    COMPOUND_STRING("BUDDY"), COMPOUND_STRING("GIZMO"), COMPOUND_STRING("CLOVER"), COMPOUND_STRING("RIVER"),
    COMPOUND_STRING("JUNO"), COMPOUND_STRING("PIP"), COMPOUND_STRING("DUSTY"), COMPOUND_STRING("MOCHI"),
    COMPOUND_STRING("SUNNY"), COMPOUND_STRING("SPROUT"), COMPOUND_STRING("FINN"), COMPOUND_STRING("TUCK"),
    COMPOUND_STRING("MABEL"), COMPOUND_STRING("ZIP"), COMPOUND_STRING("LOKI"), COMPOUND_STRING("MARBLE"),
};
static const u8 *const sNotes[] = {
    COMPOUND_STRING("Its trainer left for JOHTO\nand never returned."),
    COMPOUND_STRING("Found waiting outside a\nPOKEMON CENTER."),
    COMPOUND_STRING("Surrendered by a trainer\nwho could not care for it."),
    COMPOUND_STRING("Arrived in an unmarked\nshipment."),
    COMPOUND_STRING("Its trainer believed\nit was too weak."),
    COMPOUND_STRING("Waited beside an empty\nbackpack for three days."),
    COMPOUND_STRING("A traveling nurse brought\nit here for a fresh start."),
    COMPOUND_STRING("Found curled up inside\na delivery crate."),
    COMPOUND_STRING("Its old home closed.\nIt still trusts people."),
    COMPOUND_STRING("A timid companion looking\nfor another chance."),
    COMPOUND_STRING("Its trainer moved away.\nOAK promised it a home."),
    COMPOUND_STRING("Rescued from an abandoned\ncamp near VIRIDIAN."),
};

bool32 IsOaksDraft(void)
{
    return IS_FRLG && gSaveBlock3Ptr->starterMode == RUN_STARTER_DRAFT
        && gSaveBlock3Ptr->draft.magic == OAKS_DRAFT_MAGIC;
}
void OaksDraftInitialize(void)
{
    if (gSaveBlock3Ptr->starterMode != RUN_STARTER_DRAFT) return;
    memset(&gSaveBlock3Ptr->draft, 0, sizeof(gSaveBlock3Ptr->draft));
    gSaveBlock3Ptr->draft.magic = OAKS_DRAFT_MAGIC;
    memset(gSaveBlock3Ptr->draft.picks, 0xFF, 6);
    memset(gSaveBlock3Ptr->draft.rival, 0xFF, 6);
    gSaveBlock3Ptr->filterMode = RUN_FILTER_NONE;
    gSaveBlock3Ptr->filterValue = 0;
    gSaveBlock3Ptr->evolutionMode = RUN_EVOLUTIONS_NORMAL;
    gSaveBlock3Ptr->randomizerEnabled = RUN_WILD_NORMAL;
    gSaveBlock3Ptr->itemRandomization = FALSE;
}
bool32 OaksDraftSpeciesEligible(enum Species species)
{
    if (species <= SPECIES_NONE || species >= NUM_SPECIES || species == SPECIES_EGG || !IsSpeciesEnabled(species)) return FALSE;
    const struct SpeciesInfo *info = &gSpeciesInfo[species];
    if (GET_BASE_SPECIES_ID(species) != species && !info->isAlolanForm && !info->isGalarianForm && !info->isHisuianForm && !info->isPaldeanForm) return FALSE;
    if (info->isRestrictedLegendary || info->isSubLegendary || info->isMythical || info->isUltraBeast || info->isParadox
        || info->isMegaEvolution || info->isPrimalReversion || info->isUltraBurst || info->isGigantamax || info->isTeraForm || info->isTotem) return FALSE;
    // Custom species do not have an official National Dex identity.
    if (SpeciesToNationalPokedexNum(species) > NATIONAL_DEX_COUNT || !SpeciesToNationalPokedexNum(species)) return FALSE;
    if (!sPrebuilt) {
        for(u32 parent=1;parent<NUM_SPECIES;parent++) {
            if(!IsSpeciesEnabled(parent))continue;
            const struct Evolution *evos=gSpeciesInfo[parent].evolutions;
            if(!evos)continue;
            for(u32 j=0;evos[j].method!=EVOLUTIONS_END;j++) {
                u32 target=evos[j].targetSpecies;
                if(target && target<NUM_SPECIES)sHasPre[target/8]|=1<<(target%8);
            }
        }
        sPrebuilt=TRUE;
    }
    u32 base=GET_BASE_SPECIES_ID(species);
    return !(sHasPre[species/8]&(1<<(species%8))) && !(sHasPre[base/8]&(1<<(base%8)));
}
void OaksDraftGenerateSpecies(u16 result[18])
{
    if(sGeneratedValid && sGeneratedSeed==gSaveBlock3Ptr->worldSeed) { memcpy(result,sGenerated,sizeof(sGenerated));return; }
    u16 *pool=Alloc(NUM_SPECIES*sizeof(u16));
    if(!pool) { memset(result,0,18*sizeof(u16));return; }
    u32 count=0;
    for(u32 sp=1;sp<NUM_SPECIES;sp++)if(OaksDraftSpeciesEligible(sp))pool[count++]=sp;
    for(u32 i=0;i<18;i++) {
        u32 rank=IronmonMix(gSaveBlock3Ptr->worldSeed^(0x44524654u+i))%max(1,count);
        u16 chosen=SPECIES_NONE;
        for(u32 pass=0;pass<count;pass++) {
            chosen=pool[(rank+pass)%count];bool32 duplicate=FALSE;
            for(u32 j=0;j<i;j++)if(GET_BASE_SPECIES_ID(result[j])==GET_BASE_SPECIES_ID(chosen))duplicate=TRUE;
            if(!duplicate)break;
        }
        result[i]=chosen;
    }
    Free(pool);memcpy(sGenerated,result,sizeof(sGenerated));sGeneratedSeed=gSaveBlock3Ptr->worldSeed;sGeneratedValid=TRUE;
}
void OaksDraftSetNickname(struct Pokemon *mon, u32 index)
{
    u32 member = FALSE;
    SetMonData(mon, MON_DATA_DRAFT_MEMBER, &member);
    u8 name[POKEMON_NAME_LENGTH + 1];
    memset(name, EOS, sizeof(name));
    StringCopy(name, sNames[(IronmonMix(gSaveBlock3Ptr->worldSeed) % ARRAY_COUNT(sNames) + index) % ARRAY_COUNT(sNames)]);
    SetMonData(mon, MON_DATA_NICKNAME, name);
    member = TRUE;
    SetMonData(mon, MON_DATA_DRAFT_MEMBER, &member);
}
void OaksDraftFormatBackground(struct Pokemon *mon, u8 *dest)
{
    u32 pid = GetMonData(mon, MON_DATA_PERSONALITY);
    StringCopy(dest, sNotes[IronmonMix(pid ^ 0x4E4F5445u) % ARRAY_COUNT(sNotes)]);
}
static void EnsureAttack(struct Pokemon *mon)
{
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (IronmonMoveIsStarterAttack(GetMonData(mon, MON_DATA_MOVE1 + i))) return;
    enum Move move = gSaveBlock3Ptr->movesetMode == RUN_MOVESETS_RANDOM
        ? IronmonStarterAttack(GetMonData(mon, MON_DATA_SPECIES)) : MOVE_TACKLE;
    SetMonMoveSlot(mon, move, 0);
}
void OaksDraftCreateCandidate(struct Pokemon *mon, u32 index, u8 level)
{
    u16 species[18];
    OaksDraftGenerateSpecies(species);
    if(index>=18 || !species[index]) { ZeroMonData(mon);return; }
    rng_value_t saved = gRngValue, saved2 = gRng2Value;
    u32 pid = IronmonMix(gSaveBlock3Ptr->worldSeed ^ (0x504F4B45u + index));
    SeedRng(pid); SeedRng2(pid ^ 0x495653u);
    CreateMonWithIVs(mon, species[min(index,17)], level, pid, OTID_STRUCT_PLAYER_ID, USE_RANDOM_IVS);
    GiveMonInitialMoveset(mon);
    if (IsMinimalGrindingMode()) ApplyMinimalGrindingModeToMon(mon);
    EnsureAttack(mon);
    OaksDraftSetNickname(mon, index);
    gRngValue = saved; gRng2Value = saved2;
}
static u32 CandidateScore(struct Pokemon *mon, u32 round)
{
    u32 score = ChaosCurrentStageRating(mon) * 4;
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    enum Type type1 = GetSpeciesType(species, 0), type2 = GetSpeciesType(species, 1);
    for (u32 i = 0; i < round; i++)
    {
        struct Pokemon previous;
        OaksDraftCreateCandidate(&previous, i * 3 + gSaveBlock3Ptr->draft.rival[i], 5);
        enum Species other = GetMonData(&previous, MON_DATA_SPECIES);
        if (GetSpeciesType(other,0) == type1 || GetSpeciesType(other,1) == type1) score = score > 20 ? score - 20 : 0;
        if (type2 != type1 && (GetSpeciesType(other,0) == type2 || GetSpeciesType(other,1) == type2)) score = score > 20 ? score - 20 : 0;
    }
    return score;
}
bool32 OaksDraftCommit(u32 choice)
{
    if (!IsOaksDraft() || choice >= 3 || gSaveBlock3Ptr->draft.round >= 6) return FALSE;
    u32 round = gSaveBlock3Ptr->draft.round;
    if (gPartiesCount[B_TRAINER_PLAYER] != round) return FALSE;
    struct Pokemon mon, rival;
    OaksDraftCreateCandidate(&mon, round * 3 + choice, 5);
    if(!GetMonData(&mon,MON_DATA_SPECIES)) return FALSE;
    u32 best = 0, picked = 3;
    for (u32 i = 0; i < 3; i++) if (i != choice)
    {
        OaksDraftCreateCandidate(&rival, round * 3 + i, 5);
        u32 score = CandidateScore(&rival, round);
        if (picked == 3 || score > best) { picked = i; best = score; }
    }
    gParties[B_TRAINER_PLAYER][round] = mon;
    gPartiesCount[B_TRAINER_PLAYER] = round + 1;
    gSaveBlock3Ptr->draft.picks[round] = choice;
    gSaveBlock3Ptr->draft.rival[round] = picked;
    gSaveBlock3Ptr->draft.round = round + 1;
    HandleSetPokedexFlagFromMon(&mon, FLAG_SET_SEEN);
    HandleSetPokedexFlagFromMon(&mon, FLAG_SET_CAUGHT);
    OaksDraftCreateCandidate(&rival, round * 3 + picked, 5);
    GetMonData(&rival, MON_DATA_NICKNAME, gStringVar1);
    StringCopy(gStringVar2, GetSpeciesName(GetMonData(&rival,MON_DATA_SPECIES)));
    return TRUE;
}
bool32 OaksDraftOpeningBattle(void)
{
    return IsOaksDraft() && !gSaveBlock3Ptr->draft.openingComplete;
}
static void Develop(struct Pokemon *mon, u8 level)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    for (u32 stage = 0; stage < 4; stage++)
    {
        const struct Evolution *evolutions = gSpeciesInfo[species].evolutions;
        enum Species target = SPECIES_NONE;
        if (evolutions == NULL) break;
        for (u32 i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            const struct Evolution *evo = &evolutions[i];
            if (evo->method == EVO_NONE || evo->method == EVO_SPLIT_FROM_EVO || evo->method == EVO_LEVEL_BATTLE_ONLY) continue;
            bool32 eligible = evo->method == EVO_LEVEL ? level >= evo->param : level >= (stage ? 36 : 25);
            if (eligible && evo->targetSpecies && (!target || GetSpeciesBaseStatTotal(evo->targetSpecies) > GetSpeciesBaseStatTotal(target))) target = evo->targetSpecies;
        }
        if (!target || target == species) break;
        species = target;
        SetMonData(mon, MON_DATA_SPECIES, &species);
    }
    SetMonData(mon, MON_DATA_LEVEL, &level);
    u32 exp = gExperienceTables[gSpeciesInfo[species].growthRate][level];
    SetMonData(mon, MON_DATA_EXP, &exp);
    GiveMonInitialMoveset(mon);
    EnsureAttack(mon);
    CalculateMonStats(mon);
    HealPokemon(mon);
}
bool32 OaksDraftCreateTrainerParty(struct Pokemon *party, const struct Trainer *trainer)
{
    if (!IsOaksDraft() || gSaveBlock3Ptr->draft.round != 6) return FALSE;
    bool32 oak = trainer->trainerClass == TRAINER_CLASS_CHAMPION_FRLG;
    if (!oak && trainer->trainerClass != TRAINER_CLASS_RIVAL_EARLY_FRLG && trainer->trainerClass != TRAINER_CLASS_RIVAL_LATE_FRLG) return FALSE;
    u8 level = 5;
    for (u32 i = 0; i < trainer->partySize; i++) level = max(level, trainer->party[i].lvl);
    u32 count = (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER) && trainer->multiTeamSize == MULTI_TEAM_SIZE_HALF ? 3 : 6;
    for (u32 i = 0; i < count; i++)
    {
        u32 pick = oak ? 3 - gSaveBlock3Ptr->draft.picks[i] - gSaveBlock3Ptr->draft.rival[i] : gSaveBlock3Ptr->draft.rival[i];
        OaksDraftCreateCandidate(&party[i], i * 3 + pick, 5);
        if (level > 5) Develop(&party[i], level);
        if (level >= 20)
        {
            enum Item item = level >= 50 ? ITEM_LEFTOVERS : ITEM_SITRUS_BERRY;
            SetMonData(&party[i], MON_DATA_HELD_ITEM, &item);
        }
    }
    return TRUE;
}
const struct Trainer *OaksDraftTrainerOverride(const struct Trainer *trainer)
{
    static struct Trainer oak;
    if (!IsOaksDraft() || trainer->trainerClass != TRAINER_CLASS_CHAMPION_FRLG) return trainer;
    oak = *trainer;
    StringCopy(oak.trainerName, COMPOUND_STRING("OAK"));
    oak.trainerPic = TRAINER_PIC_PROFESSOR_OAK_FRLG;
    return &oak;
}
void OaksDraftGetRound(void) { gSpecialVar_Result = IsOaksDraft() ? gSaveBlock3Ptr->draft.round : 255; }

bool32 OaksDraftGiftTrade(u32 slot, enum Species species, enum Item item)
{
    if (!IsOaksDraft() || gSaveBlock3Ptr->draft.round != 6 || slot >= gPartiesCount[B_TRAINER_PLAYER]
        || !GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HP) || species <= SPECIES_NONE || species >= NUM_SPECIES) return FALSE;
    u8 level = GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL);
    struct Pokemon replacement;
    u32 pid = IronmonMix(gSaveBlock3Ptr->worldSeed ^ species ^ ((u32)gSaveBlock1Ptr->location.mapGroup << 24)
        ^ ((u32)gSaveBlock1Ptr->location.mapNum << 16) ^ 0x47494654u);
    rng_value_t saved = gRngValue, saved2 = gRng2Value;
    SeedRng(pid); SeedRng2(pid);
    CreateMonWithIVs(&replacement,species,level,pid,OTID_STRUCT_PLAYER_ID,USE_RANDOM_IVS);
    GiveMonInitialMoveset(&replacement);
    if (IsMinimalGrindingMode()) ApplyMinimalGrindingModeToMon(&replacement);
    EnsureAttack(&replacement);
    OaksDraftSetNickname(&replacement,18 + pid % 14);
    SetMonData(&replacement, MON_DATA_HELD_ITEM, &item);
    gRngValue = saved; gRng2Value = saved2;
    // Preserve the player's item rather than deleting it with the old friend.
    enum Item oldItem = GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM);
    if (oldItem && !AddBagItem(oldItem,1)) return FALSE;
    gParties[B_TRAINER_PLAYER][slot] = replacement;
    VarSet(VAR_TEMP_TRANSFERRED_SPECIES,species);
    HandleSetPokedexFlagFromMon(&replacement,FLAG_SET_CAUGHT);
    HandleSetPokedexFlagFromMon(&replacement,FLAG_SET_SEEN);
    return TRUE;
}
bool32 OaksDraftRerollAbility(u32 slot)
{
    if (!IsOaksDraft() || slot >= gPartiesCount[B_TRAINER_PLAYER] || GetCoins() < 2000) return FALSE;
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][slot];
    if (!GetMonData(mon, MON_DATA_HP) || GetMonData(mon,MON_DATA_DRAFT_REROLLED)) return FALSE;
    enum Ability current = GetMonAbility(mon), ability = ABILITY_NONE;
    u32 rank = IronmonMix(gSaveBlock3Ptr->worldSeed ^ GetMonData(mon,MON_DATA_PERSONALITY) ^ 0x4142494Cu);
    for (u32 i = 0; i < ABILITIES_COUNT * 2; i++)
    {
        enum Ability next = 1 + (rank + i) % (ABILITIES_COUNT - 1);
        if (next != current) { ability = next; break; }
    }
    if (!ability) return FALSE;
    RemoveCoins(2000);
    SetMonData(mon,MON_DATA_CHAOS_STARTER_ABILITY,&ability);
    u32 used = TRUE;
    SetMonData(mon,MON_DATA_DRAFT_REROLLED,&used);
    StringCopy(gStringVar1,gAbilitiesInfo[ability].name);
    return TRUE;
}
void OaksDraftRecordTM(enum Item item)
{
    if (!IsOaksDraft() || item < ITEM_TM01 || item > ITEM_TM100) return;
    u32 index = item - ITEM_TM01;
    gSaveBlock3Ptr->draft.seenTms[index/8] |= 1 << (index%8);
}
bool32 OaksDraftTMReserved(enum Item item)
{
    if (!IsOaksDraft()) return FALSE;
    for (u32 i = 0; i < 8; i++) if (item && item == gSaveBlock3Ptr->draft.gymTms[i]) return TRUE;
    return FALSE;
}
void OaksDraftGiveGymTM(void)
{
    gSpecialVar_Result = FALSE;
    if (!IsOaksDraft() || gSpecialVar_0x8004 >= 8) return;
    u32 gym = gSpecialVar_0x8004;
    enum Item selected = gSaveBlock3Ptr->draft.gymTms[gym];
    if (!selected)
    {
        s32 best = -1;
        u32 seed = IronmonMix(gSaveBlock3Ptr->worldSeed ^ gym ^ 0x544D5355u);
        for (u32 j = 0; j < 100; j++)
        {
            enum Item item = ITEM_TM01 + (j + seed%100)%100;
            u32 index = item - ITEM_TM01;
            if (gSaveBlock3Ptr->draft.seenTms[index/8] & (1 << (index%8))) continue;
            enum Move move = GetItemTMHMMoveId(item);
            if (!move || move >= MOVES_COUNT) continue;
            u32 compatible = 0, unknown = 0;
            for (u32 i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
            {
                struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
                if (CanLearnTeachableMove(GetMonData(mon,MON_DATA_SPECIES),move)) { compatible++; if (!MonKnowsMove(mon,move)) unknown++; }
            }
            if (!compatible) continue;
            s32 score = compatible + unknown*4 + (GetMovePower(move) > 0 ? 2 : 0);
            if (score > best) { best=score; selected=item; }
        }
    }
    if (!selected) // All new compatible TMs exhausted: retain a legitimate compatible fallback.
        for (u32 item = ITEM_TM01; item <= ITEM_TM100 && !selected; item++)
            for (u32 i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
                if (CanLearnTeachableMove(GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_SPECIES),GetItemTMHMMoveId(item))) { selected=item; break; }
    if (!selected) return;
    sGymAward=TRUE;
    bool32 added=AddBagItem(selected,1);
    sGymAward=FALSE;
    if (!added) return;
    gSaveBlock3Ptr->draft.gymTms[gym] = selected;
    CopyItemName(selected,gStringVar1);
    StringCopy(gStringVar2,GetMoveName(GetItemTMHMMoveId(selected)));
    gSpecialVar_Result=TRUE;
}

void OaksDraftFinishOpening(void)
{
    if (IsOaksDraft()) gSaveBlock3Ptr->draft.openingComplete = TRUE;
}
void OaksDraftSetChampionGraphic(void)
{
    VarSet(VAR_OBJ_GFX_ID_0, IsOaksDraft() ? OBJ_EVENT_GFX_PROF_OAK : OBJ_EVENT_GFX_BLUE);
}
// Every non-Gym source resolves an awarded TM to a different, valid TM.
// The same mapping is used by field messages, shop lists and acquisition.
enum Item OaksDraftExternalItem(enum Item item)
{
    if (!OaksDraftTMReserved(item)) return item;
    u32 start = IronmonMix(gSaveBlock3Ptr->worldSeed ^ item) % 100;
    for (u32 i = 0; i < 100; i++) {
        enum Item replacement = ITEM_TM01 + (start + i) % 100;
        if (!OaksDraftTMReserved(replacement)) return replacement;
    }
    return item;
}

void OaksDraftPreviewGift(enum Species species, u8 *name, u8 *note)
{
    u32 pid=IronmonMix(gSaveBlock3Ptr->worldSeed ^ species ^ ((u32)gSaveBlock1Ptr->location.mapGroup<<24)
        ^ ((u32)gSaveBlock1Ptr->location.mapNum<<16) ^ 0x47494654u);
    u32 index=18+pid%14;
    StringCopy(name,sNames[(IronmonMix(gSaveBlock3Ptr->worldSeed)%ARRAY_COUNT(sNames)+index)%ARRAY_COUNT(sNames)]);
    StringCopy(note,sNotes[IronmonMix(pid^0x4E4F5445u)%ARRAY_COUNT(sNotes)]);
}
