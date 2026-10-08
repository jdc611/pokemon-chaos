#include "global.h"
#include "main.h"
#include "malloc.h"
#include "bg.h"
#include "window.h"
#include "gpu_regs.h"
#include "palette.h"
#include "sprite.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "party_menu.h"
#include "item.h"
#include "item_menu.h"
#include "move.h"
#include "battle_main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "text_window.h"
#include "task.h"
#include "string_util.h"
#include "constants/rgb.h"
#include "constants/battle_move_effects.h"

struct ChaosTmScreen {u16 tilemap[1024];u8 icons[6];enum Move move;};
static EWRAM_DATA struct ChaosTmScreen *sTmScreen;
static const u8 sTmColors[]={TEXT_COLOR_WHITE,TEXT_COLOR_DARK_GRAY,TEXT_COLOR_LIGHT_GRAY};
static const u8 sTmGray[]={TEXT_COLOR_WHITE,TEXT_COLOR_LIGHT_GRAY,TEXT_COLOR_WHITE};
static void TmPrint(const u8 *text,u32 x,u32 y){AddTextPrinterParameterized4(0,FONT_SMALL,x,y,0,0,sTmColors,TEXT_SKIP_DRAW,text);}
static void TmMain(void){RunTasks();AnimateSprites();BuildOamBuffer();UpdatePaletteFade();}
static void TmVBlank(void){LoadOam();ProcessSpriteCopyRequests();TransferPlttBuffer();}
static void Task_TmDetails(u8 taskId)
{
 if(gPaletteFade.active)return;
 if(JOY_NEW(A_BUTTON|B_BUTTON))
 {
  for(u32 i=0;i<6;i++)if(sTmScreen->icons[i]!=SPRITE_NONE)FreeAndDestroyMonIconSprite(&gSprites[sTmScreen->icons[i]]);
  FreeAllWindowBuffers();UnsetBgTilemapBuffer(0);Free(sTmScreen);sTmScreen=NULL;
  DestroyTask(taskId);SetMainCallback2(CB2_ReturnToBagMenuPocket);
 }
}
static void InitTmDetails(void)
{
 static const struct BgTemplate bgs[]={{.bg=0,.charBaseIndex=0,.mapBaseIndex=31,.priority=1}};
 static const struct WindowTemplate windows[]={{.bg=0,.tilemapLeft=1,.tilemapTop=1,.width=28,.height=18,.paletteNum=15,.baseBlock=1},DUMMY_WIN_TEMPLATE};
 SetVBlankCallback(NULL);ResetVramOamAndBgCntRegs();ResetTasks();ResetSpriteData();FreeAllSpritePalettes();
 ResetBgsAndClearDma3BusyFlags(0);InitBgsFromTemplates(0,bgs,1);SetBgTilemapBuffer(0,sTmScreen->tilemap);
 InitWindows(windows);DeactivateAllTextPrinters();LoadPalette(GetTextWindowPalette(0),BG_PLTT_ID(15),PLTT_SIZE_4BPP);
 LoadUserWindowBorderGfx(0,0x300,BG_PLTT_ID(14));FillWindowPixelBuffer(0,PIXEL_FILL(TEXT_COLOR_WHITE));
 DrawStdFrameWithCustomTileAndPalette(0,FALSE,0x300,14);
 enum Move move=sTmScreen->move;
 AddTextPrinterParameterized4(0,FONT_NORMAL,3,0,0,0,sTmColors,TEXT_SKIP_DRAW,GetMoveName(move));
 TmPrint(COMPOUND_STRING("REUSABLE TM"),137,2);
 static const u8 *const categories[]={[DAMAGE_CATEGORY_NONE]=COMPOUND_STRING("--"),[DAMAGE_CATEGORY_PHYSICAL]=COMPOUND_STRING("PHYSICAL"),[DAMAGE_CATEGORY_SPECIAL]=COMPOUND_STRING("SPECIAL"),[DAMAGE_CATEGORY_STATUS]=COMPOUND_STRING("STATUS")};
 StringCopy(gStringVar4,gTypesInfo[GetMoveType(move)].name);StringAppend(gStringVar4,COMPOUND_STRING(" / "));StringAppend(gStringVar4,categories[GetMoveCategory(move)]);TmPrint(gStringVar4,3,17);
 StringCopy(gStringVar4,COMPOUND_STRING("Power: "));ConvertIntToDecimalStringN(gStringVar1,GetMovePower(move),STR_CONV_MODE_LEFT_ALIGN,3);StringAppend(gStringVar4,gStringVar1);
 StringAppend(gStringVar4,COMPOUND_STRING("   Acc: "));ConvertIntToDecimalStringN(gStringVar1,GetMoveAccuracy(move),STR_CONV_MODE_LEFT_ALIGN,3);StringAppend(gStringVar4,gStringVar1);
 StringAppend(gStringVar4,COMPOUND_STRING("   PP: "));ConvertIntToDecimalStringN(gStringVar1,GetMovePP(move),STR_CONV_MODE_LEFT_ALIGN,2);StringAppend(gStringVar4,gStringVar1);TmPrint(gStringVar4,3,31);
 u8 description[512];StringCopy(description,GetMoveDescription(move));
 if(GetMovePriority(move)>0){StringAppend(description,COMPOUND_STRING("\nPriority +"));ConvertIntToDecimalStringN(gStringVar1,GetMovePriority(move),STR_CONV_MODE_LEFT_ALIGN,1);StringAppend(description,gStringVar1);}
 else if(GetMovePriority(move)<0)StringAppend(description,COMPOUND_STRING("\nMoves later than normal attacks."));
 if(gMovesInfo[move].effect==EFFECT_RECOIL){StringAppend(description,COMPOUND_STRING("\nRecoil: "));ConvertIntToDecimalStringN(gStringVar1,gMovesInfo[move].argument.recoilPercentage,STR_CONV_MODE_LEFT_ALIGN,2);StringAppend(description,gStringVar1);StringAppend(description,COMPOUND_STRING("% of damage."));}
 WrapFontIdToFit(description,description+StringLength(description),FONT_SMALL,216);TmPrint(description,3,47);
 for(u32 i=0;i<6;i++)
 {
  struct Pokemon *mon=&gParties[B_TRAINER_PLAYER][i];enum Species species=GetMonData(mon,MON_DATA_SPECIES);
  sTmScreen->icons[i]=SPRITE_NONE;if(species==SPECIES_NONE)continue;
  bool32 eligible=!GetMonData(mon,MON_DATA_IS_EGG) && CanLearnTeachableMove(species,move);
  bool32 known=MonKnowsMove(mon,move);
  LoadMonIconPalette(species);sTmScreen->icons[i]=CreateMonIcon(species,SpriteCB_MonIcon,26+i*37,131,0,GetMonData(mon,MON_DATA_PERSONALITY));
  u16 iconPalette[16];const u16 *source=GetValidMonIconPalettePtr(species);
  for(u32 color=0;color<16;color++)
  {
   u16 value=source[color];if(!eligible && !known){u32 gray=((value&31)+((value>>5)&31)+((value>>10)&31))/3;value=RGB(gray/2,gray/2,gray/2);}iconPalette[color]=value;
  }
  LoadPalette(iconPalette,OBJ_PLTT_ID(6+i),sizeof(iconPalette));gSprites[sTmScreen->icons[i]].oam.paletteNum=6+i;
  AddTextPrinterParameterized4(0,FONT_SMALL,i*37+2,133,0,0,eligible||known?sTmColors:sTmGray,TEXT_SKIP_DRAW,known?COMPOUND_STRING("KNOWN"):eligible?COMPOUND_STRING("YES"):COMPOUND_STRING("NO"));
 }
 PutWindowTilemap(0);CopyWindowToVram(0,COPYWIN_FULL);SetGpuReg(REG_OFFSET_DISPCNT,DISPCNT_OBJ_ON|DISPCNT_OBJ_1D_MAP);ShowBg(0);ResetPaletteFade();
 BeginNormalPaletteFade(PALETTES_ALL,0,16,0,RGB_BLACK);SetVBlankCallback(TmVBlank);CreateTask(Task_TmDetails,0);SetMainCallback2(TmMain);
}
void CB2_ChaosTmDetails(void)
{
 sTmScreen=AllocZeroed(sizeof(*sTmScreen));
 if(sTmScreen==NULL){SetMainCallback2(CB2_ReturnToBagMenuPocket);return;}
 sTmScreen->move=GetItemTMHMMoveId(gSpecialVar_ItemId);SetMainCallback2(InitTmDetails);
}
