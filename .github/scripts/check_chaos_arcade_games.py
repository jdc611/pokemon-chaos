#!/usr/bin/env python3
"""Exercise production Memory/Type Match input, completion and payment code."""
from pathlib import Path
import re,subprocess,tempfile
root=Path(__file__).resolve().parents[2];s=(root/'src/chaos_arcade_games.c').read_text()
def fn(name):
 start=s.index(name+'(');start=s.rfind('\n',0,start)+1;brace=s.index('{',start);i=brace+1;depth=1
 while depth:depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[start:i]
struct=s[s.index('struct ArcadeGamesUi'):s.index('static const struct BgTemplate')]
code=r'''
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
typedef unsigned char u8;typedef unsigned short u16;typedef unsigned u32;typedef int bool32;
#define EWRAM_DATA
#define TRUE 1
#define FALSE 0
#define min(a,b) ((a)<(b)?(a):(b))
#define MAX_COINS 9999
#define A_BUTTON 1
#define B_BUTTON 2
#define DPAD_LEFT 4
#define DPAD_RIGHT 8
#define DPAD_UP 16
#define DPAD_DOWN 32
#define JOY_NEW(mask) (keys&(mask))
struct {int active;} gPaletteFade;
u32 keys,coins,grants,draws;u16 gSpecialVar_Result;
u32 GetCoins(void){return coins;}
void AddCoins(u32 n){assert(coins+n<=9999);coins+=n;grants++;}
u32 Random(void){static u32 rng=1;rng=rng*1664525+1013904223;return rng>>16;}
'''+struct+r'''
static void Task_NewGames(u8 task){}
static void DrawGame(void){draws++;}
static void LeaveGame(u8 task){free(sArcadeGame);sArcadeGame=NULL;}
static void MakeQuestion(void){sArcadeGame->delay=0;sArcadeGame->answer=sArcadeGame->question%4;}
'''+fn('ChaosArcadeMemoryReward')+fn('ChaosArcadeTypeReward')+fn('FinishGame')+fn('StartMemory')+fn('Task_Game')+r'''
static void tick(u32 key){keys=key;Task_Game(0);}
static void init(u32 game){sArcadeGame=calloc(1,sizeof(*sArcadeGame));sArcadeGame->game=game;sArcadeGame->first=sArcadeGame->second=255;}
int main(void){
 for(u32 difficulty=0;difficulty<3;difficulty++){
  coins=100;init(0);sArcadeGame->difficulty=difficulty;tick(A_BUTTON);
  u32 pairs=6+difficulty*2;assert(sArcadeGame->pairs==pairs);
  u32 frequencies[10]={0};for(u32 i=0;i<pairs*2;i++)frequencies[sArcadeGame->cards[i]]++;
  for(u32 i=0;i<pairs;i++)assert(frequencies[i]==2);
  for(u32 i=0;i<pairs;i++){
   u32 a=255,b=255;for(u32 j=0;j<pairs*2;j++)if(sArcadeGame->cards[j]==i){if(a==255)a=j;else b=j;}
   sArcadeGame->cursor=a;tick(A_BUTTON);tick(A_BUTTON);assert(sArcadeGame->turns==i); // Cannot match a card with itself.
   sArcadeGame->cursor=b;tick(A_BUTTON);
   assert(sArcadeGame->matched[a]&&sArcadeGame->matched[b]);
  }
  u32 reward=ChaosArcadeMemoryReward(pairs,pairs);assert(sArcadeGame->phase==3&&coins==100+reward);
  u32 old=grants;FinishGame(9999);assert(grants==old);tick(B_BUTTON);assert(!sArcadeGame);
 }
 init(0);StartMemory();u32 a=0,b=1;while(sArcadeGame->cards[b]==sArcadeGame->cards[a])b++;
 sArcadeGame->cursor=a;tick(A_BUTTON);sArcadeGame->cursor=b;tick(A_BUTTON);assert(sArcadeGame->delay==45);
 for(u32 i=0;i<45;i++){tick(0);}assert(sArcadeGame->first==255&&sArcadeGame->second==255);
 u32 old=grants;tick(B_BUTTON);assert(!sArcadeGame&&grants==old);
 for(u32 score=0;score<=10;score++){
  coins=0;init(1);sArcadeGame->phase=1;MakeQuestion();
  for(u32 q=0;q<10;q++){
   sArcadeGame->cursor=q<score?sArcadeGame->answer:(sArcadeGame->answer+1)%4;tick(A_BUTTON);
   for(u32 n=0;n<60;n++)tick(0);
  }
  assert(sArcadeGame->phase==3&&sArcadeGame->correct==score&&coins==ChaosArcadeTypeReward(score));tick(A_BUTTON);
 }
 coins=9990;init(1);sArcadeGame->phase=1;FinishGame(130);assert(coins==9999&&sArcadeGame->awarded==9);tick(B_BUTTON);
 assert(ChaosArcadeMemoryReward(7,0)==0&&ChaosArcadeTypeReward(11)==0);
 puts("PASS actual Memory/Type Match: all boards, distinct pairs, self-flip/mismatch guards, scores, completion-only capped single payouts and forfeits.");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'check.c';p.write_text(code);subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/check'],check=True);subprocess.run([d+'/check'],check=True)
# Regression for stale VAR_RESULT gates: checkflag does not assign VAR_RESULT.
p=(root/'data/maps/CeladonCity_GameCorner_PrizeRoom_Frlg/scripts.inc').read_text()
assert 'checkflag FLAG_BADGE04_GET' not in p
assert p.count('goto_if_unset FLAG_BADGE04_GET, ChaosArcadePrize_Construction')==3

# Protect the approved table art and Rocket/poster bindings during room rebuilds.
import json, struct, hashlib
raw=(root/'data/tilesets/secondary/game_corner_frlg/chaos_checkers_tiles.4bpp').read_bytes()
assert hashlib.sha256(raw[176*32:200*32]).hexdigest()=="fee2c6e7c927bcc1bb28c92a3eac4efb766b25930a88ea15efc065449ec1a438"
room=json.loads((root/'data/maps/CeladonCity_GameCorner_Frlg/map.json').read_text())
rocket=room['object_events'][10]
assert (rocket['x'],rocket['y'],rocket['script'])==(11,2,'CeladonCity_GameCorner_EventScript_RocketGrunt')
for bg in room['bg_events']:
 if bg['x']>=14 and 5<=bg['y']<=10:assert bg['script'] in ('ChaosArcade_MemoryCabinet','ChaosArcade_TypeCabinet')
blocks=struct.unpack('<270H',(root/'data/layouts/CeladonCity_GameCorner_Frlg/map.bin').read_bytes())
assert blocks[13*18+17]==0x3291
city=json.loads((root/'data/maps/CeladonCity_Frlg/map.json').read_text())
worker=next(o for o in city['object_events'] if o.get('flag')=='FLAG_HIDE_CHAOS_ARCADE_WORKER')
assert (worker['x'],worker['y'])==(39,22)
