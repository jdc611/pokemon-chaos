"""Played Route 4 story pair with controlled battle difficulty, not balance QA."""
from qa_ironmon_core import *
import json, sys
BS,BHP,BABILITY,BTYPES,BSTATUS,EXP,SWIFT,NORMAL,NONE,JESSIE,JAMES,_,_=struct.unpack(
    '<13I',(ROOT/'ironmon-played-pair-layout.bin').read_bytes())
groups=json.loads((repo/'data/maps/map_groups.json').read_text())
def key(): frames(3,1);frames(25)
def warp(name,x,y):
    for gi,g in enumerate(groups['group_order']):
        if name in groups[g]: ni=groups[g].index(name);break
    call('SetWarpDestinationToMapWarp',gi,ni,255)
    wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2)
    call('DoWarp');frames(240)
silph = '--silph' in sys.argv
if silph:JESSIE+=2;JAMES+=2
start();call('IronmonGiveStarter',BULBA)
# A high-level fixture keeps this a transition/story test rather than a seed
# balance test. Opponents are reduced to one HP and Normal/no-ability while
# active; no victory outcome or battle-end callback is injected.
setdata(p,EXP,2000000);call('CalculateMonStats',p)
for m in range(4):call('SetMonMoveSlot',p,SWIFT,m)
main_species=data(p,SP);
call('ClearTrainerFlag',JESSIE);call('ClearTrainerFlag',JAMES)
warp('SilphCo_7F_Frlg' if silph else 'Route4_Frlg',18,8)
call('ScriptContext_SetupScript',symbols['EventScript_ChaosSilphChooseThree' if silph else 'EventScript_ChaosMtMoonRockets']);call('ScriptContext_Enable')
phases=set();first_flag_seen=False;second_started=False
for step in range(2200):
    phase=rd('sIronmonPairPhase',1);phases.add(phase)
    if phase==2:second_started=True
    if rd(symbols['gMain']+4)==symbols['BattleMainCB2']|1:
        assert rd('gBattlersCount',1)==2 and rd('gBattleTypeFlags') & 8 and not rd('gBattleTypeFlags') & (1|64|32768|4194304), (step,phase,rd('gBattlersCount',1),hex(rd('gBattleTypeFlags')))
        enemy=symbols['gBattleMons']+BS
        if rd(enemy+BHP,2)>1:wr(enemy+BHP,1,2)
        wr(enemy+BABILITY,NONE,2)
        for t in range(3):wr(enemy+BTYPES+t,NORMAL,1)
        wr(symbols['gBattleMons']+BSTATUS,0)
    if call('HasTrainerBeenFought',JESSIE):first_flag_seen=True
    key()
    if (second_started and call('HasTrainerBeenFought',JAMES)
        and rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
        and not call('ArePlayerFieldControlsLocked')):break
else:
    snap('ironmon-pair-stalled')
    raise AssertionError(('pair did not finish',phases,step,rd('gBattleOutcome',1),
                          hex(rd(symbols['gMain']+4))))
assert {1,2} <= phases and first_flag_seen
assert call('HasTrainerBeenFought',JESSIE) and call('HasTrainerBeenFought',JAMES)
assert call('VarGet',0x405c if silph else 0x4000)==1
assert rd('gPartiesCount',1)==1 and data(p,SP)==main_species
assert not rd(s+IM+ENDED,1)
assert data(p,HP)>0
snap('ironmon-played-silph' if silph else 'ironmon-played-pair')
print('PASS played '+('Silph partner conversion' if silph else 'Route 4')+' Jessie -> James Singles; story completion, separate flags, sole main, return control. Controlled difficulty; no balance claim.',flush=True)
