"""Actual gift script pauses for an atomic permanent exchange."""
from qa_oaks_draft import *
start()
for i in range(6):assert call('OaksDraftCommit',i%3)
call('OaksDraftFinishOpening')
call('ScriptContext_Init');call('UnlockPlayerFieldControls')
state('draft-gifts-base')
def tap(k,n=50):frames(3,k);frames(n)
# The production Eevee event executes givemon and waits for our trade UI.
call('ScriptContext_SetupScript',symbols['CeladonCity_Condominiums_RoofRoom_EventScript_EeveeBall']);frames(100)
assert rd('sDraftScreen')
old=bytesat(p,6*MS)
snap('draft-gift-reveal');tap(2,100)
for _ in range(12):tap(1)
assert bytesat(p,6*MS)==old
print('PASS actual gift event cancel leaves all six companions unchanged',flush=True)
state('draft-gifts-base',1)
call('ScriptContext_SetupScript',symbols['CeladonCity_Condominiums_RoofRoom_EventScript_EeveeBall']);frames(100)
assert rd('sDraftScreen');tap(128);tap(128);tap(4)
snap('draft-gift-warning');tap(1,100)
for _ in range(35):tap(1)
assert rd('gPartiesCount',1)==6
assert data(p+2*MS,PID)!=int.from_bytes(old[2*MS:2*MS+4],'little')
assert data(p+2*MS,LV)==5 and data(p+2*MS,MEMBER)
assert all(bytesat(p+i*MS,MS)==old[i*MS:(i+1)*MS] for i in (0,1,3,4,5))
print('PASS actual scripted gift replaces only the selected friend and resumes the NPC event',flush=True)
# Paid reroll confirmation and cancellation use the same live party identities.
call('SetCoins',5000);call('ScriptContext_Init');call('UnlockPlayerFieldControls')
call('OaksDraftOpenAbilityReroll');frames(100);snap('draft-ability-counter');tap(4);tap(2);tap(2,100)
assert call('GetCoins')==5000 and not data(p,REROLLED)
call('ScriptContext_Init');call('UnlockPlayerFieldControls');call('OaksDraftOpenAbilityReroll');frames(100);tap(4);tap(1,100)
assert call('GetCoins')==3000 and data(p,REROLLED)
ability=call('GetMonAbility',p)
call('ScriptContext_Init');call('UnlockPlayerFieldControls');call('OaksDraftOpenAbilityReroll');frames(100);tap(4);tap(1,100)
assert call('GetCoins')==3000 and call('GetMonAbility',p)==ability
print('PASS real reroll UI cancel/confirm/repeat: hidden result, exact charge and one purchase',flush=True)
