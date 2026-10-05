#include "global.h"
#include "coins.h"
#include "event_data.h"
#include "item.h"
#include "malloc.h"
#include "pokemon.h"
#include "random.h"
#include "move.h"
#include "battle_main.h"
#include "party_menu.h"
#include "text.h"
#include "script.h"
#include "script_menu.h"
#include "string_util.h"
#include "task.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/moves.h"

struct ArcadePrize {enum Species species; u16 price;};
static const struct ArcadePrize sPrizes[] = {
    {SPECIES_PORYGON, 1500}, {SPECIES_ROTOM, 2000}, {SPECIES_DRATINI, 3500},
    {SPECIES_ZORUA, 1500}, {SPECIES_LARVESTA, 3000}, {SPECIES_JANGMO_O, 3500},
    {SPECIES_TOXEL, 1500}, {SPECIES_BAGON, 3500}, {SPECIES_BELDUM, 4000}, {SPECIES_DREEPY, 4500},
};
static const enum Species sRotomForms[] = {SPECIES_ROTOM, SPECIES_ROTOM_HEAT, SPECIES_ROTOM_WASH, SPECIES_ROTOM_FROST, SPECIES_ROTOM_FAN, SPECIES_ROTOM_MOW};
static const u8 *const sRotomNames[] = {COMPOUND_STRING("ROTOM"), COMPOUND_STRING("ROTOM (HEAT)"), COMPOUND_STRING("ROTOM (WASH)"), COMPOUND_STRING("ROTOM (FROST)"), COMPOUND_STRING("ROTOM (FAN)"), COMPOUND_STRING("ROTOM (MOW)")};
static const enum Move sRotomMoves[] = {MOVE_NONE, MOVE_OVERHEAT, MOVE_HYDRO_PUMP, MOVE_BLIZZARD, MOVE_AIR_SLASH, MOVE_LEAF_STORM};
static const u16 sSurcharges[] = {0, 1000, 3000, 4000};
static const u8 *const sTiers[] = {COMPOUND_STRING("NORMAL ABILITY"), COMPOUND_STRING("HIDDEN ABILITY"), COMPOUND_STRING("SHINY"), COMPOUND_STRING("SHINY + HA")};
struct PrizeDraft {struct Pokemon mon; enum Species species; u16 price; u8 prize, form; bool8 prepared;};
static EWRAM_DATA struct PrizeDraft *sPrizeDraft = NULL;

void ChaosArcadePrizeCancel(void)
{
    if (sPrizeDraft != NULL) Free(sPrizeDraft);
    sPrizeDraft = NULL;
}

// A failed waitstate special resumes on the next frame, after the script stops.
static void MenuFailed(u8 task)
{
    gSpecialVar_Result = 127;
    ScriptContext_Enable();
    DestroyTask(task);
}
static void FreeItems(struct ListMenuItem *items, u32 count)
{
    if (items == NULL) return;
    for (u32 i = 0; i < count; i++) if (items[i].name != NULL) Free((void *)items[i].name);
    Free(items);
}
static struct ListMenuItem *NewItems(u32 count)
{
    struct ListMenuItem *items = AllocZeroed(count * sizeof(*items));
    if (items == NULL) return NULL;
    for (u32 i = 0; i < count; i++)
    {
        items[i].name = AllocZeroed(48);
        items[i].id = i;
        if (items[i].name == NULL) {FreeItems(items, count); return NULL;}
    }
    return items;
}
static void ShowItems(struct ListMenuItem *items, u32 count)
{
    if (items == NULL || !ScriptMenu_MultichoiceDynamic(1, 4, count, items, FALSE, 6, 0, 0))
        CreateTask(MenuFailed, 0);
}
static void PriceLabel(u8 *dest, const u8 *name, u16 price)
{
    dest = StringCopy(dest, name);
    dest = StringCopy(dest, COMPOUND_STRING("  "));
    ConvertIntToDecimalStringN(dest, price, STR_CONV_MODE_LEFT_ALIGN, 4);
}
static bool32 HasHidden(enum Species species)
{
    enum Ability ha = gSpeciesInfo[species].abilities[2];
    return ha != ABILITY_NONE && ha != gSpeciesInfo[species].abilities[0] && ha != gSpeciesInfo[species].abilities[1];
}

void ChaosArcadePrizeMenu(void)
{
    ChaosArcadePrizeCancel();
    sPrizeDraft = AllocZeroed(sizeof(*sPrizeDraft));
    if (sPrizeDraft == NULL) {ShowItems(NULL, 0); return;}
    sPrizeDraft->prize = ARRAY_COUNT(sPrizes);
    struct ListMenuItem *items = NewItems(ARRAY_COUNT(sPrizes));
    if (items != NULL)
        for (u32 i = 0; i < ARRAY_COUNT(sPrizes); i++)
            PriceLabel((u8 *)items[i].name, gSpeciesInfo[sPrizes[i].species].speciesName, sPrizes[i].price);
    ShowItems(items, ARRAY_COUNT(sPrizes));
}
void ChaosArcadePrizePick(void)
{
    u32 pick = gSpecialVar_Result;
    gSpecialVar_Result = 2;
    if (sPrizeDraft == NULL || pick >= ARRAY_COUNT(sPrizes)) return;
    sPrizeDraft->prize = pick;
    sPrizeDraft->species = sPrizes[pick].species;
    sPrizeDraft->prepared = FALSE;
    gSpecialVar_Result = sPrizeDraft->species == SPECIES_ROTOM;
}
void ChaosArcadeRotomMenu(void)
{
    struct ListMenuItem *items = NewItems(ARRAY_COUNT(sRotomForms));
    u32 count = 0;
    if (items != NULL)
    {
        for (u32 i = 0; i < ARRAY_COUNT(sRotomForms); i++)
            if (IsSpeciesEnabled(sRotomForms[i]))
            {
                StringCopy((u8 *)items[count].name, sRotomNames[i]);
                items[count++].id = i;
            }
        for (u32 i = count; i < ARRAY_COUNT(sRotomForms); i++) Free((void *)items[i].name);
    }
    ShowItems(items, count);
}
void ChaosArcadeRotomPick(void)
{
    u32 form = gSpecialVar_Result;
    gSpecialVar_Result = 2;
    if (sPrizeDraft == NULL || sPrizeDraft->species != SPECIES_ROTOM || form >= ARRAY_COUNT(sRotomForms) || !IsSpeciesEnabled(sRotomForms[form])) return;
    sPrizeDraft->species = sRotomForms[form];
    sPrizeDraft->form = form;
    gSpecialVar_Result = 0;
}
void ChaosArcadePrizeHasHidden(void)
{
    gSpecialVar_Result = sPrizeDraft != NULL && HasHidden(sPrizeDraft->species);
}

void ChaosArcadeTierMenu(void)
{
    if (sPrizeDraft == NULL || sPrizeDraft->prize >= ARRAY_COUNT(sPrizes)) {ShowItems(NULL, 0); return;}
    bool32 hidden = HasHidden(sPrizeDraft->species);
    u32 count = hidden ? 4 : 2;
    struct ListMenuItem *items = NewItems(count);
    if (items != NULL)
        for (u32 i = 0; i < count; i++)
        {
            u32 tier = hidden ? i : i * 2;
            items[i].id = tier;
            PriceLabel((u8 *)items[i].name, sTiers[tier], sPrizes[sPrizeDraft->prize].price + sSurcharges[tier]);
        }
    ShowItems(items, count);
}
void ChaosArcadePrizePrepare(void)
{
    u32 tier = gSpecialVar_Result;
    gSpecialVar_Result = 2;
    if (sPrizeDraft == NULL || sPrizeDraft->prize >= ARRAY_COUNT(sPrizes) || tier >= ARRAY_COUNT(sTiers) || ((tier & 1) && !HasHidden(sPrizeDraft->species))) return;
    struct Pokemon *mon = &sPrizeDraft->mon;
    CreateMonWithIVs(mon, sPrizeDraft->species, 20, Random32(), OTID_STRUCT_PLAYER_ID, USE_RANDOM_IVS);
    GiveMonInitialMoveset(mon);
    if (tier & 1)
    {
        u8 slot = 2;
        u16 marker = CHAOS_ABILITY_NATIVE_HIDDEN;
        SetMonData(mon, MON_DATA_ABILITY_NUM, &slot);
        SetMonData(mon, MON_DATA_CHAOS_STARTER_ABILITY, &marker);
    }
    else
        TrySetMonAbilityToActiveRunFilter(mon);
    if (tier & 2)
    {
        bool32 shiny = TRUE;
        SetMonData(mon, MON_DATA_IS_SHINY, &shiny);
    }
    if (sPrizeDraft->form != 0)
        SetMonMoveSlot(mon, sRotomMoves[sPrizeDraft->form], 0);
    HealPokemon(mon);
    sPrizeDraft->price = sPrizes[sPrizeDraft->prize].price + sSurcharges[tier];
    sPrizeDraft->prepared = TRUE;
    StringCopy(gStringVar1, sPrizes[sPrizeDraft->prize].species == SPECIES_ROTOM ? sRotomNames[sPrizeDraft->form] : gSpeciesInfo[sPrizeDraft->species].speciesName);
    StringCopy(gStringVar2, gAbilitiesInfo[GetMonAbility(mon)].name);
    u8 *text = StringCopy(gStringVar3, sTiers[tier]);
    text = StringCopy(text, COMPOUND_STRING(": "));
    text = ConvertIntToDecimalStringN(text, sPrizeDraft->price, STR_CONV_MODE_LEFT_ALIGN, 4);
    StringCopy(text, COMPOUND_STRING(" COINS"));
    gSpecialVar_Result = DoesMonMatchActiveRunFilter(mon) ? 0 : 1;
}
void ChaosArcadePrizeBuy(void)
{
    gSpecialVar_Result = 4;
    if (sPrizeDraft == NULL || !sPrizeDraft->prepared || !FlagGet(FLAG_BADGE04_GET) || !CheckBagHasItem(ITEM_COIN_CASE, 1)) goto done;
    if (GetCoins() < sPrizeDraft->price) {gSpecialVar_Result = 2; goto done;}
    u32 delivered = GiveScriptedMonToPlayer(&sPrizeDraft->mon, PARTY_SIZE);
    if (delivered == MON_CANT_GIVE) {gSpecialVar_Result = 3; goto done;}
    RemoveCoins(sPrizeDraft->price);
    gSpecialVar_Result = delivered;
 done:
    ChaosArcadePrizeCancel();
}

// Reusable TM unlocks and battle equipment use ordinary bag delivery.
struct ArcadeItemPrize {enum Item item; u16 price; bool8 support;};
static const struct ArcadeItemPrize sTmPrizes[] = {
    {ITEM_TM_REFLECT,1000,TRUE}, {ITEM_TM_LIGHT_SCREEN,1000,TRUE}, {ITEM_TM_SAFEGUARD,1000,TRUE},
    {ITEM_TM_PROTECT,1000,TRUE}, {ITEM_TM_TAUNT,1200,TRUE}, {ITEM_TM_SUBSTITUTE,1200,TRUE},
    {ITEM_TM_THUNDER_WAVE,1200,TRUE}, {ITEM_TM_DEFOG,1200,TRUE}, {ITEM_TM_WILL_O_WISP,1500,TRUE},
    {ITEM_TM_ROOST,1800,TRUE}, {ITEM_TM_TRICK_ROOM,2000,TRUE}, {ITEM_TM_TAILWIND,2000,TRUE}, {ITEM_TM_ENCORE,2000,TRUE},
    {ITEM_TM_AERIAL_ACE,1000,FALSE}, {ITEM_TM_BRICK_BREAK,1200,FALSE}, {ITEM_TM_THUNDERBOLT,1500,FALSE},
    {ITEM_TM_ICE_BEAM,1500,FALSE}, {ITEM_TM_FLAMETHROWER,1500,FALSE}, {ITEM_TM_SHADOW_BALL,1500,FALSE},
    {ITEM_TM_PSYCHIC,1500,FALSE}, {ITEM_TM_SLUDGE_BOMB,1500,FALSE}, {ITEM_TM_U_TURN,1500,FALSE},
    {ITEM_TM_VOLT_SWITCH,1500,FALSE}, {ITEM_TM_ENERGY_BALL,1800,FALSE}, {ITEM_TM_FLASH_CANNON,1800,FALSE},
    {ITEM_TM_DARK_PULSE,1800,FALSE}, {ITEM_TM_DRAGON_PULSE,1800,FALSE}, {ITEM_TM_EARTH_POWER,2200,FALSE},
    {ITEM_TM_MOONBLAST,2200,FALSE}, {ITEM_TM_AURA_SPHERE,2200,FALSE},
};
static const struct ArcadeItemPrize sEquipment[] = {
    {ITEM_AIR_BALLOON,250,FALSE}, {ITEM_WHITE_HERB,250,FALSE}, {ITEM_POWER_HERB,350,FALSE},
    {ITEM_FOCUS_SASH,500,FALSE}, {ITEM_CHOICE_BAND,1500,FALSE}, {ITEM_CHOICE_SPECS,1500,FALSE},
    {ITEM_CHOICE_SCARF,1500,FALSE}, {ITEM_LIFE_ORB,1800,FALSE}, {ITEM_ASSAULT_VEST,1800,FALSE},
};
static EWRAM_DATA u16 sItemChoice = 0;
static EWRAM_DATA u8 sItemCategory = 0;

void ChaosArcadeItemCategories(void)
{
    static const u8 *const names[] = {COMPOUND_STRING("SUPPORT TMs"),COMPOUND_STRING("ATTACK TMs"),COMPOUND_STRING("BATTLE ITEMS")};
    struct ListMenuItem *items = NewItems(ARRAY_COUNT(names));
    if (items != NULL) for (u32 i=0;i<ARRAY_COUNT(names);i++) StringCopy((u8 *)items[i].name,names[i]);
    sItemChoice = 0xFFFF;
    sItemCategory = 0xFF;
    ShowItems(items,ARRAY_COUNT(names));
}
void ChaosArcadeItemMenu(void)
{
    sItemCategory = gSpecialVar_Result;
    sItemChoice = 0xFFFF;
    if (sItemCategory > 2) {ShowItems(NULL,0);return;}
    u32 total = sItemCategory == 2 ? ARRAY_COUNT(sEquipment) : ARRAY_COUNT(sTmPrizes);
    const struct ArcadeItemPrize *prizes = sItemCategory == 2 ? sEquipment : sTmPrizes;
    struct ListMenuItem *items = NewItems(total);
    u32 count = 0;
    if (items != NULL)
    {
        for (u32 i=0;i<total;i++)
        {
            if (sItemCategory < 2 && prizes[i].support != (sItemCategory == 0)) continue;
            const u8 *name = sItemCategory == 2 ? GetItemName(prizes[i].item) : GetMoveName(ItemIdToBattleMoveId(prizes[i].item));
            u8 *label = (u8 *)items[count].name;
            if (sItemCategory < 2 && CheckBagHasItem(prizes[i].item,1))
                StringCopy(StringCopy(label,name),COMPOUND_STRING(" (OWNED)"));
            else PriceLabel(label,name,prizes[i].price);
            items[count++].id = i;
        }
        for (u32 i=count;i<total;i++) Free((void *)items[i].name);
    }
    ShowItems(items,count);
}
static const struct ArcadeItemPrize *ChosenItem(void)
{
    if (sItemCategory == 2 && sItemChoice < ARRAY_COUNT(sEquipment)) return &sEquipment[sItemChoice];
    if (sItemCategory < 2 && sItemChoice < ARRAY_COUNT(sTmPrizes)) return &sTmPrizes[sItemChoice];
    return NULL;
}
static u8 *DetailNumber(u8 *text,u32 value)
{
    return value ? ConvertIntToDecimalStringN(text,value,STR_CONV_MODE_LEFT_ALIGN,3) : StringCopy(text,COMPOUND_STRING("--"));
}
void ChaosArcadeItemPreview(void)
{
    sItemChoice = gSpecialVar_Result;
    const struct ArcadeItemPrize *prize = ChosenItem();
    gSpecialVar_Result = 2;
    if (prize == NULL) return;
    const u8 *description;
    u8 *text;
    if (sItemCategory == 2)
    {
        StringCopy(gStringVar1,GetItemName(prize->item));
        description = GetItemDescription(prize->item);
        text = StringCopy(gStringVar4,gStringVar1);
        text = StringCopy(text,COMPOUND_STRING("\p"));
    }
    else
    {
        static const u8 *const categories[] = {
            [DAMAGE_CATEGORY_NONE] = COMPOUND_STRING("--"),
            [DAMAGE_CATEGORY_PHYSICAL] = COMPOUND_STRING("PHYSICAL"),
            [DAMAGE_CATEGORY_SPECIAL] = COMPOUND_STRING("SPECIAL"),
            [DAMAGE_CATEGORY_STATUS] = COMPOUND_STRING("STATUS"),
        };
        enum Move move = ItemIdToBattleMoveId(prize->item);
        StringCopy(gStringVar1,GetMoveName(move));
        text = StringCopy(gStringVar4,gStringVar1);
        text = StringCopy(text,COMPOUND_STRING("\n"));
        text = StringCopy(text,gTypesInfo[GetMoveType(move)].name);
        text = StringCopy(text,COMPOUND_STRING(" / "));
        text = StringCopy(text,categories[GetMoveCategory(move)]);
        text = StringCopy(text,COMPOUND_STRING("\pPWR: "));
        text = DetailNumber(text,GetMovePower(move));
        text = StringCopy(text,COMPOUND_STRING("  ACC: "));
        text = DetailNumber(text,GetMoveAccuracy(move));
        text = StringCopy(text,COMPOUND_STRING("\nPP: "));
        text = DetailNumber(text,GetMovePP(move));
        text = StringCopy(text,COMPOUND_STRING("  Reusable TM\p"));
        description = GetMoveDescription(move);
    }
    u8 *end = StringCopy(text,description);
    WrapFontIdToFit(text,end,FONT_NORMAL,208);
    ConvertIntToDecimalStringN(gStringVar2,prize->price,STR_CONV_MODE_LEFT_ALIGN,4);
    gSpecialVar_Result = 0;
}
void ChaosArcadeItemBuy(void)
{
    const struct ArcadeItemPrize *prize = ChosenItem();
    gSpecialVar_Result = 4;
    if (prize == NULL || !FlagGet(FLAG_BADGE04_GET) || !CheckBagHasItem(ITEM_COIN_CASE,1)) return;
    if (sItemCategory < 2 && CheckBagHasItem(prize->item,1)) {gSpecialVar_Result=5;return;}
    if (GetCoins() < prize->price) {gSpecialVar_Result=2;return;}
    if (!AddBagItem(prize->item,1)) {gSpecialVar_Result=3;return;}
    RemoveCoins(prize->price);
    gSpecialVar_Result=0;
    sItemChoice=0xFFFF;
}
