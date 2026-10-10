"""Shared learning replacement fixtures; real menu traversal tested separately."""
from qa_ironmon_core import *
for mode in (4,5,1):
    start(4 if mode==1 else mode);call('IronmonGiveStarter',BULBA)
    if mode==1:wr(s+DIFF,1,1);call('IronmonInitializeRun')
    for slot in range(4):call('SetMonMoveSlot',p,TACKLE,slot)
    setdata(p,PP,1);wr('sMoveSlotToReplace',0,1)
    task=call('CreateTask',symbols['TaskDummy']|1,1)
    address=symbols['gTasks']+task*40+8
    # REPLACE_MOVE_1 is the state immediately after the forget message.
    wr(address,17,2);wr(address+2,0,2);wr(address+4,TACKLE,2);wr(address+6,1,2)
    # No-op UI callbacks isolate replacement logic from rendering resources.
    for i in range(6):wr(scratch+i*4,symbols['TaskDummy']|1)
    result=call('LearnMove',scratch,task)
    assert data(p,PP)==(35 if mode==1 else 1),(mode,result,data(p,PP))
    print('PASS replacement PP rule',mode,flush=True)
for mode in (4,5):
    start(mode);call('IronmonGiveStarter',BULBA)
    before=bytes(rd(p+i,1) for i in range(MONSIZE))
    wr('gSpecialVar_0x8004',0,2);wr('gSpecialVar_0x8005',0,2)
    call('MoveDeleterForgetMove')
    assert before==bytes(rd(p+i,1) for i in range(MONSIZE))
    print('PASS direct move deletion cannot reopen a free-PP slot',mode,flush=True)

for mode in (4,5):
    start(mode);call('IronmonGiveStarter',BULBA)
    before=bytes(rd(p+i,1) for i in range(MONSIZE))
    call('ScriptContext_SetupScript',symbols['FuchsiaCity_House3_EventScript_MoveDeleter']);call('ScriptContext_Enable')
    for _ in range(150):
        frames(3,1);frames(25)
        if not call('ArePlayerFieldControlsLocked'):break
    else:raise AssertionError(('deleter restriction failed to return',mode))
    assert before==bytes(rd(p+i,1) for i in range(MONSIZE))
    print('PASS actual Fuchsia deleter denial and return control',mode,flush=True)
