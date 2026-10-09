#include "global.h"
#include "main.h"
#include "chaos_input.h"
// Shared across nested menus: a held key is accepted only after release.
void ChaosFilterMenuInput(void)
{
    static u16 latched;
    latched &= gMain.heldKeys;
    gMain.newKeys &= ~latched;
    latched |= gMain.heldKeys;
    gMain.newAndRepeatedKeys = gMain.newKeys;
}
