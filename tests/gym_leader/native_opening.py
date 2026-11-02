"""Exercise actual in-world opening using controller input.
Only name entry is bypassed by the established new-game fixture.
"""
import sys,struct,json,os
sys.path.insert(0,'tools/chaos/native')
from qa import *
data=(ROOT/'layout.bin').read_bytes();v=struct.unpack('<'+'I'*(len(data)//4),data)
assert lib.boot(str(repo/'pokefirered.gba').encode())
frames(1800);frames(3,8);frames(100)
for dst,text in [(rd('gSaveBlock2Ptr'),bytes([0xc4,0xbb,0xbd,0xc5,0xff])),(rd('gSaveBlock1Ptr')+v[4],bytes([0xbc,0xc6,0xcf,0xbf,0xff]))]:
 for i,b in enumerate(text):wr(dst+i,b,1)
wr('gDebugForceKantoNewGame',1,1);call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(300)
def current():return rd('gSaveBlock1Ptr')+v[0]
def stage():return rd(current()+v[17],1)
def task(name):
 for t in range(16):
  a=symbols['gTasks']+t*v[19]
  if rd(a+4,1) and rd(a)==symbols[name]|1:return t
 return 255
def menu_count():
 t=task('Task_HandleScrollingMultichoiceInput')
 return rd(symbols['gTasks']+t*v[19]+v[20]+10,2) if t<16 else 0
def yesno():return task('Task_HandleYesNoInput')<16
def press(key,n=14):frames(2,key);frames(n)
def until(check,limit=240):
 for _ in range(limit):
  if check():return
  press(1)
 snap('opening-failure');raise AssertionError(('timeout',stage(),menu_count(),yesno(),hex(rd(symbols['gMain']+4))))
def choose(index):
 frames(18)
 for _ in range(index):press(128)
 press(1)
until(lambda:menu_count()==3)
assert stage()==0
snap('opening-town-menu')
choose(0) # Saffron
until(yesno);choose(0)
until(lambda:yesno() and stage()==1)
if os.environ.get('GLE_TEST_TOWN_NAME'):
 choose(0);frames(150);snap('opening-town-name')
 press(8,30);press(1,150)
else:
 choose(1) # Keep default name
until(lambda:menu_count()==18)
assert stage()==1
snap('opening-type-menu')
choose(4) # Grass
until(lambda:menu_count()==3 and stage()==2)
snap('opening-signature-menu')
choose(0);until(yesno);choose(0)
until(lambda:menu_count()==3 and stage()==3)
# Inspect first candidate in native Summary, then choose it.
choose(0);until(lambda:menu_count()==3)
choose(1);frames(150)
snap('opening-draft-summary')
press(2,120)
until(lambda:menu_count()==3)
choose(0)
until(lambda:rd(current()+v[18],1)==1)
for round_ in [1,2]:
 until(lambda:menu_count()==3)
 choose(0);until(lambda:menu_count()==3);choose(0)
 until(lambda:rd(current()+v[18],1)==round_+1)
until(lambda:stage()==5)
frames(120)
assert rd('gPartiesCount',1)==3
assert call('GymLeaderOfficialTeamLegal')
assert call('GetCurrentLevelCap')==15
snap('opening-complete-hall')
for i in range(3):
 mon=symbols['gPlayerParty']+i*v[5] if 'gPlayerParty' in symbols else symbols['gParties']+i*v[5]
 assert call('GetMonData2',mon,v[11])==5
print('PASS controller-driven Saffron/Grass opening, native Summary, 3 level-5 starters and GLA tutorial',flush=True)

# Walk out of the hall, then through the same native doorway back in.
for _ in range(240):
 frames(1,128)
 location=rd('gSaveBlock1Ptr')+v[25]
 if rd(location,1)!=v[24]>>8 or rd(location+1,1)!=v[24]&255:break
else:raise AssertionError('hall exit blocked')
frames(120)
snap('opening-viridian-exit')
frames(30,64);frames(120)
location=rd('gSaveBlock1Ptr')+v[25]
assert rd(location,1)==v[24]>>8 and rd(location+1,1)==v[24]&255
print('PASS hall exit and native Viridian Gym doorway redirect',flush=True)
