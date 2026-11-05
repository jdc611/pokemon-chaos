"""Native GLE ferry permit refresh and controller-driven Vermilion to Five Island."""
import runpy,struct
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
data=(ROOT/'ferry-layout.bin').read_bytes();f=struct.unpack('<'+'I'*(len(data)//4),data)
def mapid():
 p=rd('gSaveBlock1Ptr')+v[25];return rd(p,1)<<8|rd(p+1,1)
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def medals():return [call('FlagGet',v[i]) for i in range(80,89)]
def settle():
 for _ in range(200):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('ferry script did not release')
before=medals();assert not any(before)
# Simulate an older completed opening whose one-time permit bit was already set.
assert rd(current()+v[69])&1
call('RemoveBagItem',f[0],1)
for var in f[1:4]:call('VarSet',var,0)
call('FlagClear',f[4]);call('GymLeaderUnlockTravel')
assert call('CheckBagHasItem',f[0],1)
assert [call('VarGet',var) for var in f[1:4]]==[3,7,3] and call('FlagGet',f[4])
assert medals()==before
print('PASS existing-permit refresh opens native Sevii travel without badges, titles or game-clear rewards',flush=True)
warp(f[5],24,32);press(128);press(1);until(lambda:task('Task_HandleMultichoiceInput')<16);snap('gle-ferry-page-one')
choose(4);until(lambda:task('Task_HandleMultichoiceInput')<16);snap('gle-ferry-page-two');choose(0)
until(lambda:mapid()==f[6],600);frames(300);settle();snap('gle-ferry-five-island')
assert medals()==before
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and mapid()==f[6] and call('CheckBagHasItem',f[0],1)
print('PASS native ferry destination pages and real departure/arrival from Vermilion to Five Island, with flash persistence',flush=True)
# Entering Celio's center must not force the replaced Bill introduction.
warp(f[7],5,8);frames(180);assert not call('ScriptContext_IsEnabled') and medals()==before
print('PASS One Island Center does not restart the inherited mandatory Bill/Celio introduction',flush=True)
print('PASS NATIVE GLE FERRY CHECKS COMPLETE',flush=True)
