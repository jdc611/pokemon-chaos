#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_screen_effect.h"
#include "main.h"
#include "malloc.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "pokemon_summary_screen.h"
#include "script.h"
#include "script_menu.h"
#include "string_util.h"
#include "item.h"
#include "mail.h"
#include "task.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/maps.h"
#include "constants/vars.h"

// One object template per real box slot. Normal viewport spawning limits the
// active sprites; a wide pasture keeps even a full box below the object budget.
static u8 RanchBox(void)
{
    u32 box = VarGet(VAR_CHAOS_RANCH_BOX);
    return box < TOTAL_BOXES_COUNT ? box : 0;
}

void ChaosRanchChooseBox(void)
{
    struct ListMenuItem *items = AllocZeroed(sizeof(*items) * TOTAL_BOXES_COUNT);
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        u8 *name = AllocZeroed(BOX_NAME_LENGTH + 1);
        StringCopy(name, GetBoxNamePtr(box));
        items[box].name = name;
        items[box].id = box;
    }
    ScriptMenu_MultichoiceDynamic(1, 1, TOTAL_BOXES_COUNT, items, FALSE, 6, RanchBox(), 0);
}

void ChaosRanchEnter(void)
{
    if (gSpecialVar_Result >= TOTAL_BOXES_COUNT)
        return;
    VarSet(VAR_CHAOS_RANCH_BOX, gSpecialVar_Result);
    s32 x = gSaveBlock1Ptr->pos.x, y = gSaveBlock1Ptr->pos.y;
    for (u32 i = 0; i < gMapHeader.events->warpCount; i++)
    {
        const struct WarpEvent *door = &gMapHeader.events->warps[i];
        if (door->mapGroup == MAP_GROUP(MAP_CHAOS_POKEMON_RANCH) && door->mapNum == MAP_NUM(MAP_CHAOS_POKEMON_RANCH))
        {
            x = door->x;
            y = door->y + 1;
            break;
        }
    }
    SetDynamicWarpWithCoords(0, gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum, -1, x, y);
}

void ChaosRanchChangeBox(void)
{
    if (gSpecialVar_Result < TOTAL_BOXES_COUNT)
        VarSet(VAR_CHAOS_RANCH_BOX, gSpecialVar_Result);
}

void ChaosRanchPopulate(void)
{
    struct Pokemon mon;
    FlagSet(FLAG_TEMP_1);
    for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
    {
        struct ObjectEventTemplate *obj = &gSaveBlock1Ptr->objectEventTemplates[slot];
        enum Species species = GetBoxMonDataAt(RanchBox(), slot, MON_DATA_SPECIES);
        obj->flagId = species == SPECIES_NONE ? FLAG_TEMP_1 : 0;
        if (species != SPECIES_NONE)
        {
            BoxMonAtToMon(RanchBox(), slot, &mon);
            bool32 egg = GetMonData(&mon, MON_DATA_IS_EGG);
            enum Species gfxSpecies = egg ? SPECIES_EGG : species;
            if (!egg && gSpeciesInfo[gfxSpecies].overworldData.images == NULL)
            {
                for (enum Species candidate = 1; candidate < NUM_SPECIES; candidate++)
                    if (gSpeciesInfo[candidate].natDexNum == gSpeciesInfo[species].natDexNum && gSpeciesInfo[candidate].overworldData.images != NULL)
                    {
                        gfxSpecies = candidate;
                        break;
                    }
            }
            obj->graphicsId = GetGraphicsIdForMon(gfxSpecies, IsMonShiny(&mon), GetMonGender(&mon) == MON_FEMALE);
            obj->movementType = egg ? MOVEMENT_TYPE_FACE_DOWN : MOVEMENT_TYPE_WANDER_AROUND;
        }
    }
}

void ChaosRanchMenu(void)
{
    static const u8 *const labels[] = {COMPOUND_STRING("Summary"), COMPOUND_STRING("Withdraw"), COMPOUND_STRING("Take Item"), COMPOUND_STRING("Cancel")};
    struct ListMenuItem *items = AllocZeroed(sizeof(*items) * ARRAY_COUNT(labels));
    for (u32 i = 0; i < ARRAY_COUNT(labels); i++)
    {
        u8 *name = AllocZeroed(16);
        StringCopy(name, labels[i]);
        items[i].name = name;
        items[i].id = i;
    }
    ScriptMenu_MultichoiceDynamic(18, 1, ARRAY_COUNT(labels), items, FALSE, 4, 0, 0);
}

static u32 RanchSlot(void)
{
    return gSpecialVar_LastTalked - 1;
}

static void Task_RanchSummary(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        ShowPokemonSummaryScreen(SUMMARY_MODE_BOX, gPokemonStoragePtr->boxes[RanchBox()], RanchSlot(), IN_BOX_COUNT - 1, CB2_ReturnToFieldContinueScript);
    }
}

void ChaosRanchSummary(void)
{
    if (RanchSlot() >= IN_BOX_COUNT || GetBoxMonDataAt(RanchBox(), RanchSlot(), MON_DATA_SPECIES) == SPECIES_NONE)
    {
        ScriptContext_Enable();
        return;
    }
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, 0);
    CreateTask(Task_RanchSummary, 10);
}

void ChaosRanchWithdraw(void)
{
    u32 slot = RanchSlot();
    gSpecialVar_Result = 1;
    if (slot >= IN_BOX_COUNT || Nuzlocke_IsGraveBox(RanchBox()))
    {
        gSpecialVar_Result = 2;
        return;
    }
    if (gPartiesCount[B_TRAINER_PLAYER] >= PARTY_SIZE || GetBoxMonDataAt(RanchBox(), slot, MON_DATA_SPECIES) == SPECIES_NONE)
        return;
    // Copy first, then clear only the selected original slot. No gift path,
    // randomization, healing, friendship adjustment or duplicate acquisition.
    BoxMonAtToMon(RanchBox(), slot, &gParties[B_TRAINER_PLAYER][gPartiesCount[B_TRAINER_PLAYER]]);
    gPartiesCount[B_TRAINER_PLAYER]++;
    ZeroBoxMonAt(RanchBox(), slot);
    gSaveBlock1Ptr->objectEventTemplates[slot].flagId = FLAG_TEMP_1;
    RemoveObjectEventByLocalIdAndMap(slot + 1, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    gSpecialVar_Result = 0;
}

void ChaosRanchTakeItem(void)
{
    u32 slot = RanchSlot();
    enum Item item;
    gSpecialVar_Result = 1;
    if (slot >= IN_BOX_COUNT || GetBoxMonDataAt(RanchBox(), slot, MON_DATA_SPECIES) == SPECIES_NONE)
        return;
    item = GetBoxMonDataAt(RanchBox(), slot, MON_DATA_HELD_ITEM);
    if (item == ITEM_NONE)
        return;
    if (Nuzlocke_IsGraveBox(RanchBox()) || ItemIsMail(item))
    {
        gSpecialVar_Result = 3;
        return;
    }
    if (!AddBagItem(item, 1))
    {
        gSpecialVar_Result = 2;
        return;
    }
    CopyItemName(item, gStringVar1);
    item = ITEM_NONE;
    SetBoxMonDataAt(RanchBox(), slot, MON_DATA_HELD_ITEM, &item);
    gSpecialVar_Result = 0;
}

void ChaosRanchReturn(void)
{
    SetWarpDestinationToDynamicWarp(0);
    DoWarp();
}
