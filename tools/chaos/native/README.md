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

## V2 checks (October 8)

Compile the five new `v2-*-layout.c`/`v2-layout.c`/`v2-ids.c`/`v2-integration.c`/`v2-tms.c` constant tables as above, using `-iquote include` rather than `-I include` so the project's `strings.h` cannot shadow newlib's header. Output matching `.bin` files into `CHAOS_QA_DIR`. Regenerate `qa_pass_boot.py` and `qa_pass_events.py`, then run `run_v2.py`. It runs the 27 native suites in three independent groups, in dependency order within each group, and records exit codes. Run `qa_v2_growth_branches.py` and `qa_v2_ranch_training.py` afterward for the 63-branch cycle and actual Center/Ranch/Train menu paths.

`emulator.c` now supports six-argument APCS calls for the evolution getter. Both direct native calls and genuine UI/script paths are used; individual assertions/logs indicate which. Center cycling uses emulator state save/load; separate reward fixtures exercise real flash save/load. The tests do not emulate iOS backgrounding or provide a complete playthrough. V2 results live in `results/v2`; the implementation/test limits are in `docs/chaos-v2-verification.md`.
