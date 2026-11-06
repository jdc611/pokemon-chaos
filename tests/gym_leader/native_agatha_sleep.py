"""Actual Agatha retry after native level-up relearning: sleep, setup, healing."""
import os,runpy,json
os.environ['GLE_CAREER_HELPERS_ONLY']='1'
globals().update(runpy.run_path('tests/gym_leader/native_career.py'))
assert rank()==6 and rd(current()+v[46],1) and len(c)>=47
register_team([1,2,0,5,4]);original_bestmove=bestmove

def prepared_bestmove():
 prepared_bestmove.last_scores=[1]*4
 own=symbols['gBattleMons'];enemy=own+c[0];moves=[rd(own+c[1]+i*c[4],c[4]) for i in range(4)]
 if rd(own,2)==549:
  asleep=bool(rd(enemy+c[39])&7);hp=rd(own+c[26],2);maxhp=rd(symbols['gParties']+rd('gBattlerPartyIndexes',2)*v[5]+v[14],2)
  desired=79 if not asleep else 235 if hp*10<maxhp*7 else 483 if rd(own+c[45]+c[46],1)<10 else 202
  if desired in moves:
   i=moves.index(desired)
   if rd(own+c[2]+i,1) and volatilefield(0)!=desired:return i
 result=original_bestmove();prepared_bestmove.last_scores=[1]*4;return result
battle.__globals__['bestmove']=prepared_bestmove
choose(4);until(yesno);choose(0);outcome=battle();until(lambda:menu_count()==8)
row={'stage':'agatha_prepared_retry','outcome':outcome,'rank':rank(),'rank_xp':rd(current()+v[31]),'wins':rd(current()+v[29],2),'losses':rd(current()+v[30],2),'party':profile()}
(ROOT/'career-progress.json').write_text(json.dumps([row],indent=2)+'\n');state('career-agatha-prepared-result');print('CAREER',json.dumps(row),flush=True)
if outcome==1:
 assert rank()==7;controller_save('career-rank-seven-native');state('career-earned-rank-seven');snap('career-earned-rank-seven')
 print('PASS EARNED CONTROLLER CAREER THROUGH AGATHA; FULL CAMPAIGN REMAINS INCOMPLETE',flush=True)
else:print('PREPARED AGATHA RETRY LOST: earned progress retained',flush=True)
