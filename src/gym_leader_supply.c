#include "global.h"
#include "gym_leader.h"
#include "gym_leader_script.h"
#include "gym_leader_supply.h"
#include "event_data.h"
#include "item.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script.h"
#include "random.h"
#include "shop.h"
#include "constants/items.h"
#include "constants/maps.h"

struct GleSupply { u16 item; u8 rank, category; };
#define STOCK(item, rank, category) {ITEM_##item, rank, category}
static const struct GleSupply sStock[] = {
    STOCK(POTION,1,0), STOCK(ANTIDOTE,1,0), STOCK(PARALYZE_HEAL,1,0),
    STOCK(POKE_BALL,1,0), STOCK(REPEL,1,0), STOCK(EXP_SHARE,1,0),
    STOCK(GREAT_BALL,2,0), STOCK(SUPER_POTION,2,0), STOCK(SUPER_REPEL,2,0),
    STOCK(ULTRA_BALL,3,0), STOCK(HYPER_POTION,3,0), STOCK(FULL_HEAL,3,0), STOCK(REVIVE,3,0),
    STOCK(MAX_REPEL,4,0), STOCK(FULL_RESTORE,5,0),
    STOCK(FIRE_STONE,2,1), STOCK(WATER_STONE,2,1), STOCK(THUNDER_STONE,2,1),
    STOCK(LEAF_STONE,2,1), STOCK(MOON_STONE,2,1), STOCK(SUN_STONE,2,1), STOCK(ICE_STONE,2,1),
    STOCK(SHINY_STONE,3,1), STOCK(DUSK_STONE,3,1), STOCK(DAWN_STONE,3,1),
    STOCK(LINKING_CORD,3,1), STOCK(METAL_COAT,3,1), STOCK(KINGS_ROCK,3,1),
    STOCK(DRAGON_SCALE,3,1), STOCK(UPGRADE,3,1), STOCK(DUBIOUS_DISC,3,1),
    STOCK(RAZOR_CLAW,3,1), STOCK(RAZOR_FANG,3,1), STOCK(PROTECTOR,3,1),
    STOCK(REAPER_CLOTH,3,1), STOCK(PRISM_SCALE,3,1), STOCK(ELECTIRIZER,3,1),
    STOCK(MAGMARIZER,3,1), STOCK(SACHET,3,1), STOCK(WHIPPED_DREAM,3,1),
    STOCK(SWEET_APPLE,3,1), STOCK(TART_APPLE,3,1), STOCK(CRACKED_POT,3,1), STOCK(CHIPPED_POT,3,1),
    STOCK(GALARICA_CUFF,3,1), STOCK(GALARICA_WREATH,3,1), STOCK(BLACK_AUGURITE,3,1), STOCK(PEAT_BLOCK,3,1),
    STOCK(STRAWBERRY_SWEET,3,1), STOCK(LOVE_SWEET,3,1), STOCK(BERRY_SWEET,3,1), STOCK(CLOVER_SWEET,3,1),
    STOCK(FLOWER_SWEET,3,1), STOCK(STAR_SWEET,3,1), STOCK(RIBBON_SWEET,3,1),
    STOCK(AUSPICIOUS_ARMOR,4,1), STOCK(MALICIOUS_ARMOR,4,1), STOCK(LEADERS_CREST,4,1),
    STOCK(SYRUPY_APPLE,4,1), STOCK(UNREMARKABLE_TEACUP,4,1), STOCK(MASTERPIECE_TEACUP,4,1),
    STOCK(SOOTHE_BELL,2,2), STOCK(POWER_WEIGHT,3,2), STOCK(POWER_BRACER,3,2),
    STOCK(POWER_BELT,3,2), STOCK(POWER_LENS,3,2), STOCK(POWER_BAND,3,2), STOCK(POWER_ANKLET,3,2),
    STOCK(LEFTOVERS,4,2), STOCK(EVIOLITE,4,2), STOCK(FOCUS_SASH,4,2),
    STOCK(ABILITY_CAPSULE,4,2), STOCK(CHOICE_BAND,5,2), STOCK(CHOICE_SPECS,5,2),
    STOCK(CHOICE_SCARF,5,2), STOCK(HEAVY_DUTY_BOOTS,5,2), STOCK(ASSAULT_VEST,5,2),
    STOCK(ABILITY_PATCH,6,2)
};
static EWRAM_DATA u16 sSupplyItems[48] = {0};
static EWRAM_DATA u8 sSupplyCount = 0;
static bool32 AtSupplyDesk(void)
{
    return IsGymLeaderRun() && sSupplyCount
        && gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_GLE_LEAGUE_HALL)
        && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_GLE_LEAGUE_HALL);
}
u32 GymLeaderSupplyPrice(enum Item item)
{
    u32 price=GetItemPrice(item);
    if(AtSupplyDesk() && GymLeaderState()->honor>=100)price=price*9/10;
    return price;
}
void GleSupplyMenu(void)
{
    static const u8 *const names[]={COMPOUND_STRING("Travel supplies"),COMPOUND_STRING("Evolution items"),
        COMPOUND_STRING("Held / battle items"),COMPOUND_STRING("Mints"),COMPOUND_STRING("Memories"),COMPOUND_STRING("Back")};
    sSupplyCount=0;
    GymLeaderMenu(names,ARRAY_COUNT(names),FALSE);
}
void GleSupplyOpen(void)
{
    struct GleState *s=GymLeaderState();
    u32 category=gSpecialVar_Result,i;
    sSupplyCount=0;
    if(IsGymLeaderRun() && category<5)
    {
        for(i=0;i<ARRAY_COUNT(sStock);i++)
            if(sSupplyCount<ARRAY_COUNT(sSupplyItems)-1 && sStock[i].category==category && s->rank>=sStock[i].rank
                && GetItemPrice(sStock[i].item)>0 && !GetItemImportance(sStock[i].item))
                sSupplyItems[sSupplyCount++]=sStock[i].item;
        if(category==3 && s->rank>=4)
            for(i=ITEM_LONELY_MINT;i<=ITEM_SERIOUS_MINT;i++)sSupplyItems[sSupplyCount++]=i;
        if(category==4 && s->projectUnlocked[0])
            for(i=ITEM_FIRE_MEMORY;i<=ITEM_FAIRY_MEMORY;i++)sSupplyItems[sSupplyCount++]=i;
    }
    sSupplyItems[sSupplyCount]=ITEM_NONE;
    gSpecialVar_Result=sSupplyCount!=0;
    if(sSupplyCount)CreatePokemartMenu(sSupplyItems);
    else GymLeaderResumeScriptNextFrame();
}

// Bit 72 follows the 72 type-exclusive captures. It records delivery, not
// party ownership, so moving, releasing or trading cannot repeat the gift.
#define NULL_CAPTURE_BYTE 9
#define NULL_CAPTURE_MASK 1
static bool32 HasSilvally(void)
{
    u32 i,box;enum Species species;
    for(i=0;i<PARTY_SIZE;i++)
        if(!GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_IS_EGG))
        {
            species=GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_SPECIES);
            if(species && GetFormSpeciesId(species,0)==SPECIES_SILVALLY_NORMAL)return TRUE;
        }
    for(box=0;box<TOTAL_BOXES_COUNT;box++)for(i=0;i<IN_BOX_COUNT;i++)
    {
        struct BoxPokemon *mon=GetBoxedMonPtr(box,i);
        species=GetBoxMonData(mon,MON_DATA_SPECIES);
        if(species && !GetBoxMonData(mon,MON_DATA_IS_EGG)
            && GetFormSpeciesId(species,0)==SPECIES_SILVALLY_NORMAL)return TRUE;
    }
    return FALSE;
}
void GleNullStatus(void)
{
    struct GleState *s=GymLeaderState();
    gSpecialVar_Result=0;
    if(!IsGymLeaderRun())return;
    if(s->uniqueObtained[NULL_CAPTURE_BYTE]&NULL_CAPTURE_MASK)
    {
        s->projectUnlocked[0]=1;
        if(s->projectStage[0]<2 && HasSilvally())s->projectStage[0]=2;
        gSpecialVar_Result=s->projectStage[0]>=2?3:2;
    }
    else if(s->rank>=3)gSpecialVar_Result=1;
}
void GleNullClaim(void)
{
    struct GleState *s=GymLeaderState();struct Pokemon mon;
    u32 result;u8 level;
    gSpecialVar_Result=GLE_NOT_READY;
    if(!IsGymLeaderRun() || s->rank<3)return;
    if(s->uniqueObtained[NULL_CAPTURE_BYTE]&NULL_CAPTURE_MASK){gSpecialVar_Result=GLE_DUPLICATE;return;}
    if(IsPlayerPartyAndPokemonStorageFull()){gSpecialVar_Result=GLE_NO_ROOM;return;}
    level=GymLeaderLevelCap()-3;
    CreateMonWithIVs(&mon,SPECIES_TYPE_NULL,level,Random32(),OTID_STRUCT_PLAYER_ID,USE_RANDOM_IVS);
    GiveMonInitialMoveset(&mon);
    result=GiveScriptedMonToPlayer(&mon,PARTY_SIZE);
    if(result==MON_CANT_GIVE){gSpecialVar_Result=GLE_NO_ROOM;return;}
    s->uniqueObtained[NULL_CAPTURE_BYTE]|=NULL_CAPTURE_MASK;
    s->projectUnlocked[0]=1;s->projectStage[0]=1;
    gSpecialVar_Result=GLE_OK;
}

void GleSlakingResearch(void)
{
    struct GleState *s=GymLeaderState();u32 i,box;u8 ability=2;
    gSpecialVar_Result=0;
    if(!IsGymLeaderRun())return;
    if(s->projectStage[14]>=2){gSpecialVar_Result=2;return;}
    if(s->rank<5)return;
    for(i=0;i<PARTY_SIZE;i++)
        if(GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_SPECIES)==SPECIES_GLE_AWAKENED_SLAKING
            && !GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_IS_EGG))
        {
            SetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_ABILITY_NUM,&ability);
            goto completed;
        }
    for(box=0;box<TOTAL_BOXES_COUNT;box++)for(i=0;i<IN_BOX_COUNT;i++)
    {
        struct BoxPokemon *mon=GetBoxedMonPtr(box,i);
        if(GetBoxMonData(mon,MON_DATA_SPECIES)==SPECIES_GLE_AWAKENED_SLAKING
            && !GetBoxMonData(mon,MON_DATA_IS_EGG))
        {
            SetBoxMonData(mon,MON_DATA_ABILITY_NUM,&ability);
            goto completed;
        }
    }
    return;
completed:
    s->projectUnlocked[14]=1;s->projectStage[14]=2;
    gSpecialVar_Result=1;
}

// Reuse the engine's persisted per-Pokemon ability field. No global species
// record changes: unresearched Regigigas retain Slow Start in every mode.
void GleRegigigasResearch(void)
{
    struct GleState *s=GymLeaderState();u32 i,box;
    enum Ability ability=ABILITY_CLEAR_BODY;
    gSpecialVar_Result=0;
    if(!IsGymLeaderRun())return;
    if(s->projectStage[4]>=2){gSpecialVar_Result=2;return;}
    if(!(s->uniqueObtained[84/8]&(1u<<(84%8))) || s->projectStage[3]<5)return;
    for(i=0;i<PARTY_SIZE;i++)
        if(GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_SPECIES)==SPECIES_REGIGIGAS
            && !GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_IS_EGG))
        {
            SetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_CHAOS_STARTER_ABILITY,&ability);
            goto completed;
        }
    for(box=0;box<TOTAL_BOXES_COUNT;box++)for(i=0;i<IN_BOX_COUNT;i++)
    {
        struct BoxPokemon *mon=GetBoxedMonPtr(box,i);
        if(GetBoxMonData(mon,MON_DATA_SPECIES)==SPECIES_REGIGIGAS && !GetBoxMonData(mon,MON_DATA_IS_EGG))
        {
            SetBoxMonData(mon,MON_DATA_CHAOS_STARTER_ABILITY,&ability);
            goto completed;
        }
    }
    return;
completed:
    s->projectUnlocked[4]=1;s->projectStage[4]=2;gSpecialVar_Result=1;
}
