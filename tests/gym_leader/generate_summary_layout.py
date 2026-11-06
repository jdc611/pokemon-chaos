"""Generate a read-only ABI probe for the native move-learning cursor.
Derive the private structure prefix from the actual screen source rather than
maintaining a hardcoded offset. Compile this file with the normal ARM flags.
"""
from pathlib import Path
import os
source=Path('src/pokemon_summary_screen.c').read_text()
prefix=source[source.index('static EWRAM_DATA struct PokemonSummaryScreenData'):]
prefix=prefix[:prefix.index('    u8 firstMoveIndex;')+len('    u8 firstMoveIndex;')]
prefix=prefix.replace('static EWRAM_DATA struct','struct',1)+'\n};\n'
root=Path(os.environ['CHAOS_QA_DIR']);root.mkdir(exist_ok=True)
(root/'summary_layout.c').write_text('#include "global.h"\n#include "pokemon_summary_screen.h"\n#include "constants/region_map_sections.h"\n#include <stddef.h>\n'+prefix+'const unsigned summaryLayout[]={offsetof(struct PokemonSummaryScreenData,firstMoveIndex)};\n')
print(root/'summary_layout.c')
