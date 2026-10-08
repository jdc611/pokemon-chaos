from qa import *
assert lib.boot(str(repo/'pokefirered.gba').encode());state('v2-capture-choice',1)
def key(k=1,n=25):frames(2,k);frames(n)
frames(150);snap('v2-capture-naming');key();key(8);key();frames(180)
for i in range(80):
 cb=rd(symbols['gMain']+4)
 if cb==symbols['CB2_UpdatePartyMenu']|1:
  frames(160);snap('v2-capture-party');state('v2-capture-party');print('PARTY',[(call('GetMonData3',symbols['gParties']+100*j,18,0),call('GetMonData3',symbols['gParties']+100*j,64,0)) for j in range(6)],flush=True);break
 key()
else:
 snap('v2-capture-unexpected');print('CB',hex(rd(symbols['gMain']+4)),flush=True);raise AssertionError('No party chooser')
# Native swap first party member, then resolve delivery message.
key()
for _ in range(90):key()
print('FINISH',rd('gBattleOutcome',1),[(call('GetMonData3',symbols['gParties']+100*j,18,0)) for j in range(6)],flush=True)
snap('v2-capture-complete')
assert call('GetMonData3',symbols['gParties'],18,0)==150
