#!/usr/bin/env python3
"""Exercise production wager/timing input and permanent cosmetic transactions."""
from pathlib import Path
import subprocess,tempfile,re
root=Path(__file__).resolve().parents[2]
def fn(s,name):
 start=s.index(name+'(');start=s.rfind('\n',0,start)+1;brace=s.index('{',start);i=brace+1;depth=1
 while depth:depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[start:i]
common=r'''
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef unsigned char u8,bool8;typedef unsigned short u16;typedef unsigned u32;typedef int bool32;
#define EWRAM_DATA
#define TRUE 1
#define FALSE 0
#define min(a,b) ((a)<(b)?(a):(b))
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define COMPOUND_STRING(s) ((const u8 *)(s))
#define MAX_COINS 9999
#define A_BUTTON 1
#define B_BUTTON 2
#define DPAD_LEFT 4
#define DPAD_RIGHT 8
#define DPAD_UP 16
#define DPAD_DOWN 32
#define JOY_NEW(mask) (keys&(mask))
#define COPYWIN_FULL 0
#define COPYWIN_GFX 1
u32 keys,coins,charges,grants,randomValue;u16 gSpecialVar_Result;
u32 GetCoins(void){return coins;}
void AddCoins(u32 n){assert(coins+n<=9999);coins+=n;grants++;}
void RemoveCoins(u32 n){assert(coins>=n);coins-=n;charges++;}
u32 Random(void){return randomValue;}
'''
s=(root/'src/chaos_arcade_games.c').read_text();struct=s[s.index('struct ArcadeGamesUi'):s.index('static const struct BgTemplate')]
code=common+struct+r'''
struct {int active;} gPaletteFade;
static void DrawGame(void){}
static void DrawBerryBar(void){}
static void Print(const u8 *s,u32 x,u32 y){}
void CopyWindowToVram(u32 w,u32 mode){}
static void LeaveGame(u8 task){free(sArcadeGame);sArcadeGame=NULL;}
static void StartMemory(void){}
static void MakeQuestion(void){}
u32 ChaosArcadeTypeReward(u32 n){return n*10;}
u32 ChaosArcadeMemoryReward(u32 a,u32 b){return 0;}
'''+fn(s,'FinishGame')+fn(s,'ChaosArcadeRiskBank')+fn(s,'ChaosArcadeBerryReward')+fn(s,'StartBerryPrompt')+fn(s,'Task_NewGames')+fn(s,'Task_Game')+r'''
static void init(u32 game){sArcadeGame=calloc(1,sizeof(*sArcadeGame));sArcadeGame->game=game;}
static void tick(u32 k){keys=k;Task_Game(0);}
int main(void){
 coins=24;init(2);tick(A_BUTTON);assert(coins==24&&sArcadeGame->phase==0);tick(B_BUTTON);assert(!sArcadeGame);
 for(u32 bank=1;bank<=5;bank++){
  coins=100;init(2);tick(A_BUTTON);assert(coins==75&&sArcadeGame->phase==1);u32 c=charges;
  randomValue=1;for(u32 n=0;n<bank;n++){tick(A_BUTTON);while(sArcadeGame->delay)tick(0);}
  if(bank<5){tick(DPAD_RIGHT);tick(A_BUTTON);}
  assert(coins==75+ChaosArcadeRiskBank(bank)&&charges==c&&sArcadeGame->phase==3);
  u32 paid=grants;FinishGame(9999);assert(grants==paid);tick(A_BUTTON);assert(!sArcadeGame);
 }
 for(u32 draw=0;draw<5;draw++){
  coins=100;init(2);tick(A_BUTTON);randomValue=1;for(u32 n=0;n<draw;n++){tick(A_BUTTON);while(sArcadeGame->delay)tick(0);}
  randomValue=0;tick(A_BUTTON);assert(coins==75&&sArcadeGame->phase==3&&sArcadeGame->awarded==0);tick(B_BUTTON);
 }
 coins=100;init(2);tick(A_BUTTON);randomValue=1;tick(A_BUTTON);u32 paid=grants;tick(B_BUTTON);assert(coins==75&&grants==paid&&!sArcadeGame);
 for(u32 hits=0;hits<=20;hits++){
  coins=0;init(3);randomValue=20;tick(A_BUTTON);
  for(u32 q=0;q<20;q++){
   sArcadeGame->clock=q<hits?90:0;tick(A_BUTTON);
   for(u32 n=0;n<20;n++)tick(0);
  }
  assert(sArcadeGame->phase==3&&sArcadeGame->correct==hits&&coins==ChaosArcadeBerryReward(hits));tick(B_BUTTON);
 }
 coins=9990;init(3);tick(A_BUTTON);FinishGame(120);assert(coins==9999&&sArcadeGame->awarded==9&&sArcadeGame->capped);tick(B_BUTTON);
 init(3);tick(A_BUTTON);for(u32 n=0;n<=180;n++)tick(0);assert(sArcadeGame->delay&&sArcadeGame->correct==0);tick(B_BUTTON);
 assert(ChaosArcadeRiskBank(6)==0&&ChaosArcadeBerryReward(21)==0);
 puts("PASS Rocket Risk: insufficient funds, all banks/busts, fee once, forfeits, replay/caps. Berry: all 21 scores, timeout and completion-only payouts.");
}
'''
def run(code):
 with tempfile.TemporaryDirectory() as d:
  p=Path(d)/'test.c';p.write_text(code);subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/test'],check=True);subprocess.run([d+'/test'],check=True)
run(code)
s=(root/'src/chaos_arcade_prizes.c').read_text();start=s.index('struct ArcadeCosmetic');end=s.index('static bool32 CosmeticOwned',start)
code=common+s[start:end]+r'''
#define FLAG_BADGE04_GET 1
#define ITEM_COIN_CASE 1
u8 gStringVar3[128];
u8 *StringCopy(u8 *d,const u8 *s){return (u8 *)strcpy((char *)d,(const char *)s);}
struct Save {u32 arcadeCosmeticsOwned[2];u8 arcadeRanchTheme,arcadeRiderTheme,arcadeOutfit;u32 arcadeDecorations;} save,*gSaveBlock3Ptr=&save;
int badge=1,coinCase=1;
int FlagGet(int f){return badge;}
int CheckBagHasItem(int i,int n){return coinCase;}
void ChaosArcadeRefreshOutfit(void){}
'''+fn(s,'CosmeticOwned')+fn(s,'ChaosArcadeCosmeticBuy')+r'''
int main(void){
 for(u32 i=0;i<ARRAY_COUNT(sCosmetics);i++){
  memset(&save,0,sizeof(save));coins=9999;sCosmeticChoice=i;ChaosArcadeCosmeticBuy();
  assert(gSpecialVar_Result==0&&coins==9999-sCosmetics[i].price&&(save.arcadeCosmeticsOwned[0]&(1u<<i)));
  if(sCosmetics[i].category==0)assert(save.arcadeRanchTheme==sCosmetics[i].value);
  if(sCosmetics[i].category==1)assert(save.arcadeRiderTheme==sCosmetics[i].value);
  if(sCosmetics[i].category==2)assert(save.arcadeDecorations==(1u<<sCosmetics[i].value));
  if(sCosmetics[i].category==3)assert(save.arcadeOutfit==sCosmetics[i].value);
  u32 balance=coins;ChaosArcadeCosmeticBuy();assert(gSpecialVar_Result==4&&coins==balance);
  sCosmeticChoice=i;coins=0;ChaosArcadeCosmeticBuy();assert(gSpecialVar_Result==0&&coins==0);
  for(u32 failure=0;failure<3;failure++){
   memset(&save,0,sizeof(save));sCosmeticChoice=i;coins=failure==0?0:9999;badge=failure!=1;coinCase=failure!=2;
   if(failure==0&&!sCosmetics[i].price)continue;
   balance=coins;ChaosArcadeCosmeticBuy();assert(gSpecialVar_Result==(failure==0?2:4)&&coins==balance&&!save.arcadeCosmeticsOwned[0]);
  }badge=coinCase=1;
 }
 puts("PASS cosmetics: permanent ownership, immediate apply, free owned/default, replay, funds/badge/Case atomicity.");
}
'''
run(code)
# Exact independent clerk bindings; details are drawn by directional highlighting.
p=(root/'data/maps/CeladonCity_GameCorner_PrizeRoom_Frlg/scripts.inc').read_text()
assert re.search(r'PrizeClerkItems::\s+goto ChaosArcadeCosmeticCounter',p)
assert re.search(r'PrizeClerkTMs::\s+goto ChaosArcadeItemCounter',p)
assert '\tmsgbox gStringVar4\n\tmsgbox ChaosArcadeItem_Confirm' not in p
assert 'DrawBrowser();' in fn(s,'Task_ItemBrowser')
print('PASS separate clerks and live highlight details.')
import struct,json
city=(root/'data/maps/CeladonCity_Frlg/scripts.inc').read_text()
assert 'map_script MAP_SCRIPT_ON_LOAD, ChaosArcade_ConstructionDoors' in city
for tile in ('0x370','0x371','0x15B'):assert tile+', 1' in city
attrs=(root/'data/tilesets/secondary/celadon_city_frlg/metatile_attributes.bin').read_bytes()
native=(root/'data/tilesets/primary/general_frlg/metatile_attributes.bin').read_bytes()[0x15b*4:0x15c*4]
assert attrs[240*4:]==native*2
room=json.loads((root/'data/maps/CeladonCity_GameCorner_Frlg/map.json').read_text())
blocks=struct.unpack('<270H',(root/'data/layouts/CeladonCity_GameCorner_Frlg/map.bin').read_bytes())
for name,y in [('Risk',7),('Berry',11)]:
 for x in (11,12):
  assert blocks[(y+1)*18+x]==0x3291
  assert any(b.get('script')=='ChaosArcade_'+name+'Cabinet' and (b['x'],b['y'])==(x,y) for b in room['bg_events'])
assert not any(b.get('script')=='CeladonCity_GameCorner_EventScript_PhotoPrinter' for b in room['bg_events'])
print('PASS construction uses map-load/native door behavior; both new cabinets have clear interaction tiles; no stale printer.')

# Native Coin windows reuse STR_VAR_1/4; previews must restore names afterward.
p=(root/'data/maps/CeladonCity_GameCorner_PrizeRoom_Frlg/scripts.inc').read_text()
assert re.search(r'special ChaosArcadeItemMenu\s+showcoinsbox 20, 0\s+goto_if_eq[^\n]+\s+special ChaosArcadeItemPreview',p)
assert '.string "{PLAYER} received {STR_VAR_1}!' not in p
assert '.string "{STR_VAR_1} applied!' not in p
print('PASS Coin-window scratch variables cannot replace prize/receipt names.')
