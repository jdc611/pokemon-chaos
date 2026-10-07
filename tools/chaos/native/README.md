# Native FireRed stabilization checks

These fixtures run the compiled ROM in mGBA; they do not replace a full Delta playthrough. They exercise actual scripts, transitions, evolution scenes, capture animations, save sectors and HUD rendering. Gym victory fixtures lower enemy HP and preserve player HP to reach every reward deterministically; they test integration, not balance or trainer strength.

Prerequisites: a freshly built `pokefirered.gba` and `.elf`, Python 3 with Pillow, ARM GCC/binutils/newlib headers, and mGBA development headers/library. All outputs are written to `CHAOS_QA_DIR` (default `/tmp/chaos-native-qa`). Set `CHAOS_ARM_NM` if ARM nm is not on PATH.

Compile `emulator.c` with `cc -shared -fPIC -lmgba` into `$CHAOS_QA_DIR/emulator.so`. Compile each `*-layout.c` with the ROM's include/configuration flags (`-DFIRERED -DMODERN=1 -mthumb -mabi=apcs-gnu -march=armv4t -iquote include` and newlib headers), then extract `.rodata` with ARM objcopy into the output directory under the corresponding `.bin` name. This is necessary because enum/structure layouts must match the ROM exactly.

Run from this directory, in order:

1. `qa_pass_boot.py` creates a valid new-game fixture with initialized player/rival names.
2. `qa_pass_events.py` creates the field fixture and tests Brock's genuine loss/re-entry.
3. `qa_pass_jj.py`, `qa_pass_training.py`, `qa_pass_core.py`.
4. `qa_pass_battle.py`, `qa_pass_doubles.py`, `qa_pass_retry.py`, `qa_pass_rewards.py`.
5. `qa_pass_gym_battles.py`, `qa_pass_hof.py`, `qa_pass_progression.py`, `qa_pass_rider.py`.

Regenerate every state after rebuilding the ROM. Code addresses are embedded in native save states. The fixture memory at `0x0203e000` must remain outside allocated EWRAM; inspect the linker map if future features change RAM usage. Private controller function names can repeat across translation units, so the helper selects the Player controller rather than the Oak/Safari controller.

Logs and reviewed screenshots from the October 6 stabilization directive are described in `docs/chaos-stabilization-verification.md`.
