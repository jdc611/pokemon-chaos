"""Native script gift cap/reservation checks, real Eevee ball and mode isolation."""
import runpy,struct
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
data=(ROOT/'gift-layout.bin').read_bytes();g=struct.unpack('<'+'I'*(len(data)//4),data)
def mon(i):return symbols['gParties']+i*v[5]
def level(i):return call('GetMonData2',mon(i),v[11])
assert call('ScriptGiveMon',g[2],99,0)==0 and rd('gPartiesCount',1)==4 and level(3)==call('GymLeaderLevelCap')
assert call('ScriptGiveMon',g[3],50,0)==2 and rd('gPartiesCount',1)==4
p=call('Alloc_',g[4],0);assert p
for i in range(g[4]):wr(p+i,0,1)
wr(p+g[5],g[2],g[7]);wr(p+g[6],99,g[8]);wr(p+g[12],g[14],2);wr(p+g[13],g[15],2);wr(p+g[16],g[18],g[17])
assert call('ScriptGiveMonParameterized',g[9],6,p)==0 and rd('gPartiesCount',1)==5 and level(4)==call('GymLeaderLevelCap')
assert rd(p+g[6],g[8])==99
assert call('ScriptGiveMonParameterized',g[10],0,p)==0 and level(6)==99
wr(p+g[5],g[3],g[7]);assert call('ScriptGiveMonParameterized',g[9],6,p)==2 and rd('gPartiesCount',1)==5
call('Free',p);print('gift fixture ready for roof room',flush=True)
print('PASS legacy and parameterized gift adapters clamp only GLE player gifts, preserve templates/opponent levels and refuse quest reservations',flush=True)
# Exercise the existing gift script and givemon macro, not only a direct adapter call.
call('FlagClear',g[11]);lib.call6(symbols['SetWarpDestination'],g[0]>>8,g[0]&255,-1,7,4,0)
call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(300)
print('gift roof loaded',flush=True);snap('gift-roof-before');press(64);press(1);until(yesno);print('gift nickname prompt',flush=True);choose(1)
for _ in range(180):
 if not call('ScriptContext_IsEnabled'):break
 press(1,20)
else:raise AssertionError('Eevee gift script did not release')
assert rd('gPartiesCount',1)==6 and call('GetMonData2',mon(5),v[10])==g[1] and level(5)==call('GymLeaderLevelCap')
assert call('FlagGet',g[11]);snap('gle-original-eevee-gift')
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and level(5)==call('GymLeaderLevelCap')
print('PASS actual native Eevee ball/nickname refusal, delivery cap, one-time flag and flash persistence',flush=True)
# Ordinary mode and enemy templates retain their intended levels.
magic=rd(current());wr(current(),0)
for i in range(3,6):call('ZeroMonData',mon(i))
wr('gPartiesCount',3,1)
assert call('ScriptGiveMon',g[2],25,0)==0 and level(3)==25
wr(current(),magic)
print('PASS ordinary-mode gift level preservation',flush=True)
print('PASS NATIVE GIFT CAP CHECKS COMPLETE',flush=True)
