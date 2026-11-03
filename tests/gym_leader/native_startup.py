"""Normal title/New Game/Oak/naming route using only controller input.

No injected names, forced-newgame flag, callback jump or Gym state fixture.
"""
import sys,struct
sys.path.insert(0,'tools/chaos/native')
from qa import *
data=(ROOT/'layout.bin').read_bytes();v=struct.unpack('<'+'I'*(len(data)//4),data)
def task(name):
 for i in range(16):
  a=symbols['gTasks']+i*v[19]
  if rd(a+4,1) and rd(a)==symbols[name]|1:return i
 return 255
def menu_count():
 t=task('Task_HandleScrollingMultichoiceInput')
 return rd(symbols['gTasks']+t*v[19]+v[20]+10,2) if t<16 else 0
def press(key,n=20):frames(2,key);frames(n)
assert lib.boot(str(repo/'pokefirered.gba').encode())
frames(1800);press(8,120)
naming=0
for iteration in range(1000):
 s=rd('gSaveBlock1Ptr')+v[0]
 if rd(s)==0x474c4531 and menu_count()==3:
  break
 callback=rd(symbols['gMain']+4)
 if callback==symbols['CB2_NamingScreen']|1:
  frames(120);snap('normal-start-naming-'+str(naming))
  # The native keyboard starts with Oak's default name. START selects OK.
  press(8,35);press(1,160);naming+=1
 elif task('Task_OakSpeech_HandleRivalNameInput')<16:
  press(128,20);press(1,35) # Select a native preset rival name.
 else:
  press(1,24)
else:
 snap('normal-start-failure')
 raise AssertionError(('normal title-to-town route stalled',hex(rd(symbols['gMain']+4)),naming))
assert naming>=1
assert not rd('gDebugForceKantoNewGame',1)
assert rd(s+v[2],1)==1 and rd(s+v[3],1)==255
assert rd(s+v[17],1)==0 and rd(s+v[18],1)==0
assert rd(rd('gSaveBlock2Ptr'),1)!=255
assert rd(rd('gSaveBlock1Ptr')+v[4],1)!=255
assert call('IsGymLeaderRun')
snap('normal-start-hometown-menu')
print('PASS normal title/New Game/Oak/native player keyboard/rival preset into Gym Leader hometown selection; no debug/name/state bypass',flush=True)
