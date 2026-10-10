"""Exercise exact storage transactions and audit the Ranch object budget."""
from pathlib import Path
import json
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
source = (root / 'src/chaos_ranch.c').read_text()

def function(name):
    match = re.search(r'^(?:static )?(?:u8|u32|void) ' + name + r'\([^;]*?\)\n\{', source, re.M)
    assert match, name
    end, depth = match.end(), 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[match.start():end]

prefix = r'''
#include <assert.h>
#include <string.h>
#include <stdio.h>
typedef unsigned char u8;typedef unsigned u32;typedef int s32;
enum Item {ITEM_NONE,ITEM_TEST,ITEM_MAIL};
#define PARTY_SIZE 6
#define IN_BOX_COUNT 30
#define TOTAL_BOXES_COUNT 14
#define B_TRAINER_PLAYER 0
#define VAR_CHAOS_RANCH_BOX 1
#define SPECIES_NONE 0
#define MON_DATA_SPECIES 1
#define MON_DATA_HELD_ITEM 2
#define FLAG_TEMP_1 1
struct Pokemon {u32 species,item,identity,hp,ability;u8 bytes[80];};
struct Pokemon boxes[14][30],gParties[1][6];
u32 gPartiesCount[1],box,gSpecialVar_LastTalked,gSpecialVar_Result,grave,bagSpace,bagCount,bagItem;
u8 gStringVar1[40];
struct {struct {u32 flagId;} objectEventTemplates[30];struct {int mapNum,mapGroup;} location;} save,*gSaveBlock1Ptr=&save;
u32 VarGet(u32 v){return box;}
u32 GetBoxMonDataAt(u32 b,u32 s,u32 f){return f==MON_DATA_SPECIES?boxes[b][s].species:boxes[b][s].item;}
int Nuzlocke_IsGraveBox(u32 b){return grave && b==13;}
void BoxMonAtToMon(u32 b,u32 s,struct Pokemon *p){*p=boxes[b][s];}
void ZeroBoxMonAt(u32 b,u32 s){memset(&boxes[b][s],0,sizeof boxes[b][s]);}
void RemoveObjectEventByLocalIdAndMap(u32 id,int num,int group){}
int ItemIsMail(enum Item i){return i==ITEM_MAIL;}
int AddBagItem(enum Item i,int count){if(!bagSpace)return 0;bagCount+=count;bagItem=i;return 1;}
void CopyItemName(enum Item i,u8 *dst){strcpy((char*)dst,"ITEM");}
void SetBoxMonDataAt(u32 b,u32 s,u32 f,const void *v){boxes[b][s].item=*(const enum Item*)v;}
'''
tests = r'''
int main(void){
    for(int s=0;s<30;s++){boxes[0][s].species=25;boxes[0][s].identity=1000+s;boxes[0][s].hp=7;boxes[0][s].ability=37;memset(boxes[0][s].bytes,s,80);}
    box=0;gSpecialVar_LastTalked=8;gPartiesCount[0]=2;
    struct Pokemon exact=boxes[0][7];ChaosRanchWithdraw();assert(gSpecialVar_Result==0);assert(gPartiesCount[0]==3);assert(memcmp(&exact,&gParties[0][2],sizeof exact)==0);assert(boxes[0][7].species==0);assert(boxes[0][6].identity==1006&&boxes[0][8].identity==1008);
    gSpecialVar_LastTalked=9;gPartiesCount[0]=6;exact=boxes[0][8];ChaosRanchWithdraw();assert(gSpecialVar_Result==1);assert(memcmp(&exact,&boxes[0][8],sizeof exact)==0);
    boxes[0][8].item=ITEM_TEST;bagSpace=0;ChaosRanchTakeItem();assert(gSpecialVar_Result==2);assert(boxes[0][8].item==ITEM_TEST&&bagCount==0);
    bagSpace=1;ChaosRanchTakeItem();assert(gSpecialVar_Result==0);assert(boxes[0][8].item==ITEM_NONE&&bagCount==1&&bagItem==ITEM_TEST);ChaosRanchTakeItem();assert(gSpecialVar_Result==1&&bagCount==1);
    boxes[0][8].item=ITEM_MAIL;ChaosRanchTakeItem();assert(gSpecialVar_Result==3&&boxes[0][8].item==ITEM_MAIL&&bagCount==1);
    box=13;grave=1;gPartiesCount[0]=2;boxes[13][8]=exact;boxes[13][8].item=ITEM_TEST;ChaosRanchWithdraw();assert(gSpecialVar_Result==2&&boxes[13][8].species==25&&gPartiesCount[0]==2);ChaosRanchTakeItem();assert(gSpecialVar_Result==3&&boxes[13][8].item==ITEM_TEST);
    puts("PASS: exact boxed identity/HP/ability copied without healing, neighboring slots preserved, full party/bag atomic, no item duplication and Grave/mail guards.");
}
'''
code = prefix + '\n'.join(function(name) for name in ['RanchBox', 'RanchSlot', 'ChaosRanchWithdraw', 'ChaosRanchTakeItem']) + tests
with tempfile.TemporaryDirectory() as tmp:
    src, exe = Path(tmp) / 'test.c', Path(tmp) / 'test'
    src.write_text('int IsIronmonRun(void){return 0;}\n' + code)
    subprocess.run(['cc', '-std=c99', str(src), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)

map_data = json.loads((root / 'data/maps/ChaosPokemonRanch/map.json').read_text())
assert len(map_data['object_events']) == 30
assert not map_data['connections']
assert all(o['script'] == 'ChaosRanch_Interact' for o in map_data['object_events'])
# Native culling keeps a 20x17 inclusive rectangle. Conservatively permit a
# one-tile wander margin around each initial slot, plus player and follower.
maximum = 0
for x in range(48):
    for y in range(40):
        count = sum(x - 1 <= o['x'] <= x + 20 and y - 1 <= o['y'] <= y + 17 for o in map_data['object_events'])
        maximum = max(maximum, count)
assert maximum + 2 <= 16, maximum
for path in (root / 'data/maps').glob('*PokemonCenter_1F_Frlg/map.json'):
    center = json.loads(path.read_text())
    assert sum(w['dest_map'] == 'MAP_CHAOS_POKEMON_RANCH' for w in center['warp_events']) == 1, path
print(f'PASS: all native Centers have a Ranch door; 30 real slots fit the viewport budget (at most {maximum} pasture objects plus player/follower).')
