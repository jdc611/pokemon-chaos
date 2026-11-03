"""Old-mode editor/training flags cannot bypass GLE reward progression."""
import runpy
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
# Explicit legacy unlock fixtures; they must not expose free ability/nature or
# hidden-ability editing in Gym Leader Edition.
call('VarSet',0x40FC,1)
# Discover the training var from its named generated constant in the source.
# The build's normal, fixed macro lives in constants/vars.h; no private state ID.
import re
vars_text=(repo/'include/constants/vars.h').read_text()
training=int(re.search(r'#define VAR_CHAOS_TRAINING_UNLOCKED\s+(0x[0-9A-Fa-f]+)',vars_text).group(1),16)
call('VarSet',training,1)
def callbacks():
 return [rd(symbols['sStartMenuItems']+rd(symbols['sCurrentStartMenuActions']+i,1)*8+4) for i in range(rd('sNumStartMenuActions',1))]
call('ShowQuickToolsMenu');frames(130)
assert rd('sStartMenuPage',1)==0
press(16,50)
assert rd('sStartMenuPage',1)==0
for fn in ['StartMenuChangeNature','StartMenuChangeGender','StartMenuChangeAbility']:
 assert symbols[fn]|1 not in callbacks()
snap('gle-quick-tools-no-editor');press(2,100)
call('ShowStartMenu');frames(130);press(16,60)
assert rd('sStartMenuPage',1)==1
assert symbols['StartMenuTrainToCap']|1 not in callbacks()
snap('gle-menu-no-train-to-cap');press(2,100)
# Open the real player-party Summary, where the old editor would be accessible
# using SELECT. Draft previews alone would not exercise that entry point.
call('CleanupOverworldWindowsAndTilemaps')
lib.call6(symbols['ShowPokemonSummaryScreen'],0,symbols['gParties'],0,2,symbols['CB2_ReturnToField']|1,0)
frames(180);press(4,70)
assert task('Task_ChaosSummaryEdit')==255
snap('gle-summary-no-free-profile-editor')
press(2,180)
print('PASS legacy unlock flags cannot expose free Quick Tools editors, Summary hidden-ability/profile editor or Train to Cap in GLE',flush=True)
