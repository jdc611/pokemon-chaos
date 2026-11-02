"""Teach the protected signature item through the native bag and party menus."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
item=v[42];move=rd(current()+v[44],2)
assert call('CheckBagHasItem',item,1)
assert call('GetItemImportance',item)==1
assert call('ItemIdToBattleMoveId',item)==move
# Find a naturally compatible off-type with space for the move. This is a test
# gift fixture; the actual teaching and legality checks use normal game menus.
scratch=call('Alloc_',v[5],0);assert scratch
candidate=None
for species in range(1,1026):
 if not call('GymLeaderCanLearnSignature',species,move):continue
 call('CreateRandomMon',scratch,species,5)
 if call('GymLeaderTeamLegal',scratch,1):continue
 if sum(bool(call('GetMonData2',scratch,v[38]+j)) for j in range(4))>=4:continue
 candidate=species;break
call('Free',scratch)
assert candidate is not None
assert call('ScriptGiveMon',candidate,5,0)==0
assert rd('gPartiesCount',1)==4
mon=symbols['gParties']+3*v[5]
assert not call('GymLeaderTeamLegal',mon,1)
# Open the real bag in KEY ITEMS, then use the protected TM.
call('CleanupOverworldWindowsAndTilemaps')
call('GoToBagMenu',0,v[43],symbols['CB2_ReturnToField']|1)
frames(150);snap('signature-key-item')
press(1,30);press(1,30)
until(lambda:task('Task_HandleChooseMonInput')<16)
frames(100)
snap('signature-teach-party')
# Native single-party layout: right to slot 1, then down to slots 2 and 3.
press(16,20);press(128,20);press(128,20)
assert call('GetCursorSelectionMonId')==3
press(1,30)
until(lambda:any(call('GetMonData2',mon,v[38]+j)==move for j in range(4)))
assert call('GymLeaderTeamLegal',mon,1)
assert call('CheckBagHasItem',item,1)
assert not call('CheckBagHasItem',item,2)
frames(90);snap('signature-learned')
for _ in range(30):press(2,20)
assert call('CheckBagHasItem',item,1)
assert rd('gMapHeader')!=0
print('PASS protected signature item, real native teaching to compatible off-type, roster legality and retained reusable copy',flush=True)
