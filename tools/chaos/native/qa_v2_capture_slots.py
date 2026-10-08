from qa import *
assert lib.boot(str(repo/'pokefirered.gba').encode())
p=symbols['gParties'];initial=[59,3,9,6,25,123]
def key(k=1,n=25):frames(2,k);frames(n)
for slot in range(6):
 state('v2-capture-party',1)
 original=[call('GetMonData3',p+100*j,0,0) for j in range(6)]
 for _ in range(slot):key(128)
 key()
 for _ in range(100):key()
 actual=[call('GetMonData3',p+100*j,18,0) for j in range(6)]
 expected=initial.copy();expected[slot]=150
 assert actual==expected,(slot,actual)
 for j in range(6):
  if j!=slot:assert call('GetMonData3',p+100*j,0,0)==original[j]
 boxed=[]
 for box in range(14):
  for pos in range(30):
   if call('GetBoxMonDataAt',box,pos,18):boxed.append([call('GetBoxMonDataAt',box,pos,18),call('GetBoxMonDataAt',box,pos,0)])
 assert boxed==[[initial[slot],original[slot]]],(slot,boxed)
 assert rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
 print('PASS native full-party swap slot',slot,'actual six species, exact identities, PC transfer and field return',flush=True)
# Cancellation stores the new catch, preserving all six original members.
state('v2-capture-party',1);key(2)
for _ in range(100):key()
assert [call('GetMonData3',p+100*j,18,0) for j in range(6)]==initial
assert call('GetBoxMonDataAt',0,0,18)==150
print('PASS cancel party replacement stores caught Pokemon without changing party.',flush=True)
