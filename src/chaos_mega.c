#include "global.h"
#include "battle.h"
#include "battle_util.h"
#include "chaos_mega.h"
#include "constants/abilities.h"
#include "constants/moves.h"

void ChaosRampageStart(u32 battler, u32 move)
{
    if (GetBattlerAbility(battler) == ABILITY_RAMPAGE
     && move != MOVE_STRUGGLE && move != MOVE_NONE
     && !IsBattleMoveStatus(move)
     && gBattleMons[battler].volatiles.chaosRampageTurns == 0)
    {
        gBattleMons[battler].volatiles.chaosRampageTurns = 3;
        gBattleMons[battler].volatiles.chaosRampageMove = move;
    }
}

void ChaosRampageEndTurn(u32 battler)
{
    u32 move = gBattleMons[battler].volatiles.chaosRampageMove;
    bool32 hasPP = FALSE;
    if (gBattleMons[battler].volatiles.chaosRampageTurns == 0)
        return;
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (gBattleMons[battler].moves[i] == move && gBattleMons[battler].pp[i] > 0)
            hasPP = TRUE;
    if (!hasPP || !gBattleMons[battler].hp || GetBattlerAbility(battler) != ABILITY_RAMPAGE)
        gBattleMons[battler].volatiles.chaosRampageTurns = 0;
    else
        gBattleMons[battler].volatiles.chaosRampageTurns--;
    if (gBattleMons[battler].volatiles.chaosRampageTurns == 0)
        gBattleMons[battler].volatiles.chaosRampageMove = MOVE_NONE;
}
