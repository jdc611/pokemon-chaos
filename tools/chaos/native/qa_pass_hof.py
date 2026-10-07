from qa import *
import struct
v=struct.unpack('<22I',(ROOT/'verify-layout.bin').read_bytes());assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
call('ZeroPlayerPartyMons')
for sp in [1,7,25,59,130,150]:call('ScriptGiveMon',sp,60,0)
call('ChaosEnsureRunRecords');s=rd('gSaveBlock3Ptr');wr(s+v[8],32);wr(s+v[20],123456789);wr(s+v[21],2,1)
call('SetMainCallback2',symbols['CB2_DoHallOfFameScreenFrlg']|1)
for n in range(30000):
 if any(rd(symbols['gTasks']+i*40)==symbols['Task_Hof_ExitOnKeyPressed']|1 for i in range(16)):break
 frames(1,1 if n%25<2 else 0)
else:raise AssertionError('Hall of Fame summary not reached')
s=rd('gSaveBlock3Ptr');assert rd(s+v[9],1) and rd(s+v[6])==32
frames(2);snap('qa-pass-league-summary')
for page in range(1,10):frames(2,1);frames(30)
snap('qa-pass-league-final-team');assert rd('gSpecialVar_0x8004',2)==9
# Ongoing run changes do not mutate saved League totals.
wr(s+v[8],40);wr('gSpecialVar_0x8004',1,2);wr('gSpecialVar_0x8005',1,2);call('ChaosBuildRecordsPage');assert rd(s+v[6])==32
print('PASS automatic native Hall of Fame -> ten-page League Summary, final-team pages and separate ongoing records.',flush=True)
