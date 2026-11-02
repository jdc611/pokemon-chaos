# Gym Leader internal checks

These validate implemented foundations, not a completed Gym Leader Edition.
Opening/menu/map/event integration is still incomplete. Do not publish the local
ROM as the requested finished game.

Host scenarios:

```
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Iinclude src/gym_leader_state.c tests/gym_leader/state_test.c -o /tmp/gle-state-test
ASAN_OPTIONS=detect_leaks=0 /tmp/gle-state-test
```

Leak scanning is disabled because this container disallows its process inspection;
address/undefined-behavior checks remain enabled. The state core allocates no heap.

Native ROM fixtures:

- Materialize title assets with the existing `.github/scripts/materialize_chaos_title.py`.
- Run the three generators in `tools/gym_leader/`, then `make firered`.
- Compile `native_layout.c` with matching ARM flags from the existing native QA
  README and extract `.rodata` to `$CHAOS_QA_DIR/layout.bin`.
- Compile `tools/chaos/native/emulator.c` as described there, adding
  `-DCHAOS_NATIVE_CALL_STEPS=200000000`. Flash save/reload can exceed the original
  20-million-step direct-call timeout. This is test instrumentation, not a gameplay
  timing change.
- Run `tests/gym_leader/native_smoke.py` from repository root with matching
  `CHAOS_QA_DIR`, `CHAOS_ARM_NM` and dependency library paths.

The fixture bypasses name entry/ordinary new-game setup, then explicitly activates
GLE state. It tests real linked ARM functions and flash sectors. It does not
exercise a player-facing GLE opening or claim a full playthrough. Always regenerate
fixtures after rebuilding; no old emulator states are imported.

Controller-driven opening fixture:

```
python3 tests/gym_leader/native_opening.py
GLE_TEST_TOWN_NAME=1 python3 tests/gym_leader/native_opening.py
```

Uses the same environment as native_smoke.py. Only player/rival name entry is
bypassed. Exercises hometown/type/signature menus, native Summary return, three
starter picks and GLA introduction. This is a limited opening route, not a full
playthrough. Summary profile text still has a documented presentation defect.
