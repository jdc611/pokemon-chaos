#!/usr/bin/env python3
"""Exercise native recovery-warp validation, including pre-Center quick starts."""
from pathlib import Path
import re, subprocess, tempfile
root = Path(__file__).resolve().parents[2]
source = (root / 'src/overworld.c').read_text()
def function(name):
    match = re.search(r'^(?:static )?void ' + name + r'\([^;]*?\)\n\{', source, re.M)
    assert match, name
    end = match.end(); depth = 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}'); end += 1
    return source[match.start():end]
code = r'''
#include <assert.h>
#include <stdio.h>
typedef int bool32;
#define MALE 0
#define IS_FRLG frlg
#define HEAL_LOCATION_NONE 0
#define HEAL_LOCATION_PALLET_TOWN 1
#define HEAL_LOCATION_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F 2
#define HEAL_LOCATION_LITTLEROOT_TOWN_MAYS_HOUSE_2F 3
struct WarpData {int location, x;};
struct {struct WarpData lastHealLocation;} save1,*gSaveBlock1Ptr=&save1;
struct {int playerGender;} save2,*gSaveBlock2Ptr=&save2;
struct {int startRegion;} save3,*gSaveBlock3Ptr=&save3;
struct WarpData sWarpDestination;
int frlg=1, writes=0, cutscene=1;
int GetHealLocationIndexByWarpData(struct WarpData *w){return w->location>=1&&w->location<=4&&w->x==9?w->location:0;}
void SetLastHealLocationWarp(int loc){save1.lastHealLocation=(struct WarpData){loc,9};writes++;}
int IsWhiteoutCutscene(void){assert(GetHealLocationIndexByWarpData(&save1.lastHealLocation));return cutscene;}
void SetWhiteoutRespawnWarpAndHealerNPC(struct WarpData *w){*w=save1.lastHealLocation;w->x=5;}
''' + '\n'.join(function(n) for n in ['EnsureValidLastHealLocation', 'SetWarpDestinationToLastHealLocation', 'SetWarpDestinationForTeleport']) + r'''
int main(void){
 // A pre-Center save with an unset or malformed warp returns to Pallet/Mom.
 for(int bad=0;bad<2;bad++){
  save1.lastHealLocation=(struct WarpData){bad?4:0,0};writes=0;
  SetWarpDestinationToLastHealLocation();assert(writes==1&&sWarpDestination.location==1&&sWarpDestination.x==5);
 }
 // Preserve an actual Center; never send a healed player back to Mom.
 save1.lastHealLocation=(struct WarpData){4,9};writes=0;
 SetWarpDestinationToLastHealLocation();assert(writes==0&&sWarpDestination.location==4);
 cutscene=0;SetWarpDestinationToLastHealLocation();assert(sWarpDestination.x==9&&writes==0);
 // Teleport/Center return use the same repaired recovery record.
 save1.lastHealLocation=(struct WarpData){0,0};SetWarpDestinationForTeleport();assert(sWarpDestination.location==1&&sWarpDestination.x==9);
 frlg=0;save3.startRegion=1;save1.lastHealLocation=(struct WarpData){0,0};EnsureValidLastHealLocation();assert(save1.lastHealLocation.location==1);
 save3.startRegion=0;for(int gender=0;gender<2;gender++){
  save2.playerGender=gender;save1.lastHealLocation=(struct WarpData){0,0};EnsureValidLastHealLocation();assert(save1.lastHealLocation.location==2+gender);
 }
 puts("PASS: native recovery validation repairs unset/malformed pre-Center warps, preserves recorded Centers, and retains region/gender home fallback.");
}
'''
with tempfile.TemporaryDirectory() as directory:
    file=Path(directory)/'check.c';file.write_text(code)
    subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(file),'-o',directory+'/check'],check=True)
    subprocess.run([directory+'/check'],check=True)
new_game=(root/'src/new_game.c').read_text();start=new_game.index('void NewGameInitData(void)');body=new_game[start:]
assert body.index('SetLastHealLocationWarp(HEAL_LOCATION_PALLET_TOWN)') < body.index('WarpToTruck(startInKanto)')
print('PASS: Kanto home recovery is initialized before either new-game path enters the map.')
