#!/usr/bin/env python3
"""Exercise the actual BST/evolution C functions with bounded species fixtures."""
from pathlib import Path
import ctypes
import subprocess
import tempfile

repo = Path(__file__).resolve().parents[2]
source = (repo / 'src/pokemon.c').read_text()
def section(start, end, after=0):
    a = source.index(start, after)
    return source[a:source.index(end, a)]

prefix = r'''
#include <stdint.h>
typedef uint8_t u8; typedef uint32_t u32; typedef int bool32;
#define TRUE 1
#define FALSE 0
#define NULL ((void *)0)
#define NUM_STATS 6
#define NUM_SPECIES 128
#define SPECIES_NONE 0
#define SPECIES_EGG 127
#define STAT_HP 0
#define STAT_ATK 1
#define STAT_DEF 2
#define STAT_SPEED 3
#define STAT_SPATK 4
#define STAT_SPDEF 5
#define RUN_BST_OFF 0
#define RUN_BST_SHUFFLE 1
#define RUN_BST_RANDOM 2
#define RUN_DIFFICULTY_HARD 2
#define RUN_DIFFICULTY_NUZLOCKE 3
#define RANDOM_EVO_CLASS_HAS_NEXT 2
#define RANDOM_EVO_CLASS_THREE_START 4
enum Species { SPECIES_FIXTURE = 1 };
struct SpeciesInfo {u8 baseHP,baseAttack,baseDefense,baseSpeed,baseSpAttack,baseSpDefense;};
struct SpeciesInfo gSpeciesInfo[128];
struct Save {u32 bstMode,worldSeed;} save, *gSaveBlock3Ptr=&save;
static u8 sRandomEvolutionClass[128];
static enum Species sRandomEvolutionMiddlePool[128],sRandomEvolutionFinalPool[128],sRandomEvolutionSpecialPool[128];
static u32 sRandomEvolutionMiddleCount=0,sRandomEvolutionFinalCount=0,sRandomEvolutionSpecialCount=0;
static void BuildRandomEvolutionPools(void) {}
static enum Species SanitizeSpeciesId(enum Species s) {return s;}
'''
code = prefix
code += section('static u32 GetRawSpeciesBaseStat(', 'static void GetRunRandomizedBaseStats(', source.index('static u32 RunBstHash(')-1500)
code += section('static void GetRunBaseStatsForSettings(', 'static bool32 SpeciesHasFurtherEvolution(')
code += section('enum Species GetRandomEvolutionTargetForSettings(', 'static bool32 DoesSpeciesOrReachableFormMatchRunFilterInternal(')
code += r'''
void fixture(void) {
 for(u32 s=0;s<128;s++) {
  u8 *p=(u8*)&gSpeciesInfo[s];
  for(u32 i=0;i<6;i++) p[i]=5+(s*17+i*31)%216;
  sRandomEvolutionClass[s]=RANDOM_EVO_CLASS_HAS_NEXT;
 }
 for(u32 s=2;s<127;s++) sRandomEvolutionFinalPool[sRandomEvolutionFinalCount++]=s;
}
void stats(u32 s,u32 mode,u32 seed,u8 *out) {GetRunBaseStatsForSettings(s,out,mode,seed);}
u32 evolution(u32 s,u32 mode,u32 seed) {save.bstMode=mode;return GetRandomEvolutionTargetForSettings(s,RUN_DIFFICULTY_HARD,seed);}
'''
with tempfile.TemporaryDirectory() as tmp:
    path=Path(tmp); (path/'test.c').write_text(code)
    subprocess.run(['cc','-std=c11','-O2','-shared','-fPIC',str(path/'test.c'),'-o',str(path/'test.so')],check=True)
    lib=ctypes.CDLL(str(path/'test.so')); lib.fixture()
    def stats(s,mode,seed):
        out=(ctypes.c_uint8*6)();lib.stats(s,mode,seed,out);return tuple(out)
    extremes=rolls=0; totals=set()
    for seed in range(200):
        for s in range(1,127):
            normal=stats(s,0,seed); shuffle=stats(s,1,seed); random=stats(s,2,seed)
            assert sum(shuffle)==sum(normal),(s,seed,normal,shuffle)
            assert all(5<=v<=220 for v in shuffle+random)
            assert random==stats(s,2,seed)
            extremes+=sum(v>160 for v in random);rolls+=6;totals.add(sum(random))
    rate=extremes/rolls
    assert .047<rate<.053,rate
    assert max(totals)-min(totals)>500
    for mode in range(3):
        for seed in range(20):
            pool={s:sum(stats(s,mode,seed)) for s in range(2,127)}
            for s in range(1,127):
                target=lib.evolution(s,mode,seed)
                candidates={k:v for k,v in pool.items() if k!=s}
                current=sum(stats(s,mode,seed))
                assert target!=s and target in candidates
                if any(v>=current for v in candidates.values()):
                    assert candidates[target]>=current
                else:
                    assert candidates[target]==max(candidates.values())
    print(f'PASS: 25,200 species/seed stat profiles; extremes {rate:.2%}; 7,560 evolution selections.')
