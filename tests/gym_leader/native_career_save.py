"""Native Start > Save, emulator power reset, and title-screen Continue.
Flash export/import preserves the game's own bytes and supplies no game progress.
"""
import runpy,ctypes as C,json
ctx=runpy.run_path('tests/gym_leader/native_campaign.py');globals().update(ctx)
close_to_field()
before={'rank':rd(current()+v[2],1),'wins':rd(current()+v[29],2),'losses':rd(current()+v[30],2),'xp':rd(current()+v[31]),'party':profile()}
counter=rd('gSaveCounter')
press(8,60);waitfor(lambda:task('Task_ShowStartMenu')<16 and rd('gMenuCallback')==symbols['HandleStartMenuInput']|1)
actions=[rd(symbols['sCurrentStartMenuActions']+i,1) for i in range(rd('sNumStartMenuActions',1))];target=actions.index(5)
for _ in range(20):
 cursor=rd('sStartMenuCursorPos',1)
 if cursor==target:break
 press(128 if cursor<target else 64,15)
press(1,80)
for _ in range(200):
 if rd('gSaveCounter')!=counter:break
 press(1,30)
else:snap('career-native-save-failed');raise AssertionError('Native save did not finish')
close_to_field();frames(120)
lib.save_dump.argtypes=[C.c_char_p];lib.save_restore.argtypes=[C.c_char_p]
flash=ROOT/'earned-career.sav';assert lib.save_dump(str(flash).encode())
print('CAREER native Save wrote flash',flash.stat().st_size,flush=True)
lib.reset();assert lib.save_restore(str(flash).encode());frames(180)
for _ in range(1200):
 if rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1 and call('IsGymLeaderRun'):break
 press(1,25)
else:snap('career-continue-failure');raise AssertionError('Native title-screen Continue did not return')
frames(120)
after={'rank':rd(current()+v[2],1),'wins':rd(current()+v[29],2),'losses':rd(current()+v[30],2),'xp':rd(current()+v[31]),'party':profile()}
assert after==before,(before,after)
state('career-native-continued');snap('career-native-continued')
(ROOT/'native-save-result.json').write_text(json.dumps({'before':before,'after':after,'flash_bytes':flash.stat().st_size,'pass':True},indent=2)+'\n')
print('PASS EARNED CAREER NATIVE SAVE, POWER RESET AND TITLE CONTINUE',json.dumps(after),flush=True)
