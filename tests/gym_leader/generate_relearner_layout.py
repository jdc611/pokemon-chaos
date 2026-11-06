"""Extract private menu declarations from the current source for read-only ABI probes."""
from pathlib import Path
import sys,re
party=Path('src/party_menu.c').read_text();moves=Path('src/move_relearner.c').read_text()
enum=party[party.index('enum {\n    MENU_SUMMARY,'):];enum=enum[:enum.index('};')+2]
p=party[party.index('struct PartyMenuInternal\n'):];p=p[:p.index('};')+2]
m=moves[moves.index('static EWRAM_DATA struct\n{'):];m=m[:m.index('} *sMoveRelearnerStruct')+1].replace('static EWRAM_DATA struct','struct QaMoveRelearner',1)+';'
s='#include "global.h"\n#include "pokemon.h"\n#include "task.h"\n#include "main.h"\n#include "list_menu.h"\n#include "constants/move_relearner.h"\n#include "constants/vars.h"\n#include <stddef.h>\n'+enum+'\n'+p+'\n'+m+'\nconst unsigned qaRelearn[]={offsetof(struct PartyMenuInternal,actions),offsetof(struct PartyMenuInternal,numActions),MENU_RELEARN,offsetof(struct QaMoveRelearner,movesToLearn),offsetof(struct QaMoveRelearner,numMenuChoices),offsetof(struct QaMoveRelearner,moveListMenuTask),VAR_CHAOS_TRAINING_UNLOCKED,VAR_MAP_SCENE_PEWTER_CITY};\n'
Path(sys.argv[1]).write_text(s)
