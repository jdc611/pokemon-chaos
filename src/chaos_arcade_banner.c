#include "global.h"
#include "chaos_arcade.h"
#include "coins.h"
#include "menu.h"
#include "palette.h"
#include "script.h"
#include "sprite.h"
#include "string_util.h"
#include "text.h"
#include "window.h"
#include "constants/maps.h"
#include "constants/rgb.h"

// An OBJ banner stays fixed when BG0 scrolls and never shares dialogue tiles.
#define BANNER_TAG 0xFCEA
static const u16 sBannerPalette[16] = {RGB(0,0,0), RGB(31,31,29), RGB(3,6,13), RGB(31,24,7)};
static const struct OamData sBannerOam = {.shape = SPRITE_SHAPE(64x32), .size = SPRITE_SIZE(64x32), .priority = 0};
static const struct SpriteTemplate sBannerTemplate = { .tileTag = BANNER_TAG, .paletteTag = BANNER_TAG, .oam = &sBannerOam, .anims = gDummySpriteAnimTable, .images = NULL, .affineAnims = gDummySpriteAffineAnimTable, .callback = SpriteCallbackDummy };

static bool32 IsArcadeMap(void)
{
    u16 map = (gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum;
    return map == MAP_CELADON_CITY_GAME_CORNER || map == MAP_CELADON_CITY_GAME_CORNER_PRIZE_ROOM;
}

static bool32 RenderBanner(void)
{
    const struct WindowTemplate window = {.bg = 0, .tilemapLeft = 0, .tilemapTop = 0, .width = 8, .height = 4, .paletteNum = 0, .baseBlock = 0};
    u32 id = AddWindow(&window);
    if (id == WINDOW_NONE) return FALSE;
    FillWindowPixelBuffer(id, PIXEL_FILL(0));
    FillWindowPixelRect(id, 3, 2, 2, 60, 28);
    FillWindowPixelRect(id, 3, 0, 4, 64, 24);
    FillWindowPixelRect(id, 2, 2, 4, 60, 24);
    static const u8 colors[] = {2, 1, 2};
    u8 digits[5];
    ConvertIntToDecimalStringN(digits, GetCoins(), STR_CONV_MODE_RIGHT_ALIGN, 4);
    AddTextPrinterParameterized4(id, FONT_SMALL, 19, 3, 0, 0, colors, TEXT_SKIP_DRAW, COMPOUND_STRING("COINS"));
    AddTextPrinterParameterized4(id, FONT_NORMAL, 20, 14, 0, 0, colors, TEXT_SKIP_DRAW, digits);
    const struct SpriteSheet sheet = {(const void *)GetWindowAttribute(id, WINDOW_TILE_DATA), 1024, BANNER_TAG};
    u16 tile = GetSpriteTileStartByTag(BANNER_TAG);
    if (tile == TAG_NONE)
    {
        LoadSpriteSheet(&sheet);
        tile = GetSpriteTileStartByTag(BANNER_TAG);
    }
    else
        CpuCopy16(sheet.data, (u8 *)OBJ_VRAM0 + tile * TILE_SIZE_4BPP, sheet.size);
    RemoveWindow(id); // No tilemap/VRAM copy: this window was only a CPU canvas.
    return tile != TAG_NONE;
}

void ChaosArcadeUpdateCoinBanner(void)
{
    u32 sprite = MAX_SPRITES;
    for (u32 i = 0; i < MAX_SPRITES; i++)
        if (gSprites[i].inUse && gSprites[i].template == &sBannerTemplate) { sprite = i; break; }
    if (!IsArcadeMap())
    {
        if (sprite < MAX_SPRITES)
        {
            DestroySprite(&gSprites[sprite]);
            FreeSpriteTilesByTag(BANNER_TAG);
            FreeSpritePaletteByTag(BANNER_TAG);
        }
        return;
    }
    bool32 hide = ArePlayerFieldControlsLocked() || gPaletteFade.active;
    if (sprite == MAX_SPRITES)
    {
        if (hide || !RenderBanner()) return;
        const struct SpritePalette palette = {sBannerPalette, BANNER_TAG};
        if (LoadSpritePalette(&palette) == 0xFF)
        {
            FreeSpriteTilesByTag(BANNER_TAG);
            return;
        }
        sprite = CreateSprite(&sBannerTemplate, 200, 16, 0);
        if (sprite == MAX_SPRITES)
        {
            FreeSpriteTilesByTag(BANNER_TAG);
            FreeSpritePaletteByTag(BANNER_TAG);
            return;
        }
        gSprites[sprite].data[0] = GetCoins();
    }
    gSprites[sprite].invisible = hide;
    if (!hide && gSprites[sprite].data[0] != GetCoins() && RenderBanner())
        gSprites[sprite].data[0] = GetCoins();
}
