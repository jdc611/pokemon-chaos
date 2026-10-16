"""Atomic gift workflow, no vacancy acquisition, and seeded field/TM rewards."""
from qa_custom_build_ui import *
state('custom-ui-six',1)
call('OaksDraftFinishOpening');call('VarSet',LABSCENE,4)
before=bytesat(p,6*MS);level_before=data(p,LV)
wr('gSpecialVar_0x8004',133,2);wr('gSpecialVar_0x8005',0,2)
call('CustomBuildOpenGift');frames(240);assert ui() and page()==2
snap('custom-gift-preview');tap(2,180)
assert not ui() and bytesat(p,6*MS)==before
print('PASS gift inspection can be declined without changing the roster',flush=True)
# Build the proposed gift, then cancel at the final atomic confirmation.
def build_gift(confirm):
 wr('gSpecialVar_0x8004',133,2);wr('gSpecialVar_0x8005',0,2)
 call('CustomBuildOpenGift');frames(240);assert page()==2
 bst=rd(ui()+TARGET,2);tap(1);assert page()==3
 tap(1,300);assert rd('sNamingScreen')
 tap(1,100);tap(8,70);tap(1,240);assert page()==4
 assert rd(ui()+TARGET,2)==bst
 tap(1);tap(1);tap(1);assert page()==7
 for _ in range(100):
  if not left():break
  tap(256,3)
 assert not left();tap(8);tap(1);assert page()==9
 snap('custom-gift-final-confirmation')
 tap(1 if confirm else 2,180)
 assert not ui()
build_gift(False)
assert bytesat(p,6*MS)==before
build_gift(True)
assert rd('gPartiesCount',1)==6 and data(p,SP)==133
assert bytesat(p+MS,5*MS)==before[MS:]
assert data(p,LV)==level_before
assert call('CustomBuildFind',data(p,PID))
print('PASS complete gift build cancels atomically or replaces exactly one living member with a persistent custom build',flush=True)
# Gifts cannot turn a vacant Nuzlocke slot into an additional usable companion.
call('ZeroMonData',p+5*MS);call('CalculatePlayerPartyCount');assert rd('gPartiesCount',1)==5
old=bytesat(p,5*MS)
wr('gSpecialVar_0x8004',133,2);call('CustomBuildOpenGift');frames(240);tap(1)
assert page()==3
tap(2,180);assert rd('gPartiesCount',1)==5 and bytesat(p,5*MS)==old
print('PASS gift workflow requires a real current companion and does not fill vacancies',flush=True)
# Location rolls are fixed; protected items and ordinary shops remain separate.
for item in (1,13,17,18,19):
 wr('gSpecialVar_Result',item,2);wr('gSpecialVar_LastTalked',7,2);call('ChaosRandomizeOverworldItem');a=rd('gSpecialVar_Result',2)
 wr('gSpecialVar_Result',item,2);call('ChaosRandomizeOverworldItem');assert a==rd('gSpecialVar_Result',2)
 assert not call('ChaosRemovedItem',a)
wr('gSpecialVar_Result',HMCUT,2);call('ChaosRandomizeOverworldItem');assert rd('gSpecialVar_Result',2)==HMCUT
rewards=[]
for gym in range(8):
 wr('gSpecialVar_0x8004',gym,2);call('OaksDraftGiveGymTM');tm=rd(rd('gSaveBlock3Ptr')+DRAFT+GYMTMS+gym*2,2)
 assert TM1<=tm<=TM100 and tm not in rewards
 move=call('ItemIdToBattleMoveId',tm)
 assert any(call('CanLearnTeachableMove',data(p+i*MS,SP),move) for i in range(5))
 assert call('OaksDraftExternalItem',tm)!=tm
 rewards.append(tm)
print('PASS deterministic field rolls, protected HM and eight distinct compatible reserved Gym TMs',flush=True)
