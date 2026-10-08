#include "global.h"
#include "chaos_v2.h"
#include "battle.h"
#include "battle_main.h"
#include "constants/rtc.h"
#include "pokemon_storage_system.h"
#include "daycare.h"
#include "event_data.h"
#include "money.h"
#include "random.h"
#include "item.h"
#include "run_settings.h"
#include "script.h"
#include "string_util.h"
#include "window.h"
#include "palette.h"
#include "menu.h"
#include "text_window.h"
#include "task.h"
#include "sound.h"
#include "move.h"
#include "constants/items.h"
#include "constants/map_groups.h"
#include "constants/moves.h"
#include "constants/abilities.h"
#include "constants/songs.h"
#include "constants/rgb.h"

static const u16 sBabyEggs[] = {SPECIES_TOGEPI,SPECIES_PICHU,SPECIES_AZURILL,SPECIES_RIOLU,SPECIES_BUDEW,SPECIES_CLEFFA,SPECIES_IGGLYBUFF};
static const u16 sStarterEggs[] = {SPECIES_BULBASAUR,SPECIES_CHARMANDER,SPECIES_SQUIRTLE,SPECIES_CHIKORITA,SPECIES_CYNDAQUIL,SPECIES_TOTODILE,SPECIES_TREECKO,SPECIES_TORCHIC,SPECIES_MUDKIP,SPECIES_TURTWIG,SPECIES_CHIMCHAR,SPECIES_PIPLUP,SPECIES_SNIVY,SPECIES_TEPIG,SPECIES_OSHAWOTT,SPECIES_CHESPIN,SPECIES_FENNEKIN,SPECIES_FROAKIE,SPECIES_ROWLET,SPECIES_LITTEN,SPECIES_POPPLIO,SPECIES_GROOKEY,SPECIES_SCORBUNNY,SPECIES_SOBBLE,SPECIES_SPRIGATITO,SPECIES_FUECOCO,SPECIES_QUAXLY};
static const u16 sRegionalEggs[] = {SPECIES_VULPIX_ALOLA,SPECIES_ZORUA_HISUI,SPECIES_PONYTA_GALAR,SPECIES_WOOPER_PALDEA,SPECIES_SANDSHREW_ALOLA,SPECIES_MEOWTH_GALAR,SPECIES_GROWLITHE_HISUI,SPECIES_SLOWPOKE_GALAR};

void ChaosBuyMysteryEgg(void)
{
    u32 vendor = gSpecialVar_0x8004;
    u32 flags = VarGet(VAR_CHAOS_EGG_VENDORS);
    struct Pokemon mon;
    const u16 *pool;
    u32 count;
    gSpecialVar_Result = 3;
    if (vendor > 2 || flags & (1u << vendor)) return;
    gSpecialVar_Result = 1;
    if (GetMoney(&gSaveBlock1Ptr->money) < 5000) return;
    if (vendor == 0) {pool=sBabyEggs;count=ARRAY_COUNT(sBabyEggs);}
    else if (vendor == 1) {pool=sStarterEggs;count=ARRAY_COUNT(sStarterEggs);}
    else {pool=sRegionalEggs;count=ARRAY_COUNT(sRegionalEggs);}
    enum Species species = pool[Random() % count];
    gSpecialVar_Result = 2;
    if (SanitizeSpeciesId(species) != species) return;
    CreateEgg(&mon, species, TRUE);
    TrySetMonAbilityToActiveRunFilter(&mon);
    if (GiveScriptedMonToPlayer(&mon, PARTY_SIZE) == MON_CANT_GIVE) return;
    RemoveMoney(&gSaveBlock1Ptr->money, 5000);
    VarSet(VAR_CHAOS_EGG_VENDORS, flags | (1u << vendor));
    gSpecialVar_Result = 0;
}

struct CareItem {u16 item; u16 quantity;};
static const struct CareItem sCare1[] = {
 {ITEM_QUICK_BALL,100},{ITEM_RARE_CANDY,0},
 {ITEM_CHERI_BERRY,10},{ITEM_CHESTO_BERRY,10},{ITEM_PECHA_BERRY,10},{ITEM_RAWST_BERRY,10},{ITEM_ASPEAR_BERRY,10},{ITEM_LEPPA_BERRY,10},{ITEM_ORAN_BERRY,10},{ITEM_PERSIM_BERRY,10},{ITEM_LUM_BERRY,10},{ITEM_SITRUS_BERRY,10},{ITEM_FIGY_BERRY,10},{ITEM_WIKI_BERRY,10},{ITEM_MAGO_BERRY,10},{ITEM_AGUAV_BERRY,10},{ITEM_IAPAPA_BERRY,10},{ITEM_ENIGMA_BERRY,10},
 {ITEM_OCCA_BERRY,10},{ITEM_PASSHO_BERRY,10},{ITEM_WACAN_BERRY,10},{ITEM_RINDO_BERRY,10},{ITEM_YACHE_BERRY,10},{ITEM_CHOPLE_BERRY,10},{ITEM_KEBIA_BERRY,10},{ITEM_SHUCA_BERRY,10},{ITEM_COBA_BERRY,10},{ITEM_PAYAPA_BERRY,10},{ITEM_TANGA_BERRY,10},{ITEM_CHARTI_BERRY,10},{ITEM_KASIB_BERRY,10},{ITEM_HABAN_BERRY,10},{ITEM_COLBUR_BERRY,10},{ITEM_BABIRI_BERRY,10},{ITEM_CHILAN_BERRY,10},{ITEM_ROSELI_BERRY,10},{ITEM_MOON_STONE,1}};
static const struct CareItem sCare2[] = {{ITEM_GREAT_BALL,100},{ITEM_PREMIER_BALL,10},{ITEM_NET_BALL,100},{ITEM_NEST_BALL,100},{ITEM_DIVE_BALL,100},{ITEM_RARE_CANDY,0},{ITEM_FIRE_STONE,1},{ITEM_WATER_STONE,1},{ITEM_THUNDER_STONE,1},{ITEM_LEAF_STONE,1}};
static const struct CareItem sCare3[] = {{ITEM_ULTRA_BALL,100},{ITEM_DUSK_BALL,100},{ITEM_TIMER_BALL,100},{ITEM_QUICK_BALL,100},{ITEM_REPEAT_BALL,100},{ITEM_LUXURY_BALL,100},{ITEM_RARE_CANDY,0},{ITEM_MOON_STONE,1},{ITEM_SUN_STONE,1},{ITEM_SHINY_STONE,1},{ITEM_DUSK_STONE,1},{ITEM_DAWN_STONE,1}};
static EWRAM_DATA u8 sCarePackage;
static EWRAM_DATA u8 sCarePage;
static EWRAM_DATA u16 sCareCandyQuantity;
extern const u8 EventScript_ChaosV2CareReceived[];
extern const u8 EventScript_ChaosV2CareFull[];
static const struct CareItem *CareItems(u32 bundle, u32 *count)
{
 if(bundle==0){*count=ARRAY_COUNT(sCare1);return sCare1;}
 if(bundle==1){*count=ARRAY_COUNT(sCare2);return sCare2;}
 *count=ARRAY_COUNT(sCare3);return sCare3;
}
bool32 ChaosTryCareMilestone(s16 x, s16 y)
{
 static EWRAM_DATA u16 sFailedMap = 0;
 if (!IS_FRLG) return FALSE;
 u16 map = gSaveBlock1Ptr->location.mapNum | gSaveBlock1Ptr->location.mapGroup << 8;
 if (sFailedMap != map) sFailedMap = 0;
 s32 bundle = -1;
 if(map==MAP_ROUTE2 && y <= 14 && x < 10) bundle=0;
 if(map==MAP_ROUTE4 && x >= 31) bundle=1;
 if(map==MAP_ROUTE10 && y < 25) bundle=2;
 if(bundle<0) return FALSE;
 u16 flags=VarGet(VAR_CHAOS_V2_CARE);
 VarSet(VAR_CHAOS_V2_CARE,flags | (1u << (bundle+3)));
 if(!VarGet(VAR_CHAOS_CARE_PACKAGES) || (flags & (1u<<bundle))) return FALSE;
 // A failed delivery is retried on a new map visit, rather than every footstep.
 if(sFailedMap==map)return FALSE;
 u32 count; const struct CareItem *items=CareItems(bundle,&count);
 sCareCandyQuantity=MAX_BAG_ITEM_CAPACITY-min(MAX_BAG_ITEM_CAPACITY,CountTotalItemQuantityInBag(ITEM_RARE_CANDY));
 u32 i;
 for(i=0;i<count;i++)
 {
  u32 quantity=items[i].item==ITEM_RARE_CANDY?sCareCandyQuantity:items[i].quantity;
  if(quantity && !AddBagItem(items[i].item,quantity))break;
 }
 if(i<count)
 {
  while(i>0){i--;u32 quantity=items[i].item==ITEM_RARE_CANDY?sCareCandyQuantity:items[i].quantity;if(quantity)RemoveBagItem(items[i].item,quantity);}
  sFailedMap=map;
  ScriptContext_SetupScript(EventScript_ChaosV2CareFull);
  return TRUE;
 }
 sFailedMap=0;sCarePackage=bundle;sCarePage=0;
 VarSet(VAR_CHAOS_V2_CARE,VarGet(VAR_CHAOS_V2_CARE)|(1u<<bundle));
 ScriptContext_SetupScript(EventScript_ChaosV2CareReceived);
 return TRUE;
}
void ChaosCareNextPage(void)
{
 u32 count;const struct CareItem *items=CareItems(sCarePackage,&count);
 gSpecialVar_Result=sCarePage<count;
 if(!gSpecialVar_Result)return;
 u8 *end=gStringVar4;*end=EOS;
 for(u32 line=0;line<4 && sCarePage<count;line++,sCarePage++)
 {
  CopyItemName(items[sCarePage].item,gStringVar1);
  u32 quantity=items[sCarePage].item==ITEM_RARE_CANDY?sCareCandyQuantity:items[sCarePage].quantity;
  ConvertIntToDecimalStringN(gStringVar2,quantity,STR_CONV_MODE_LEFT_ALIGN,3);
  end=StringCopy(end,gStringVar1);end=StringCopy(end,COMPOUND_STRING(" x"));end=StringCopy(end,gStringVar2);
  if(line<3 && sCarePage+1<count)*end++=CHAR_NEWLINE;
 }
 *end=EOS;
}

static EWRAM_DATA u8 sAbilityWindow;
static EWRAM_DATA u8 sAbilityCursor;
static EWRAM_DATA u8 sAbilitySlot;
static const u8 sAbilityColors[] = {TEXT_COLOR_WHITE,TEXT_COLOR_DARK_GRAY,TEXT_COLOR_LIGHT_GRAY};
static const u8 sAbilityGray[] = {TEXT_COLOR_WHITE,TEXT_COLOR_LIGHT_GRAY,TEXT_COLOR_WHITE};
static bool32 AbilityAllowed(enum Ability ability)
{
 enum Ability filter=GetActiveRunFilterAbilityForMonChanges();
 return ability!=ABILITY_NONE && (filter==ABILITY_NONE || ability==filter);
}
static void DrawAbilityMenu(void)
{
 struct Pokemon *mon=&gParties[B_TRAINER_PLAYER][sAbilitySlot];
 enum Species species=GetMonData(mon,MON_DATA_SPECIES);
 enum Ability current=GetMonAbility(mon);
 FillWindowPixelBuffer(sAbilityWindow,PIXEL_FILL(TEXT_COLOR_WHITE));
 AddTextPrinterParameterized4(sAbilityWindow,FONT_NORMAL,4,0,0,0,sAbilityColors,0,COMPOUND_STRING("ABILITY"));
 StringCopy(gStringVar4,COMPOUND_STRING("Current: "));StringAppend(gStringVar4,gAbilitiesInfo[current].name);
 AddTextPrinterParameterized4(sAbilityWindow,FONT_SMALL,4,18,0,0,sAbilityColors,0,gStringVar4);
 for(u32 slot=0;slot<3;slot++)
 {
  enum Ability ability=GetSpeciesAbility(species,slot);
  StringCopy(gStringVar4,slot==sAbilityCursor?COMPOUND_STRING("{RIGHT_ARROW} "):COMPOUND_STRING("  "));
  StringAppend(gStringVar4,ability==ABILITY_NONE?COMPOUND_STRING("--"):gAbilitiesInfo[ability].name);
  if(ability==current)StringAppend(gStringVar4,COMPOUND_STRING(" (SET)"));
  AddTextPrinterParameterized4(sAbilityWindow,FONT_NORMAL,4,34+slot*16,0,0,AbilityAllowed(ability)?sAbilityColors:sAbilityGray,0,gStringVar4);
 }
 enum Ability ability=GetSpeciesAbility(species,sAbilityCursor);
 u8 description[256];StringCopy(description,gAbilitiesInfo[ability].description);
 WrapFontIdToFit(description,description+StringLength(description),FONT_SMALL,216);
 AddTextPrinterParameterized4(sAbilityWindow,FONT_SMALL,4,86,0,0,sAbilityColors,0,description);
 if(!AbilityAllowed(ability))AddTextPrinterParameterized4(sAbilityWindow,FONT_SMALL,4,128,0,0,sAbilityGray,0,COMPOUND_STRING("Blocked by your active filter."));
 PutWindowTilemap(sAbilityWindow);CopyWindowToVram(sAbilityWindow,COPYWIN_FULL);
}
static void Task_AbilityMenu(u8 taskId)
{
 if(JOY_NEW(DPAD_UP)){sAbilityCursor=(sAbilityCursor+2)%3;DrawAbilityMenu();}
 else if(JOY_NEW(DPAD_DOWN)){sAbilityCursor=(sAbilityCursor+1)%3;DrawAbilityMenu();}
 else if(JOY_NEW(A_BUTTON))
 {
  enum Ability ability=GetSpeciesAbility(GetMonData(&gParties[B_TRAINER_PLAYER][sAbilitySlot],MON_DATA_SPECIES),sAbilityCursor);
  if(!AbilityAllowed(ability)){PlaySE(SE_FAILURE);return;}
  u16 override = ABILITY_NONE;
  SetMonData(&gParties[B_TRAINER_PLAYER][sAbilitySlot],MON_DATA_CHAOS_STARTER_ABILITY,&override);
  SetMonData(&gParties[B_TRAINER_PLAYER][sAbilitySlot],MON_DATA_ABILITY_NUM,&sAbilityCursor);
  PlaySE(SE_SELECT);gSpecialVar_Result=TRUE;
  ClearStdWindowAndFrame(sAbilityWindow,TRUE);RemoveWindow(sAbilityWindow);DestroyTask(taskId);ScriptContext_Enable();
 }
 else if(JOY_NEW(B_BUTTON))
 {gSpecialVar_Result=FALSE;ClearStdWindowAndFrame(sAbilityWindow,TRUE);RemoveWindow(sAbilityWindow);DestroyTask(taskId);ScriptContext_Enable();}
}
void ChaosOpenAbilityMenu(void)
{
 struct WindowTemplate window={.bg=0,.tilemapLeft=1,.tilemapTop=1,.width=28,.height=18,.paletteNum=15,.baseBlock=0x100};
 sAbilitySlot=gSpecialVar_0x8004;if(sAbilitySlot>=PARTY_SIZE)sAbilitySlot=0;
 sAbilityCursor=GetMonData(&gParties[B_TRAINER_PLAYER][sAbilitySlot],MON_DATA_ABILITY_NUM);
 if(sAbilityCursor>2)sAbilityCursor=0;
 sAbilityWindow=AddWindow(&window);
 LoadUserWindowBorderGfx(sAbilityWindow,0x85,BG_PLTT_ID(14));
 DrawStdFrameWithCustomTileAndPalette(sAbilityWindow,FALSE,0x85,14);
 DrawAbilityMenu();CreateTask(Task_AbilityMenu,8);ScriptContext_Stop();
}

#define CLAMP(value, low, high) min(max(value, low), high)

struct RatingMoves {u16 bestPhysical;u16 bestSpecial;u32 coverage;u8 recovery;u8 setup;u8 utility;u8 priority;u8 technician;};
static void RatingMove(struct RatingMoves *data, enum Move move, enum Type type1, enum Type type2)
{
 if(move==MOVE_NONE || move>=MOVES_COUNT)return;
 u32 power=GetMovePower(move),category=GetMoveCategory(move);
 if(category!=DAMAGE_CATEGORY_STATUS)
 {
  u32 quality=min(power,120);
  u32 accuracy=GetMoveAccuracy(move);if(accuracy)quality=quality*accuracy/100;
  if(GetMoveType(move)==type1 || GetMoveType(move)==type2)quality=quality*3/2;
  if(category==DAMAGE_CATEGORY_PHYSICAL)data->bestPhysical=max(data->bestPhysical,quality);
  else data->bestSpecial=max(data->bestSpecial,quality);
  if(power>=60 && GetMoveType(move)<NUMBER_OF_MON_TYPES)data->coverage|=1u<<GetMoveType(move);
  if(GetMovePriority(move)>0 && power>=30)data->priority=1;
  if(power>=40 && power<=60)data->technician=1;
 }
 else
 {
  if(move==MOVE_RECOVER || move==MOVE_ROOST || move==MOVE_SOFT_BOILED || move==MOVE_SLACK_OFF || move==MOVE_MILK_DRINK || move==MOVE_SYNTHESIS || move==MOVE_MOONLIGHT || move==MOVE_MORNING_SUN || move==MOVE_STRENGTH_SAP)data->recovery=1;
  if(move==MOVE_SWORDS_DANCE || move==MOVE_NASTY_PLOT || move==MOVE_QUIVER_DANCE || move==MOVE_DRAGON_DANCE || move==MOVE_SHELL_SMASH || move==MOVE_CALM_MIND || move==MOVE_BULK_UP)data->setup=1;
  if(move==MOVE_THUNDER_WAVE || move==MOVE_WILL_O_WISP || move==MOVE_TOXIC || move==MOVE_STEALTH_ROCK || move==MOVE_SPIKES || move==MOVE_DEFOG || move==MOVE_TRICK_ROOM || move==MOVE_ENCORE || move==MOVE_TAUNT)data->utility=1;
 }
}
u32 ChaosCurrentStageRating(struct Pokemon *mon)
{
 enum Species species=GetMonData(mon,MON_DATA_SPECIES);
 if(species==SPECIES_NONE || GetMonData(mon,MON_DATA_IS_EGG))return 0;
 enum Type type1=GetSpeciesType(species,0),type2=GetSpeciesType(species,1);
 struct RatingMoves moves={0};u8 seen[(MOVES_COUNT+7)/8]={0};
 const struct LevelUpMove *level=GetSpeciesLevelUpLearnset(species);
 for(u32 i=0;level[i].move!=LEVEL_UP_MOVE_END;i++)
 {enum Move move=level[i].move;if(move<MOVES_COUNT && !(seen[move/8]&(1u<<(move%8)))){seen[move/8]|=1u<<(move%8);RatingMove(&moves,move,type1,type2);}}
 const u16 *lists[]={GetSpeciesTeachableLearnset(species),GetSpeciesEggMoves(species)};
 for(u32 list=0;list<2;list++)for(u32 i=0;lists[list][i]!=MOVE_UNAVAILABLE;i++)
 {enum Move move=lists[list][i];if(move<MOVES_COUNT && !(seen[move/8]&(1u<<(move%8))) && (list==1 || GetTMHMItemIdFromMoveId(move)!=ITEM_NONE)){seen[move/8]|=1u<<(move%8);RatingMove(&moves,move,type1,type2);}}
 u32 stats[6];for(u32 i=0;i<6;i++)stats[i]=GetSpeciesBaseStat(species,i);
 enum Ability ability=GetMonAbility(mon);
 u32 atk=stats[STAT_ATK],spa=stats[STAT_SPATK];
 if(ability==ABILITY_HUGE_POWER || ability==ABILITY_PURE_POWER)atk*=2;
 u32 physical=atk*moves.bestPhysical/150,special=spa*moves.bestSpecial/150;
 u32 offense=max(physical,special),bulk=stats[STAT_HP]*(stats[STAT_DEF]+stats[STAT_SPDEF])/200;
 // Scale current-form attack quality, sustainable bulk and speed, rather than BST.
 s32 score=offense*20/100+bulk*16/100+stats[STAT_SPEED]*12/100;
 score+=min(6,__builtin_popcount(moves.coverage))*2;
 score+=moves.recovery*(bulk>=70?7:3)+moves.setup*(offense>=70?5:2)+moves.utility*4+moves.priority*3;
 if(ability==ABILITY_TECHNICIAN && moves.technician)score+=7;
 else if(ability==ABILITY_ADAPTABILITY && offense>=70)score+=8;
 else if(ability==ABILITY_SHEER_FORCE || ability==ABILITY_MAGIC_GUARD || ability==ABILITY_REGENERATOR || ability==ABILITY_PROTEAN || ability==ABILITY_LIBERO)score+=8;
 else if(ability==ABILITY_PRANKSTER && (moves.utility || moves.setup || moves.recovery))score+=6;
 else if(ability==ABILITY_INTIMIDATE || ability==ABILITY_MULTISCALE)score+=6;
 else score+=CLAMP(gAbilitiesInfo[ability].aiRating,-2,8)/2;
 if(ability==ABILITY_TRUANT)score=score*55/100;
 if(ability==ABILITY_SLOW_START)score=score*70/100;
 if(ability==ABILITY_DEFEATIST)score=score*75/100;
 if(ability==ABILITY_WONDER_GUARD)score+=15;
 // IVs matter at every level, but the displayed level and EVs do not enter the formula.
 u32 ivs=0;for(u32 i=0;i<6;i++)ivs+=GetMonData(mon,MON_DATA_HP_IV+i);
 score+=ivs*6/186;
 // Dual-type defensive coverage: resists/immunities help; weaknesses carry a cost.
 for(u32 type=0;type<NUMBER_OF_MON_TYPES;type++)
 {
  u32 effectiveness=gTypeEffectivenessTable[type][type1];
  if(type2!=type1)effectiveness=effectiveness*gTypeEffectivenessTable[type][type2]/UQ_4_12(1.0);
  if(effectiveness==0)score+=2;else if(effectiveness<UQ_4_12(1.0))score++;
  else if(effectiveness>UQ_4_12(2.0))score-=2;else if(effectiveness>UQ_4_12(1.0))score--;
 }
 // Exceptional results above 9 require several independent strengths.
 if(score>90 && (offense<120 || bulk<90 || stats[STAT_SPEED]<90))score=90+(score-90)/4;
 return CLAMP(score,0,100);
}
static void EvoLine(u8 *dest,const u8 *text){if(*dest!=EOS)StringAppend(dest,COMPOUND_STRING("\n"));StringAppend(dest,text);}
void ChaosFormatEvolution(const struct Evolution *evo,u8 *dest)
{
 *dest=EOS;
 if(evo->method==EVO_ITEM){CopyItemName(evo->param,gStringVar1);StringCopy(dest,COMPOUND_STRING("Use "));StringAppend(dest,gStringVar1);}
 else if(evo->method==EVO_LEVEL || evo->method==EVO_LEVEL_BATTLE_ONLY)
 {StringCopy(dest,COMPOUND_STRING("Level "));ConvertIntToDecimalStringN(gStringVar1,evo->param,STR_CONV_MODE_LEFT_ALIGN,3);StringAppend(dest,gStringVar1);}
 else if(evo->method==EVO_SPIN)
 {
  static const u8 *const spin[] = {COMPOUND_STRING("Spin clockwise briefly"),COMPOUND_STRING("Spin clockwise longer"),COMPOUND_STRING("Spin counterclockwise briefly"),COMPOUND_STRING("Spin counterclockwise longer"),COMPOUND_STRING("Spin either direction")};
  StringCopy(dest,spin[min(evo->param,SPIN_EITHER)]);
 }
 else if(evo->method==EVO_BATTLE_END)StringCopy(dest,COMPOUND_STRING("After a battle"));
 else if(evo->method==EVO_SPLIT_FROM_EVO)StringCopy(dest,COMPOUND_STRING("Nincada evolves; free party slot"));
 else StringCopy(dest,COMPOUND_STRING("Special evolution"));
 if(evo->params==NULL)return;
 for(u32 i=0;evo->params[i].condition!=CONDITIONS_END;i++)
 {
  const struct EvolutionParam *p=&evo->params[i];const u8 *text=NULL;
  switch(p->condition)
  {
   case IF_TIME:text=p->arg1==TIME_NIGHT?COMPOUND_STRING("At night"):p->arg1==TIME_EVENING?COMPOUND_STRING("During evening"):p->arg1==TIME_MORNING?COMPOUND_STRING("During morning"):COMPOUND_STRING("During daytime");break;
   case IF_NOT_TIME:text=p->arg1==TIME_NIGHT?COMPOUND_STRING("During daytime"):COMPOUND_STRING("Outside daytime");break;
   case IF_HOLD_ITEM:CopyItemName(p->arg1,gStringVar1);StringCopy(gStringVar2,COMPOUND_STRING("Hold "));StringAppend(gStringVar2,gStringVar1);text=gStringVar2;break;
   case IF_GENDER:text=p->arg1==MON_FEMALE?COMPOUND_STRING("Female only"):COMPOUND_STRING("Male only");break;
   case IF_KNOWS_MOVE:StringCopy(gStringVar2,COMPOUND_STRING("Know "));StringAppend(gStringVar2,GetMoveName(p->arg1));text=gStringVar2;break;
   case IF_KNOWS_MOVE_TYPE:text=COMPOUND_STRING("Know a FAIRY-type move");break;
   case IF_SPECIES_IN_PARTY:StringCopy(gStringVar2,COMPOUND_STRING("Party: "));StringAppend(gStringVar2,GetSpeciesName(p->arg1));text=gStringVar2;break;
   case IF_ATK_GT_DEF:text=COMPOUND_STRING("Attack > Defense");break;
   case IF_ATK_EQ_DEF:text=COMPOUND_STRING("Attack = Defense");break;
   case IF_ATK_LT_DEF:text=COMPOUND_STRING("Attack < Defense");break;
   case IF_WEATHER:text=COMPOUND_STRING("Rain in the overworld");break;
   case IF_IN_MAP:case IF_IN_MAPSEC:text=COMPOUND_STRING("Special location; item available");break;
   case IF_TYPE_IN_PARTY:text=COMPOUND_STRING("Dark-type Pokemon in party");break;
   case IF_AMPED_NATURE:text=COMPOUND_STRING("Amped nature group");break;
   case IF_LOW_KEY_NATURE:text=COMPOUND_STRING("Low Key nature group");break;
   case IF_PID_UPPER_MODULO_10_GT:text=COMPOUND_STRING("Personality tail >= 5");break;
   case IF_PID_UPPER_MODULO_10_LT:text=COMPOUND_STRING("Personality tail < 5");break;
   case IF_PID_MODULO_100_EQ:text=COMPOUND_STRING("Rare personality (1 in 100)");break;
   case IF_PID_MODULO_100_GT:text=COMPOUND_STRING("Common personality (99 in 100)");break;
   case IF_NOT_REGION:break; // Kanto satisfies the unmodified native branch.
   case IF_BAG_ITEM_COUNT:ConvertIntToDecimalStringN(gStringVar2,p->arg2,STR_CONV_MODE_LEFT_ALIGN,3);StringAppend(gStringVar2,COMPOUND_STRING(" "));CopyItemName(p->arg1,gStringVar1);StringAppend(gStringVar2,gStringVar1);text=gStringVar2;break;
   case IF_MIN_BEAUTY:text=COMPOUND_STRING("High Beauty; or Link Cable");break;
   case IF_CURRENT_DAMAGE_GE:text=COMPOUND_STRING("Lose 49 HP; or Link Cable");break;
   case IF_RECOIL_DAMAGE_GE:text=COMPOUND_STRING("Recoil; or Link Cable");break;
   case IF_MIN_OVERWORLD_STEPS:text=COMPOUND_STRING("Walk 1000 steps; or Link Cable");break;
   case IF_USED_MOVE_X_TIMES:text=COMPOUND_STRING("Use move repeatedly; or Cable");break;
   case IF_CRITICAL_HITS_GE:text=COMPOUND_STRING("3 critical hits; or Link Cable");break;
   case IF_DEFEAT_X_WITH_ITEMS:text=COMPOUND_STRING("Leader battles; or Link Cable");break;
   default:text=COMPOUND_STRING("Form/personality condition");break;
  }
  if(text)EvoLine(dest,text);
 }
}

void ChaosRequireNickname(void) { if (IsNuzlockeRun()) gSpecialVar_Result = TRUE; }
