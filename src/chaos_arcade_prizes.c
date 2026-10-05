#include "global.h"
#include "coins.h"
#include "event_data.h"
#include "item.h"
#include "malloc.h"
#include "pokemon.h"
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
static const u8 *const sTiers[] = {COMPOUND_STRING("NORMAL"), COMPOUND_STRING("HIDDEN ABILITY"), COMPOUND_STRING("SHINY"), COMPOUND_STRING("SHINY + HA")};
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
    CreateMon(mon, sPrizeDraft->species, 20, 0, OTID_STRUCT_PLAYER_ID);
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
