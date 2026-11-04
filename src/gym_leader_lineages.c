#include "global.h"
#include "gym_leader.h"
#include "gym_leader_finale.h"
#include "gym_leader_script.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "string_util.h"
#include "constants/maps.h"

// Four one-time placements follow Shaymin's capture/discovery/seal bits.
static const u16 sLineages[]={SPECIES_COSMOG,SPECIES_POIPOLE,SPECIES_MELTAN,SPECIES_KUBFU};
static EWRAM_DATA u8 sLineage=0;
static bool32 AtDesk(void)
{
    return GymLeaderFinalOakComplete()
        && gSaveBlock1Ptr->location.mapGroup==MAP_GROUP(MAP_GLE_LEAGUE_HALL)
        && gSaveBlock1Ptr->location.mapNum==MAP_NUM(MAP_GLE_LEAGUE_HALL);
}
static bool32 Claimed(u32 slot)
{
    u32 bit=243+slot;return !!(GymLeaderState()->uniqueObtained[bit/8]&(1u<<(bit%8)));
}
void GleLineageMenu(void)
{
    static const u8 *const names[]={COMPOUND_STRING("COSMOG"),COMPOUND_STRING("POIPOLE"),COMPOUND_STRING("MELTAN"),COMPOUND_STRING("KUBFU"),COMPOUND_STRING("Back")};
    GymLeaderMenu(names,ARRAY_COUNT(names),FALSE);
}
void GleLineageInfo(void)
{
    u32 slot=gSpecialVar_Result;gSpecialVar_Result=2;
    if(slot>=ARRAY_COUNT(sLineages))return;
    sLineage=slot;StringCopy(gStringVar1,GetSpeciesName(sLineages[slot]));
    gSpecialVar_Result=Claimed(slot)?1:AtDesk()?0:2;
}
void GleLineageClaim(void)
{
    struct Pokemon mon;u8 ability=0;u32 delivered;gSpecialVar_Result=GLE_NOT_READY;
    if(!AtDesk() || sLineage>=ARRAY_COUNT(sLineages))return;
    if(Claimed(sLineage)){gSpecialVar_Result=GLE_DUPLICATE;return;}
    if(IsPlayerPartyAndPokemonStorageFull()){gSpecialVar_Result=GLE_NO_ROOM;return;}
    CreateMonWithIVs(&mon,sLineages[sLineage],5,Random32(),OTID_STRUCT_PLAYER_ID,USE_RANDOM_IVS);
    SetMonData(&mon,MON_DATA_ABILITY_NUM,&ability);GiveMonInitialMoveset(&mon);
    delivered=GiveScriptedMonToPlayer(&mon,PARTY_SIZE);
    if(delivered==MON_CANT_GIVE){gSpecialVar_Result=GLE_NO_ROOM;return;}
    GleUniqueCapture(GymLeaderState(),243+sLineage);gSpecialVar_Result=GLE_OK;
}
