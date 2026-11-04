#include "global.h"
#include "pokemon.h"
#include "main.h"
#include "constants/maps.h"
#include <stddef.h>
const unsigned gleWindsLayout[]={offsetof(struct GleState,winds),sizeof(struct GleState),GLE_VERSION,sizeof(struct GleWind),offsetof(struct GleWindsState,selected),offsetof(struct GleWind,flags),offsetof(struct GleWind,ivs),offsetof(struct GleWind,personality),offsetof(struct GleWind,status),offsetof(struct GleWind,hp),offsetof(struct GleWind,maxHp),offsetof(struct GleWind,mapGroup),offsetof(struct GleWind,mapNum),offsetof(struct GleWind,level),offsetof(struct SaveBlock1,roamer),sizeof(struct Roamer),offsetof(struct Roamer,species),offsetof(struct Roamer,level),offsetof(struct Roamer,hp),offsetof(struct Roamer,active),offsetof(struct Roamer,personality),offsetof(struct Roamer,ivs),MON_DATA_STATUS,MON_DATA_IS_SHINY,B_OUTCOME_PLAYER_TELEPORTED,BATTLE_TYPE_ROAMER,MAP_ROUTE9,MON_DATA_MAX_HP,offsetof(struct SaveBlock1,location.mapGroup),offsetof(struct SaveBlock1,location.mapNum),SPECIES_LATIAS,offsetof(struct Main,savedCallback),MON_DATA_IVS};
