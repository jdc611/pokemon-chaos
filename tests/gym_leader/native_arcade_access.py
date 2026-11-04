"""GLE Game Corner access without fabricated badges; native prize and cabinet UI."""
import runpy,struct
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
data=(ROOT/'arcade-layout.bin').read_bytes();a=struct.unpack('<'+'I'*(len(data)//4),data)
def warp(m,x,y):
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,x,y,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
def settle():
 for _ in range(200):
  if not call('ScriptContext_IsEnabled'):return
  press(1,20)
 raise AssertionError('arcade script did not release')
call('FlagClear',v[83]);assert not call('FlagGet',v[83]) and call('ChaosArcadeUnlocked')
magic=rd(current()+a[3]);wr(current()+a[3],0);assert not call('ChaosArcadeUnlocked')
call('FlagSet',v[83]);assert call('ChaosArcadeUnlocked');call('FlagClear',v[83]);wr(current()+a[3],magic)
assert call('AddBagItem',a[0],1);call('SetCoins',8000)
warp(a[1],4,4)
def offer():
 press(64);press(1);until(lambda:menu_count()==10);choose(0);until(lambda:menu_count() in [2,4]);choose(0);until(yesno)
# Normal prize choice, cancel and retry through the actual counter.
offer();snap('gle-arcade-prize-confirm');choose(1);settle();assert call('GetCoins')==8000 and rd('gPartiesCount',1)==3
offer();choose(0);settle();assert call('GetCoins')==6500 and rd('gPartiesCount',1)==4
mon=symbols['gParties']+3*v[5];assert call('GetMonData2',mon,v[10])==a[4] and call('GetMonData2',mon,v[11])==call('GymLeaderLevelCap')
assert not call('FlagGet',v[83])
print('PASS native no-badge GLE access, preserved ordinary badge gate, prize cancellation/payment and rank-capped Porygon delivery',flush=True)
# Future catalog edits may not introduce a reserved research acquisition.
offer();draft=rd('sPrizeDraft');assert draft;wr(draft+v[5],a[5],2);choose(0);settle()
assert call('GetCoins')==6500 and rd('gPartiesCount',1)==4
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and call('GetCoins')==6500 and not call('FlagGet',v[83])
print('PASS reserved-species delivery refusal, no charge on refusal and native prize flash persistence',flush=True)
# The native detail browser and coin transaction share the corrected gate.
warp(a[1],6,4);press(64);press(1);until(lambda:menu_count()==3);choose(0)
until(lambda:task('Task_ItemBrowser')<16);frames(60);snap('gle-arcade-tm-browser');press(1);until(yesno);choose(0);until(lambda:menu_count()==3)
assert call('GetCoins')==5500 and call('CheckBagHasItem',a[6],1)
choose(0);until(lambda:task('Task_ItemBrowser')<16);frames(60);press(1);until(yesno);choose(0);until(lambda:menu_count()==3)
assert call('GetCoins')==5500;press(2);settle()
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and call('CheckBagHasItem',a[6],1)
print('PASS native TM detail browser, paid delivery, owned-item refusal and flash persistence without the original badge',flush=True)
# Use the real Memory cabinet script and native game, then cancel without a stake.
warp(a[2],14,8);settle();press(64);press(1);until(yesno);choose(0)
for _ in range(240):
 if rd('sArcadeGame'):break
 frames(1)
assert rd('sArcadeGame');frames(300);snap('gle-arcade-memory-game');press(2,180);settle()
assert not rd('sArcadeGame') and call('GetCoins')==5500 and not call('FlagGet',v[83])
print('PASS actual native Memory cabinet launches and returns without the original fourth badge or invented completion flags',flush=True)
print('PASS GLE ARCADE ACCESS CHECKS COMPLETE',flush=True)
