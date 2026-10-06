#!/usr/bin/env python3
"""Exercise production scenery and outfit transforms, and the native shop gate."""
from pathlib import Path
import re,struct,subprocess,tempfile,json
root=Path(__file__).resolve().parents[2]
def fn(s,name):
 a=s.index('\n',s.index(name+'('))+1;b=s.index('{',a);n=1;e=b+1
 while n:
  n+=(s[e]=='{')-(s[e]=='}');e+=1
 return s[s.rfind('\n',0,a-1)+1:e]+'\n'
s=(root/'src/chaos_arcade_cosmetics.c').read_text()
header=(root/'include/constants/chaos_ranch_cosmetics.h').read_text()
events=(root/'include/constants/event_objects.h').read_text()
tags=dict(re.findall(r'#define (OBJ_EVENT_PAL_TAG_[A-Z_]+)\s+(0x[0-9A-Fa-f]+)',events))
tags={k:v for k,v in tags.items() if k in s}
raw=struct.unpack('<1920H',(root/'data/layouts/ChaosPokemonRanch/map.bin').read_bytes())
start=s.index('struct RanchProp');end=s.index('void ChaosArcadeRanchScenery',start)
code='''#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef uint8_t u8;typedef uint16_t u16;typedef uint32_t u32;typedef unsigned bool32;
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define MAP_CHAOS_POKEMON_RANCH 1
#define MAP_GROUP(x) 38
#define MAP_NUM(x) 28
#define OBJ_PLTT_ID(x) (256+(x)*16)
#define RGB(r,g,b) ((r)|((g)<<5)|((b)<<10))
struct {struct {u8 mapGroup,mapNum;} location;} save1,*gSaveBlock1Ptr=&save1;
struct {u8 arcadeRanchTheme,arcadeOutfit;u32 arcadeDecorations;} save3,*gSaveBlock3Ptr=&save3;
u16 grid[54][62],gPlttBufferUnfaded[512],gPlttBufferFaded[512];
enum {WEATHER_NONE,WEATHER_DROUGHT,WEATHER_SNOW};
unsigned weather;void SetWeather(unsigned w){weather=w;}
void ChaosArcadeEnsureSave(void){}
void MapGridSetMetatileIdAt(u32 x,u32 y,u16 v){assert(x>=7&&x<55&&y>=7&&y<47);grid[y][x]=v;}
'''+header+'\n'.join('#define '+k+' '+v for k,v in tags.items())+'\nstatic const u16 sRanchBase[]={'+','.join(map(str,raw))+'};\n'+s[start:end]+fn(s,'ChaosArcadeRanchScenery')+fn(s,'ChaosArcadeOutfitPalette')+'''
int main(void){
 save1.location.mapGroup=38;save1.location.mapNum=28;
 for(u32 theme=0;theme<5;theme++)for(u32 bits=0;bits<512;bits++){
  save3.arcadeRanchTheme=theme;save3.arcadeDecorations=bits;ChaosArcadeRanchScenery();
  assert(weather==(theme==2?WEATHER_DROUGHT:theme==3?WEATHER_SNOW:WEATHER_NONE));
  const u16 ids[]={8,RANCH_TILE_FOREST,RANCH_TILE_BEACH,RANCH_TILE_SNOW,RANCH_TILE_NIGHT};
  for(u32 y=0;y<40;y++)for(u32 x=0;x<48;x++){
   u16 expected=sRanchBase[y*48+x];if((expected&1023)==8)expected=(expected&~1023)|ids[theme];
   if(theme)for(u32 i=0;i<ARRAY_COUNT(sRanchComposites);i++)if((expected&1023)==sRanchComposites[i].original){expected=(expected&~1023)|sRanchComposites[i].themes[theme-1];break;}
   for(u32 i=0;i<ARRAY_COUNT(sRanchProps);i++){
    const struct RanchProp *p=&sRanchProps[i];
    if((bits&(1u<<p->bit))&&x>=p->x&&x<p->x+p->w&&y>=p->y&&y<p->y+p->h)expected=0x3C00|(p->tile+(y-p->y)*p->w+x-p->x+(theme?RANCH_THEME_PROP_OFFSET+(theme-1)*RANCH_THEME_PROP_STRIDE:0));
   }
   assert(grid[y+7][x+7]==expected);
  }
 }
 save3.arcadeRanchTheme=save3.arcadeDecorations=0;ChaosArcadeRanchScenery();
 for(u32 y=0;y<40;y++)for(u32 x=0;x<48;x++)assert(grid[y+7][x+7]==sRanchBase[y*48+x]);
 u16 before[512];for(u32 i=0;i<512;i++)before[i]=i;
 const u16 tags[]={OBJ_EVENT_PAL_TAG_BRENDAN,OBJ_EVENT_PAL_TAG_MAY,OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION,OBJ_EVENT_PAL_TAG_MAY_REFLECTION,OBJ_EVENT_PAL_TAG_PLAYER_RED,OBJ_EVENT_PAL_TAG_PLAYER_GREEN,OBJ_EVENT_PAL_TAG_PLAYER_RED_REFLECTION,OBJ_EVENT_PAL_TAG_PLAYER_GREEN_REFLECTION,OBJ_EVENT_PAL_TAG_PLAYER_UNDERWATER};
 for(u32 t=0;t<9;t++)for(u32 outfit=0;outfit<=6;outfit++){
  memcpy(gPlttBufferUnfaded,before,sizeof(before));memcpy(gPlttBufferFaded,before,sizeof(before));save3.arcadeOutfit=outfit;ChaosArcadeOutfitPalette(tags[t],5);
  for(u32 i=0;i<512;i++){
   u32 idx=i-336;bool32 clothing=(t<4||t==8)?(idx==5||idx==6||idx==9||idx>=10&&idx<=14):(idx==5||idx==6||idx==7||idx>=9&&idx<=14);
   if(!outfit||!clothing)assert(gPlttBufferUnfaded[i]==before[i]);
   else assert(gPlttBufferUnfaded[i]!=before[i]);
   assert(gPlttBufferFaded[i]==gPlttBufferUnfaded[i]);
  }
 }
 memcpy(gPlttBufferUnfaded,before,sizeof(before));ChaosArcadeOutfitPalette(0x9999,5);assert(!memcmp(before,gPlttBufferUnfaded,sizeof(before)));
 puts("PASS all 2,560 theme/decoration combinations, full removal/restoration, clothing-only transforms for all nine avatar/reflection/underwater tags.");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'test.c';p.write_text(code);subprocess.run(['cc','-std=gnu11',str(p),'-o',d+'/test'],check=True);subprocess.run([d+'/test'],check=True)
# Actual production bindings / stock table and badge-only access.
s=(root/'data/maps/ChaosPokemonRanch/scripts.inc').read_text()
assert 'MAP_SCRIPT_ON_LOAD, ChaosRanch_Scenery' in s
for special in ['ChaosRanchOptions','ChaosRanchThemeMenu','ChaosRanchDecorationMenu','ChaosRanchApplyCosmetic']:assert 'special '+special in s
shop=(root/'data/maps/CeladonCity_DepartmentStore_4F_Frlg/scripts.inc').read_text()
assert 'goto_if_unset FLAG_BADGE06_GET, ChaosCeladonSpecialStock_Locked' in shop
stock=shop.split('ChaosCeladonSpecialStock_Items::')[1]
for item in ['NIDOKINGITE','STEELIXITE','GYARADOSITE','MANECTITE','ALAKAZITE','BEEDRILLITE','VENUSAURITE','GARCHOMPITE','HOUNDOOMINITE','PIDGEOTITE']:assert 'ITEM_'+item+'\n' not in stock
assert 'ITEM_ABILITY_PATCH' in stock and 'ITEM_LINKING_CORD' in shop
assets=root/'data/tilesets/secondary/chaos_ranch'
assert len((assets/'metatiles.bin').read_bytes())==329*16
assert len((assets/'metatile_attributes.bin').read_bytes())==329*4
# Collision footprints remain clear of every boxed Pokemon's roaming margin.
objs=json.loads((root/'data/maps/ChaosPokemonRanch/map.json').read_text())['object_events']
props=[(4,3,2,2),(10,3,2,2),(16,3,2,2),(30,3,2,2),(36,3,2,2),(42,3,2,2),(21,13,2,1),(26,13,2,1),(16,13,2,2),(30,13,2,2),(7,24,4,4)]
for x,y,w,h in props:
 for o in objs:assert not(x<=o['x']+1 and x+w>o['x']-1 and y<=o['y']+1 and y+h>o['y']-1)
print('PASS sign ownership menus, native Marsh Badge gate/reserved stock, terrain attributes and sprite-free decoration footprint safety.')

# Every custom furniture cell blocks entry, and foreground art stays behind
# an avatar standing on the reachable floor in front of it.
arcade=struct.unpack('<270H',(root/'data/layouts/CeladonCity_GameCorner_Frlg/map.bin').read_bytes())
furniture_attrs=(root/'data/tilesets/secondary/game_corner_frlg/metatile_attributes.bin').read_bytes()
for cell in arcade:
 id=cell&1023
 if 792<=id<=821:
  assert cell&0x0C00==0x0C00
  assert struct.unpack_from('<I',furniture_attrs,(id-640)*4)[0]&0x60000000==0x20000000
from PIL import Image
for theme in ['midnight','rocket']:
 im=Image.open(root/f'graphics/pokenav/region_map/map_kanto_{theme}.png')
 assert im.tobytes()==Image.open(root/'graphics/pokenav/region_map/map_kanto.png').tobytes()
print('PASS furniture collision/layer coverage and clean native chart geometry without grid or star artifacts.')

# Decode shipped affine charts and verify complete land-route bands.
import runpy
shipped=Image.open(root/'graphics/pokenav/region_map/map_kanto.png')
chart=Image.new('P',(512,512))
for i,t in enumerate((root/'graphics/pokenav/region_map/map_kanto.bin').read_bytes()):
 assert t//16*8 < shipped.height
 chart.paste(shipped.crop((t%16*8,t//16*8,t%16*8+8,t//16*8+8)),(i%64*8,i//64*8))
chart_data=runpy.run_path(str(root/'.github/scripts/clean_chaos_kanto_chart.py'))
original=chart_data['original']; mask=chart_data['mask']
assert all(chart.getpixel(pos) not in chart_data['terrain'] for pos in mask)
assert all(chart.getpixel((x,y))==original.getpixel((x,y))
           for y in range(160) for x in range(240) if (x,y) not in mask)
assert all(chart.getpixel(pos)==original.getpixel(pos) for pos in mask
           if original.getpixel(pos) not in chart_data['terrain'])
print('PASS continuous route bands; original markers, road shading and surrounding terrain preserved.')
