"""Terminal callback/rendering and flash persistence; not a battle playthrough."""
from qa_ironmon_core import *
start();call('IronmonGiveStarter',BULBA)
setdata(p,HP,0)
assert call('IronmonCheckRunOver') == 1
# Flash programming spans many frames; wait for the input callback, not a
# fixed short rendering delay that screenshots the middle of the save.
for _ in range(100):
 frames(10)
 if rd(symbols['gMain']+4) == symbols['RunOverInput']|1: break
else: raise AssertionError('terminal callback did not finish')
assert rd(0x04000008,2) & 0x1f00 == 31 << 8
snap('ironmon-run-over')
assert rd(s+IM+ENDED,1) == 1
call('ClearSav3');assert call('LoadGameSave',0)==1
assert rd(s+IM+ENDED,1)==1 and call('IronmonCheckRunOver')==1
frames(10)
assert rd('gPartiesCount',1)==1 and data(p,SP)==BULBA
print('PASS run-over callback renders, saves terminal state, blocks resume, and retains main.')
