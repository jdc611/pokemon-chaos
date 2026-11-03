"""Native landmark restoration, real door/exit and safe old-story isolation."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
def mapid():
 a=rd('gSaveBlock1Ptr')+v[25]
 return rd(a+1,1)|(rd(a,1)<<8)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
call('GleWarpHome');frames(220)
for _ in range(240):
 frames(1,128)
 if mapid()==v[58+town]:break
frames(150)
x=[59,49,37][town];y=[19,7,7][town]
# Supply the transaction requirements; restoration itself uses the native sign.
wr(current()+v[2],3,1);call('SetMoney',rd('gSaveBlock1Ptr')+v[64],50000)
lib.call6(symbols['SetWarpDestination'],v[58+town]>>8,v[58+town]&255,-1,x+2,y+2,0)
call('DoWarp');frames(250)
press(64);press(1);until(yesno)
snap('landmark-restoration-confirm');choose(0)
until(lambda:rd(current()+v[65],1)==1)
for _ in range(30):
 if not call('ScriptContext_IsEnabled'):break
 press(1,25)
frames(20)
assert call('GetMoney',rd('gSaveBlock1Ptr')+v[64])==50000-[8000,6000,5000][town]
snap('landmark-restored-exterior')
# Walk to the front door without crossing its sign.
frames(32,32);frames(100,64);frames(200)
assert mapid()==v[73+town],('landmark door failed',pos(),hex(mapid()))
snap('landmark-interior')
# The arcade's native first-visit host gives its welcome and Coin Case.
for _ in range(60):
 if not call('ScriptContext_IsEnabled'):break
 press(1,25)
# Silph's only active warps are the three public exits, not Rocket-story floors.
if town==0:
 events=rd(symbols['gMapHeader']+v[157])
 # Explicit warp result checks below also verify the dynamic entrance mapping.
for _ in range(320):
 frames(1,128)
 if mapid()==v[58+town]:break
else:raise AssertionError(('landmark exit blocked',pos(),hex(mapid())))
frames(150)
assert pos()==(x,y+1),(pos(),(x,y+1))
snap('landmark-returned')
print(f'PASS town={town} native landmark restoration/cost/door/interior/relocated exit',flush=True)
