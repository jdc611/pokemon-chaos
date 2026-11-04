#include "global.h"
#include "gym_leader.h"
#include "gym_leader_script.h"
#include "gym_leader_research.h"
#include "gym_leader_recruit.h"
#include "gym_leader_news.h"
#include "event_data.h"
#include "fieldmap.h"
#include "field_camera.h"
#include "money.h"
#include "malloc.h"
#include "overworld.h"
#include "string_util.h"
#include "script.h"
#include "constants/event_bg.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/flags.h"
#include "constants/maps.h"

extern const u8 GleRocket_Contact[],GleChampion_Celebrant[],GleChampion_Display[],GleHosting_Desk[],GleCompetition_Desk[];
extern const u8 GleTown_MapScripts[], GleTownResident[], GleLandmarkSign[], GleSilphResearch[], GleSilphPrivate[];
extern const u8 GleTownPlot0[];
extern const u8 GleTownPlot1[];
extern const u8 GleTownPlot2[];
extern const u8 GleTownPlot3[];
extern const u8 GleTownPlot4[];
extern const u8 GleTownPlot5[];
extern const u8 GleTownPlot6[];
extern const u8 GleTownPlot7[];
extern const u8 GleTownPlot8[];
extern const u8 GleTownPlot9[];
extern const u8 GleTownPlot10[];
extern const u8 GleTownPlot11[];
extern const u8 GleTownPlot12[];
extern const u8 GleTownPlot13[];
extern const u8 GleTownPlot14[];
extern const u8 GleTownPlot15[];
extern const u8 GleTownPlot16[];
extern const u8 GleTownPlot17[];
extern const u8 GleTownPlot18[];
extern const u8 GleTownPlot19[];
extern const u8 GleTownPlot20[];
extern const u8 GleTownPlot21[];
extern const u8 GleTownPlot22[];
extern const u8 GleTownPlot23[];
static const u8 *const sPlotScripts[] = {GleTownPlot0,GleTownPlot1,GleTownPlot2,GleTownPlot3,GleTownPlot4,GleTownPlot5,GleTownPlot6,GleTownPlot7,GleTownPlot8,GleTownPlot9,GleTownPlot10,GleTownPlot11,GleTownPlot12,GleTownPlot13,GleTownPlot14,GleTownPlot15,GleTownPlot16,GleTownPlot17,GleTownPlot18,GleTownPlot19,GleTownPlot20,GleTownPlot21,GleTownPlot22,GleTownPlot23};
static const u16 sTowns[] = {MAP_SAFFRON_CITY, MAP_CELADON_CITY, MAP_FUCHSIA_CITY};
static const u16 sLandmarks[] = {MAP_SILPH_CO_1F, MAP_CELADON_CITY_GAME_CORNER, MAP_FUCHSIA_CITY_SAFARI_ZONE_ENTRANCE};
static const u16 sGyms[] = {MAP_SAFFRON_CITY_GYM, MAP_CELADON_CITY_GYM, MAP_FUCHSIA_CITY_GYM};
static const u16 sHouses[] = {MAP_SAFFRON_CITY_COPYCATS_HOUSE_1F, MAP_CELADON_CITY_HOUSE1, MAP_FUCHSIA_CITY_HOUSE1};
static const u8 *const sBuildings[] = {
    COMPOUND_STRING("EMPTY PLOT"), COMPOUND_STRING("POKéMON CENTER"), COMPOUND_STRING("POKé MART"),
    COMPOUND_STRING("HOUSE"), COMPOUND_STRING("HOTEL"), COMPOUND_STRING("RESTAURANT"), COMPOUND_STRING("DOJO"),
    COMPOUND_STRING("MUSEUM"), COMPOUND_STRING("RESEARCH LAB"), COMPOUND_STRING("HABITAT"), COMPOUND_STRING("PARK"), COMPOUND_STRING("BLACK MARKET")
};
static EWRAM_DATA struct MapEvents sEvents = {0};
static EWRAM_DATA struct WarpEvent sWarps[64] = {0};
static EWRAM_DATA struct BgEvent sSigns[GLE_PLOTS+1] = {0};
static EWRAM_DATA struct ObjectEventTemplate sResidents[13] = {0};
static EWRAM_DATA u8 sOriginalWarpCount = 0;

static s32 TownIndex(u16 map)
{
    u32 i;
    if (!IsGymLeaderRun() || GymLeaderState()->openingStage < 1)
        return -1;
    for (i = 0; i < 3; i++)
        if (map == sTowns[i] && (GymLeaderState()->town == i || GymLeaderState()->rivalTown == i))
            return i;
    return -1;
}

static u16 CurrentMapId(void) { return (u8)gSaveBlock1Ptr->location.mapNum | ((u8)gSaveBlock1Ptr->location.mapGroup << 8); }
static u32 PlotX(u32 plot) { return (TownIndex(CurrentMapId()) == 0 ? 13 : 3) + plot % 6 * 7; }
static u32 PlotY(u32 plot) { return 10 + plot / 6 * 7; }
static u32 PlotBuilding(u32 town, u32 plot)
{
    struct GleState *s = GymLeaderState();
    if (town == s->town)
        return s->plots[plot].building;
    if (plot >= min(GLE_PLOTS, s->activity / 40))
        return GLE_EMPTY;
    return plot == 0 ? GLE_CENTER : plot == 1 ? GLE_MART : plot % 4 == 0 ? GLE_HOTEL : GLE_HOUSE;
}
static u32 LandmarkTier(u32 town) { return town==GymLeaderState()->town?GymLeaderState()->landmarkTier:(GymLeaderState()->activity>=400); }
static s32 LandmarkX(u32 town) { return town==0?59:town==1?49:37; }
static s32 LandmarkY(u32 town) { return town==0?19:7; }
static const struct MapHeader *OriginalTown(u32 town)
{
    return Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(sTowns[town]), MAP_NUM(sTowns[town]));
}
static void PlotDoor(u32 town, u32 plot, s16 *x, s16 *y)
{
    u32 building = PlotBuilding(town,plot);
    bool32 service = building == GLE_CENTER || building == GLE_MART || building == GLE_EMPTY;
    *x = PlotX(plot) + (service ? 2 : 1);
    *y = PlotY(plot) + (building == GLE_CENTER || (!service && town != 2) ? 4 : 3);
}

void GymLeaderConfigureTown(struct MapHeader *header)
{
    s32 town = TownIndex(CurrentMapId());
    const struct MapEvents *source;
    u32 i, visitors;
    GymLeaderUnlockTravel();
    GymLeaderConfigureExclusiveMap(header);
    GymLeaderConfigureResearchMap(header);
    if (IsGymLeaderRun())
    {
        u16 map = CurrentMapId();
        if (map == MAP_GLE_GYM_INTERIOR)
            header->regionMapSectionId = OriginalTown(GymLeaderState()->town)->regionMapSectionId;
        else if ((map == MAP_GLE_BUILDING_INTERIOR || map == MAP_GLE_CENTER_INTERIOR || map == MAP_GLE_MART_INTERIOR)
            && GymLeaderState()->interiorTown < 3)
            header->regionMapSectionId = OriginalTown(GymLeaderState()->interiorTown)->regionMapSectionId;
    }
    if (town < 0)
    {
        // The restored Silph public laboratory replaces the old Rocket siege.
        // Closed staff stair/elevator doors cannot launch the vanilla campaign.
        if(IsGymLeaderRun() && CurrentMapId()==MAP_SILPH_CO_1F)
        {
            sEvents=*header->events;
            sEvents.warpCount=3;
            sResidents[0]=(struct ObjectEventTemplate){.localId=1,.graphicsId=OBJ_EVENT_GFX_WORKER_F,.x=3,.y=7,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleSilphResearch};
            sResidents[1]=(struct ObjectEventTemplate){.localId=2,.graphicsId=OBJ_EVENT_GFX_SCIENTIST_1,.x=31,.y=2,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleSilphPrivate};
            sResidents[2]=(struct ObjectEventTemplate){.localId=3,.graphicsId=OBJ_EVENT_GFX_SCIENTIST_1,.x=22,.y=3,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleSilphPrivate};
            sEvents.objectEventCount=3;sEvents.objectEvents=sResidents;
            header->events=&sEvents;
        }
        else if(IsGymLeaderRun() && CurrentMapId()==MAP_CELADON_CITY_GAME_CORNER)
        {
            sEvents=*header->events;sEvents.warpCount=3;header->events=&sEvents;
            FlagSet(FLAG_HIDE_GAME_CORNER_ROCKET);
        }
        GymLeaderConfigureStaffMap(header);
        return;
    }
    source = OriginalTown(town)->events;
    sOriginalWarpCount = source->warpCount;
    memset(&sEvents, 0, sizeof(sEvents));
    memcpy(sWarps, source->warps, source->warpCount * sizeof(*sWarps));
    sWarps[sOriginalWarpCount] = (struct WarpEvent){10,8,0,0,MAP_NUM(town == GymLeaderState()->town ? MAP_GLE_GYM_INTERIOR : MAP_GLE_RIVAL_GYM),MAP_GROUP(MAP_GLE_GYM_INTERIOR)};
    for (i = 0; i < GLE_PLOTS; i++)
    {
        u32 building = PlotBuilding(town, i);
        u16 destination = building == GLE_CENTER ? MAP_GLE_CENTER_INTERIOR : building == GLE_MART ? MAP_GLE_MART_INTERIOR : MAP_GLE_BUILDING_INTERIOR;
        struct WarpEvent *door = &sWarps[sOriginalWarpCount + 1 + i];
        PlotDoor(town, i, &door->x, &door->y);
        door->elevation = 0; door->warpId = 1;
        door->mapNum = MAP_NUM(destination); door->mapGroup = MAP_GROUP(destination);
        sSigns[i] = (struct BgEvent){.x=PlotX(i)+5,.y=PlotY(i)+5,.elevation=0,.kind=BG_EVENT_PLAYER_FACING_ANY,.bgUnion.script=sPlotScripts[i]};
    }
    visitors = town == GymLeaderState()->town ? min(8, GymLeaderState()->population / 3) : min(8, GymLeaderState()->activity / 80);
    for (i = 0; i < visitors; i++)
        sResidents[i] = (struct ObjectEventTemplate){.localId=i+1,.graphicsId=i%2 ? OBJ_EVENT_GFX_LASS_FRLG : OBJ_EVENT_GFX_GENTLEMAN_FRLG,.x=4+i*5,.y=16+(i%3)*7,.elevation=3,.movementType=MOVEMENT_TYPE_WANDER_LEFT_AND_RIGHT,.movementRangeX=1,.movementRangeY=0,.script=GleTownResident};
    {
        u32 index=sOriginalWarpCount+1+GLE_PLOTS;
        sWarps[index]=(struct WarpEvent){LandmarkX(town),LandmarkY(town),0,town==0?1:town==1?0:2,MAP_NUM(sLandmarks[town]),MAP_GROUP(sLandmarks[town])};
        sSigns[GLE_PLOTS]=(struct BgEvent){.x=LandmarkX(town)+2,.y=LandmarkY(town)+1,.elevation=0,.kind=BG_EVENT_PLAYER_FACING_ANY,.bgUnion.script=GleLandmarkSign};
    }
    sEvents.warpCount = sOriginalWarpCount + 2 + GLE_PLOTS;
    sEvents.warps = sWarps; sEvents.bgEventCount = GLE_PLOTS+1; sEvents.bgEvents = sSigns;
    if(town==GymLeaderState()->town && GymLeaderState()->rocket.stage)
        sResidents[visitors++]=(struct ObjectEventTemplate){.localId=235,.graphicsId=OBJ_EVENT_GFX_ROCKET_M,
            .x=15,.y=8,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleRocket_Contact};
    if(town==GymLeaderState()->town && GymLeaderState()->championshipWins){
        for(i=0;i<2;i++)sResidents[visitors++]=(struct ObjectEventTemplate){.localId=247+i,.graphicsId=OBJ_EVENT_GFX_TRICK_HOUSE_STATUE,.x=7+i*6,.y=9,.elevation=3,.movementType=MOVEMENT_TYPE_NONE,.script=GleChampion_Display};
        if(GymLeaderCelebrationPending()){
            FlagClear(FLAG_TEMP_1E);FlagClear(FLAG_TEMP_1F);
            sResidents[visitors++]=(struct ObjectEventTemplate){.localId=245,.flagId=FLAG_TEMP_1E,.graphicsId=OBJ_EVENT_GFX_GENTLEMAN_FRLG,.x=8,.y=9,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleChampion_Celebrant};
            sResidents[visitors++]=(struct ObjectEventTemplate){.localId=246,.flagId=FLAG_TEMP_1F,.graphicsId=GymLeaderCelebrationStaffGraphics(),.x=12,.y=9,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleChampion_Celebrant};
        }
    }
    if(GymLeaderHostingTown()==town)sResidents[visitors++]=(struct ObjectEventTemplate){.localId=249,.graphicsId=OBJ_EVENT_GFX_GENTLEMAN_FRLG,.x=11,.y=9,.elevation=3,.movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GymLeaderState()->hosting.stage==GLE_HOST_INVITED&&town==GymLeaderState()->town?GleHosting_Desk:GleCompetition_Desk};
    sEvents.objectEventCount = visitors; sEvents.objectEvents = sResidents;
    header->events = &sEvents;
    header->mapScripts = GleTown_MapScripts;
    GymLeaderConfigureStaffMap(header);
}

// Copy real native building blocks from the original city's own tileset. We
// retain the original ROM layouts, rather than editing the unselected city.
static void DrawBuilding(u32 town, u16 targetMap, s32 x, s32 y, u32 width, u32 height)
{
    const struct MapHeader *source = OriginalTown(town);
    u32 i, xx, yy;
    for (i = 0; i < source->events->warpCount; i++)
    {
        const struct WarpEvent *door = &source->events->warps[i];
        if (door->mapNum != MAP_NUM(targetMap) || door->mapGroup != MAP_GROUP(targetMap))
            continue;
        for (yy = 0; yy < height; yy++)
            for (xx = 0; xx < width; xx++)
            {
                s32 sx = door->x - width / 2 + xx, sy = door->y - height + 1 + yy;
                if (sx >= 0 && sy >= 0 && sx < source->mapLayout->width && sy < source->mapLayout->height)
                    MapGridSetMetatileIdAt(x + xx + 7, y + yy + 7, source->mapLayout->map[sy * source->mapLayout->width + sx]);
            }
        return;
    }
}

static void DrawHouse(u32 town,u32 plot)
{
    const struct MapHeader *source=OriginalTown(town);
    u32 i,x,y,width=town==2?5:4,height=town==2?4:5;
    for(i=0;i<source->events->warpCount;i++)
    {
        const struct WarpEvent *door=&source->events->warps[i];
        if(door->mapNum!=MAP_NUM(sHouses[town]) || door->mapGroup!=MAP_GROUP(sHouses[town]))continue;
        for(y=0;y<height;y++)for(x=0;x<width;x++)
            MapGridSetMetatileIdAt(PlotX(plot)+x+7,PlotY(plot)+y+7,
                source->mapLayout->map[(door->y-height+1+y)*source->mapLayout->width+door->x-1+x]);
        return;
    }
}

static void DrawServiceBuilding(u32 building, u32 plot)
{
    static const u16 center[] = {
        0x317B,0x317C,0x317D,0x317E,0x317F,
        0x0448,0x0449,0x044A,0x044B,0x0587,
        0x0450,0x0451,0x0452,0x0453,0x058F,
        0x0458,0x0459,0x045A,0x045B,0x0597,
        0x0460,0x0461,0x0462,0x0586,0x059F
    };
    static const u16 mart[] = {
        0x3028,0x3029,0x302A,0x302B,
        0x0430,0x0431,0x0432,0x0433,
        0x0438,0x0439,0x043A,0x043B,
        0x0440,0x0441,0x0462,0x0463
    };
    const u16 *blocks = building == GLE_CENTER ? center : mart;
    u32 width = building == GLE_CENTER ? 5 : 4, x, y;
    for (y=0;y<width;y++)
        for(x=0;x<width;x++)
            MapGridSetMetatileIdAt(PlotX(plot)+x+7,PlotY(plot)+y+7,blocks[y*width+x]);
}

void GleTownDraw(void)
{
    s32 town = TownIndex(CurrentMapId());
    u32 x, y, i;
    if (town < 0)
        return;
    for (y = 3; y < gMapHeader.mapLayout->height - 2; y++)
        for (x = 3; x < gMapHeader.mapLayout->width - 3; x++)
            MapGridSetMetatileIdAt(x+7,y+7,0x3001);
    // Saffron's four gate buildings connect to native gate interiors. Preserve
    // those blocks and their original warp indices so route exits stay valid.
    if (town == 0)
    {
        const struct MapLayout *source = OriginalTown(town)->mapLayout;
        for (i = 8; i < 14; i++)
        {
            const struct WarpEvent *gate = &OriginalTown(town)->events->warps[i];
            for (y = max(0,gate->y-4); y <= gate->y+1; y++)
                for (x = max(0,gate->x-3); x <= gate->x+3; x++)
                    if (x < source->width && y < source->height)
                        MapGridSetMetatileIdAt(x+7,y+7,source->map[y*source->width+x]);
        }
    }
    DrawBuilding(town, sGyms[town], 7, 4, 7, 5);
    for (i = 0; i < GLE_PLOTS; i++)
    {
        u32 building = PlotBuilding(town, i);
        if (!building)
        {
            u32 xx,yy;
            for(yy=0;yy<5;yy++)for(xx=0;xx<5;xx++)
                MapGridSetMetatileIdAt(PlotX(i)+xx+7,PlotY(i)+yy+7,0x300D);
        }
        if (building)
        {
            if (building == GLE_PARK || building == GLE_HABITAT)
            {
                u32 xx,yy;
                for(yy=0;yy<5;yy++)for(xx=0;xx<5;xx++)
                    MapGridSetMetatileIdAt(PlotX(i)+xx+7,PlotY(i)+yy+7,(xx==0||yy==0||xx==4||yy==4)?0x3001:0x300D);
            }
            else if (building == GLE_CENTER || building == GLE_MART)
                DrawServiceBuilding(building,i);
            else
                DrawHouse(town,i);
        }
        MapGridSetMetatileIdAt(PlotX(i)+5+7,PlotY(i)+5+7,0x0402);
    }
    if(LandmarkTier(town))
        DrawBuilding(town,sLandmarks[town],LandmarkX(town)-(town==0?4:3),LandmarkY(town)-(town==0?9:4),town==0?9:7,town==0?10:5);
    if(LandmarkTier(town))MapGridSetMetatileIdAt(LandmarkX(town)+7,LandmarkY(town)+7,0x0462);
    MapGridSetMetatileIdAt(LandmarkX(town)+2+7,LandmarkY(town)+1+7,0x0402);
    FlagSet(town == 0 ? FLAG_WORLD_MAP_SAFFRON_CITY : town == 1 ? FLAG_WORLD_MAP_CELADON_CITY : FLAG_WORLD_MAP_FUCHSIA_CITY);
}

bool32 GymLeaderTownWarp(s8 *group, s8 *number, s8 *warp, s8 *x, s8 *y)
{
    s32 town = TownIndex(CurrentMapId());
    u16 destination = (u8)*number | ((u8)*group << 8);
    struct GleState *s;
    u32 i;
    if (!IsGymLeaderRun())
        return FALSE;
    s = GymLeaderState();
    if(s->interiorPlot==GLE_PLOTS && s->interiorTown<3
        && CurrentMapId()==sLandmarks[s->interiorTown] && destination==sTowns[s->interiorTown])
    {
        *warp=-1;*x=LandmarkX(s->interiorTown);*y=LandmarkY(s->interiorTown)+1;
        s->interiorPlot=GLE_NONE;
        return TRUE;
    }
    if(town>=0 && destination==sLandmarks[town] && LandmarkTier(town))
    {
        s->interiorPlot=GLE_PLOTS;s->interiorTown=town;
        return TRUE;
    }
    if (town >= 0 && destination == MAP_GLE_GYM_INTERIOR)
    {
        SetDynamicWarpWithCoords(0, MAP_GROUP(sTowns[town]), MAP_NUM(sTowns[town]), -1, 10, 9);
        *warp=-1;*x=6;*y=13;
        return TRUE;
    }
    if (town >= 0 && destination == MAP_GLE_RIVAL_GYM)
    {
        SetDynamicWarpWithCoords(0, MAP_GROUP(sTowns[town]), MAP_NUM(sTowns[town]), -1, 10, 9);
        *warp=1;
        return TRUE;
    }
    if (town < 0 || (destination != MAP_GLE_BUILDING_INTERIOR && destination != MAP_GLE_CENTER_INTERIOR && destination != MAP_GLE_MART_INTERIOR))
        return FALSE;
    for (i=0;i<GLE_PLOTS;i++)
    {
        s16 dx,dy;
        PlotDoor(town,i,&dx,&dy);
        if (gSaveBlock1Ptr->pos.x == dx && (gSaveBlock1Ptr->pos.y == dy || gSaveBlock1Ptr->pos.y == dy+1))
        {
            if (!PlotBuilding(town,i))
                return FALSE;
            s->interiorPlot=i;s->interiorTown=town;
            SetDynamicWarpWithCoords(0,MAP_GROUP(sTowns[town]),MAP_NUM(sTowns[town]),-1,dx,dy+1);
            return TRUE;
        }
    }
    return FALSE;
}

void GlePlotInfo(void)
{
    struct GleState *s = GymLeaderState();
    s32 town = TownIndex(CurrentMapId());
    u32 plot=gSpecialVar_0x8007;
    gSpecialVar_Result=0;
    if (town < 0 || plot >= GLE_PLOTS)
        return;
    if (town != s->town)
    {gSpecialVar_Result=2;return;}
    ConvertIntToDecimalStringN(gStringVar1,plot+1,STR_CONV_MODE_LEFT_ALIGN,2);
    StringCopy(gStringVar2,s->plots[plot].size==1?COMPOUND_STRING("SMALL"):s->plots[plot].size==2?COMPOUND_STRING("MEDIUM"):COMPOUND_STRING("LARGE"));
    StringCopy(gStringVar3,sBuildings[s->plots[plot].building]);
    gSpecialVar_Result=1;
}
void GlePlotMenu(void)
{
    const u8 *names[GLE_BUILDING_COUNT];
    u32 i,plot=gSpecialVar_0x8007;
    if (plot >= GLE_PLOTS) {gSpecialVar_Result=255;GymLeaderResumeScriptNextFrame();return;}
    if (GymLeaderState()->plots[plot].building)
    {
        u32 building=GymLeaderState()->plots[plot].building;
        static const u8 *const back[]={COMPOUND_STRING("Back")};
        if(building==GLE_CENTER || building==GLE_MART || building==GLE_LAB || GymLeaderState()->plots[plot].tier>=3)
        {GymLeaderMenu(back,1,FALSE);return;}
        static const u8 *const choices[]={COMPOUND_STRING("Upgrade"),COMPOUND_STRING("Back")};
        GymLeaderMenu(choices,2,FALSE);
    }
    else
    {
        for(i=1;i<GLE_BUILDING_COUNT;i++)names[i-1]=sBuildings[i];
        GymLeaderMenu(names,GLE_BUILDING_COUNT-1,FALSE);
    }
}
void GlePlotBuildInfo(void)
{
    struct GleState *s=GymLeaderState();
    u32 plot=gSpecialVar_0x8007,choice=gSpecialVar_Result;
    gSpecialVar_Result=0;
    if(plot>=GLE_PLOTS)return;
    if(s->plots[plot].building)
    {
        u32 building=s->plots[plot].building;
        if(choice!=0 || building==GLE_CENTER || building==GLE_MART || building==GLE_LAB || s->plots[plot].tier>=3)return;
        gSpecialVar_0x8006=building;
    }
    else
    {if(choice>=GLE_BUILDING_COUNT-1)return;gSpecialVar_0x8006=choice+1;}
    StringCopy(gStringVar1,sBuildings[gSpecialVar_0x8006]);
    ConvertIntToDecimalStringN(gStringVar2,GleBuildCost(gSpecialVar_0x8006,s->plots[plot].tier+1),STR_CONV_MODE_LEFT_ALIGN,7);
    ConvertIntToDecimalStringN(gStringVar3,s->plots[plot].tier+1,STR_CONV_MODE_LEFT_ALIGN,1);
    gSpecialVar_Result=1;
}
void GlePlotConstruct(void)
{
    u32 money=GetMoney(&gSaveBlock1Ptr->money),plot=gSpecialVar_0x8007;
    s32 town=TownIndex(CurrentMapId());
    if(town<0 || town!=GymLeaderState()->town || plot>=GLE_PLOTS)
    {gSpecialVar_Result=GLE_INVALID;return;}
    // A player reading a sign from its north side could occupy the new roof.
    // Refuse that placement, rather than trapping or teleporting the player.
    if(gSaveBlock1Ptr->pos.x>=PlotX(plot) && gSaveBlock1Ptr->pos.x<PlotX(plot)+7
        && gSaveBlock1Ptr->pos.y>=PlotY(plot) && gSaveBlock1Ptr->pos.y<PlotY(plot)+5)
    {gSpecialVar_Result=GLE_NOT_READY;return;}
    if(gSpecialVar_0x8006==GLE_MARKET && GymLeaderState()->honor>-100)
    {gSpecialVar_Result=GLE_NOT_READY;return;}
    gSpecialVar_Result=GleConstruct(GymLeaderState(),plot,gSpecialVar_0x8006,&money);
    if(gSpecialVar_Result==GLE_OK)
    {
        SetMoney(&gSaveBlock1Ptr->money,money);
        GymLeaderConfigureTown(&gMapHeader);
        GleTownDraw();DrawWholeMapView();
    }
}
void GleTownStatus(void)
{
    struct GleState *s=GymLeaderState();
    u32 population=s->population,capacity=s->capacity,reputation=s->reputation,i;
    if(TownIndex(CurrentMapId())==s->rivalTown
        || ((CurrentMapId()==MAP_GLE_BUILDING_INTERIOR || CurrentMapId()==MAP_GLE_MART_INTERIOR) && s->interiorTown==s->rivalTown))
    {
        capacity=0;
        for(i=0;i<GLE_PLOTS;i++)
        {
            u32 building=PlotBuilding(s->rivalTown,i);
            capacity+=building==GLE_HOUSE?12:building==GLE_HOTEL?20:0;
        }
        population=min(capacity,s->activity/20);
        reputation=100+2*min(GLE_PLOTS,s->activity/40);
    }
    ConvertIntToDecimalStringN(gStringVar1,population,STR_CONV_MODE_LEFT_ALIGN,4);
    ConvertIntToDecimalStringN(gStringVar2,capacity,STR_CONV_MODE_LEFT_ALIGN,4);
    ConvertIntToDecimalStringN(gStringVar3,reputation,STR_CONV_MODE_LEFT_ALIGN,5);
}
void GleBuildingInfo(void)
{
    struct GleState *s=GymLeaderState();
    gSpecialVar_Result=GLE_EMPTY;
    if(s->interiorPlot>=GLE_PLOTS || s->interiorTown>=3)return;
    gSpecialVar_Result=PlotBuilding(s->interiorTown,s->interiorPlot);
    StringCopy(gStringVar1,sBuildings[gSpecialVar_Result]);
}

static s32 HabitatPlot(void)
{
    u32 i;
    s32 town=TownIndex(CurrentMapId());
    if(town<0 || town!=GymLeaderState()->town)return -1;
    for(i=0;i<GLE_PLOTS;i++)
        if(GymLeaderState()->plots[i].building==GLE_HABITAT
            && gSaveBlock1Ptr->pos.x>PlotX(i) && gSaveBlock1Ptr->pos.x<PlotX(i)+4
            && gSaveBlock1Ptr->pos.y>PlotY(i) && gSaveBlock1Ptr->pos.y<PlotY(i)+4)
            return i;
    return -1;
}
bool32 GymLeaderHabitatActive(void) {return HabitatPlot()>=0;}
u16 GymLeaderHabitatSpecies(u32 slot)
{
    u16 *pool,species=SPECIES_NONE;
    u32 count,tier;
    s32 plot=HabitatPlot();
    struct GleState *s=GymLeaderState();
    if(plot<0 || s->plots[plot].type>=18)return SPECIES_NONE;
    pool=Alloc(GLE_SPECIES_LIMIT*sizeof(u16));
    if(pool==NULL)return SPECIES_NONE;
    tier=min(8,s->rank+s->plots[plot].tier-1);
    // Upgrades broaden normal ecology. They never turn habitats into a
    // legendary/Paradox voucher pool or bypass unique research encounters.
    count=GymLeaderBuildSpeciesPool(s->plots[plot].type,tier,GLE_STANDARD,FALSE,pool,GLE_SPECIES_LIMIT);
    if(count && count<=GLE_SPECIES_LIMIT)
        species=pool[GleMix(s->seed ^ s->rank ^ (plot*104729u) ^ (s->plots[plot].tier*919u) ^ (slot*7919u))%count];
    Free(pool);
    return species;
}
void GlePlotNeedsHabitatType(void)
{
    u32 plot=gSpecialVar_0x8007;
    gSpecialVar_Result=plot<GLE_PLOTS && gSpecialVar_0x8006==GLE_HABITAT && !GymLeaderState()->plots[plot].building;
}
void GlePlotApplyHabitatType(void)
{
    u32 plot=gSpecialVar_0x8007,type=gSpecialVar_0x8009;
    if(IsGymLeaderRun() && TownIndex(CurrentMapId())==GymLeaderState()->town
        && plot<GLE_PLOTS && type<18 && GymLeaderState()->plots[plot].building==GLE_HABITAT)
        GymLeaderState()->plots[plot].type=type;
}

void GleLandmarkInfo(void)
{
    static const u8 *const names[]={COMPOUND_STRING("SILPH CO."),COMPOUND_STRING("GAME CORNER"),COMPOUND_STRING("SAFARI ZONE")};
    static const u32 costs[]={8000,6000,5000};
    s32 town=TownIndex(CurrentMapId());
    u32 tier;
    gSpecialVar_Result=0;
    if(town<0)return;
    if(town!=GymLeaderState()->town){gSpecialVar_Result=2;return;}
    tier=GymLeaderState()->landmarkTier+1;
    StringCopy(gStringVar1,names[town]);
    ConvertIntToDecimalStringN(gStringVar2,costs[town]*tier*tier,STR_CONV_MODE_LEFT_ALIGN,6);
    ConvertIntToDecimalStringN(gStringVar3,tier,STR_CONV_MODE_LEFT_ALIGN,1);
    gSpecialVar_Result=tier<=3?1:3;
}
void GleLandmarkConstruct(void)
{
    static const u32 costs[]={8000,6000,5000};
    s32 town=TownIndex(CurrentMapId());
    struct GleState *s=GymLeaderState();
    u32 tier=s->landmarkTier+1,cost;
    gSpecialVar_Result=0;
    if(town<0 || town!=s->town || tier>3)return;
    if(s->rank<1+tier*2 || s->population<(tier-1)*12)return;
    if(gSaveBlock1Ptr->pos.x>=LandmarkX(town)-(town==0?4:3)
        && gSaveBlock1Ptr->pos.x<=LandmarkX(town)+(town==0?4:3)
        && gSaveBlock1Ptr->pos.y>=LandmarkY(town)-(town==0?9:4)
        && gSaveBlock1Ptr->pos.y<=LandmarkY(town))return;
    cost=costs[town]*tier*tier;
    if(!IsEnoughMoney(&gSaveBlock1Ptr->money,cost))return;
    RemoveMoney(&gSaveBlock1Ptr->money,cost);
    s->landmarkTier=tier;
    GymLeaderConfigureTown(&gMapHeader);GleTownDraw();DrawWholeMapView();
    gSpecialVar_Result=1;
}
