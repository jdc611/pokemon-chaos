#include "global.h"
#include "chaos_arcade.h"
#include "palette.h"
#include "constants/maps.h"

// Cosmetic transformations only touch background colors, never object palettes,
// map behavior, collision, encounter tables or any Pokemon data.
void ChaosArcadeRanchPalette(u16 start,u16 count,bool8 skipFaded)
{
    if(gSaveBlock1Ptr->location.mapGroup!=MAP_GROUP(MAP_CHAOS_POKEMON_RANCH)||gSaveBlock1Ptr->location.mapNum!=MAP_NUM(MAP_CHAOS_POKEMON_RANCH))return;
    ChaosArcadeEnsureSave();u32 theme=gSaveBlock3Ptr->arcadeRanchTheme;
    if(theme==0||theme>4)return;
    for(u32 i=start;i<start+count;i++){
        u16 color=gPlttBufferUnfaded[i];u32 r=color&31,g=(color>>5)&31,b=(color>>10)&31;
        if(theme==4){r=r*3/5;g=g*3/5;b=min(31,b*3/5+4);}
        else if(g>r+2&&g>b+2){
            if(theme==1){r=r*2/3;g=min(31,g+2);b=b*2/3;}
            if(theme==2){r=min(31,12+g*3/5);b=5+g/3;g=min(31,10+g/2);}
            if(theme==3){r=18+g/3;b=20+g/3;g=19+g/3;}
        }
        color=r|(g<<5)|(b<<10);gPlttBufferUnfaded[i]=color;if(!skipFaded)gPlttBufferFaded[i]=color;
    }
}
void ChaosArcadeRiderPalette(void)
{
    ChaosArcadeEnsureSave();u32 theme=gSaveBlock3Ptr->arcadeRiderTheme;
    if(theme==0||theme>2)return;
    for(u32 i=17;i<32;i++){
        u32 color=gPlttBufferUnfaded[i],r=color&31,g=(color>>5)&31,b=(color>>10)&31;
        u32 light=(r+g+b)/3;
        if(theme==1){r=light/3;g=light/2;b=min(31,light/2+7);}
        else {r=min(31,light+4);g=light/4;b=light/3;}
        gPlttBufferUnfaded[i]=gPlttBufferFaded[i]=r|(g<<5)|(b<<10);
    }
}
