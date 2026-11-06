#include "global.h"
#include "battle.h"
// Compiled masks capture exact nested bitfield locations and widths.
#pragma GCC diagnostic ignored "-Woverflow"
const struct BattlePokemon volatileMasks[] __attribute__((section(".qa_volatiles")))={
 {.volatiles.disabledMove=~0u},{.volatiles.torment=1},
 {.volatiles.tauntTimer=~0u},{.volatiles.encoreTimer=~0u},
 {.volatiles.encoredMove=~0u},{.volatiles.throatChopTimer=~0u}
};
