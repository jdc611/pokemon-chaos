"""Tracker display restoration and paused input; journal has separate fixtures."""
from qa_ironmon_core import *
import sys
def blob(ptr,size):return bytes(rd(ptr+i,1) for i in range(size))
def snapshot():
    return {name:blob(address,size) for name,address,size in (
        ('party',p,6*MONSIZE),('save3',s,SIZE),
        ('storage',rd('gPokemonStoragePtr'),STORAGE),
        ('chars',0x06008000,0x4000),('map',0x0600f800,0x800),
        ('windows',symbols['gWindows'],32*12),
        ('tasks',symbols['gTasks'],16*40))}
def check():
    before=snapshot();callbacks=blob(symbols['gMain'],24);rng=blob(symbols['gRngValue'],16)
    assert call('ChaosTrackerTryOpen')
    frames(3)
    assert call('ChaosTrackerIsOpen')
    snap('ironmon-tracker-stats')
    for page in range(6):
        frames(12,256);chars=blob(0x06008000,0x4000)
        frames(60,256)
        assert chars==blob(0x06008000,0x4000), 'held R repeated pages'
        frames(4)
    snap('ironmon-tracker-profile')
    assert before['party']==blob(p,6*MONSIZE)
    assert before['tasks']==blob(symbols['gTasks'],16*40)
    frames(3,2);frames(1)
    assert not call('ChaosTrackerIsOpen')
    assert callbacks==blob(symbols['gMain'],24)
    a,b,c,ctr=struct.unpack('<4I',rng)
    result=(a+b+ctr)&0xffffffff
    expected=struct.pack('<4I',b^(b>>9),(c*9)&0xffffffff,(result+((c<<21)|(c>>11)))&0xffffffff,(ctr+1)&0xffffffff)
    assert blob(symbols['gRngValue'],16) in (rng,expected), 'RNG advanced while tracker paused'
    after=snapshot()
    for name in before:assert before[name]==after[name],name
start();call('IronmonGiveStarter',BULBA);check()
print('PASS IronMON field tracker restores borrowed VRAM/window/tasks/callbacks and preserves Pokemon/save/RNG.',flush=True)
# Retired history is read-only and paginates the bounded 30-snapshot ring.
start();call('IronmonGiveStarter',BULBA)
for _ in range(35):
    call('CreateWildMon',PIKA,3);assert call('IronmonAcceptCapture',p+6*MONSIZE)==GIVEN
before=snapshot();assert call('ChaosTrackerTryOpen');frames(60)
for _ in range(5):frames(12,256);frames(20)
snap('ironmon-tracker-progress')
frames(12,256);frames(20)
for _ in range(9):frames(12,1);frames(20)
snap('ironmon-tracker-retired')
frames(3,2);frames(1)
for name,value in before.items():assert value==snapshot()[name],('retired',name)
print('PASS progress/retired pages, 35 captures and read-only history pagination preserve party/storage/save.',flush=True)
# Regular Chaos supports a party rather than enforcing one main.
wr(s+DIFF,1,1);call('IronmonInitializeRun');check()
print('PASS regular Chaos field tracker restoration.',flush=True)
# Exercise lossless graphics restoration with a long run, a worst-case literal
# span, and mixed spans. These are VRAM fixtures, not stored Pokémon edits.
original_chars=blob(0x06008000,0x4000)
for pattern in ('run','literal','mixed'):
    for i in range(505*16):
        value=0x3333 if pattern=='run' else ((i*257)^0xa55a)&0xffff
        if pattern=='mixed' and i%256<128:value=0
        wr(0x06008000+i*2,value,2)
    check()
    print('PASS tracker exact graphics backup round-trip',pattern,flush=True)
for i in range(0,len(original_chars),2):
    wr(0x06008000+i,int.from_bytes(original_chars[i:i+2],'little'),2)
# Allocation failure must refuse safely rather than invoke the engine fatal UI.
heap=symbols['gHeap'];block=heap;allocations=[]
for _ in range(1000):
    size=rd(block+4)&0x3ffff
    if not rd(block,2)&1 and size>512:allocations.append(call('AllocUnchecked_',size-128,0))
    block=rd(block+12)
    if block==heap:break
before=snapshot()
assert not call('ChaosTrackerTryOpen')
for ptr in allocations:
    if ptr:call('Free',ptr)
for name,value in before.items():assert value==snapshot()[name],('denial',name)
print('PASS low-heap tracker denial preserves scene and avoids fatal allocation.',flush=True)
# Real battle stable action and move-selection input. No injected completion.
STRUCTSIZE,BS,MAINSTATE,BATK,BMOVES,BPP,BSTAGES=struct.unpack('<7I',(ROOT/'ironmon-tracker-layout.bin').read_bytes())
start();call('IronmonGiveStarter',BULBA)
if '--doubles' in sys.argv:
    wr(s+DIFF,1,1);call('IronmonInitializeRun')
    assert call('ScriptGiveMon',PIKA,20,0)==GIVEN
    _,A,_,DA,_,koga,_,_=struct.unpack('<8I',(ROOT/'ironmon-singles-layout.bin').read_bytes())
    call('InitTrainerBattleParameter')
    wr(symbols['gTrainerBattleParameter']+A,koga,2)
    wr(symbols['gTrainerBattleParameter']+DA,symbols['Text_ChaosJessieDefeat'])
    wr('gNoOfApproachingTrainers',1,1);call('BattleSetup_StartTrainerBattle')
else:
    call('CreateWildMon',PIKA,3);call('BattleSetup_StartWildBattle')
for _ in range(400):
    frames(3,1);frames(5)
    if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
else:raise AssertionError('battle did not reach stable action selection')
if '--doubles' in sys.argv:
    assert rd('gBattlersCount',1)==4 and rd('gBattleTypeFlags')&1
for move_screen in (False,True):
    frames(60)
    wr(rd('gSaveBlock2Ptr')+0x13,2,1) # L=A must not select a move through the chord.
    # Controlled battle-only changes make stored-party fallbacks visibly wrong.
    wr(symbols['gBattleMons']+BATK,321,2)
    wr(symbols['gBattleMons']+BMOVES,TACKLE,2)
    wr(symbols['gBattleMons']+BPP,3,1)
    wr(symbols['gBattleMons']+BSTAGES+1,8,1) # Attack +2, Defense -2 remain visible.
    wr(symbols['gBattleMons']+BSTAGES+2,4,1)
    battle_struct=blob(rd('gBattleStruct'),STRUCTSIZE)
    battle_mons=blob(symbols['gBattleMons'],4*BS)
    before=snapshot();callbacks=blob(symbols['gMain'],24)
    controller=rd('gBattlerControllerFuncs');mainstate=rd(symbols['gMain']+MAINSTATE,1)
    frames(1,512|4);frames(60)
    assert call('ChaosTrackerIsOpen'),('shortcut did not open',move_screen)
    frames(12,256);frames(20);snap("ironmon-tracker-battle-moves");frames(12,512);frames(20)
    snap('ironmon-tracker-battle')
    assert battle_struct==blob(rd('gBattleStruct'),STRUCTSIZE)
    assert battle_mons==blob(symbols['gBattleMons'],4*BS)
    frames(3,2);frames(1)
    assert not call('ChaosTrackerIsOpen')
    assert callbacks==blob(symbols['gMain'],24)
    assert controller==rd('gBattlerControllerFuncs')
    assert mainstate==rd(symbols['gMain']+MAINSTATE,1)
    for name,value in before.items():assert value==snapshot()[name],(move_screen,name)
    if not move_screen:
        frames(3,1);frames(10)
        for _ in range(100):
            frames(3)
            if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseMove']|1:break
        else:raise AssertionError('move selection did not open')
print('PASS actual '+('Doubles' if '--doubles' in sys.argv else 'Singles')+' battle L+Select at action/move selection: no turns/actions/Pokemon/state changes, full borrowed graphics/window/callback restoration.',flush=True)
