#!/usr/bin/env python3
"""Give FRLG/Kanto trainers their own ID range in the Emerald Chaos build.

Expansion normally compiles either the Emerald or FRLG trainer table. Chaos uses
both regions in one Emerald ROM, so the stock FRLG IDs (0..623) collide with
Emerald IDs. For the playtest build we offset every FRLG trainer except
TRAINER_NONE by TRAINERS_COUNT_EMERALD (855), then enlarge the combined trainer
and trainer-flag ranges.

This runs in CI before trainerproc/make; it intentionally edits the checkout
only. The source FRLG header stays compatible with upstream Expansion.
"""
from pathlib import Path
import re

root = Path(__file__).resolve().parents[2]
frlg = root / "include/constants/opponents_frlg.h"
emerald = root / "include/constants/opponents.h"

text = frlg.read_text()

def offset_define(match: re.Match[str]) -> str:
    name, value = match.group(1), int(match.group(2))
    if name == "TRAINER_NONE":
        return match.group(0)
    return f"#define {name:<46} (TRAINERS_COUNT_EMERALD + {value})"

# Only trainer IDs are transformed; count/max constants are left untouched.
text = re.sub(
    r"^#define\s+(TRAINER_(?!PARTNER)[A-Z0-9_]+)\s+(\d+)\s*$",
    offset_define,
    text,
    flags=re.MULTILINE,
)
frlg.write_text(text)

text = emerald.read_text()
old = """#if IS_FRLG
#define TRAINERS_COUNT                      TRAINERS_COUNT_FRLG
#define MAX_TRAINERS_COUNT                  MAX_TRAINERS_COUNT_FRLG
#else
#define TRAINERS_COUNT                      TRAINERS_COUNT_EMERALD
#define MAX_TRAINERS_COUNT                  MAX_TRAINERS_COUNT_EMERALD
#endif"""
new = """#if IS_FRLG
#define TRAINERS_COUNT                      TRAINERS_COUNT_FRLG
#define MAX_TRAINERS_COUNT                  MAX_TRAINERS_COUNT_FRLG
#else
// Chaos contains Hoenn + Kanto trainers in one Emerald ROM. FRLG trainer IDs
// are shifted by TRAINERS_COUNT_EMERALD by the playtest preparation step.
#define TRAINERS_COUNT                      (TRAINERS_COUNT_EMERALD + TRAINERS_COUNT_FRLG)
#define MAX_TRAINERS_COUNT                  1536
#endif"""
if old not in text:
    raise SystemExit("Could not find trainer-count block in opponents.h")
emerald.write_text(text.replace(old, new))

print("Prepared Chaos trainer ranges: Emerald 0..854, Kanto 855..1478")
