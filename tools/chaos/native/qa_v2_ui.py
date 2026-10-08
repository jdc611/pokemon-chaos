from qa import *
assert lib.boot(str(repo/'pokefirered.gba').encode())
def key(k=1,n=35):frames(2,k);frames(n)
def reset():state('pass-world',1)
reset();call('SetMainCallback2',symbols['CB2_InitOptionMenu']|1);frames(200);snap('v2-options')
# Ability selector showing all legitimate slots and current ability, native close.
reset();call('ZeroPlayerPartyMons');call('ScriptGiveMon',123,20,0);wr('gSpecialVar_0x8004',0,2);call('ChaosOpenAbilityMenu');frames(50);snap('v2-ability');key(128);snap('v2-ability-description');key(2)
# Party unlock / three-way relearner directly from the selected slot.
reset();call('VarSet',0x408f,1);call('ZeroPlayerPartyMons');call('ScriptGiveMon',123,20,0);call('CB2_PartyMenuFromStartMenu');frames(180);key();key(128);key(128);key();snap('v2-relearn-types');key(128);key(128);key();frames(150);snap('v2-relearn-egg');key(2);key();frames(120);snap('v2-relearn-return')
# Native Bag details with six correct icons, disabled learners and a known move.
reset();call('ZeroPlayerPartyMons')
for sp in [123,25,93,1,7,150]:call('ScriptGiveMon',sp,20,0)
import struct
move,_,item,_=struct.unpack('<4I',(ROOT/'v2-ids.bin').read_bytes());assert item
call('ClearBag');call('AddBagItem',item,1);call('CB2_BagMenuFromStartMenu');frames(180);wr('gSpecialVar_ItemId',item,2)
# Transition uses the same native action handler and task close as the Bag menu.
active=[]
for i in range(16):
 if rd(symbols['gTasks']+40*i+4,1):active.append(i)
call('ItemMenu_TmDetails',active[0]);frames(200);snap('v2-tm-details');key(2);frames(180);snap('v2-tm-return');print('PASS native Options, ability selector, three relearn categories/egg list and Bag TM details/return screenshots.',flush=True)
