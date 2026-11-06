"""First earned-team Agatha strategy retry; actual outcome was a loss.
Uses native registration, paralysis support and actual Ghost coverage.
"""
import os,runpy,json
os.environ['GLE_CAREER_HELPERS_ONLY']='1'
globals().update(runpy.run_path('tests/gym_leader/native_career.py'))
assert rank()==6 and rd(current()+v[46],1) and len(c)>=40
register_team([1,2,0,5,4])
original_bestmove=bestmove
def retry_bestmove():
 own=symbols['gBattleMons'];enemy=own+c[0];species=rd(own,2);foe=rd(enemy,2)
 moves=[rd(own+c[1]+i*c[4],c[4]) for i in range(4)]
 if species==549 and foe==24 and not rd(enemy+c[39]) and 78 in moves:
  i=moves.index(78)
  if rd(own+c[2]+i,1) and volatilefield(0)!=78:return i
 if species==781 and foe in [429,94] and 566 in moves:
  i=moves.index(566)
  if rd(own+c[2]+i,1) and volatilefield(0)!=566:return i
 return original_bestmove()
battle.__globals__['bestmove']=retry_bestmove
choose(4);until(yesno);choose(0);outcome=battle();until(lambda:menu_count()==8)
row={'stage':'agatha_inspection_retry','outcome':outcome,'rank':rank(),'rank_xp':rd(current()+v[31]),'wins':rd(current()+v[29],2),'losses':rd(current()+v[30],2),'party':profile()}
(ROOT/'career-progress.json').write_text(json.dumps([row],indent=2)+'\n');state('career-agatha-retry-result');print('CAREER',json.dumps(row),flush=True)
if outcome==1:
 assert rank()==7;controller_save('career-rank-seven-native');state('career-earned-rank-seven');snap('career-earned-rank-seven')
 print('PASS EARNED CONTROLLER CAREER THROUGH AGATHA; FULL CAMPAIGN REMAINS INCOMPLETE',flush=True)
else:print('AGATHA RETRY LOST: earned progress retained; further preparation required',flush=True)
