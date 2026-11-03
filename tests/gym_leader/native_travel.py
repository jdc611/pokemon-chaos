"""Rank-one travel permits and native trainer-generation scaling.
Names and warps to the test gates are fixtures; crossing uses controller input.
"""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
def mapid():
 a=rd('gSaveBlock1Ptr')+v[25]
 return rd(a+1,1)|(rd(a,1)<<8)
def pos():return rd(rd('gSaveBlock1Ptr'),2),rd(rd('gSaveBlock1Ptr')+2,2)
def warp(map,x,y):
 lib.call6(symbols['SetWarpDestination'],map>>8,map&255,-1,x,y,0)
 call('DoWarp');frames(240)
for flag in v[80:89]:assert not call('FlagGet',flag),'travel falsely awarded badge/Champion status'
assert call('VarGet',v[89])==1
assert call('VarGet',v[90])==8 and call('VarGet',v[91])==2
assert call('VarGet',v[95])==1
assert call('CheckBagHasItem',v[96],1) and call('CheckBagHasItem',v[97],1)
for move in range(8):assert call('HasBadgeForFieldMove',move)
# Northern League gate: cross the old Boulder-Badge coordinate with no badge.
warp(v[92],7,7)
for _ in range(300):
 frames(1,64)
 if mapid()==v[93]:break
else:raise AssertionError(('League gate blocked',pos(),hex(mapid())))
frames(160);snap('travel-league-gate-no-badges')
# Saffron's north gate: cross its former thirst trigger with no Tea quest.
warp(v[94],3,7);frames(64,64);frames(20)
assert mapid()==v[94] and pos()[1]<5,(pos(),hex(mapid()))
assert not call('ScriptContext_IsEnabled')
snap('travel-saffron-gate-no-tea')
# Late-game trainer catalog entries must use rank-scaled native generation.
mon=call('Alloc_',v[5],0);gen=call('Alloc_',v[99],0)
trainer=symbols['gTrainers']+(v[103]*v[104]+v[98])*v[101];party=rd(trainer+v[102])
assert trainer and party
for rank in range(1,9):
 wr(current()+v[2],rank,1)
 call('MakeTrainerGenerator',gen,trainer)
 call('GenerateMonFromTrainerMon',mon,party,gen)
 level=call('GetMonData2',mon,v[11])
 assert 3<=level<=call('GymLeaderLevelCap'),(rank,level)
 assert any(call('GetMonData2',mon,v[38]+i) for i in range(4))
call('Free',gen);call('Free',mon)
wr(current()+v[2],1,1)
print('PASS rank-one native League/Saffron gate crossings, badge-free field permission, equipment/PokéRider and all-rank trainer scaling',flush=True)
