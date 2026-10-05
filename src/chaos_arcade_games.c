#include "global.h"
#include "bg.h"
#include "coins.h"
#include "event_data.h"
#include "field_screen_effect.h"
#include "gpu_regs.h"
#include "item.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "field_weather.h"
#include "constants/field_weather.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "random.h"
#include "script.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "battle_main.h"
#include "constants/coins.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/rgb.h"

// Compact round state; ownership/payout is held here until the field returns.
struct ArcadeGamesUi {
    u16 tilemap[1024];
    u16 species, turns, awarded;
    u8 game, phase, difficulty, cursor, pairs, columns, first, second, delay;
    u8 cards[20], matched[20], icons[20];
    u8 options[4], answer, question, correct, feedback, capped, initialRedraw;
    u16 clock;u8 target,draws;
};
static EWRAM_DATA struct ArcadeGamesUi *sArcadeGame;
static const struct BgTemplate sGameBg[] = {{.bg=0,.charBaseIndex=0,.mapBaseIndex=31,.priority=1}};
static const struct WindowTemplate sGameWindows[] = {{.bg=0,.tilemapLeft=1,.tilemapTop=1,.width=28,.height=18,.paletteNum=15,.baseBlock=1},DUMMY_WIN_TEMPLATE};
static const u16 sGamePalette[16] = {RGB(2,2,6),RGB(3,4,10),RGB(30,31,31),RGB(11,14,20),RGB(4,9,17),RGB(3,27,31),RGB(26,5,30),RGB(31,26,4),RGB(7,25,13),RGB(29,8,9)};
static const enum Species sCardSpecies[] = {SPECIES_BULBASAUR,SPECIES_CHARMANDER,SPECIES_SQUIRTLE,SPECIES_PIKACHU,SPECIES_EEVEE,SPECIES_PSYDUCK,SPECIES_JIGGLYPUFF,SPECIES_MEOWTH,SPECIES_GASTLY,SPECIES_MACHOP};
static const enum Species sQuizSpecies[] = {SPECIES_VENUSAUR,SPECIES_CHARIZARD,SPECIES_BLASTOISE,SPECIES_PIKACHU,SPECIES_GENGAR,SPECIES_ALAKAZAM,SPECIES_MACHAMP,SPECIES_GOLEM,SPECIES_LAPRAS,SPECIES_SCIZOR,SPECIES_GARDEVOIR,SPECIES_LUCARIO,SPECIES_GARCHOMP,SPECIES_SNORLAX,SPECIES_GYARADOS,SPECIES_UMBREON,SPECIES_TOGETIC,SPECIES_MAGNEZONE};
static const enum Type sQuizTypes[] = {TYPE_NORMAL,TYPE_FIRE,TYPE_WATER,TYPE_ELECTRIC,TYPE_GRASS,TYPE_ICE,TYPE_FIGHTING,TYPE_POISON,TYPE_GROUND,TYPE_FLYING,TYPE_PSYCHIC,TYPE_BUG,TYPE_ROCK,TYPE_GHOST,TYPE_DRAGON,TYPE_DARK,TYPE_STEEL,TYPE_FAIRY};

u32 ChaosArcadeMemoryReward(u32 pairs,u32 turns)
{
    if(pairs==6)return 30+(turns<=8?10:0);
    if(pairs==8)return 50+(turns<=11?15:0);
    if(pairs==10)return 75+(turns<=14?25:0);
    return 0;
}
u32 ChaosArcadeTypeReward(u32 correct)
{
    return correct<=10?correct*10+(correct==10?30:0):0;
}
// Use the native standard type chart; no ability/item/form assumptions in quizzes.
u32 ChaosArcadeQuizEffect(enum Type attack,enum Species species)
{
    enum Type t1=gSpeciesInfo[species].types[0],t2=gSpeciesInfo[species].types[1];
    u32 effect=gTypeEffectivenessTable[attack][t1];
    if(t1!=t2)effect=effect*gTypeEffectivenessTable[attack][t2]/4096;
    return effect;
}
static void Print(const u8 *text,u32 x,u32 y)
{
    static const u8 colors[]={1,2,3};
    AddTextPrinterParameterized4(0,FONT_SMALL,x,y,0,0,colors,TEXT_SKIP_DRAW,text);
}
static void Number(u8 *text,u32 value){ConvertIntToDecimalStringN(text,value,STR_CONV_MODE_LEFT_ALIGN,4);}
static void ClearIcons(void)
{
    for(u32 i=0;i<20;i++)if(sArcadeGame->icons[i]!=MAX_SPRITES){FreeAndDestroyMonIconSprite(&gSprites[sArcadeGame->icons[i]]);sArcadeGame->icons[i]=MAX_SPRITES;}
}
static void DrawBerryBar(void)
{
    struct ArcadeGamesUi *g=sArcadeGame;
    FillWindowPixelRect(0,1,12,74,200,28);
    FillWindowPixelRect(0,4,12,78,200,20);
    FillWindowPixelRect(0,8,12+(g->target-7)*2,78,30,20);
    FillWindowPixelRect(0,7,12+min(99,g->clock*100/180)*2,75,2,26);
}
static void DrawGame(void)
{
    struct ArcadeGamesUi *g=sArcadeGame;
    u8 text[80],coins[16];
    FillWindowPixelBuffer(0,PIXEL_FILL(1));
    FillWindowPixelRect(0,4,0,0,224,18);
    Print(g->game==0?COMPOUND_STRING("POKEMON MEMORY"):g->game==1?COMPOUND_STRING("TYPE MATCH"):g->game==2?COMPOUND_STRING("ROCKET RISK"):COMPOUND_STRING("BERRY TIMING"),4,1);
    u8 *end;
    ClearIcons();
    if(g->phase==0 && g->game<2){
        static const u8 *const levels[]={COMPOUND_STRING("EASY: 6 pairs / 30 + 10 bonus"),COMPOUND_STRING("NORMAL: 8 pairs / 50 + 15 bonus"),COMPOUND_STRING("HARD: 10 pairs / 75 + 25 bonus")};
        Print(levels[g->difficulty],4,30);
        Print(COMPOUND_STRING("LEFT/RIGHT: Board   A: Start\nMatch every pair to earn COINS.\nEfficient clears earn a bonus.\nFree entry. B: Leave"),4,53);
    }else if(g->phase==0){
        if(g->game==2)Print(COMPOUND_STRING("25 COINS per round.\nEach draw: 20% chance of a bust.\nBank after any safe draw.\n5 safe draws bank 300 COINS.\nA: Pay and play   B: Leave"),4,29);
        else Print(COMPOUND_STRING("20 timing prompts. Free entry.\nPress A while the marker is green.\nOne attempt per prompt.\n5 COINS per hit + 20 if perfect!\nA: Start   B: Leave"),4,29);
    }else if(g->phase==3){
        Print(g->game==2&&g->feedback==0?COMPOUND_STRING("VOLTORB! Round winnings lost."):COMPOUND_STRING("Round complete!"),4,30);
        end=StringCopy(text,COMPOUND_STRING("COINS earned: "));Number(end,g->awarded);Print(text,4,53);
        if(g->game==1){end=StringCopy(text,COMPOUND_STRING("Correct: "));Number(end,g->correct);StringCopy(text+StringLength(text),COMPOUND_STRING(" / 10"));Print(text,4,74);}
        if(g->capped)Print(COMPOUND_STRING("Coin Case limit: reward reduced."),4,92);
        Print(COMPOUND_STRING("A/B: Return to the arcade"),4,110);
    }else if(g->game==2){
        end=StringCopy(text,COMPOUND_STRING("Safe draws: "));Number(end,g->draws);Print(text,4,28);
        static const u16 banks[]={0,25,50,100,175,300};
        end=StringCopy(text,COMPOUND_STRING("Unbanked COINS: "));Number(end,banks[g->draws]);Print(text,4,49);
        Print(COMPOUND_STRING("Risk another draw, or bank?"),4,70);
        FillWindowPixelRect(0,g->cursor==0?6:4,4,90,106,23);Print(COMPOUND_STRING("DRAW"),9,94);
        FillWindowPixelRect(0,g->cursor==1?6:4,114,90,106,23);Print(COMPOUND_STRING("BANK"),119,94);
        Print(COMPOUND_STRING("A: Choose  B: Forfeit (no refund)"),4,133);
    }else if(g->game==3){
        end=StringCopy(text,COMPOUND_STRING("Prompt "));Number(end,g->question+1);Print(text,4,28);
        end=StringCopy(text,COMPOUND_STRING("Hits: "));Number(end,g->correct);Print(text,130,28);
        Print(COMPOUND_STRING("Press A in the green window!"),4,51);
        DrawBerryBar();
        if(g->delay)Print(g->feedback?COMPOUND_STRING("Nice catch!"):COMPOUND_STRING("Missed!"),4,111);
        else Print(COMPOUND_STRING("A: Catch  B: Forfeit (no reward)"),4,133);
    }else if(g->game==0){
        for(u32 i=0;i<g->pairs*2;i++){
            u32 x=8+(i%g->columns)*42,y=28+(i/g->columns)*25;
            FillWindowPixelRect(0,i==g->cursor?7:5,x,y,34,23);
            FillWindowPixelRect(0,g->matched[i]?8:4,x+2,y+2,30,19);
            if(g->matched[i]||i==g->first||i==g->second){
                u8 id=CreateMonIcon(sCardSpecies[g->cards[i]],SpriteCallbackDummy,x+25,y+16,0,0);
                g->icons[i]=id;
                if(id!=MAX_SPRITES)gSprites[id].oam.priority=0;
            }else Print(COMPOUND_STRING("?"),x+14,y+3);
        }
        end=StringCopy(text,COMPOUND_STRING("Turns: "));Number(end,g->turns);Print(text,4,133);Print(COMPOUND_STRING("A: Flip  B: Forfeit"),104,133);
    }else{
        end=StringCopy(text,COMPOUND_STRING("Question "));Number(end,g->question+1);StringCopy(text+StringLength(text),COMPOUND_STRING(" / 10"));Print(text,4,21);
        Print(gSpeciesInfo[g->species].speciesName,4,39);
        end=StringCopy(text,gTypesInfo[gSpeciesInfo[g->species].types[0]].name);
        if(gSpeciesInfo[g->species].types[0]!=gSpeciesInfo[g->species].types[1])end=StringCopy(StringCopy(end,COMPOUND_STRING(" / ")),gTypesInfo[gSpeciesInfo[g->species].types[1]].name);
        Print(text,4,54);Print(COMPOUND_STRING("Which attack is super effective?"),4,70);
        for(u32 i=0;i<4;i++){
            u32 x=4+(i%2)*110,y=87+(i/2)*22;
            FillWindowPixelRect(0,i==g->cursor?6:4,x,y,106,20);Print(gTypesInfo[g->options[i]].name,x+5,y+2);
        }
        if(g->delay){Print(g->feedback?COMPOUND_STRING("Correct!"):COMPOUND_STRING("Answer:"),4,133);if(!g->feedback)Print(gTypesInfo[g->options[g->answer]].name,52,133);}
        else Print(COMPOUND_STRING("A: Answer  B: Forfeit (no reward)"),4,133);
    }
    end=StringCopy(coins,COMPOUND_STRING("COINS "));Number(end,GetCoins());Print(coins,155,1);
    PutWindowTilemap(0);CopyWindowToVram(0,COPYWIN_FULL);
}
static void FinishGame(u32 reward)
{
    if(sArcadeGame->phase==3)return;
    sArcadeGame->phase=3;
    sArcadeGame->capped=reward>MAX_COINS-GetCoins();
    reward=min(reward,MAX_COINS-GetCoins());
    AddCoins(reward);sArcadeGame->awarded=reward;gSpecialVar_Result=reward;
    DrawGame();
}
static void MakeQuestion(void)
{
    struct ArcadeGamesUi *g=sArcadeGame;
    // Every selected dual/single typing has a weakness and >=3 non-weak types.
    g->species=sQuizSpecies[Random()%ARRAY_COUNT(sQuizSpecies)];
    enum Type correct[18],wrong[18];u32 n=0,m=0;
    for(u32 i=0;i<ARRAY_COUNT(sQuizTypes);i++){
        enum Type type=sQuizTypes[i];
        if(ChaosArcadeQuizEffect(type,g->species)>4096)correct[n++]=type;else wrong[m++]=type;
    }
    g->answer=Random()%4;g->options[g->answer]=correct[Random()%n];
    for(u32 i=m-1;i>0;i--){u32 j=Random()%(i+1);enum Type t=wrong[i];wrong[i]=wrong[j];wrong[j]=t;}
    for(u32 i=0,j=0;i<4;i++)if(i!=g->answer)g->options[i]=wrong[j++];
    g->cursor=0;g->delay=0;DrawGame();
}
static void StartMemory(void)
{
    struct ArcadeGamesUi *g=sArcadeGame;
    g->pairs=6+g->difficulty*2;g->columns=g->pairs==10?5:4;g->phase=1;
    for(u32 i=0;i<g->pairs*2;i++)g->cards[i]=i/2;
    for(u32 i=g->pairs*2-1;i>0;i--){u32 j=Random()%(i+1);u8 t=g->cards[i];g->cards[i]=g->cards[j];g->cards[j]=t;}
    DrawGame();
}
static void LeaveGame(u8 taskId)
{
    ClearIcons();FreeMonIconPalettes();FreeAllWindowBuffers();UnsetBgTilemapBuffer(0);
    Free(sArcadeGame);sArcadeGame=NULL;DestroyTask(taskId);SetMainCallback2(CB2_ReturnToFieldContinueScript);
}
u32 ChaosArcadeRiskBank(u32 draws){static const u16 banks[]={0,25,50,100,175,300};return draws<=5?banks[draws]:0;}
u32 ChaosArcadeBerryReward(u32 correct){return correct<=20?correct*5+(correct==20?20:0):0;}
static void StartBerryPrompt(void){sArcadeGame->clock=0;sArcadeGame->target=30+Random()%41;sArcadeGame->delay=0;DrawGame();}
static void Task_NewGames(u8 taskId)
{
    struct ArcadeGamesUi *g=sArcadeGame;
    if(g->phase==0){
        if(JOY_NEW(A_BUTTON)){
            if(g->game==2){if(GetCoins()<25){Print(COMPOUND_STRING("You need 25 COINS to play."),4,117);CopyWindowToVram(0,COPYWIN_FULL);return;}RemoveCoins(25);}
            g->phase=1;g->feedback=1;
            if(g->game==3)StartBerryPrompt();else DrawGame();
        }return;
    }
    if(g->game==2){
        if(JOY_NEW(DPAD_LEFT|DPAD_RIGHT|DPAD_UP|DPAD_DOWN)){g->cursor^=1;DrawGame();}
        if(JOY_NEW(A_BUTTON)){
            if(g->cursor==1){if(g->draws)FinishGame(ChaosArcadeRiskBank(g->draws));return;}
            if(Random()%5==0){g->feedback=0;FinishGame(0);return;}
            if(++g->draws==5){FinishGame(300);return;}DrawGame();
        }return;
    }
    if(g->delay){
        if(--g->delay==0){if(++g->question==20)FinishGame(ChaosArcadeBerryReward(g->correct));else StartBerryPrompt();}return;
    }
    if(JOY_NEW(A_BUTTON)||g->clock>=180){
        u32 position=g->clock*100/180;
        g->feedback=JOY_NEW(A_BUTTON)&&position+7>=g->target&&position<=g->target+7;
        g->correct+=g->feedback;g->delay=20;DrawGame();return;
    }
    g->clock++;if(g->clock%3==0){DrawBerryBar();CopyWindowToVram(0,COPYWIN_GFX);}
}
static void Task_Game(u8 taskId)
{
    struct ArcadeGamesUi *g=sArcadeGame;
    if(gPaletteFade.active)return;
    if(g->initialRedraw){g->initialRedraw=FALSE;DrawGame();}
    if(g->phase==3){if(JOY_NEW(A_BUTTON|B_BUTTON))LeaveGame(taskId);return;}
    if(JOY_NEW(B_BUTTON)){gSpecialVar_Result=0;LeaveGame(taskId);return;}
    if(g->game>=2){Task_NewGames(taskId);return;}
    if(g->phase==0){
        if(JOY_NEW(DPAD_LEFT))g->difficulty=(g->difficulty+2)%3;
        if(JOY_NEW(DPAD_RIGHT))g->difficulty=(g->difficulty+1)%3;
        if(JOY_NEW(A_BUTTON))StartMemory();else if(JOY_NEW(DPAD_LEFT|DPAD_RIGHT))DrawGame();return;
    }
    if(g->delay){
        if(--g->delay==0){
            if(g->game==0){g->first=g->second=255;DrawGame();}
            else if(++g->question==10)FinishGame(ChaosArcadeTypeReward(g->correct));else MakeQuestion();
        }return;
    }
    u32 count=g->game?4:g->pairs*2,columns=g->game?2:g->columns;
    if(JOY_NEW(DPAD_LEFT))g->cursor=(g->cursor+count-1)%count;
    if(JOY_NEW(DPAD_RIGHT))g->cursor=(g->cursor+1)%count;
    if(JOY_NEW(DPAD_UP))g->cursor=(g->cursor+count-columns)%count;
    if(JOY_NEW(DPAD_DOWN))g->cursor=(g->cursor+columns)%count;
    if(JOY_NEW(A_BUTTON)){
        if(g->game){g->feedback=g->cursor==g->answer;g->correct+=g->feedback;g->delay=60;}
        else if(!g->matched[g->cursor]&&g->cursor!=g->first){
            if(g->first==255)g->first=g->cursor;
            else{
                g->second=g->cursor;g->turns++;
                if(g->cards[g->first]==g->cards[g->second]){
                    g->matched[g->first]=g->matched[g->second]=1;g->first=g->second=255;
                    bool32 complete=TRUE;for(u32 i=0;i<count;i++)if(!g->matched[i])complete=FALSE;
                    if(complete){FinishGame(ChaosArcadeMemoryReward(g->pairs,g->turns));return;}
                }else g->delay=45;
            }
        }
    }
    if(JOY_NEW(A_BUTTON|(DPAD_LEFT|DPAD_RIGHT|DPAD_UP|DPAD_DOWN)))DrawGame();
}
static void GameMain(void){RunTasks();AnimateSprites();BuildOamBuffer();UpdatePaletteFade();}
static void GameVBlank(void){LoadOam();ProcessSpriteCopyRequests();TransferPlttBuffer();}
static void InitGame(void)
{
    SetVBlankCallback(NULL);ResetVramOamAndBgCntRegs();ResetTasks();ResetSpriteData();FreeAllSpritePalettes();
    ResetBgsAndClearDma3BusyFlags(0);InitBgsFromTemplates(0,sGameBg,ARRAY_COUNT(sGameBg));
    SetBgTilemapBuffer(0,sArcadeGame->tilemap);InitWindows(sGameWindows);DeactivateAllTextPrinters();
    LoadPalette(sGamePalette,BG_PLTT_ID(15),sizeof(sGamePalette));LoadMonIconPalettes();
    if(sArcadeGame->game==1){sArcadeGame->phase=1;MakeQuestion();}else DrawGame();
    SetGpuReg(REG_OFFSET_DISPCNT,DISPCNT_OBJ_ON|DISPCNT_OBJ_1D_MAP);ShowBg(0);ResetPaletteFade();
    BeginNormalPaletteFade(PALETTES_ALL,0,16,0,RGB_BLACK);SetVBlankCallback(GameVBlank);CreateTask(Task_Game,0);SetMainCallback2(GameMain);
}
static void Task_LaunchGame(u8 taskId){if(!gPaletteFade.active){DestroyTask(taskId);SetMainCallback2(InitGame);}}
static void Task_GameFailed(u8 taskId){ScriptContext_Enable();DestroyTask(taskId);}
static void LaunchGame(u32 game)
{
    gSpecialVar_Result=0;
    if(!FlagGet(FLAG_BADGE04_GET)||!CheckBagHasItem(ITEM_COIN_CASE,1)){CreateTask(Task_GameFailed,0);return;}
    sArcadeGame=AllocZeroed(sizeof(*sArcadeGame));
    if(!sArcadeGame){CreateTask(Task_GameFailed,0);return;}
    sArcadeGame->initialRedraw=TRUE;
    sArcadeGame->game=game;sArcadeGame->first=sArcadeGame->second=255;
    for(u32 i=0;i<20;i++)sArcadeGame->icons[i]=MAX_SPRITES;
    FadeScreen(FADE_TO_BLACK,0);CreateTask(Task_LaunchGame,0);
}
void ChaosArcadeMemoryMatch(void){LaunchGame(0);}
void ChaosArcadeTypeMatch(void){LaunchGame(1);}

void ChaosArcadeRocketRisk(void){LaunchGame(2);}
void ChaosArcadeBerryTiming(void){LaunchGame(3);}
