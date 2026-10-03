#!/usr/bin/env python3
"""Compile the actual Rampage state code against small battle-engine fixtures."""
from pathlib import Path
import re, subprocess, tempfile
root=Path(__file__).resolve().parents[2]
source=(root/'src/chaos_mega.c').read_text()
source=re.sub(r'^#include.*\n','',source,flags=re.M)
stubs=r'''
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef unsigned int u32;
typedef int bool32;
#define TRUE 1
#define FALSE 0
#define MAX_MON_MOVES 4
#define MOVE_NONE 0
#define MOVE_STRUGGLE 1
#define ABILITY_RAMPAGE 10
struct Mon { struct {u32 chaosRampageTurns, chaosRampageMove;} volatiles; u32 moves[4],pp[4],hp; } gBattleMons[4];
u32 abilities[4];
u32 GetBattlerAbility(u32 b){return abilities[b];}
bool32 IsBattleMoveStatus(u32 m){return m==2;}
'''
tests=r'''
int main(void){
 abilities[0]=ABILITY_RAMPAGE;
 gBattleMons[0].hp=100;gBattleMons[0].moves[0]=3;gBattleMons[0].pp[0]=10;
 ChaosRampageStart(0,2);assert(!gBattleMons[0].volatiles.chaosRampageTurns);
 ChaosRampageStart(0,MOVE_STRUGGLE);assert(!gBattleMons[0].volatiles.chaosRampageTurns);
 ChaosRampageStart(0,3);assert(gBattleMons[0].volatiles.chaosRampageTurns==3);
 // Repeated invocation for spread targets/called move handling cannot restart.
 ChaosRampageStart(0,3);ChaosRampageStart(0,4);
 assert(gBattleMons[0].volatiles.chaosRampageMove==3);
 ChaosRampageEndTurn(0);assert(gBattleMons[0].volatiles.chaosRampageTurns==2);
 ChaosRampageStart(0,3);ChaosRampageEndTurn(0);assert(gBattleMons[0].volatiles.chaosRampageTurns==1);
 ChaosRampageStart(0,3);ChaosRampageEndTurn(0);assert(!gBattleMons[0].volatiles.chaosRampageTurns && !gBattleMons[0].volatiles.chaosRampageMove);
 ChaosRampageStart(0,3);gBattleMons[0].pp[0]=0;ChaosRampageEndTurn(0);assert(!gBattleMons[0].volatiles.chaosRampageTurns);
 gBattleMons[0].pp[0]=5;ChaosRampageStart(0,3);abilities[0]=0;ChaosRampageEndTurn(0);assert(!gBattleMons[0].volatiles.chaosRampageTurns);
 abilities[0]=ABILITY_RAMPAGE;ChaosRampageStart(0,3);gBattleMons[0].hp=0;ChaosRampageEndTurn(0);assert(!gBattleMons[0].volatiles.chaosRampageTurns);
 ChaosRampageStart(1,3);assert(!gBattleMons[1].volatiles.chaosRampageTurns);
 puts("PASS: actual Rampage state machine, status/Struggle exclusion, three turns, spread target stability, PP exhaustion, fainting and ability loss.");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'mega.c';p.write_text(stubs+source+tests)
 subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/mega'],check=True)
 subprocess.run([d+'/mega'],check=True)
# Check the real integration points and transactional script paths.
assert 'UQ_4_12(1.4)' in (root/'src/battle_util.c').read_text()
assert 'ChaosRampageStart(gBattlerAttacker, gChosenMove)' in (root/'src/battle_script_commands.c').read_text()
assert 'ChaosRampageEndTurn(i)' in (root/'src/battle_end_turn.c').read_text()
assert 'chaosRampageTurns > 0' in (root/'src/battle_main.c').read_text()
f=(root/'data/maps/CinnabarIsland_PokemonLab_ExperimentRoom_Frlg/scripts.inc').read_text()
s=f[f.index('Chaos_EventScript_GiveNidokingite::'):]
assert s.index('giveitem ITEM_NIDOKINGITE') < s.index('goto_if_eq VAR_RESULT, FALSE') < s.index('removeitem ITEM_STRANGE_FOSSIL') < s.index('setvar VAR_CHAOS_STRANGE_FOSSIL, 2')
assert '&& forms[i].param1 != ITEM_NIDOKINGITE' in (root/'src/chaos_progression.c').read_text()
assert 'Nidoking @ Nidokingite\nLevel: 58' in (root/'src/data/trainers_frlg.party').read_text()
print('PASS: battle integration, protected fossil transaction and Giovanni stone.')
