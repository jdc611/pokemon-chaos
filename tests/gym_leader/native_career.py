"""Continue the earned controller campaign; no game-state writes or supplied outcomes."""
import runpy,os,json
assert os.environ.get('GLE_CAMPAIGN_RESUME'), 'Use an earned, ROM-matched checkpoint'
globals().update(runpy.run_path('tests/gym_leader/native_campaign.py'))
def rank():return rd(current()+v[2],1)
def desk():
 walkto(7,12);press(16);press(1);until(lambda:menu_count()==5)
def register_team(indices):
 choose(1);until(lambda:call('GymLeaderRegistering'));frames(160)
 for target in indices:
  for _ in range(24):
   cursor=call('GetCursorSelectionMonId')
   if cursor==target:break
   press(32 if target==0 else 16 if cursor==0 else 128 if cursor<target else 64,15)
  assert call('GetCursorSelectionMonId')==target
  press(1,25);press(1,25)
 press(8,25);press(1,100);until(lambda:menu_count()==8)
 assert rd(current()+v[26],1)==len(indices)
def claim_reward():
 count=rd('gPartiesCount',1);assert count<6, 'Native PC withdrawal required before next reward'
 goleague();desk();choose(1);until(lambda:menu_count()==5)
 vouchers=rd(current()+v[66],1);assert vouchers>0
 choose(0);until(lambda:menu_count()==vouchers);choose(0);until(lambda:menu_count()==3)
 scores=[]
 for i in range(3):
  mon=symbols['gGlePreviewParty']+i*v[5]
  moves=[call('GetMonData2',mon,v[38]+j) for j in range(4)]
  scores.append(sum(sorted([movevalue(m) for m in moves],reverse=True)[:2]))
 choose(max(range(3),key=lambda i:scores[i]));until(lambda:menu_count()==3);choose(0);until(yesno);choose(0)
 until(lambda:menu_count()==5);assert rd('gPartiesCount',1)==count+1
 print('CAREER earned milestone reward',json.dumps(profile()),flush=True)
 gohome();return count
def reach_rank_four():
 start_rank=rank();assert start_rank==3
 new=claim_reward();register_team([new,3,1,0])
 for match in range(60):
  if rank()>=4:break
  assert not rd(current()+v[46],1), 'Unexpected inspection before rank four'
  choose(0);until(yesno);choose(0);outcome=battle();until(lambda:menu_count()==8)
  row={'stage':'rank_three_defense','match':match+1,'outcome':outcome,'rank':rank(),'rank_xp':rd(current()+v[31]),'wins':rd(current()+v[29],2),'losses':rd(current()+v[30],2),'party':profile()}
  history.append(row);(ROOT/'career-progress.json').write_text(json.dumps(history,indent=2)+'\n')
  state('career-latest');print('CAREER',json.dumps(row),flush=True)
 else:raise AssertionError('Rank four not reached within match budget')
 assert rank()==4
 state('career-earned-rank-four');snap('career-earned-rank-four')
 print('PASS EARNED CONTROLLER CAREER TO RANK FOUR; FULL CAMPAIGN REMAINS INCOMPLETE',flush=True)

if not os.environ.get('GLE_CAREER_HELPERS_ONLY'):reach_rank_four()
