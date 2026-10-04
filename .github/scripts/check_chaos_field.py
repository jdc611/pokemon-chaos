#!/usr/bin/env python3
"""Verify HM acquisition/badge gates and unrestricted native Kanto Safari entry."""
from pathlib import Path
import json,re,subprocess,tempfile
root=Path(__file__).resolve().parents[2]
s=(root/'src/scrcmd.c').read_text();a=s.index('bool8 ScrCmd_checkfieldmove(');b=s.index('\nbool8 ScrCmd_addmoney',a)
code=r'''
#include <assert.h>
#include <stdio.h>
typedef unsigned char u8;typedef unsigned int u32;typedef int bool8;typedef int bool32;
#define FALSE 0
#define TRUE 1
#define PARTY_SIZE 6
#define B_TRAINER_PLAYER 0
#define SCREFF_V1 0
#define MON_DATA_SPECIES 0
#define MON_DATA_IS_EGG 1
enum FieldMove {CUT,FLY,SURF,STRENGTH,FLASH,SMASH,WATERFALL,DIVE};
enum Move {MOVE_CUT=1,MOVE_FLY,MOVE_SURF,MOVE_STRENGTH,MOVE_FLASH,MOVE_ROCK_SMASH,MOVE_WATERFALL,MOVE_DIVE};
enum Item {ITEM_NONE=0,ITEM_HM_CUT=1,ITEM_HM_FLY,ITEM_HM_SURF,ITEM_HM_STRENGTH,ITEM_HM_FLASH,ITEM_HM_ROCK_SMASH,ITEM_HM_WATERFALL,ITEM_HM_DIVE};
enum Species {SPECIES_FIXTURE=1};
struct Pokemon {int species;} gParties[1][PARTY_SIZE]={{{1}}};
struct ScriptContext {const u8 *scriptPtr;};
#define ScriptReadByte(ctx) (*(ctx->scriptPtr++))
u32 gSpecialVar_Result,gSpecialVar_0x8004;int badge,owned,knows;
void Script_RequestEffects(int effects){}
int IsFieldMoveUnlocked(enum FieldMove move){return badge;}
enum Move FieldMove_GetMoveId(enum FieldMove move){return move+1;}
int CheckBagHasItem(enum Item item,int count){return owned;}
u32 GetMonData(struct Pokemon *m,u32 field){return field==MON_DATA_SPECIES?m->species:0;}
int MonKnowsMove(struct Pokemon *m,enum Move move){return knows;}
'''+s[a:b]+r'''
int main(void){for(u8 move=0;move<8;move++)for(badge=0;badge<2;badge++)for(owned=0;owned<2;owned++)for(knows=0;knows<2;knows++){
 u8 args[]={move,1};struct ScriptContext ctx={args};ScrCmd_checkfieldmove(&ctx);
 assert(gSpecialVar_Result==(badge&&owned?0:PARTY_SIZE));
}puts("PASS: all eight HM checks require acquisition plus badge; no learned HM needed, and known HMs cannot bypass acquisition.");}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'check.c';p.write_text(code);subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/check'],check=True);subprocess.run([d+'/check'],check=True)
m=json.loads((root/'data/maps/FuchsiaCity_SafariZone_Entrance_Frlg/map.json').read_text())
assert not m['coord_events']
s=(root/'data/maps/FuchsiaCity_SafariZone_Entrance_Frlg/scripts.inc').read_text()
header=s[:s.index('FuchsiaCity_SafariZone_Entrance_OnFrame::')]
assert 'MAP_SCRIPT_ON_FRAME_TABLE' not in header
assert 'clearflag FLAG_SYS_SAFARI_MODE' in header
assert 'setvar VAR_MAP_SCENE_FUCHSIA_CITY_SAFARI_ZONE_ENTRANCE, 0' in header
assert any(w['dest_map']=='MAP_SAFARI_ZONE_CENTER' for w in m['warp_events'])
assert any(w['dest_map']=='MAP_FUCHSIA_CITY' for w in m['warp_events'])
info=s[s.index('FuchsiaCity_SafariZone_Entrance_EventScript_InfoAttendant::'):s.index('FuchsiaCity_SafariZone_Entrance_Text_ChaosWelcome::')]
assert 'ChaosWelcome' in info and 'checkmoney' not in info
print('PASS: Kanto Safari entrance has ordinary bidirectional doors, no forced fee/ball/timer entry, and clears legacy Safari state.')
