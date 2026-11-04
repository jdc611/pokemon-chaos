"""Read-only FireRed Championship Hall art, using real competition archives."""
import runpy,struct
ctx=runpy.run_path('tests/gym_leader/native_competitions.py');globals().update(ctx)
data=(ROOT/'hall-art-layout.bin').read_bytes();h=struct.unpack('<'+'I'*(len(data)//4),data)
assert field(12)==2
# Simulate the next native NPC-only circuit at Rank 8 so the newest archive
# contains a full six-partner champion; earlier player/NPC teams remain.
wr(current()+v[2],8,1)
for _ in range(3):
 setfield(3,rd(current()+v[56]),4);call('GymLeaderAdvanceActivity');assert field(8)==1;start();assert field(8)==0
assert field(12)==3
def bytes_at(a,n):return bytes(rd(a+i,1) for i in range(n))
def archive_banks():
 buf=call('Alloc_',3968,0);result=[]
 for bank in range(2):
  status=call('GleReadArchiveSector',bank,buf);result.append((status,bytes_at(buf,3968)))
 call('Free',buf);return result
# Give one archived NPC partner a known shiny bit and ten-character nickname;
# this is an archive-display fixture, not a claim of a played shiny capture.
banks=archive_banks();valid=[(int.from_bytes(p[8:12],'little'),bank,p) for bank,(status,p) in enumerate(banks) if status==1 and p[:4]==struct.pack('<I',0x474c4348)]
_,bank,p=max(valid);p=bytearray(p);q=16+112*2+4;p[q+7]=1;p[q+8:q+18]=bytes([0xc4]*10)
buf=call('Alloc_',3968,0)
for i,b in enumerate(p):wr(buf+i,b,1)
assert call('GleWriteArchiveSector',bank,buf)==1;call('Free',buf)
before_banks=archive_banks();before_party=bytes_at(symbols['gParties'],6*v[5]);before_state=bytes_at(current(),v[1])
warp(v[24],9,9);press(64);press(1);until(lambda:menu_count()==9);choose(5);until(lambda:menu_count()==4)
before_state=bytes_at(current(),v[1])
snap('championship-hall-art-option')
def open_art(expected_record):
 choose(0)
 for _ in range(900):
  if task('Task_HofPC_HandleInput')<16:break
  frames(1)
 else:snap('hall-art-failure');raise AssertionError('native FireRed Hall viewer did not open')
 frames(60) # Allow queued window/palette DMA to reach the visible screen.
 team=rd('gHoFSaveBuffer');assert team
 count=p[16+112*expected_record+3]
 for i in range(count):
  archive=16+112*expected_record+4+18*i;mon=team+i*h[1]
  assert rd(mon+h[2])==int.from_bytes(p[archive:archive+4],'little')
  packed=rd(mon+h[3]-2,2)
  assert packed>>1==int.from_bytes(p[archive+4:archive+6],'little') and (packed&1)==bool(p[archive+7])
  assert rd(mon+h[3],1)==p[archive+6] and bytes_at(mon+h[4],10)==p[archive+8:archive+18]
 return count
count=open_art(2);assert count==6
snap('championship-hall-npc-shiny')
for i in range(1,count):
 press(128,30);assert rd(symbols['gTasks']+task('Task_HofPC_HandleInput')*v[19]+v[20]+4,2)==i
snap('championship-hall-sixth-partner');press(64,30)
press(2,100);until(lambda:menu_count()==4)
assert bytes_at(symbols['gParties'],6*v[5])==before_party and bytes_at(current(),v[1])==before_state
choose(1);until(lambda:menu_count()==4);choose(1);until(lambda:menu_count()==4);assert rd('sHallCursor',1)==0
assert open_art(0)==3;snap('championship-hall-player-team');press(2,100);until(lambda:menu_count()==4)
# Reopening and A-exit use the same native screen and resume the field script.
assert open_art(0)==3;press(1,100);until(lambda:menu_count()==4)
choose(3);until(lambda:menu_count()==9);press(2);finish_text()
assert archive_banks()==before_banks and bytes_at(symbols['gParties'],6*v[5])==before_party
assert call('TrySavingData',0)==1 and call('LoadGameSave',0)==1 and archive_banks()==before_banks
print('PASS native FireRed Hall artwork for NPC/player teams, all six partners, PID/nickname/level/shiny fidelity, older-team navigation, repeated A/B exit, field return and byte-for-byte read-only archive/party/state behavior',flush=True)
