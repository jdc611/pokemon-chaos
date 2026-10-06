#include "global.h"
#include "chaos_arcade.h"
#include "palette.h"
#include "constants/maps.h"
#include "constants/rgb.h"

// Cosmetic transformations only touch background colors, never object palettes,
// map behavior, collision, encounter tables or any Pokemon data.
void ChaosArcadeRanchPalette(u16 start,u16 count,bool8 skipFaded)
{
    if(gSaveBlock1Ptr->location.mapGroup!=MAP_GROUP(MAP_CHAOS_POKEMON_RANCH)||gSaveBlock1Ptr->location.mapNum!=MAP_NUM(MAP_CHAOS_POKEMON_RANCH))return;
    ChaosArcadeEnsureSave();u32 theme=gSaveBlock3Ptr->arcadeRanchTheme;
    if(theme!=4)return;
    for(u32 i=start;i<start+count;i++){
        u16 color=gPlttBufferUnfaded[i];u32 r=color&31,g=(color>>5)&31,b=(color>>10)&31;
        if(theme==4){r=r*3/5;g=g*3/5;b=min(31,b*3/5+4);}
        else if(g>r+2&&g>b+2){
            if(theme==1){r=r*2/3;g=min(31,g+2);b=b*2/3;}
            if(theme==2){r=min(31,12+g*3/5);b=5+g/3;g=min(31,10+g/2);}
            if(theme==3){r=18+g/3;b=20+g/3;g=19+g/3;}
        }
        color=r|(g<<5)|(b<<10);gPlttBufferUnfaded[i]=color;if(!skipFaded)gPlttBufferFaded[i]=color;
    }
}
void ChaosArcadeRiderPalette(void)
{
    ChaosArcadeEnsureSave();u32 theme=gSaveBlock3Ptr->arcadeRiderTheme;
    if(theme==0||theme>2)return;
    for(u32 i=17;i<32;i++){
        u32 color=gPlttBufferUnfaded[i],r=color&31,g=(color>>5)&31,b=(color>>10)&31;
        u32 light=(r+g+b)/3;
        if(theme==1){r=light/3;g=light/2;b=min(31,light/2+7);}
        else {r=min(31,light+4);g=light/4;b=light/3;}
        gPlttBufferUnfaded[i]=gPlttBufferFaded[i]=r|(g<<5)|(b<<10);
    }
}

#include "fieldmap.h"
#include "sprite.h"
#include "field_player_avatar.h"
#include "event_object_movement.h"
#include "field_weather.h"
#include "constants/event_objects.h"
#include "constants/chaos_ranch_cosmetics.h"

static const u16 sRanchBase[] = INCBIN_U16("data/layouts/ChaosPokemonRanch/map.bin");
struct RanchProp {u8 x,y,w,h;u16 tile;u8 bit;};
struct RanchComposite {u16 original, themes[4];};
static const struct RanchComposite sRanchComposites[] = RANCH_THEME_COMPOSITES;
static const struct RanchProp sRanchProps[] = {
 {4,3,2,2,RANCH_TILE_RHYDON,0},{10,3,2,2,RANCH_TILE_LAPRAS,1},
 {16,3,2,2,RANCH_TILE_SNORLAX,2},{30,3,2,2,RANCH_TILE_VENUSAUR,3},
 {36,3,2,2,RANCH_TILE_CHARIZARD,4},{42,3,2,2,RANCH_TILE_BLASTOISE,5},
 {21,13,2,1,RANCH_TILE_BENCH,6},{26,13,2,1,RANCH_TILE_BENCH,6},
 {16,13,2,2,RANCH_TILE_FLOWERS,7},{30,13,2,2,RANCH_TILE_FLOWERS,7},
 {7,24,4,4,RANCH_TILE_FOUNTAIN,8},
};
void ChaosArcadeRanchScenery(void)
{
    if(gSaveBlock1Ptr->location.mapGroup!=MAP_GROUP(MAP_CHAOS_POKEMON_RANCH)||gSaveBlock1Ptr->location.mapNum!=MAP_NUM(MAP_CHAOS_POKEMON_RANCH))return;
    ChaosArcadeEnsureSave();
    const u16 themes[]={8,RANCH_TILE_FOREST,RANCH_TILE_BEACH,RANCH_TILE_SNOW,RANCH_TILE_NIGHT};
    u32 theme=gSaveBlock3Ptr->arcadeRanchTheme;
    if(theme>=ARRAY_COUNT(themes))theme=0;
    SetWeather(theme==2?WEATHER_DROUGHT:theme==3?WEATHER_SNOW:WEATHER_NONE);
    // Restore the complete immutable pasture before applying owned scenery.
    // This handles ordinary entry, saved views, PC summaries and free toggles.
    for(u32 y=0;y<40;y++)for(u32 x=0;x<48;x++){
        u16 base=sRanchBase[y*48+x];
        if((base&0x3FF)==8)base=(base&~0x3FF)|themes[theme];
        if(theme)for(u32 i=0;i<ARRAY_COUNT(sRanchComposites);i++)
            if((base&0x3FF)==sRanchComposites[i].original){base=(base&~0x3FF)|sRanchComposites[i].themes[theme-1];break;}
        MapGridSetMetatileIdAt(x+7,y+7,base);
    }
    for(u32 i=0;i<ARRAY_COUNT(sRanchProps);i++){
        const struct RanchProp *p=&sRanchProps[i];
        if(!(gSaveBlock3Ptr->arcadeDecorations&(1u<<p->bit)))continue;
        for(u32 y=0;y<p->h;y++)for(u32 x=0;x<p->w;x++)
            MapGridSetMetatileIdAt(p->x+x+7,p->y+y+7,0x3C00|(p->tile+y*p->w+x+(theme?RANCH_THEME_PROP_OFFSET+(theme-1)*RANCH_THEME_PROP_STRIDE:0)));
    }
}

// Current live avatar state tables use Brendan/May for both intro model
// choices. FRLG palettes are covered as well, without recoloring face/hair.
// Hoenn skin is 1-4 and hair/outline shades are 7-8/15. FRLG hair
// uses 1/8, skin uses 2-4. Recolor clothing plus hat/sock highlights only.
void ChaosArcadeOutfitPalette(u16 tag,u8 slot)
{
    bool32 reflection=tag==OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION||tag==OBJ_EVENT_PAL_TAG_MAY_REFLECTION||tag==OBJ_EVENT_PAL_TAG_PLAYER_RED_REFLECTION||tag==OBJ_EVENT_PAL_TAG_PLAYER_GREEN_REFLECTION;
    bool32 hoenn=tag==OBJ_EVENT_PAL_TAG_PLAYER_UNDERWATER||tag==OBJ_EVENT_PAL_TAG_BRENDAN||tag==OBJ_EVENT_PAL_TAG_MAY||tag==OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION||tag==OBJ_EVENT_PAL_TAG_MAY_REFLECTION;
    bool32 kanto=tag==OBJ_EVENT_PAL_TAG_PLAYER_RED||tag==OBJ_EVENT_PAL_TAG_PLAYER_GREEN||tag==OBJ_EVENT_PAL_TAG_PLAYER_RED_REFLECTION||tag==OBJ_EVENT_PAL_TAG_PLAYER_GREEN_REFLECTION;
    if((!hoenn&&!kanto)||slot>=16)return;
    ChaosArcadeEnsureSave();u32 outfit=gSaveBlock3Ptr->arcadeOutfit;
    if(!outfit||outfit>6)return;
    static const u16 colors[][4]={
        {0,0,0,0},
        {RGB(7,8,10),RGB(3,4,6),RGB(29,6,6),RGB(18,3,4)},
        {RGB(7,15,29),RGB(3,8,19),RGB(30,31,31),RGB(19,23,27)},
        {RGB(19,8,28),RGB(11,4,19),RGB(31,25,9),RGB(21,15,4)},
        {RGB(31,31,31),RGB(23,26,29),RGB(31,31,31),RGB(23,26,29)},
        {RGB(8,9,12),RGB(3,4,6),RGB(8,9,12),RGB(3,4,6)},
        {RGB(31,27,12),RGB(23,16,4),RGB(31,27,12),RGB(23,16,4)},
    };
    static const u8 hi[]={5,6,9,10,11,12,13,14},hm[]={0,1,2,0,1,2,3,2};
    static const u8 ki[]={5,6,7,9,10,11,12,13,14},km[]={0,1,0,2,3,2,3,0,1};
    u32 count=hoenn?ARRAY_COUNT(hi):ARRAY_COUNT(ki);
    for(u32 j=0;j<count;j++){
        u16 c=colors[outfit][hoenn?hm[j]:km[j]];
        if(reflection){u32 rr=c&31,gg=(c>>5)&31,bb=c>>10;c=((rr+12)/2)|(((gg+24)/2)<<5)|(((bb+28)/2)<<10);}
        u32 idx=OBJ_PLTT_ID(slot)+(hoenn?hi[j]:ki[j]);
        gPlttBufferUnfaded[idx]=gPlttBufferFaded[idx]=c;
    }
}
void ChaosArcadeRefreshOutfit(void)
{
    struct ObjectEvent *obj=&gObjectEvents[gPlayerAvatar.objectEventId];
    const struct ObjectEventGraphicsInfo *gfx=GetObjectEventGraphicsInfo(obj->graphicsId);
    u8 slot=gSprites[obj->spriteId].oam.paletteNum;
    PatchObjectPalette(gfx->paletteTag,slot);
    UpdateSpritePaletteWithWeather(slot,FALSE);
}
