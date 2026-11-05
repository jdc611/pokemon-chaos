"""Native Gym Guy design transactions, migration, trials, palette and actor budget."""
import runpy,struct,re
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
b=(ROOT/'gym-design-layout.bin').read_bytes();d=struct.unpack('<'+'I'*(len(b)//4),b)
def design(i):return rd(current()+d[0]+d[i],1)
def money():return call('GetMoney',rd('gSaveBlock1Ptr')+v[64])
def setmoney(n):call('SetMoney',rd('gSaveBlock1Ptr')+v[64],n)
def warp(x,y):
 lib.call6(symbols['SetWarpDestination'],v[23]>>8,v[23]&255,-1,x,y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(500)
def settle():
 for _ in range(200):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('gym design script did not release')
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def walkto(x,y):
 for _ in range(160):
  px,py=pos()
  if (px,py)==(x,y):frames(20);return
  frames(16,16 if px<x else 32 if px>x else 128 if py<y else 64);frames(1)
 raise AssertionError(('design walking blocked',pos(),(x,y)))
def gate():return call('MapGridGetCollisionAt',12,13)
def palette():return bytes(rd(symbols['gPlttBufferUnfaded']+384+i,1) for i in range(32))
def finishmenu():until(lambda:menu_count()==7)
def change(kind,choice,accept=True):
 choose(kind);until(lambda:menu_count()==(7 if kind==2 else 5));choose(choice);until(yesno);choose(0 if accept else 1);finishmenu()
assert d[:3]==(3196,3200,15)
wr(current()+d[7],14,2)
expected=bytearray(rd(current()+i,1) for i in range(d[0]));expected[d[7]:d[7]+2]=d[2].to_bytes(2,'little')
for i in range(4):wr(current()+d[0]+i,255,1)
call('GymLeaderState');assert bytes(rd(current()+i,1) for i in range(d[0]))==expected and all(design(i)==0 for i in range(3,7))
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
print('PASS exact v14 prefix preservation, new four-byte design tail and reserved-union native flash',flush=True)
wr(current()+v[2],3,1);wr(current()+v[-1],3,1);wr(current()+d[8],0,2);setmoney(100000)
warp(7,12);press(16);press(1);until(lambda:menu_count()==8);choose(6);finishmenu();snap('gym-design-main-menu')
choose(3);until(yesno);choose(1);finishmenu();assert design(6)==0 and money()==100000
choose(3);until(yesno);choose(0);finishmenu();assert design(6)==1 and money()==95000
choose(3);finishmenu();assert design(6)==1 and money()==95000
setmoney(0);change(0,1);assert design(3)==0 and money()==0
setmoney(95000);change(0,1);assert design(3)==1 and money()==94000
neutral=palette();change(2,3);assert design(5)==3 and palette()!=neutral and money()==93250
snap('gym-ocean-theme-menu');change(2,0);assert palette()==neutral;change(2,3);assert palette()!=neutral
change(1,1);assert [design(i) for i in range(3,7)]==[1,1,3,1] and money()==90250
press(2);until(lambda:menu_count()==8);press(2);settle()
print('PASS native guide/menu, upgrade locks, cancellation, funds, independent settings and palette restoration',flush=True)
warp(4,7);assert gate()==1;press(64);press(1);settle();assert gate()==1
walkto(8,7);press(64);press(1);settle();assert gate()==0;snap('gym-two-switch-gate-open')
walkto(5,7);walkto(5,5);assert pos()==(5,5)
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1
warp(5,5);assert gate()==0;walkto(5,7)
warp(7,12);assert gate()==1 and design(4)==1
assert not call('MapGridGetCollisionAt',14,19) and not call('MapGridGetCollisionAt',13,20)
print('PASS controller switches/gate walking, re-entry reset, upper-room owner access and guide/exit floor',flush=True)
wr(current()+v[-1],7,1);wr(current()+v[2],1,1);scratch=symbols['gStringVar1'];wr(scratch,100000)
assert call('GleGymDesignPurchase',current(),3,0,scratch)==0 and call('GleGymDesignPurchase',current(),3,0,scratch)==0
for puzzle,sequence in [(2,[0,2,1]),(3,[0,1,2,1,0])]:
 assert call('GleGymDesignPurchase',current(),1,puzzle,scratch)==0;warp(6,7);assert gate()==1
 for which in sequence:wr('gSpecialVar_LastTalked',245+which,2);call('GleGymDesignSwitch')
 assert gate()==0;call('GleGymDesignReset');assert gate()==1
assert call('GleGymDesignPurchase',current(),1,0,scratch)==0;warp(7,12);assert gate()==0
assert [design(i) for i in range(3,7)]==[1,0,3,3]
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and design(4)==0 and design(6)==3
print('PASS demotion-safe tiers, relay/master adapters, reset, independent removal and flash',flush=True)
wr(current()+v[2],8,1);wr(current()+v[-1],8,1);constants=(repo/'include/constants/opponents_frlg.h').read_text()
for i,name in enumerate(['TRAINER_LASS_KAY','TRAINER_BEAUTY_BRIDGET','TRAINER_PICNICKER_TINA','TRAINER_BEAUTY_TAMIA','TRAINER_BEAUTY_LORI']):
 trainer=int(re.search(r'^#define '+name+r'\s+(\d+)',constants,re.M)[1]);assert call('GleRecruitTrainer',current(),trainer,3,80)==0;assert call('GleAssignTrainer',current(),i,1)==0
for bit in [217,219,221]:assert call('GleUniqueCapture',current(),bit)==0
assert call('GleGymDesignPurchase',current(),1,3,scratch)==0
locals_=[230,231,232,233,234,236,237,238,245,246,247]
for layout in range(4):
 assert call('GleGymDesignPurchase',current(),0,layout,scratch) in [0,5];warp(7,12)
 events=rd(symbols['gMapHeader']+v[157]);objects=rd(events+v[158]);ids={rd(objects+i*v[116],1) for i in range(rd(events,1))}
 assert all(local in ids for local in locals_)
 warp(5,5)
 assert all(call('GetObjectEventIdByLocalId',local)<16 for local in locals_)
 for x,y in [(2,5),(10,5),(2,8),(10,8),(4,4),(7,12),(6,13)]:assert not call('MapGridGetCollisionAt',x+7,y+7),(layout,x,y)
snap('gym-full-staff-trophies-trial')
print('PASS all four presets with five staff, three trophies, three consoles and clear approach tiles',flush=True)
print('PASS NATIVE GYM DESIGN CHECKS COMPLETE',flush=True)
