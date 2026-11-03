"""Rank EXP uses the real Trainer Card, including native exit and flip paths."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
def bar_pixels():
 window=symbols['gWindows']+12 # WIN_CARD_TEXT
 width=rd(window+3,1);pixels=rd(window+8)
 assert width==28 and pixels
 def pixel(x,y):
  offset=((y//8)*width+x//8)*32+(y%8)*4+(x%8)//2
  return (rd(pixels+offset,1)>>(4*(x%2)))&15
 return [pixel(x,116) for x in range(21,143)]
for label,rank,xp,inspection,filled in [('empty',1,0,0,0),('half',1,50,0,61),('inspection',2,180,1,122),('maximum',8,0,0,122)]:
 wr(current()+v[2],rank,1);wr(current()+v[31],xp);wr(current()+v[46],inspection,1)
 call('CleanupOverworldWindowsAndTilemaps')
 call('ShowPlayerTrainerCard',symbols['CB2_ReturnToField']|1)
 frames(300)
 bar=bar_pixels();assert bar.count(9)==filled,(label,bar)
 assert bar==[9]*filled+[1]*(122-filled)
 snap('rank-card-'+label)
 # Clock colon updates must not erase the rank row/bar.
 frames(120);assert bar_pixels()==bar
 if label=='half':
  press(1,240);snap('rank-card-back')
  press(2,240);assert bar_pixels()==bar
 press(2,180)
 assert task('Task_TrainerCard')==255
print('PASS native Trainer Card rank/EXP bar at empty, half, inspection and max; clock redraw, back/front flip and field return',flush=True)
