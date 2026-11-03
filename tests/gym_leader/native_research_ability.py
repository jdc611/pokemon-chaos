"""The real Bag Ability Patch cannot bypass special Slaking research."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
assert call('ScriptGiveMon',v[143],5,0)==0
assert call('AddBagItem',v[141],1)
mon=symbols['gParties']+3*v[5]
assert call('GetMonData2',mon,v[12])==0
state('ability-before-bag')
def usepatch():
 call('CleanupOverworldWindowsAndTilemaps')
 call('GoToBagMenu',0,0,symbols['CB2_ReturnToField']|1)
 frames(150)
 ptr=rd('gBagPockets');capacity=rd(symbols['gBagPockets']+4,2)
 target=next(i for i in range(capacity) if rd(ptr+i*v[123],2)==v[141])
 cursor=rd(symbols['gBagPosition']+v[121],2)+rd(symbols['gBagPosition']+v[122],2)
 for _ in range(abs(target-cursor)):press(128 if target>cursor else 64)
 press(1,30);press(1,30)
 until(lambda:task('Task_HandleChooseMonInput')<16)
 frames(100);press(16,20);press(128,20);press(128,20)
 assert call('GetCursorSelectionMonId')==3
 press(1,30)
usepatch()
until(lambda:task('Task_ClosePartyMenuAfterText')<16)
assert call('GetMonData2',mon,v[12])==0
assert call('CheckBagHasItem',v[141],1)
snap('slaking-patch-research-gate')
state('ability-before-bag',1)
wr(current()+v[2],5,1);call('GleSlakingResearch')
assert call('GetMonData2',mon,v[12])==2
# Returning to slot zero is a fixture to verify that the completed research
# authorizes native patch usage; the actual upgrade and consumption use the UI.
ptr=symbols['gStringVar4'];wr(ptr,0,1);call('SetMonData',mon,v[12],ptr)
usepatch()
until(lambda:call('GetMonData2',mon,v[12])==2)
assert not call('CheckBagHasItem',v[141],1)
snap('slaking-patch-after-research')
print('PASS native Bag/party patch refusal before research without consumption; native permitted patch after research consumes item',flush=True)
