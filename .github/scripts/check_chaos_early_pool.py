#!/usr/bin/env python3
"""Verify the opening region shares a bounded seeded pool across land and Old Rod."""
from pathlib import Path
import re,subprocess,tempfile
root=Path(__file__).resolve().parents[2];source=(root/'src/wild_encounter.c').read_text()
def function(name):
 m=re.search(r'^static (?:bool32|enum Species) '+name+r'\([^;]*?\)\n\{',source,re.M);assert m,name
 i=m.end();depth=1
 while depth:depth+=(source[i]=='{')-(source[i]=='}');i+=1
 return source[m.start():i]
code=r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint8_t u8;typedef uint16_t u16;typedef uint32_t u32;typedef u32 rng_value_t;typedef int bool32;typedef int bool8;
#define TRUE 1
#define FALSE 0
#define SPECIES_NONE 0
#define FILTER_FUNC_ARG_NONE 0xffff
#define RUN_WILD_RANDOM 1
#define RUN_WILD_SCALED 2
#define RUN_FILTER_TYPE 1
#define RUN_FILTER_ABILITY 2
#define RUN_FILTER_TYPE_ABILITY 3
#define SPECIES_GENERATOR_TYPE_FILTERED 1
#define SPECIES_GENERATOR_SCALED_TYPE_FILTERED 2
#define SPECIES_GENERATOR_ABILITY_FILTERED 3
#define SPECIES_GENERATOR_SCALED_ABILITY_FILTERED 4
#define SPECIES_GENERATOR_TYPE_ABILITY_FILTERED 5
#define SPECIES_GENERATOR_SCALED_TYPE_ABILITY_FILTERED 6
#define MAP_PALLET_TOWN 0x2600
#define MAP_VIRIDIAN_CITY 0x2602
#define MAP_PEWTER_CITY 0x2612
#define MAP_ROUTE1 0x2601
#define MAP_ROUTE2 0x2510
#define MAP_ROUTE22 0x2524
#define MAP_VIRIDIAN_FOREST 0x2611
enum Species {dummy};enum WildPokemonArea {WILD_AREA_LAND,WILD_AREA_WATER,WILD_AREA_ROCKS,WILD_AREA_FISHING};
struct {u32 worldSeed;u16 filterValue;u8 filterMode,randomizerEnabled,abilityMode,evolutionMode,runDifficulty;} save,*gSaveBlock3Ptr=&save;
struct FilterFuncArgs {u32 arg1,arg2;};
u16 sEarlyKantoFilteredPool[3],sEarlyKantoFilteredValue;u8 sEarlyKantoFilteredCount,sEarlyKantoFilteredSettings[5];u32 sEarlyKantoFilteredSeed,gRngValue;bool8 sEarlyKantoFilteredValid;
u32 calls,lastGenerator,lastTier;
void SeedRng(u32 seed){gRngValue=seed;}
u32 PickRandomStarterSpecies(u32 generator,const struct FilterFuncArgs *args,u16 *out){
 calls++;lastGenerator=generator;lastTier=args->arg2;
 u16 base=100+gRngValue%17+args->arg1%13;
 for(u32 i=0;i<3;i++){out[i]=base+i;}gRngValue++;return 3;
}
'''+'\n'.join(function(n) for n in ['IsEarlyKantoFilteredArea','GetEarlyKantoFilteredSpecies'])+r'''
int main(void){
 int maps[]={MAP_ROUTE1,MAP_ROUTE2,MAP_ROUTE22,MAP_VIRIDIAN_FOREST};
 for(int filter=1;filter<=3;filter++)for(int mode=1;mode<=2;mode++){
  save.filterMode=filter;save.randomizerEnabled=mode;save.worldSeed=123;save.filterValue=16;sEarlyKantoFilteredValid=0;gRngValue=999;calls=0;
  u16 seen[3]={0};int count=0;
  for(int m=0;m<4;m++)for(int slot=0;slot<12;slot++){
   int map=maps[m];assert(IsEarlyKantoFilteredArea(map>>8,map&255));
   u16 s=GetEarlyKantoFilteredSpecies(WILD_AREA_LAND,slot,map>>8,map&255);int found=0;
   for(int i=0;i<count;i++)found|=seen[i]==s;
   if(!found){assert(count<3);seen[count++]=s;}
  }
  assert(count==3&&calls==1&&gRngValue==999);assert(lastGenerator==2*filter-(mode==1));assert(lastTier==(mode==2?0:0xffff));
  for(int m=0;m<3;m++)for(int slot=0;slot<2;slot++){
   int cities[]={MAP_PALLET_TOWN,MAP_VIRIDIAN_CITY,MAP_PEWTER_CITY};int map=cities[m];
   assert(GetEarlyKantoFilteredSpecies(WILD_AREA_FISHING,slot,map>>8,map&255)==seen[2]);
  }
  assert(calls==1);u16 first=seen[0];sEarlyKantoFilteredValid=0;assert(GetEarlyKantoFilteredSpecies(WILD_AREA_LAND,0,38,1)==first&&calls==2);
  save.worldSeed++;GetEarlyKantoFilteredSpecies(WILD_AREA_LAND,0,38,1);assert(calls==3);
  save.filterValue++;GetEarlyKantoFilteredSpecies(WILD_AREA_LAND,0,38,1);assert(calls==4);
  save.abilityMode++;GetEarlyKantoFilteredSpecies(WILD_AREA_LAND,0,38,1);assert(calls==5);
  save.evolutionMode++;GetEarlyKantoFilteredSpecies(WILD_AREA_LAND,0,38,1);assert(calls==6);
 }
 assert(!IsEarlyKantoFilteredArea(42,0));
 puts("PASS: actual early Kanto allocation shares at most three wild choices across four land areas and Old Rod, with stable seeds, Scaled tier zero, all filter modes, RNG restoration and cache invalidation.");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'check.c';p.write_text(code)
 subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/check'],check=True)
 subprocess.run([d+'/check'],check=True)
generator=source[source.index('static enum Species GenerateRandomizedWildSpeciesForMap'):source.index('enum Species GetRandomizedWildSpeciesForMap')]
assert 'IS_FRLG && gSaveBlock3Ptr->filterMode != RUN_FILTER_NONE' in generator
assert 'area == WILD_AREA_FISHING && wildMonIndex < 2' in generator
print('PASS: early pacing applies to filtered Kanto land/Old Rod; unfiltered, later areas, Surf and upgraded rods retain their existing paths.')
