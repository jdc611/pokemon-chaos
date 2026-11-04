#include "global.h"
#include <string.h>
#include "gym_leader.h"
#include "data.h"
#include "overworld.h"
#include "wild_encounter.h"
#include "constants/maps.h"
#include "constants/map_types.h"
#include "data/gym_leader_ecology.h"
enum {GLE_BIOME_ROUTE,GLE_BIOME_FOREST,GLE_BIOME_CAVE,GLE_BIOME_WATER,GLE_BIOME_ICE,GLE_BIOME_POWER,GLE_BIOME_MANSION,GLE_BIOME_VOLCANO};
struct EcologyCache {u32 seed,key;u16 species;};
static EWRAM_DATA struct EcologyCache sEcologyCache[64];
static EWRAM_DATA u8 sAllowed[(ARRAY_COUNT(sEcologySpecies)+7)/8];
static EWRAM_DATA u8 sAllowedRank;
static bool32 HasType(const struct SpeciesInfo *p,u32 t){return p->types[0]==t||p->types[1]==t;}
u32 GymLeaderEcologyBiome(u32 map,u32 area)
{
 const struct MapHeader *header;
 if(area==WILD_AREA_WATER||area==WILD_AREA_FISHING)return GLE_BIOME_WATER;
 if(map==MAP_VIRIDIAN_FOREST||map==MAP_THREE_ISLAND_BERRY_FOREST||map==MAP_SIX_ISLAND_PATTERN_BUSH)return GLE_BIOME_FOREST;
 if(map==MAP_POWER_PLANT)return GLE_BIOME_POWER;
 if(map>=MAP_POKEMON_MANSION_1F&&map<=MAP_POKEMON_MANSION_B1F)return GLE_BIOME_MANSION;
 if(map>=MAP_SEAFOAM_ISLANDS_1F&&map<=MAP_SEAFOAM_ISLANDS_B4F)return GLE_BIOME_ICE;
 if(map>=MAP_MT_EMBER_RUBY_PATH_B4F&&map<=MAP_MT_EMBER_RUBY_PATH_B2F_STAIRS)return GLE_BIOME_VOLCANO;
 header=Overworld_GetMapHeaderByGroupAndId(map>>8,map&255);
 return area==WILD_AREA_ROCKS||header->mapType==MAP_TYPE_UNDERGROUND?GLE_BIOME_CAVE:GLE_BIOME_ROUTE;
}
static bool32 MatchesBiome(const struct SpeciesInfo *p,u32 biome)
{
 switch(biome){
 case GLE_BIOME_WATER:return HasType(p,TYPE_WATER);
 case GLE_BIOME_FOREST:return HasType(p,TYPE_GRASS)||HasType(p,TYPE_BUG)||HasType(p,TYPE_POISON)||HasType(p,TYPE_FAIRY);
 case GLE_BIOME_CAVE:return HasType(p,TYPE_ROCK)||HasType(p,TYPE_GROUND)||HasType(p,TYPE_STEEL)||HasType(p,TYPE_DARK)||HasType(p,TYPE_GHOST)||HasType(p,TYPE_FIGHTING);
 case GLE_BIOME_ICE:return HasType(p,TYPE_ICE)||HasType(p,TYPE_WATER);
 case GLE_BIOME_POWER:return HasType(p,TYPE_ELECTRIC)||HasType(p,TYPE_STEEL);
 case GLE_BIOME_MANSION:return HasType(p,TYPE_FIRE)||HasType(p,TYPE_POISON)||HasType(p,TYPE_GHOST)||HasType(p,TYPE_DARK);
 case GLE_BIOME_VOLCANO:return HasType(p,TYPE_FIRE)||HasType(p,TYPE_ROCK)||HasType(p,TYPE_GROUND);
 case GLE_BIOME_ROUTE:return !((p->types[0]==TYPE_WATER&&p->types[1]==TYPE_WATER)||(p->types[0]==TYPE_ICE&&p->types[1]==TYPE_ICE));
 }
 return TRUE;
}
bool32 GymLeaderEcologyEligible(u32 species,u32 biome,u32 rank)
{
 const struct SpeciesInfo *p;u32 bst;
 if(!species||species>=SPECIES_GLE_AWAKENED_SLAKING||rank<1||rank>8)return FALSE;
 p=&gSpeciesInfo[species];
 if(!p->baseHP||p->isRestrictedLegendary||p->isSubLegendary||p->isMythical||p->isUltraBeast||p->isParadox||GymLeaderSpeciesReserved(species))return FALSE;
 if(p->isMegaEvolution||p->isPrimalReversion||p->isGigantamax||p->isTotem||p->isUltraBurst||p->isTeraForm)return FALSE;
 if(species>SPECIES_PECHARUNT&&!p->isAlolanForm&&!p->isGalarianForm&&!p->isHisuianForm&&!p->isPaldeanForm)return FALSE;
 bst=p->baseHP+p->baseAttack+p->baseDefense+p->baseSpAttack+p->baseSpDefense+p->baseSpeed;if(bst>320+45*rank)return FALSE;
 return MatchesBiome(p,biome);
}
u32 GymLeaderEcologySpecies(u32 map,u32 area,u32 slot,u32 time)
{
 struct GleState *s=GymLeaderState();u32 biome=GymLeaderEcologyBiome(map,area),key,seed,best=0xffffffffu,i;u16 result=SPECIES_NONE;struct EcologyCache *cache;
 if(!IsGymLeaderRun())return SPECIES_NONE;
 if(area==WILD_AREA_WATER)slot%=3;
 else if(area==WILD_AREA_FISHING)slot=slot<2?0:slot<5?1+(slot%2):3+(slot%2);
 else slot%=6;
 key=(map&65535)|((area&7)<<16)|((slot&15)<<19)|((time&7)<<23)|((s->rank&15)<<26);
 seed=s->seed;cache=&sEcologyCache[GleMix(key^seed)&63];if(cache->species&&cache->key==key&&cache->seed==seed)return cache->species;
 if(sAllowedRank!=s->rank){
  memset(sAllowed,0,sizeof(sAllowed));for(i=0;i<ARRAY_COUNT(sEcologySpecies);i++)if(GymLeaderEcologyEligible(sEcologySpecies[i],8,s->rank))sAllowed[i/8]|=1u<<(i%8);sAllowedRank=s->rank;
 }
 for(i=0;i<ARRAY_COUNT(sEcologySpecies);i++)if((sAllowed[i/8]&(1u<<(i%8)))&&MatchesBiome(&gSpeciesInfo[sEcologySpecies[i]],biome)){
  u32 score=GleMix(seed^key^((u32)sEcologySpecies[i]*0x9e3779b9u));if(score<best){best=score;result=sEcologySpecies[i];}
 }
 cache->seed=seed;cache->key=key;cache->species=result;return result;
}
