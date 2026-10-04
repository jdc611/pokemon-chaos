#!/usr/bin/env python3
"""Exercise native departure validation and safe PC repair return selection."""
from pathlib import Path
import re,subprocess,tempfile
root=Path(__file__).resolve().parents[2]
def function(path,name):
 s=(root/path).read_text();m=re.search(r'^(?:static )?(?:bool32|void) '+name+r'\([^;]*?\)\n\{',s,re.M);assert m,name
 i=m.end();depth=1
 while depth:depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[m.start():i]
code=r'''
#include <assert.h>
#include <stdio.h>
typedef unsigned char u8;typedef int bool32;
#define TRUE 1
#define FALSE 0
#define MAP_TYPE_TOWN 1
#define MAP_TYPE_CITY 2
#define MAP_TYPE_ROUTE 3
#define MAP_TYPE_INDOOR 4
#define IS_FRLG 1
#define HEAL_LOCATION_NONE 0
#define MAP_VIRIDIAN_CITY_POKEMON_CENTER_1F 0x260b
#define MAP_OLDALE_TOWN_POKEMON_CENTER_1F 0x0200
#define MAP_GROUP(m) ((m)>>8)
#define MAP_NUM(m) ((m)&255)
struct MapHeader {int mapType,mapLayoutId;} gMapHeader,from;
struct WarpData {int mapGroup,mapNum;} gLastUsedWarp;
struct {struct WarpData lastHealLocation;} save1,*gSaveBlock1Ptr=&save1;
struct {int startRegion;} save3,*gSaveBlock3Ptr=&save3;
struct {int active;} gPaletteFade;
int pending, destroyed, scripts, preview, standing=1, legal=1, healIndex=1, home, dest=-1,warps;
const u8 EventScript_RunFilterReturnToCenter[]={0};
const struct MapHeader *Overworld_GetMapHeaderByGroupAndId(int group,int num){return &from;}
int IsPokemonCenterLayout(int layout){return layout==1;}
int GetHealLocationIndexByWarpData(struct WarpData *warp){return healIndex;}
int IsLastHealLocationPlayerHouse(void){return home;}
void SetWarpDestinationToMapWarp(int group,int num,int warp){assert(warp==0);dest=group<<8|num;}
void SetWarpDestinationToLastHealLocation(void){dest=999;}
void DoWarp(void){warps++;}
int FadeInMapPreviewScreenIsRunning(void){return preview;}
int IsPlayerStandingStill(void){return standing;}
int IsPlayerPartyLegalForRun(u8 *index,u8 *reason){return legal;}
void ScriptContext_SetupScript(const u8 *script){assert(script==EventScript_RunFilterReturnToCenter);scripts++;}
void DestroyTask(u8 id){destroyed++;}
void CreateTask(void (*fn)(u8),int priority){assert(priority==80);pending++;}
'''
code+='\n'.join(function('src/field_screen_effect.c',n) for n in ['ShouldValidatePartyAfterBuildingExit','Task_ValidatePartyAfterBuildingExit','FinishWarpExit'])
code+='\n'+function('src/field_specials.c','ReturnPlayerToLastPokemonCenter')
code+=r'''
int main(void){
 gLastUsedWarp=(struct WarpData){38,10};gMapHeader.mapType=MAP_TYPE_CITY;from.mapType=MAP_TYPE_INDOOR;
 assert(ShouldValidatePartyAfterBuildingExit());FinishWarpExit(0);assert(pending==1&&destroyed==1);
 legal=0;gPaletteFade.active=1;Task_ValidatePartyAfterBuildingExit(1);assert(scripts==0);
 gPaletteFade.active=0;preview=1;Task_ValidatePartyAfterBuildingExit(1);assert(scripts==0);
 preview=0;standing=0;Task_ValidatePartyAfterBuildingExit(1);assert(scripts==0);
 standing=1;Task_ValidatePartyAfterBuildingExit(1);assert(scripts==1&&destroyed==2);
 legal=1;Task_ValidatePartyAfterBuildingExit(1);assert(scripts==1&&destroyed==3);
 // Internal Center floors, entry into a Center, and ordinary route crossings
 // must not send the player away while they are trying to repair a party.
 gMapHeader.mapType=MAP_TYPE_INDOOR;assert(!ShouldValidatePartyAfterBuildingExit());
 gMapHeader.mapType=MAP_TYPE_ROUTE;from.mapType=MAP_TYPE_ROUTE;assert(!ShouldValidatePartyAfterBuildingExit());
 gLastUsedWarp.mapGroup=-1;assert(!ShouldValidatePartyAfterBuildingExit());
 // PC withdrawal before healing returns to the Center actually just left.
 gLastUsedWarp=(struct WarpData){38,11};from.mapLayoutId=1;home=1;healIndex=0;ReturnPlayerToLastPokemonCenter();assert(dest==0x260b&&warps==1);
 // Mart/home departures with no Center record return to an accessible PC,
 // rather than looping through Mom's house with an unrepairable bad type.
 from.mapLayoutId=0;gLastUsedWarp.mapNum=10;ReturnPlayerToLastPokemonCenter();assert(dest==0x260b&&warps==2);
 healIndex=1;home=0;ReturnPlayerToLastPokemonCenter();assert(dest==999&&warps==3);
 puts("PASS: native Mart/Center building departure checks wait for warp completion, allow repairs indoors, block illegal parties, and return to a usable PC before the first heal.");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'check.c';p.write_text(code)
 subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/check'],check=True)
 subprocess.run([d+'/check'],check=True)
for name in ['Task_ExitDoor','Task_ExitNonAnimDoor','Task_ExitNonDoor','Task_ExitStairs']:
 assert 'FinishWarpExit(taskId)' in function('src/field_screen_effect.c',name),name
print('PASS: animated doors, non-animated doors, ordinary exits and stairs all use the same completed-warp validation hook.')
