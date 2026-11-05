#include "global.h"
#include "gym_leader.h"
#include "gym_leader_script.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "field_camera.h"
#include "money.h"
#include "overworld.h"
#include "palette.h"
#include "sound.h"
#include "string_util.h"
#include "constants/maps.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/songs.h"
#include "constants/rgb.h"
static EWRAM_DATA u16 sGymMap[13*16];
static EWRAM_DATA struct MapLayout sGymLayout;
static EWRAM_DATA struct MapEvents sGymEvents;
static EWRAM_DATA struct ObjectEventTemplate sGymObjects[12];
static EWRAM_DATA u8 sPuzzleProgress,sPuzzleMask,sChoice,sKind;
static const u8 *const sLayouts[]={COMPOUND_STRING("Classic"),COMPOUND_STRING("Twin courts"),COMPOUND_STRING("Training ring"),COMPOUND_STRING("League arena")};
static const u8 *const sPuzzles[]={COMPOUND_STRING("No puzzle"),COMPOUND_STRING("Two switches"),COMPOUND_STRING("Relay sequence"),COMPOUND_STRING("Master sequence")};
static const u8 *const sThemes[]={COMPOUND_STRING("Classic stone"),COMPOUND_STRING("Forest"),COMPOUND_STRING("Flame"),COMPOUND_STRING("Ocean"),COMPOUND_STRING("Mystic"),COMPOUND_STRING("Steel")};
extern const u8 GleGymDesign_Switch[];
static bool32 AtGym(void){return IsGymLeaderRun()&&GymLeaderState()->openingStage==5&&gSaveBlock1Ptr->location.mapGroup==MAP_GROUP(MAP_GLE_GYM_INTERIOR)&&gSaveBlock1Ptr->location.mapNum==MAP_NUM(MAP_GLE_GYM_INTERIOR);}
static bool32 GateOpen(void){u32 puzzle=GymLeaderState()->gymDesign.puzzle;return !puzzle||(puzzle==1?sPuzzleMask==3:puzzle==2?sPuzzleProgress>=3:sPuzzleProgress>=5);}
static void Gate(bool32 live){u32 x;for(x=1;x<12;x++){u16 tile=GateOpen()||x==4||x==6||x==8?0x3294:0x36a3;sGymMap[6*13+x]=tile;if(live)MapGridSetMetatileEntryAt(x+7,13,tile);}}
void GymLeaderConfigureGymDesign(struct MapHeader *header)
{
 const struct MapHeader *base;const struct GleGymDesign *d;u32 i,n;
 if(!AtGym())return;
 base=Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_GLE_GYM_INTERIOR),MAP_NUM(MAP_GLE_GYM_INTERIOR));
 if(base->mapLayout->width!=13||base->mapLayout->height!=16)return;
 d=&GymLeaderState()->gymDesign;sGymLayout=*base->mapLayout;memcpy(sGymMap,base->mapLayout->map,sizeof(sGymMap));sPuzzleProgress=sPuzzleMask=0;
 if(d->layout==1||d->layout==3){sGymMap[8*13+3]=0x36a3;sGymMap[8*13+9]=0x36a3;}
 if(d->layout>=2){sGymMap[5*13+3]=0x36a3;sGymMap[5*13+9]=0x36a3;sGymMap[9*13+3]=0x36a3;sGymMap[9*13+9]=0x36a3;}
 if(d->puzzle)Gate(FALSE);
 sGymLayout.map=sGymMap;header->mapLayout=&sGymLayout;sGymEvents=*base->events;n=sGymEvents.objectEventCount;
 if(n>ARRAY_COUNT(sGymObjects)-3)return;
 memcpy(sGymObjects,base->events->objectEvents,n*sizeof(*sGymObjects));
 if(d->puzzle)for(i=0;i<3;i++)sGymObjects[n++]=(struct ObjectEventTemplate){.localId=245+i,.graphicsId=OBJ_EVENT_GFX_ITEM_BALL,.x=i==0?4:i==1?8:6,.y=6,.elevation=3,.movementType=MOVEMENT_TYPE_NONE,.script=GleGymDesign_Switch};
 sGymEvents.objectEvents=sGymObjects;sGymEvents.objectEventCount=n;header->events=&sGymEvents;
}
void GymLeaderGymPalette(u16 start,u16 count,u8 skipFaded)
{
 static const u16 tints[]={RGB(0,0,0),RGB(4,25,8),RGB(29,8,3),RGB(3,14,29),RGB(22,7,27),RGB(16,20,23)};u16 colors[16];u32 i,theme;
 if(!AtGym()||start>192||start+count<208||(theme=GymLeaderState()->gymDesign.theme)==0||theme>=ARRAY_COUNT(tints))return;
 for(i=0;i<16;i++){u32 a=gPlttBufferUnfaded[192+i],b=tints[theme];colors[i]=i?RGB(((a&31)*3+(b&31))/4,(((a>>5)&31)*3+((b>>5)&31))/4,(((a>>10)&31)*3+((b>>10)&31))/4):a;}
 if(skipFaded)CpuCopy16(colors,&gPlttBufferUnfaded[192],sizeof(colors));else LoadPalette(colors,192,sizeof(colors));
}
static void Refresh(void)
{
 u32 i,x,y;for(i=0;i<3;i++)RemoveObjectEventByLocalIdAndMap(245+i,MAP_NUM(MAP_GLE_GYM_INTERIOR),MAP_GROUP(MAP_GLE_GYM_INTERIOR));
 GymLeaderConfigureTown(&gMapHeader);for(y=0;y<16;y++)for(x=0;x<13;x++)MapGridSetMetatileEntryAt(x+7,y+7,sGymMap[y*13+x]);
 LoadSecondaryTilesetPalette(gMapHeader.mapLayout,FALSE);DrawWholeMapView();
 if(GymLeaderState()->gymDesign.puzzle)for(i=0;i<3;i++)TrySpawnObjectEvent(245+i,MAP_NUM(MAP_GLE_GYM_INTERIOR),MAP_GROUP(MAP_GLE_GYM_INTERIOR));
}
void GleGymDesignInfo(void)
{
 const struct GleGymDesign *d=&GymLeaderState()->gymDesign;
 StringCopy(gStringVar1,sLayouts[min(d->layout,3)]);StringCopy(gStringVar2,sPuzzles[min(d->puzzle,3)]);StringCopy(gStringVar3,sThemes[min(d->theme,5)]);
 StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("Layout: {STR_VAR_1}\nPuzzle: {STR_VAR_2}\lTheme: {STR_VAR_3}\pLayout, puzzle and theme are\nindependent. Changes have a fee.\lInstalled upgrades survive demotion.\pPuzzles are optional. Your guide\nand doorway always stay accessible.\lStaff filtering earns no rank XP."));
 ConvertIntToDecimalStringN(gStringVar1,d->tier,STR_CONV_MODE_LEFT_ALIGN,1);StringAppend(gStringVar4,COMPOUND_STRING("\pPermanent upgrade tier: "));StringAppend(gStringVar4,gStringVar1);StringAppend(gStringVar4,COMPOUND_STRING("/3.\nEarned ranks 3, 5 and 7 permit\lthe three paid upgrades."));
}
void GleGymDesignMenu(void){static const u8 *const n[]={COMPOUND_STRING("Layout"),COMPOUND_STRING("Puzzle"),COMPOUND_STRING("Theme"),COMPOUND_STRING("Upgrade GYM"),COMPOUND_STRING("Reset trial"),COMPOUND_STRING("Puzzle clues"),COMPOUND_STRING("Back")};GymLeaderMenu(n,ARRAY_COUNT(n),FALSE);}
void GleGymDesignChoices(void)
{
 static const u8 *const layouts[]={COMPOUND_STRING("Classic"),COMPOUND_STRING("Twin courts"),COMPOUND_STRING("Training ring"),COMPOUND_STRING("League arena"),COMPOUND_STRING("Back")};
 static const u8 *const puzzles[]={COMPOUND_STRING("No puzzle"),COMPOUND_STRING("Two switches"),COMPOUND_STRING("Relay sequence"),COMPOUND_STRING("Master sequence"),COMPOUND_STRING("Back")};
 static const u8 *const themes[]={COMPOUND_STRING("Classic stone"),COMPOUND_STRING("Forest"),COMPOUND_STRING("Flame"),COMPOUND_STRING("Ocean"),COMPOUND_STRING("Mystic"),COMPOUND_STRING("Steel"),COMPOUND_STRING("Back")};
 sKind=gSpecialVar_0x8005;GymLeaderMenu(sKind==0?layouts:sKind==1?puzzles:themes,sKind==2?7:5,FALSE);
}
void GleGymDesignQuote(void)
{
 struct GleState *s=GymLeaderState();const struct GleGymDesign *d=&s->gymDesign;u32 required,old;
 sKind=gSpecialVar_0x8005;sChoice=sKind==3?0:gSpecialVar_Result;gSpecialVar_Result=2;
 if(sKind>3||sChoice>=(sKind==2?6:4))return;
 gSpecialVar_Result=0;old=sKind==0?d->layout:sKind==1?d->puzzle:sKind==2?d->theme:d->tier;
 if((sKind<3&&old==sChoice)||(sKind==3&&old>=3)){StringCopy(gStringVar4,COMPOUND_STRING("That installation is already owned.\nNothing was charged."));return;}
 required=sKind==3?old+1:sChoice;
 if((sKind<2&&required>d->tier)||(sKind==3&&max(s->rank,s->highestRank)<3+2*old)){
  ConvertIntToDecimalStringN(gStringVar1,required,STR_CONV_MODE_LEFT_ALIGN,1);ConvertIntToDecimalStringN(gStringVar2,sKind==3?3+2*old:1+2*required,STR_CONV_MODE_LEFT_ALIGN,1);
  StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("This needs GYM upgrade {STR_VAR_1}.\nEarned rank {STR_VAR_2} permits that upgrade.\lYour current design is retained."));return;
 }
 StringCopy(gStringVar1,sKind==0?sLayouts[sChoice]:sKind==1?sPuzzles[sChoice]:sKind==2?sThemes[sChoice]:COMPOUND_STRING("Next GYM upgrade"));
 ConvertIntToDecimalStringN(gStringVar2,GleGymDesignPrice(s,sKind,sChoice),STR_CONV_MODE_LEFT_ALIGN,6);gSpecialVar_Result=1;
}
void GleGymDesignConfirm(void){u32 money=GetMoney(&gSaveBlock1Ptr->money);gSpecialVar_Result=AtGym()?GleGymDesignPurchase(GymLeaderState(),sKind,sChoice,&money):GLE_NOT_READY;SetMoney(&gSaveBlock1Ptr->money,money);if(!gSpecialVar_Result)Refresh();}
void GleGymDesignReset(void){if(AtGym()){sPuzzleProgress=sPuzzleMask=0;Gate(TRUE);DrawWholeMapView();}}
void GleGymDesignClues(void){StringCopy(gStringVar4,COMPOUND_STRING("Two switches: turn on LEFT and RIGHT.\nRelay: LEFT, CENTER, RIGHT.\pMaster: LEFT, RIGHT, CENTER,\nRIGHT, LEFT. A wrong input resets it.\pTest consoles use familiar native\nitem-ball markers. The center\lconsole resets the two-switch trial.\pVisitors face the puzzle before\nyour staff. It modestly improves\ltheir filtering, never your rank."));}
void GleGymDesignSwitch(void)
{
 static const u8 relay[]={0,2,1},master[]={0,1,2,1,0};u32 which=gSpecialVar_LastTalked-245,puzzle=GymLeaderState()->gymDesign.puzzle;bool32 was;
 if(!AtGym()||which>=3||!puzzle){StringCopy(gStringVar4,COMPOUND_STRING("No puzzle is installed."));return;}
 was=GateOpen();if(puzzle==1){if(which<2)sPuzzleMask^=1u<<which;else sPuzzleMask=0;}
 else if(!was){const u8 *sequence=puzzle==2?relay:master;sPuzzleProgress=which==sequence[sPuzzleProgress]?sPuzzleProgress+1:0;}
 Gate(TRUE);DrawWholeMapView();PlaySE(GateOpen()?SE_DING_DONG:SE_SWITCH);
 StringCopy(gStringVar4,GateOpen()?COMPOUND_STRING("The trial gate is open!\nUse Reset trial to try again."):COMPOUND_STRING("The console clicks.\nThe trial gate is still closed.\lYour guide remains available."));
}

void GleGymDesignOwnerAccess(void){u32 x,y;if(!AtGym())return;if(gSaveBlock1Ptr->pos.y<6){sPuzzleMask=3;sPuzzleProgress=5;}Gate(FALSE);for(y=0;y<16;y++)for(x=0;x<13;x++)MapGridSetMetatileEntryAt(x+7,y+7,sGymMap[y*13+x]);}
