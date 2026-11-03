"""Native GLA expedition guide, rank refusal and five entrance transports."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
def mapid():
 a=rd('gSaveBlock1Ptr')+v[25]
 return rd(a+1,1)|(rd(a,1)<<8)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(100):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('expedition walk blocked',pos(),(x,y)))
def guide():
 walkto(6,13);walkto(4,13);press(64);press(1)
 until(lambda:menu_count()==6)
guide();choose(0);until(lambda:menu_count()==6)
assert mapid()==v[24],'rank-one transport should be refused'
press(2);frames(50)
assert not call('ScriptContext_IsEnabled')
wr(current()+v[2],8,1)
for slot,expected in enumerate([(6,20),(4,38),(28,47),(33,20),(35,6)]):
 if mapid()!=v[24]:call('GleWarpLeague');frames(240)
 guide();snap('expedition-guide-menu');choose(slot)
 for _ in range(500):
  frames(1)
  if mapid()==(v[125+slot] if slot<4 else v[-14]):break
 else:raise AssertionError(('transport failed',slot,hex(mapid())))
 frames(220)
 assert pos()==expected,(slot,pos(),expected)
 snap('expedition-entrance-'+str(slot))
 if slot==0:
  for _ in range(200):
   frames(1,128)
   if mapid()==v[129]:break
  else:raise AssertionError('Seafoam entrance transport did not permit a native exit')
  frames(140)
print('PASS native expedition guide rank refusal, five entrance transports and Seafoam entrance exit',flush=True)
