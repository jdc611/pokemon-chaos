#include "global.h"
#include "pokemon.h"
#include "load_save.h"
#include "event_data.h"

static EWRAM_DATA bool8 sArcanineChallengeActive = FALSE;
static EWRAM_DATA u8 sArcanineChallengeSlot = 0;

void ChaosBeginArcanineChallenge(void)
{
    u32 slot = gSpecialVar_0x8004;
    struct Pokemon chosen;

    gSpecialVar_Result = FALSE;
    if (sArcanineChallengeActive || slot >= PARTY_SIZE
     || GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES) == SPECIES_NONE
     || GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_IS_EGG)
     || GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HP) == 0)
        return;

    chosen = gParties[B_TRAINER_PLAYER][slot];
    SavePlayerParty();
    sArcanineChallengeSlot = slot;
    memset(gParties[B_TRAINER_PLAYER], 0, sizeof(gParties[B_TRAINER_PLAYER]));
    gParties[B_TRAINER_PLAYER][0] = chosen;
    CalculatePlayerPartyCount();
    sArcanineChallengeActive = TRUE;
    gSpecialVar_Result = TRUE;
}

void ChaosRestoreArcanineChallengeParty(void)
{
    if (!sArcanineChallengeActive)
        return;
    // Keep real battle changes to the chosen individual, including fainting,
    // EXP and consumed items. Every unchosen party member remains untouched.
    SavePlayerPartyMon(sArcanineChallengeSlot, &gParties[B_TRAINER_PLAYER][0]);
    LoadPlayerParty();
    CalculatePlayerPartyCount();
    sArcanineChallengeActive = FALSE;
}
