"""Native held-form legality, including all Memories/Plates and stale PC forms."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
def setdata(mon,field,value):
 ptr=call('Alloc_',4,0);wr(ptr,value);call('SetMonData',mon,field,ptr);call('Free',ptr)
mon=call('Alloc_',v[5],0)
# Remove signature allowance in this fixture so every result isolates typing.
wr(current()+v[44],0,2)
for species,first_item in [(v[130],v[134]),(v[132],v[135])]:
 call('CreateRandomMon',mon,species,5)
 for ty in range(1,18):
  wr(current()+v[3],ty,1);setdata(mon,v[41],first_item+ty-1)
  assert call('GymLeaderTeamLegal',mon,1),(species,ty)
  wr(current()+v[3],0,1)
  assert not call('GymLeaderTeamLegal',mon,1),(species,ty,'wrong Normal eligibility')
 setdata(mon,v[41],0)
 assert call('GymLeaderTeamLegal',mon,1)
# A stored Fire-form Silvally cannot remain eligible after removing its Memory.
call('CreateRandomMon',mon,v[131],5);wr(current()+v[3],1,1)
setdata(mon,v[41],0);assert not call('GymLeaderTeamLegal',mon,1)
setdata(mon,v[41],v[134]);assert call('GymLeaderTeamLegal',mon,1)
call('Free',mon)
# Register a stale Normal species holding Fire Memory. The temporary official
# team must materialize the real Fire form, then preserve its identity/EXP.
mon=symbols['gParties'];call('CreateRandomMon',mon,v[130],5)
setdata(mon,v[41],v[134])
wr(current()+v[26],1,1)
wr(current()+v[27],call('GetMonData2',mon,v[36]))
wr(current()+v[28],call('GetMonData2',mon,v[37]))
assert call('GymLeaderPrepareDefense')==1
assert call('GetMonData2',mon,v[10])==v[131]
wr('gBattleOutcome',2,1);call('GymLeaderFinishDefense')
assert call('GetMonData2',mon,v[10])==v[131]
setdata(mon,v[41],0)
queue=rd(current()+v[32],1)
assert call('GymLeaderPrepareDefense')==2
assert rd(current()+v[32],1)==queue
print('PASS all 17 Memory/Plate types, stale form removal, registered held-form materialization and refusal without queue consumption',flush=True)
