"""Tracker display restoration and paused input; observation journal pending."""
from qa_ironmon_core import *
def blob(ptr,size):return bytes(rd(ptr+i,1) for i in range(size))
def snapshot():
    return {name:blob(address,size) for name,address,size in (
        ('party',p,6*MONSIZE),('save3',s,SIZE),
        ('chars',0x06008000,0x4000),('map',0x0600f800,0x800),
        ('windows',symbols['gWindows'],32*12),
        ('tasks',symbols['gTasks'],16*40))}
def check():
    before=snapshot();callbacks=blob(symbols['gMain'],24);rng=blob(symbols['gRngValue'],16)
    assert call('ChaosTrackerTryOpen')
    frames(3)
    assert call('ChaosTrackerIsOpen')
    snap('ironmon-tracker-stats')
    for page in range(2):
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
# Regular Chaos supports a party rather than enforcing one main.
wr(s+DIFF,1,1);call('IronmonInitializeRun');check()
print('PASS regular Chaos field tracker restoration.',flush=True)
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
STRUCTSIZE,BS,MAINSTATE=struct.unpack('<3I',(ROOT/'ironmon-tracker-layout.bin').read_bytes())
start();call('IronmonGiveStarter',BULBA);call('CreateWildMon',PIKA,3)
call('BattleSetup_StartWildBattle')
for _ in range(400):
    frames(3,1);frames(5)
    if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
else:raise AssertionError('battle did not reach stable action selection')
for move_screen in (False,True):
    frames(60)
    wr(rd('gSaveBlock2Ptr')+0x13,2,1) # L=A must not select a move through the chord.
    battle_struct=blob(rd('gBattleStruct'),STRUCTSIZE)
    battle_mons=blob(symbols['gBattleMons'],4*BS)
    before=snapshot();callbacks=blob(symbols['gMain'],24)
    controller=rd('gBattlerControllerFuncs');mainstate=rd(symbols['gMain']+MAINSTATE,1)
    frames(1,512|4);frames(3)
    assert call('ChaosTrackerIsOpen'),('shortcut did not open',move_screen)
    frames(3,256);frames(4);frames(3,512);frames(4)
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
print('PASS actual battle L+Select at action/move selection: no turns/actions/Pokemon/state changes, full borrowed graphics/window/callback restoration.',flush=True)
