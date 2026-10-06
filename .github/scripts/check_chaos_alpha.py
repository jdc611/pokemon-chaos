#!/usr/bin/env python3
"""Exercise the actual one-mon challenge transaction with party identity fixtures."""
from pathlib import Path
import re, subprocess, tempfile
root=Path(__file__).resolve().parents[2]
source=re.sub(r'^#include.*\n','',(root/'src/chaos_mega_challenge.c').read_text(),flags=re.M)
fixture=r"""
#include <assert.h>
#include <string.h>
#include <stdio.h>
typedef unsigned char u8;typedef unsigned int u32;typedef int bool8;
#define TRUE 1
#define FALSE 0
#define EWRAM_DATA
#define PARTY_SIZE 6
#define B_TRAINER_PLAYER 0
#define SPECIES_NONE 0
#define MON_DATA_SPECIES 0
#define MON_DATA_IS_EGG 1
#define MON_DATA_HP 2
struct Pokemon {unsigned species, egg, hp, exp, item, identity;} gParties[1][6],saved[6];
u32 gSpecialVar_0x8004,gSpecialVar_Result,count,savedCount;
u32 GetMonData(struct Pokemon*p,int field){return field==0?p->species:field==1?p->egg:p->hp;}
void CalculatePlayerPartyCount(void){count=0;for(int i=0;i<6;i++)count+=gParties[0][i].species!=0;}
void SavePlayerParty(void){memcpy(saved,gParties[0],sizeof(saved));savedCount=count;}
void SavePlayerPartyMon(unsigned slot,struct Pokemon*p){saved[slot]=*p;}
void LoadPlayerParty(void){memcpy(gParties[0],saved,sizeof(saved));count=savedCount;}
"""
tests=r"""
int main(void){
 struct Pokemon original[6];
 for(unsigned slot=0;slot<6;slot++){
  for(unsigned i=0;i<6;i++)gParties[0][i]=(struct Pokemon){i+1,0,100,1000+i,20+i,100+i};
  CalculatePlayerPartyCount();memcpy(original,gParties[0],sizeof(original));
  gSpecialVar_0x8004=slot;ChaosBeginArcanineChallenge();assert(gSpecialVar_Result && count==1);
  assert(!memcmp(&gParties[0][0],&original[slot],sizeof(struct Pokemon)));
  for(int i=1;i<6;i++)assert(!gParties[0][i].species);
  ChaosBeginArcanineChallenge();assert(!gSpecialVar_Result);
  // A loss/consumed item/EXP update must affect the selected individual only.
  gParties[0][0].hp=0;gParties[0][0].item=0;gParties[0][0].exp+=50;
  original[slot].hp=0;original[slot].item=0;original[slot].exp+=50;
  ChaosRestoreArcanineChallengeParty();assert(count==6 && !memcmp(original,gParties[0],sizeof(original)));
  ChaosRestoreArcanineChallengeParty();assert(!memcmp(original,gParties[0],sizeof(original)));
 }
 for(unsigned bad=0;bad<4;bad++){
  gSpecialVar_0x8004=bad==0?6:0;gParties[0][0]=(struct Pokemon){1,0,100,0,0,7};
  if(bad==1)gParties[0][0].species=0;
  if(bad==2)gParties[0][0].egg=1;
  if(bad==3)gParties[0][0].hp=0;
  memcpy(original,gParties[0],sizeof(original));ChaosBeginArcanineChallenge();
  assert(!gSpecialVar_Result && !memcmp(original,gParties[0],sizeof(original)));
 }
 puts("PASS: all six challenge selections, exact party identity, faint/EXP/item persistence, double-restore and invalid/cancel/egg/fainted rejection.");
}
"""
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'challenge.c';p.write_text(fixture+source+tests)
 subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/challenge'],check=True)
 subprocess.run([d+'/challenge'],check=True)
setup=(root/'src/battle_setup.c').read_text();end=setup[setup.index('static void CB2_EndTrainerBattle(void)\n{'):]
assert end.index('ChaosRestoreArcanineChallengeParty();')<end.index('Nuzlocke_ProcessBattleDeaths();')<end.index('CB2_WhiteOut')
script=(root/'data/maps/LavenderTown_VolunteerPokemonHouse_Frlg/scripts.inc').read_text()
s=script[script.index('Chaos_EventScript_ArcanineClaim::'):script.index('Chaos_EventScript_ArcanineBagFull::')]
assert s.index('giveitem ITEM_ARCANITE')<s.index('goto_if_eq VAR_RESULT, FALSE')<s.index('setvar VAR_CHAOS_ARCANITE_REWARD, 2')
assert 'FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM, SPECIES_ARCANINE_MEGA, ITEM_ARCANITE' in (root/'src/data/pokemon/form_change_tables.h').read_text()
assert 'FORM_CHANGE_END_BATTLE, SPECIES_ARCANINE' in (root/'src/data/pokemon/form_change_tables.h').read_text()
print('PASS: restore-before-death/whiteout ordering, recoverable prize claim and player Mega/reversion integration.')

# checktrainerflag returns a comparison result, not VAR_RESULT. Never use stale receipt state.
assert 'ctx->comparisonResult = HasTrainerBeenFought(index)' in (root/'src/scrcmd.c').read_text()
assert 'checktrainerflag TRAINER_CHAOS_ARCANINE_SPECIALIST\n\tgoto_if TRUE, Chaos_EventScript_ArcanineWon' in script

# Exterior animated-door arrival can skip coordinate events on the landing tile.
import json
coords=json.loads((root/'data/maps/SaffronCity_Frlg/map.json').read_text())['coord_events']
assert {(46,13),(46,14),(47,13)} <= {(c['x'],c['y']) for c in coords if c['script']=='EventScript_ChaosOakMegaReward'}
