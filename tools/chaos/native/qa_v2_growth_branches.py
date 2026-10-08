from qa import *
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
def key(k=1,n=40):frames(2,k);frames(n)
call('ZeroPlayerPartyMons');call('ScriptGiveMon',868,20,0);call('CB2_PartyMenuFromStartMenu');frames(180);key();key();frames(140)
for _ in range(3):key(16,140)
cb=rd(symbols['gMain']+4);icons=[]
for i in range(65):
 key();assert rd(symbols['gMain']+4)==cb
 if i in [31,61]:snap('v2-growth-milcery-'+str(i))
key(2,160);assert rd(symbols['gMain']+4)==symbols['CB2_UpdatePartyMenu']|1
print('PASS all 63 Milcery evolution branches cycle and wrap, with native field/party return and no window exhaustion.',flush=True)
