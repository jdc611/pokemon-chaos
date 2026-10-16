#include "global.h"
#include "custom_build.h"
#include "oaks_draft.h"
#include "pokemon_storage_system.h"
#include "overworld.h"
#include "script.h"
#include "main.h"
#include "malloc.h"
#include "bg.h"
#include "window.h"
#include "gpu_regs.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "menu.h"
#include "menu_helpers.h"
#include "sound.h"
#include "string_util.h"
#include "data.h"
#include "move.h"
#include "trainer_pokemon_sprites.h"
#include "naming_screen.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/pokemon.h"
#include "constants/abilities.h"
#include "constants/species.h"
#include "event_data.h"
#include "battle.h"
#include "run_settings.h"
#include "ironmon.h"
// All state is heap-owned and stays alive while the native Naming screen runs.
enum {CB_CRATE,CB_PICK_CONFIRM,CB_GIFT_INSPECT,CB_REPLACE,CB_ABILITY,CB_MOVES,CB_FLOORS,CB_STATS,CB_REVIEW,CB_SAVE};
struct CustomScreen {
 u16 tilemap[1024];
 struct Pokemon mon;
 struct Pokemon *live;
 struct CustomBuildRecord record;
 u16 floors[6],abilities[3],species[18],gift,giftItem,target;
 u8 page,row,choice,giftSlot,detail,icon;
 bool8 growth,script,ready,failed;
 u8 nickname[POKEMON_NAME_BUFFER_SIZE];
 void (*callback)(void);
};
static EWRAM_DATA struct CustomScreen *sCustom;
static const u8 sColors[]={3,1,3};
static const u8 sWhite[]={5,13,5};
static const u8 sHighlight[]={9,1,9};
static EWRAM_DATA const u8 *sInk;
static const u8 sOrder[]={STAT_HP,STAT_ATK,STAT_DEF,STAT_SPATK,STAT_SPDEF,STAT_SPEED};
static const u8 *const sStatNames[]={COMPOUND_STRING("HP"),COMPOUND_STRING("ATTACK"),COMPOUND_STRING("DEFENSE"),COMPOUND_STRING("SP. ATK"),COMPOUND_STRING("SP. DEF"),COMPOUND_STRING("SPEED")};
static const u8 *const sCategories[]={COMPOUND_STRING("EVOLUTION POTENTIAL"),COMPOUND_STRING("IMMEDIATE POWER"),COMPOUND_STRING("MEGA POTENTIAL")};
static const u16 sPalette[]={RGB(0,0,0),RGB(5,7,9),RGB(23,25,26),RGB(30,31,31),RGB(15,20,25),RGB(25,6,7),RGB(29,21,21),RGB(5,12,24),RGB(21,25,30),RGB(25,28,31),RGB(12,17,22),RGB(7,11,15),RGB(17,20,23),RGB(31,31,31),RGB(27,29,30),RGB(20,23,26)};
static void Init(void);
static void Draw(void);
static void Input(u8 task);
static void Print(const u8 *text,u32 x,u32 y){AddTextPrinterParameterized4(0,FONT_SMALL,x,y,0,0,sInk,TEXT_SKIP_DRAW,text);}
static void Fit(const u8 *text,u32 x,u32 y,u32 width){AddTextPrinterParameterized4(0,GetFontIdToFit(text,FONT_SMALL,0,width),x,y,0,0,sInk,TEXT_SKIP_DRAW,text);}
static void Number(u32 n,u32 x,u32 y){u8 text[12];ConvertIntToDecimalStringN(text,n,STR_CONV_MODE_LEFT_ALIGN,4);Print(text,x,y);}
static void Main(void){RunTasks();AnimateSprites();BuildOamBuffer();UpdatePaletteFade();}
static void VBlank(void){LoadOam();ProcessSpriteCopyRequests();TransferPlttBuffer();}
static void Close(bool32 given)
{
 void (*cb)(void)=sCustom->callback;bool32 script=sCustom->script;
 if(sCustom->icon!=SPRITE_NONE)FreeAndDestroyMonPicSprite(sCustom->icon);
 FreeAllWindowBuffers();UnsetBgTilemapBuffer(0);Free(sCustom);sCustom=NULL;
 if(script)gSpecialVar_Result=given?MON_GIVEN_TO_PARTY:MON_CANT_GIVE;
 SetMainCallback2(cb);
}
static enum Species Species(void){return GetMonData(&sCustom->mon,MON_DATA_SPECIES);}
static u32 Remaining(void){return sCustom->target-CustomBuildSum(&sCustom->record);}
// Fit description paragraphs to native 240x160 with real word wrapping and
// four 14px baselines. LEFT/RIGHT page; no truncation of the remaining text.
static u32 Description(const u8 *source,u32 page,u32 y,u32 rows)
{
 u8 line[80];u32 length=0,lineId=0;const u8 *text=source;
 while(*text!=EOS) {
  if(*text==CHAR_NEWLINE || *text==CHAR_PROMPT_SCROLL || *text==CHAR_PROMPT_CLEAR) {
   if(lineId/rows==page){line[length]=EOS;Fit(line,12,y+(lineId%rows)*14,200);}lineId++;length=0;text++;continue;
  }
  const u8 *word=text;u32 count=0;while(word[count]!=EOS && word[count]!=CHAR_SPACE && word[count]!=CHAR_NEWLINE && count<60)count++;
  if(length && length+count>34){line[length]=EOS;if(lineId/rows==page)Fit(line,12,y+(lineId%rows)*14,200);lineId++;length=0;}
  while(count-- && length<75)line[length++]=*text++;
  if(*text==CHAR_SPACE){if(length<75)line[length++]=*text;text++;}
 }
 line[length]=EOS;if(lineId/rows==page)Fit(line,12,y+(lineId%rows)*14,200);
 return lineId/rows+1;
}
static void Header(const u8 *title)
{
 FillWindowPixelBuffer(0,PIXEL_FILL(11));
 FillWindowPixelRect(0,PIXEL_FILL(4),1,1,222,142);
 FillWindowPixelRect(0,PIXEL_FILL(3),3,3,218,138);
 FillWindowPixelRect(0,PIXEL_FILL(5),3,3,218,15);
 AddTextPrinterParameterized4(0,FONT_SMALL,9,4,0,0,sWhite,TEXT_SKIP_DRAW,title);
}
static void Icon(void)
{
 if(sCustom->icon!=SPRITE_NONE)FreeAndDestroyMonPicSprite(sCustom->icon);
 sCustom->icon=CreateMonPicSprite(Species(),GetMonData(&sCustom->mon,MON_DATA_IS_SHINY),GetMonData(&sCustom->mon,MON_DATA_PERSONALITY),TRUE,sCustom->page>=CB_FLOORS?43:49,sCustom->page>=CB_FLOORS?78:61,0,0xffff);
}
static void Candidate(u32 choice)
{
 sCustom->choice=choice;CustomBuildCandidate(&sCustom->mon,&sCustom->record,gSaveBlock3Ptr->draft.round*3+choice,0,5);
 sCustom->target=CustomBuildSum(&sCustom->record);if(sCustom->ready)Icon();
}
static void BeginBuild(void)
{
 if(sCustom->icon!=SPRITE_NONE){FreeAndDestroyMonPicSprite(sCustom->icon);sCustom->icon=SPRITE_NONE;}
 sCustom->page=CB_ABILITY;sCustom->row=0;sCustom->detail=0;
 CustomBuildAbilities(&sCustom->record,sCustom->abilities,sCustom->growth);
}
static void Draw(void)
{
 u32 page=sCustom->page;sInk=sColors;
 if(page>=CB_FLOORS && page<=CB_REVIEW && sCustom->icon==SPRITE_NONE)Icon();
 if((page==CB_SAVE || page==CB_ABILITY || page==CB_MOVES) && sCustom->icon!=SPRITE_NONE){FreeAndDestroyMonPicSprite(sCustom->icon);sCustom->icon=SPRITE_NONE;}
 Header(page==12?COMPOUND_STRING("GYM LEADER TM REWARD"):sCustom->growth && page==CB_FLOORS?(gSpeciesInfo[Species()].isMegaEvolution?COMPOUND_STRING("PERMANENT MEGA: NEW POINTS"):CustomBuildDepth(Species())?COMPOUND_STRING("NEW EVOLUTION POINTS"):COMPOUND_STRING("FINAL NORMAL BST REVEALED")):page<=CB_PICK_CONFIRM?COMPOUND_STRING("OAK'S DRAFT: CUSTOM BUILD"):page==CB_GIFT_INSPECT || page==CB_REPLACE?COMPOUND_STRING("A NEW TEAMMATE"):sCustom->growth?COMPOUND_STRING("EVOLUTION: GROW YOUR BUILD"):COMPOUND_STRING("CREATE YOUR POKEMON"));
 if(page==12) {
  enum Move move=GetItemTMHMMoveId(sCustom->giftItem);u8 itemName[32];CopyItemName(sCustom->giftItem,itemName);Fit(itemName,9,23,65);Fit(GetMoveName(move),83,23,130);
  Fit(gTypesInfo[GetMoveType(move)].name,9,41,66);Print(COMPOUND_STRING("PP"),83,41);Number(GetMovePP(move),104,41);Print(COMPOUND_STRING("POWER"),140,41);Number(GetMovePower(move),183,41);
  Print(COMPOUND_STRING("ACCURACY"),9,55);Number(GetMoveAccuracy(move),85,55);Description(GetMoveDescription(move),sCustom->detail,71,4);Print(COMPOUND_STRING("A: RETURN   LEFT/RIGHT: DETAILS"),9,128);
 } else if(page==11) {
  Print(COMPOUND_STRING("YOUR SIX CUSTOM BUILDS"),9,23);
  for(u32 slot=0;slot<gPartiesCount[B_TRAINER_PLAYER];slot++){u8 name[POKEMON_NAME_BUFFER_SIZE];struct Pokemon *mon=&gParties[B_TRAINER_PLAYER][slot];GetMonData(mon,MON_DATA_NICKNAME,name);Fit(name,12,40+slot*12,104);struct CustomBuildRecord *r=CustomBuildFind(GetMonData(mon,MON_DATA_PERSONALITY));if(r)Number(CustomBuildSum(r),164,40+slot*12);}
  Print(COMPOUND_STRING("B: RETURN TO YOUR ADVENTURE"),9,124);
 } else if(page==CB_CRATE || page==CB_PICK_CONFIRM || page==CB_GIFT_INSPECT) {
  Fit(GetSpeciesName(Species()),88,23,125);
  Fit(gTypesInfo[GetSpeciesType(Species(),0)].name,88,39,64);
  Fit(gTypesInfo[GetSpeciesType(Species(),1)].name,153,39,60);
  Print(COMPOUND_STRING("CURRENT BST"),88,55);Number(sCustom->target,169,55);
  Fit(page==CB_GIFT_INSPECT?COMPOUND_STRING("GIFT EXCHANGE"):sCategories[sCustom->choice],88,71,125);
  Print(COMPOUND_STRING("EVOLUTIONS:"),88,86);Number(CustomBuildDepth(Species()),180,86);
  Print(CustomBuildHasMega(Species())?COMPOUND_STRING("MEGA: ELIGIBLE"):COMPOUND_STRING("MEGA: NONE"),8,105);
  Print(page==CB_CRATE?COMPOUND_STRING("LEFT/RIGHT: COMPARE   A: CHOOSE"):page==CB_GIFT_INSPECT?COMPOUND_STRING("A: EXCHANGE   B: DECLINE"):COMPOUND_STRING("A: COMMIT PICK   B: GO BACK"),8,124);
 } else if(page==CB_REPLACE) {
  Print(COMPOUND_STRING("Choose a living friend to surrender."),9,22);
  for(u32 i=0;i<gPartiesCount[B_TRAINER_PLAYER];i++) {
   u8 name[POKEMON_NAME_BUFFER_SIZE];GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_NICKNAME,name);
   Print(i==sCustom->row?COMPOUND_STRING("> "):COMPOUND_STRING("  "),9,38+i*13);Fit(name,24,38+i*13,115);
   Print(GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_HP)?COMPOUND_STRING("ALIVE"):COMPOUND_STRING("FAINTED"),153,38+i*13);
  }
  Print(COMPOUND_STRING("A: BUILD REPLACEMENT   B: DECLINE"),8,124);
 } else if(page==CB_ABILITY) {
  for(u32 i=0;i<3;i++) {
   if(i==sCustom->row)FillWindowPixelRect(0,PIXEL_FILL(9),5,22+i*14,214,14);
   AddTextPrinterParameterized4(0,GetFontIdToFit(gAbilitiesInfo[sCustom->abilities[i]].name,FONT_SMALL,0,165),10,22+i*14,0,0,i==sCustom->row?sHighlight:sColors,TEXT_SKIP_DRAW,gAbilitiesInfo[sCustom->abilities[i]].name);
   if(sCustom->growth && !i)Print(COMPOUND_STRING("KEEP"),181,22+i*14);
  }
  u32 pages=Description(gAbilitiesInfo[sCustom->abilities[sCustom->row]].description,sCustom->detail,68,4);
  Print(COMPOUND_STRING("A: CHOOSE   LEFT/RIGHT: DETAILS"),8,124);{u8 counter[16],value[8];ConvertIntToDecimalStringN(counter,sCustom->detail+1,STR_CONV_MODE_LEFT_ALIGN,2);StringAppend(counter,COMPOUND_STRING("/"));ConvertIntToDecimalStringN(value,pages,STR_CONV_MODE_LEFT_ALIGN,2);StringAppend(counter,value);AddTextPrinterParameterized4(0,FONT_SMALL,188,4,0,0,sWhite,TEXT_SKIP_DRAW,counter);}
 } else if(page==CB_MOVES) {
  enum Move move=GetMonData(&sCustom->mon,MON_DATA_MOVE1+sCustom->row);
  for(u32 i=0;i<4;i++) {
   enum Move m=GetMonData(&sCustom->mon,MON_DATA_MOVE1+i);
   Print(i==sCustom->row?COMPOUND_STRING(">"):COMPOUND_STRING(" "),8,21+i*13);Fit(GetMoveName(m),20,21+i*13,118);
   Fit(gTypesInfo[GetMoveType(m)].name,139,21+i*13,70);
  }
  Print(COMPOUND_STRING("PP"),9,75);Number(GetMovePP(move),25,75);Print(COMPOUND_STRING("POWER"),65,75);Number(GetMovePower(move),106,75);Print(COMPOUND_STRING("ACC"),147,75);Number(GetMoveAccuracy(move),175,75);
  Description(GetMoveDescription(move),sCustom->detail,89,2);
  // Two-line move description viewport: additional text appears on the next detail page.
  FillWindowPixelRect(0,PIXEL_FILL(3),4,117,216,23);
  Print(COMPOUND_STRING("UP/DOWN: MOVE   L/R: DETAILS   A: NEXT"),8,124);
 } else if(page==CB_FLOORS || page==CB_STATS || page==CB_REVIEW) {
  Fit(GetSpeciesName(Species()),9,22,98);Print(COMPOUND_STRING("BST"),111,22);Number(sCustom->target,139,22);Print(COMPOUND_STRING("LEFT"),170,22);Number(Remaining(),200,22);
  for(u32 row=0;row<6;row++) {
   u32 i=sOrder[row],y=38+row*13;
   if(row==sCustom->row && page==CB_STATS)FillWindowPixelRect(0,PIXEL_FILL(9),81,y,138,13);
   sInk=row==sCustom->row && page==CB_STATS?sHighlight:sColors;
   Fit(sStatNames[row],85,y,51);Number(sCustom->record.stats[i],138,y);Number(sCustom->floors[i],168,y);
   FillWindowPixelRect(0,PIXEL_FILL(4),192,y+4,25,5);
   FillWindowPixelRect(0,PIXEL_FILL(5),192,y+4,min(25,sCustom->record.stats[i]*25/max(1,sCustom->target)),5);
  }
  sInk=sColors;
  Print(page==CB_FLOORS?COMPOUND_STRING("MINIMUMS ASSIGNED     A: ALLOCATE"):page==CB_REVIEW?COMPOUND_STRING("A: SAVE BUILD     B: RETURN"):COMPOUND_STRING("L/R: 10   SELECT: RESET   START: REVIEW"),7,124);
 } else {
  Print(sCustom->gift?COMPOUND_STRING("Your old friend leaves for good."):COMPOUND_STRING("Every point is assigned."),12,32);
  Print(sCustom->failed?COMPOUND_STRING("Make room for its held item first."):COMPOUND_STRING("This build will become permanent."),12,53);
  Print(sCustom->gift?COMPOUND_STRING("A: CONFIRM     B: CANCEL EXCHANGE"):COMPOUND_STRING("A: CONFIRM     B: RETURN"),12,107);
 }
 PutWindowTilemap(0);CopyWindowToVram(0,COPYWIN_FULL);
}
static void Named(void)
{
 if(sCustom->nickname[0]!=EOS)SetMonData(&sCustom->mon,MON_DATA_NICKNAME,sCustom->nickname);
 BeginBuild();SetMainCallback2(Init);
}
static void Name(void)
{
 sCustom->nickname[0]=EOS;sCustom->ready=FALSE;
 DoNamingScreen(NAMING_SCREEN_CAUGHT_MON,sCustom->nickname,Species(),GetMonGender(&sCustom->mon),GetMonData(&sCustom->mon,MON_DATA_PERSONALITY),Named);
}
static void Input(u8 task)
{
 if(gPaletteFade.active)return;
 u32 page=sCustom->page;bool32 changed=TRUE;
 if(page==12){if(JOY_NEW(A_BUTTON|B_BUTTON)){DestroyTask(task);Close(FALSE);return;}if(JOY_NEW(DPAD_LEFT|DPAD_RIGHT)){u32 pages=Description(GetMoveDescription(GetItemTMHMMoveId(sCustom->giftItem)),255,71,4);sCustom->detail=(sCustom->detail+(JOY_NEW(DPAD_LEFT)?pages-1:1))%pages;Draw();}return;}
 if(page==11){if(JOY_NEW(A_BUTTON|B_BUTTON)){DestroyTask(task);Close(FALSE);return;}return;}
 if(page==CB_CRATE) {
  if(JOY_NEW(B_BUTTON)){DestroyTask(task);Close(FALSE);return;}
  if(JOY_NEW(DPAD_LEFT|DPAD_RIGHT)){Candidate((sCustom->choice+(JOY_NEW(DPAD_LEFT)?2:1))%3);}
  else if(JOY_NEW(A_BUTTON))sCustom->page=CB_PICK_CONFIRM;
  else changed=FALSE;
 } else if(page==CB_PICK_CONFIRM) {
  if(JOY_NEW(B_BUTTON))sCustom->page=CB_CRATE;
  else if(JOY_NEW(A_BUTTON)){if(sCustom->icon!=SPRITE_NONE){FreeAndDestroyMonPicSprite(sCustom->icon);sCustom->icon=SPRITE_NONE;}DestroyTask(task);Name();return;}
  else changed=FALSE;
 } else if(page==CB_GIFT_INSPECT) {
  if(JOY_NEW(B_BUTTON)){DestroyTask(task);Close(FALSE);return;}
  if(JOY_NEW(A_BUTTON)){if(sCustom->icon!=SPRITE_NONE){FreeAndDestroyMonPicSprite(sCustom->icon);sCustom->icon=SPRITE_NONE;}sCustom->page=CB_REPLACE;sCustom->row=0;}
  else changed=FALSE;
 } else if(page==CB_REPLACE) {
  u32 count=gPartiesCount[B_TRAINER_PLAYER];
  if(JOY_NEW(B_BUTTON)){DestroyTask(task);Close(FALSE);return;}
  if(JOY_NEW(DPAD_UP))sCustom->row=(sCustom->row+count-1)%count;
  else if(JOY_NEW(DPAD_DOWN))sCustom->row=(sCustom->row+1)%count;
  else if(JOY_NEW(A_BUTTON) && GetMonData(&gParties[B_TRAINER_PLAYER][sCustom->row],MON_DATA_HP)) {
   sCustom->giftSlot=sCustom->row;u32 level=GetMonData(&gParties[B_TRAINER_PLAYER][sCustom->row],MON_DATA_LEVEL);
   CustomBuildCandidate(&sCustom->mon,&sCustom->record,0,sCustom->gift,level);sCustom->target=CustomBuildSum(&sCustom->record);
   DestroyTask(task);Name();return;
  }else changed=FALSE;
 } else if(page==CB_ABILITY) {
  if(JOY_NEW(DPAD_UP|DPAD_DOWN)){sCustom->row=(sCustom->row+(JOY_NEW(DPAD_UP)?2:1))%3;sCustom->detail=0;}
  else if(JOY_NEW(DPAD_LEFT|DPAD_RIGHT)){u32 pages=Description(gAbilitiesInfo[sCustom->abilities[sCustom->row]].description,255,68,4);sCustom->detail=(sCustom->detail+(JOY_NEW(DPAD_LEFT)?pages-1:1))%pages;}
  else if(JOY_NEW(A_BUTTON)) {
   sCustom->record.ability=sCustom->abilities[sCustom->row];sCustom->page=sCustom->growth?CB_FLOORS:CB_MOVES;sCustom->row=sCustom->detail=0;
   if(sCustom->growth)memcpy(sCustom->floors,sCustom->record.stats,sizeof(sCustom->floors));
  }else changed=FALSE;
 } else if(page==CB_MOVES) {
  if(JOY_NEW(L_BUTTON|R_BUTTON|DPAD_LEFT|DPAD_RIGHT)){u32 pages=Description(GetMoveDescription(GetMonData(&sCustom->mon,MON_DATA_MOVE1+sCustom->row)),255,89,2);sCustom->detail=(sCustom->detail+(JOY_NEW(L_BUTTON|DPAD_LEFT)?pages-1:1))%pages;}
  else if(JOY_NEW(DPAD_UP|DPAD_DOWN)){sCustom->row=(sCustom->row+(JOY_NEW(DPAD_UP)?3:1))%4;sCustom->detail=0;}
  else if(JOY_NEW(A_BUTTON)) {
   CustomBuildFloors(&sCustom->record,sCustom->floors);memcpy(sCustom->record.stats,sCustom->floors,sizeof(sCustom->floors));sCustom->row=0;sCustom->page=CB_FLOORS;
  }else changed=FALSE;
 } else if(page==CB_FLOORS) {
  if(JOY_NEW(A_BUTTON))sCustom->page=CB_STATS;else changed=FALSE;
 } else if(page==CB_STATS) {
  u32 i=sOrder[sCustom->row];
  if(JOY_NEW(DPAD_UP|DPAD_DOWN))sCustom->row=(sCustom->row+(JOY_NEW(DPAD_UP)?5:1))%6;
  else if(JOY_REPEAT(DPAD_LEFT|DPAD_RIGHT|L_BUTTON|R_BUTTON)) {
   u32 amount=(JOY_HELD(L_BUTTON|R_BUTTON)?10:1);
   if(JOY_HELD(DPAD_LEFT|L_BUTTON))sCustom->record.stats[i]-=min(amount,sCustom->record.stats[i]-sCustom->floors[i]);
   else sCustom->record.stats[i]+=min(amount,Remaining());
  } else if(JOY_NEW(SELECT_BUTTON))memcpy(sCustom->record.stats,sCustom->floors,sizeof(sCustom->floors));
  else if(JOY_NEW(START_BUTTON|A_BUTTON) && !Remaining())sCustom->page=CB_REVIEW;
  else changed=FALSE;
 } else if(page==CB_REVIEW) {
  if(JOY_NEW(B_BUTTON))sCustom->page=CB_STATS;
  else if(JOY_NEW(A_BUTTON))sCustom->page=CB_SAVE;else changed=FALSE;
 } else if(page==CB_SAVE) {
  if(JOY_NEW(B_BUTTON)){if(sCustom->gift){DestroyTask(task);Close(FALSE);return;}sCustom->page=CB_REVIEW;}
  else if(JOY_NEW(A_BUTTON)) {
   struct Pokemon *mon=sCustom->growth?sCustom->live:&sCustom->mon;
   if(CustomBuildCommit(mon,&sCustom->record,sCustom->growth || sCustom->gift?255:sCustom->choice,sCustom->gift?sCustom->giftSlot:255,sCustom->giftItem)){DestroyTask(task);Close(TRUE);return;}
   sCustom->failed=TRUE;
  }else changed=FALSE;
 }
 if(changed){PlaySE(SE_SELECT);Draw();}
}
static void Init(void)
{
 static const struct BgTemplate bg[]={{.bg=0,.charBaseIndex=0,.mapBaseIndex=31,.screenSize=0,.paletteMode=0,.priority=0}};
 static const struct WindowTemplate windows[]={{.bg=0,.tilemapLeft=1,.tilemapTop=1,.width=28,.height=18,.paletteNum=9,.baseBlock=1},DUMMY_WIN_TEMPLATE};
 SetVBlankCallback(NULL);ResetVramOamAndBgCntRegs();ResetTasks();ResetSpriteData();FreeAllSpritePalettes();ResetBgsAndClearDma3BusyFlags(0);
 InitBgsFromTemplates(0,bg,1);SetBgTilemapBuffer(0,sCustom->tilemap);InitWindows(windows);DeactivateAllTextPrinters();
 LoadPalette(sPalette,BG_PLTT_ID(9),sizeof(sPalette));LoadPalette(sPalette,BG_PLTT_ID(0),sizeof(sPalette));sCustom->icon=SPRITE_NONE;
 sCustom->ready=TRUE;
 if(sCustom->page<=CB_GIFT_INSPECT)Icon();
 Draw();SetGpuReg(REG_OFFSET_DISPCNT,DISPCNT_OBJ_ON|DISPCNT_OBJ_1D_MAP);ShowBg(0);ResetPaletteFade();BeginNormalPaletteFade(PALETTES_ALL,0,16,0,RGB_BLACK);
 SetVBlankCallback(VBlank);CreateTask(Input,0);SetMainCallback2(Main);
}
static bool32 Open(bool32 script,void (*cb)(void))
{
 if(sCustom || !IsCustomBuild())return FALSE;
 sCustom=AllocZeroed(sizeof(*sCustom));if(!sCustom)return FALSE;
 sCustom->callback=cb;sCustom->script=script;sCustom->icon=SPRITE_NONE;sCustom->giftSlot=255;
 if(script)ScriptContext_Stop();SetMainCallback2(Init);return TRUE;
}
void CustomBuildOpenCrate(void)
{
 gSpecialVar_Result=MON_CANT_GIVE;
 if(gSaveBlock3Ptr->draft.round>=6 || !Open(TRUE,CB2_ReturnToFieldContinueScriptPlayMapMusic))return;
 sCustom->page=CB_CRATE;CustomBuildGenerate(sCustom->species);Candidate(0);
}
void CustomBuildOpenGift(void)
{
 gSpecialVar_Result=MON_CANT_GIVE;
 if(gSaveBlock3Ptr->draft.round!=6 || !gPartiesCount[B_TRAINER_PLAYER] || !Open(TRUE,CB2_ReturnToFieldContinueScriptPlayMapMusic))return;
 sCustom->gift=gSpecialVar_0x8004;sCustom->giftItem=ChaosRemovedItem(gSpecialVar_0x8005)?ITEM_NONE:gSpecialVar_0x8005;sCustom->page=CB_GIFT_INSPECT;
 CustomBuildCandidate(&sCustom->mon,&sCustom->record,0,sCustom->gift,5);sCustom->target=CustomBuildSum(&sCustom->record);
}
bool32 CustomBuildOpenGrowth(struct Pokemon *mon,void (*cb)(void))
{
 struct CustomBuildRecord record;
 if(!CustomBuildGrowth(mon,&record) || !Open(FALSE,cb))return FALSE;
 sCustom->record=record;sCustom->mon=*mon;sCustom->live=mon;sCustom->growth=TRUE;
 sCustom->target=gSpeciesInfo[GetMonData(mon,MON_DATA_SPECIES)].isMegaEvolution?record.finalBST+record.megaGain:record.finalBST-CustomBuildReserve(GetMonData(mon,MON_DATA_SPECIES));
 BeginBuild();return TRUE;
}
bool32 CustomBuildResumePending(void)
{
 if(!IsCustomBuild() || sCustom || gMain.inBattle || gPaletteFade.active || ScriptContext_IsEnabled() || ArePlayerFieldControlsLocked())return FALSE;
 for(u32 i=0;i<gPartiesCount[B_TRAINER_PLAYER];i++)if(CustomBuildOpenGrowth(&gParties[B_TRAINER_PLAYER][i],CB2_ReturnToField))return TRUE;
 return FALSE;
}

void CustomBuildOpenReport(void)
{
 if(Open(TRUE,CB2_ReturnToFieldContinueScriptPlayMapMusic))sCustom->page=11;
}

void CustomBuildShowTM(void){u32 gym=gSpecialVar_0x8004;if(!IsCustomBuild() || gym>=8 || !gSaveBlock3Ptr->draft.gymTms[gym])return;if(Open(TRUE,CB2_ReturnToFieldContinueScriptPlayMapMusic)){sCustom->giftItem=gSaveBlock3Ptr->draft.gymTms[gym];sCustom->page=12;}}
