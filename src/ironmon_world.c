#include "global.h"
#include "ironmon.h"
#include "battle.h"
#include "event_data.h"
#include "item.h"
#include "constants/items.h"
#include "overworld.h"
#include "constants/map_types.h"
#include "constants/maps.h"
#include "constants/trainers.h"
#include "constants/flags.h"

static bool32 GymCompleted(u32 section)
{
    switch (section)
    {
    case MAPSEC_PEWTER_CITY: return FlagGet(FLAG_DEFEATED_BROCK);
    case MAPSEC_CERULEAN_CITY: return FlagGet(FLAG_DEFEATED_MISTY);
    case MAPSEC_VERMILION_CITY: return FlagGet(FLAG_DEFEATED_LT_SURGE);
    case MAPSEC_CELADON_CITY: return FlagGet(FLAG_DEFEATED_ERIKA);
    case MAPSEC_FUCHSIA_CITY: return FlagGet(FLAG_DEFEATED_KOGA);
    case MAPSEC_SAFFRON_CITY: return FlagGet(FLAG_DEFEATED_SABRINA);
    case MAPSEC_CINNABAR_ISLAND: return FlagGet(FLAG_DEFEATED_BLAINE);
    case MAPSEC_VIRIDIAN_CITY: return FlagGet(FLAG_DEFEATED_LEADER_GIOVANNI);
    default: return TRUE;
    }
}

bool32 IronmonGymTrainersDefeated(u32 section)
{
#define CHECK_TRAINER(id) if (!FlagGet(TRAINER_FLAGS_START + (id))) return FALSE
    switch (section)
    {
    case MAPSEC_PEWTER_CITY:
        CHECK_TRAINER(TRAINER_CAMPER_LIAM); return TRUE;
    case MAPSEC_CERULEAN_CITY:
        CHECK_TRAINER(TRAINER_PICNICKER_DIANA); CHECK_TRAINER(TRAINER_SWIMMER_MALE_LUIS); return TRUE;
    case MAPSEC_VERMILION_CITY:
        CHECK_TRAINER(TRAINER_SAILOR_DWAYNE); CHECK_TRAINER(TRAINER_ENGINEER_BAILY); CHECK_TRAINER(TRAINER_GENTLEMAN_TUCKER); return TRUE;
    case MAPSEC_CELADON_CITY:
        CHECK_TRAINER(TRAINER_LASS_KAY); CHECK_TRAINER(TRAINER_LASS_LISA); CHECK_TRAINER(TRAINER_PICNICKER_TINA); CHECK_TRAINER(TRAINER_BEAUTY_BRIDGET); CHECK_TRAINER(TRAINER_BEAUTY_TAMIA); CHECK_TRAINER(TRAINER_BEAUTY_LORI); CHECK_TRAINER(TRAINER_COOLTRAINER_MARY); return TRUE;
    case MAPSEC_FUCHSIA_CITY:
        CHECK_TRAINER(TRAINER_TAMER_PHIL); CHECK_TRAINER(TRAINER_TAMER_EDGAR); CHECK_TRAINER(TRAINER_JUGGLER_KIRK); CHECK_TRAINER(TRAINER_JUGGLER_SHAWN); CHECK_TRAINER(TRAINER_JUGGLER_KAYDEN); CHECK_TRAINER(TRAINER_JUGGLER_NATE); return TRUE;
    case MAPSEC_SAFFRON_CITY:
        CHECK_TRAINER(TRAINER_PSYCHIC_JOHAN); CHECK_TRAINER(TRAINER_PSYCHIC_TYRON); CHECK_TRAINER(TRAINER_PSYCHIC_CAMERON); CHECK_TRAINER(TRAINER_PSYCHIC_PRESTON); CHECK_TRAINER(TRAINER_CHANNELER_AMANDA); CHECK_TRAINER(TRAINER_CHANNELER_STACY); CHECK_TRAINER(TRAINER_CHANNELER_TASHA); return TRUE;
    case MAPSEC_CINNABAR_ISLAND:
        CHECK_TRAINER(TRAINER_SUPER_NERD_ERIK); CHECK_TRAINER(TRAINER_SUPER_NERD_AVERY); CHECK_TRAINER(TRAINER_SUPER_NERD_DEREK); CHECK_TRAINER(TRAINER_SUPER_NERD_ZAC); CHECK_TRAINER(TRAINER_BURGLAR_QUINN); CHECK_TRAINER(TRAINER_BURGLAR_RAMON); CHECK_TRAINER(TRAINER_BURGLAR_DUSTY); return TRUE;
    case MAPSEC_VIRIDIAN_CITY:
        CHECK_TRAINER(TRAINER_TAMER_JASON); CHECK_TRAINER(TRAINER_TAMER_COLE); CHECK_TRAINER(TRAINER_BLACK_BELT_ATSUSHI); CHECK_TRAINER(TRAINER_BLACK_BELT_KIYO); CHECK_TRAINER(TRAINER_BLACK_BELT_TAKASHI); CHECK_TRAINER(TRAINER_COOLTRAINER_SAMUEL); CHECK_TRAINER(TRAINER_COOLTRAINER_YUJI); CHECK_TRAINER(TRAINER_COOLTRAINER_WARREN); return TRUE;
    default: return TRUE;
    }
#undef CHECK_TRAINER
}

bool32 IronmonLeaderLocked(u32 trainer)
{
    if (!IsIronmonRun()) return FALSE;
    switch (trainer)
    {
    case TRAINER_LEADER_BROCK: return !IronmonGymTrainersDefeated(MAPSEC_PEWTER_CITY);
    case TRAINER_LEADER_MISTY: return !IronmonGymTrainersDefeated(MAPSEC_CERULEAN_CITY);
    case TRAINER_LEADER_LT_SURGE: return !IronmonGymTrainersDefeated(MAPSEC_VERMILION_CITY);
    case TRAINER_LEADER_ERIKA: return !IronmonGymTrainersDefeated(MAPSEC_CELADON_CITY);
    case TRAINER_LEADER_KOGA: return !IronmonGymTrainersDefeated(MAPSEC_FUCHSIA_CITY);
    case TRAINER_LEADER_SABRINA: return !IronmonGymTrainersDefeated(MAPSEC_SAFFRON_CITY);
    case TRAINER_LEADER_BLAINE: return !IronmonGymTrainersDefeated(MAPSEC_CINNABAR_ISLAND);
    case TRAINER_LEADER_GIOVANNI: return !IronmonGymTrainersDefeated(MAPSEC_VIRIDIAN_CITY);
    default: return FALSE;
    }
}

static bool32 DungeonRecorded(u32 section)
{
    return section < 256 && (gSaveBlock3Ptr->challengeCaveCompleted[section >> 3] & (1 << (section & 7)));
}
static bool32 DungeonCompleted(u32 section)
{
    if (DungeonRecorded(section)) return TRUE;
    switch (section)
    {
    case MAPSEC_POKEMON_TOWER: return FlagGet(FLAG_RESCUED_MR_FUJI);
    case MAPSEC_ROCKET_HIDEOUT: return FlagGet(FLAG_HIDE_HIDEOUT_GIOVANNI);
    case MAPSEC_SILPH_CO: return FlagGet(FLAG_HIDE_SILPH_ROCKETS);
    case MAPSEC_POKEMON_MANSION: return CheckBagHasItem(ITEM_SECRET_KEY,1);
    default: return FALSE;
    }
}
static bool32 DungeonEligible(u32 section)
{
    switch (section)
    {
    case MAPSEC_MT_MOON: case MAPSEC_ROCK_TUNNEL:
    case MAPSEC_ROCKET_HIDEOUT: case MAPSEC_SILPH_CO: case MAPSEC_POKEMON_MANSION:
        return TRUE;
    // The first Tower visit precedes the Scope and cannot complete the ghost.
    case MAPSEC_POKEMON_TOWER: return CheckBagHasItem(ITEM_SILPH_SCOPE,1);
    case MAPSEC_KANTO_VICTORY_ROAD:
        return FlagGet(FLAG_BADGE08_GET) && CheckBagHasItem(ITEM_HM_STRENGTH,1);
    default: return FALSE;
    }
}
static bool32 ProgressionExit(const struct MapHeader *destination)
{
    u32 map = (gSaveBlock1Ptr->location.mapGroup << 8) | (u8)gSaveBlock1Ptr->location.mapNum;
    switch (gSaveBlock3Ptr->ironmon.commitmentSection)
    {
    case MAPSEC_MT_MOON: return map == MAP_MT_MOON_B1F && destination->regionMapSectionId == MAPSEC_ROUTE_4;
    case MAPSEC_ROCK_TUNNEL: return map == MAP_ROCK_TUNNEL_1F && gSaveBlock1Ptr->pos.y >= 36 && destination->regionMapSectionId == MAPSEC_ROUTE_10;
    case MAPSEC_KANTO_VICTORY_ROAD: return map == MAP_VICTORY_ROAD_2F && destination->regionMapSectionId == MAPSEC_ROUTE_23;
    default: return FALSE;
    }
}

void IronmonOnMapLoaded(void)
{
    if (!IsIronmonRun()) return;
    struct IronmonRunState *state = &gSaveBlock3Ptr->ironmon;
    if ((state->commitmentKind == 1 && GymCompleted(state->commitmentSection))
     || (state->commitmentKind == 2 && DungeonCompleted(state->commitmentSection)))
        state->commitmentKind = 0;
    if (gMapHeader.battleType == MAP_BATTLE_SCENE_GYM && !GymCompleted(gMapHeader.regionMapSectionId))
    {
        state->commitmentKind = 1;
        state->commitmentSection = gMapHeader.regionMapSectionId;
    }
    else if (!state->commitmentKind && DungeonEligible(gMapHeader.regionMapSectionId)
          && !DungeonCompleted(gMapHeader.regionMapSectionId))
    {
        state->commitmentKind = 2;
        state->commitmentSection = gMapHeader.regionMapSectionId;
    }
}

bool32 IronmonEscapeLocked(void)
{
    if (!IsIronmonRun()) return FALSE;
    IronmonOnMapLoaded();
    return gSaveBlock3Ptr->ironmon.commitmentKind != 0;
}

bool32 IronmonWarpAllowed(const struct MapHeader *destination)
{
    if (!IronmonEscapeLocked()) return TRUE;
    if (gSaveBlock3Ptr->ironmon.commitmentKind == 1)
        return destination->battleType == MAP_BATTLE_SCENE_GYM
            && destination->regionMapSectionId == gSaveBlock3Ptr->ironmon.commitmentSection;
    return destination->regionMapSectionId == gSaveBlock3Ptr->ironmon.commitmentSection
        || ProgressionExit(destination);
}

void IronmonOnMapTransition(const struct MapHeader *destination)
{
    if (!IsIronmonRun() || gSaveBlock3Ptr->ironmon.commitmentKind != 2) return;
    u32 section = gSaveBlock3Ptr->ironmon.commitmentSection;
    if (destination->regionMapSectionId != section && (DungeonCompleted(section) || ProgressionExit(destination)))
    {
        if (section < 256) gSaveBlock3Ptr->challengeCaveCompleted[section >> 3] |= 1 << (section & 7);
        gSaveBlock3Ptr->ironmon.commitmentKind = 0;
    }
}


static EWRAM_DATA enum Item sGymRewardPermit;
static EWRAM_DATA u16 sGymRewardSection;
void IronmonPrepareGymTM(void)
{
    sGymRewardPermit = ITEM_NONE;
    if (!IsIronmonRun() || gMapHeader.battleType != MAP_BATTLE_SCENE_GYM
     || !GymCompleted(gMapHeader.regionMapSectionId)) return;
    u32 count = 0;
    for (u32 item = ITEM_TM01; item <= ITEM_TM100; item++)
        if (IronmonMoveAllowed(GetItemTMHMMoveId(item))) count++;
    if (!count) return;
    u32 rank = IronmonMix(gSaveBlock3Ptr->worldSeed ^ gMapHeader.regionMapSectionId ^ 0x544D4759u) % count;
    for (u32 item = ITEM_TM01; item <= ITEM_TM100; item++)
        if (IronmonMoveAllowed(GetItemTMHMMoveId(item)) && rank-- == 0)
        {
            sGymRewardPermit = item;
            sGymRewardSection = gMapHeader.regionMapSectionId;
            gSpecialVar_0x8000 = item;
            return;
        }
}
bool32 IronmonPermitItemAward(enum Item item, u32 count)
{
    if (!IsIronmonRun() || item < ITEM_TM01 || item > ITEM_TM100) return TRUE;
    if (item != sGymRewardPermit || count != 1 || sGymRewardSection != gMapHeader.regionMapSectionId
     || gMapHeader.battleType != MAP_BATTLE_SCENE_GYM || !GymCompleted(sGymRewardSection)) return FALSE;
    sGymRewardPermit = ITEM_NONE;
    return TRUE;
}
