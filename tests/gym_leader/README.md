# Gym Leader internal checks

These validate implemented foundations, not a completed Gym Leader Edition.
Opening/menu/map/event integration is still incomplete. Do not publish the local
ROM as the requested finished game.

Host scenarios:

```
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Iinclude src/gym_leader_state.c src/gym_leader_competitions_core.c src/gym_leader_hosting_state.c tests/gym_leader/state_test.c -o /tmp/gle-state-test
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
playthrough. Summary profile/stats/moves now render and return safely.

```
python3 tests/gym_leader/run_opening_matrix.py
python3 tests/gym_leader/native_defense.py
```

The matrix runs all 18 types across all three towns. The defense fixture uses
actual guide registration and a real Fight/Powder Snow battle, then direct ARM
transaction scenarios for PC storage, loss, EXP, level normalization, invalid
teams, and registration persistence. No forced outcome/HP change is used in the
controller-driven fight. Direct transaction scenarios explicitly set outcomes.

```
python3 tests/gym_leader/native_signature.py
```

This fixture uses a test gift for a naturally compatible off-type Pokémon, then
teaches through the real bag and party menus. It checks the learned move, official
legality and the protected item remaining. It is not all-options compatibility QA.


```sh
python3 tests/gym_leader/native_inspection.py
python3 tests/gym_leader/native_town.py
GLE_TEST_TOWN=1 python3 tests/gym_leader/native_town.py
GLE_TEST_TOWN=2 python3 tests/gym_leader/native_town.py
```

Inspection gates use explicit outcomes for transaction checks and real UI input
for launch/loss/return. Town tests purchase a Center through its sign, use its Nurse
and door, test housing/growth/deposits and flash/map restoration. Development funds
are supplied for the Habitat fixture; its walking-generated wild encounter is real.
The direct whiteout callback fixture verifies home recovery, not a full wild loss.
Build with conservative parallelism if a compiler job is killed by memory pressure;
remove any resulting zero-byte generated objects before rebuilding.

Hosting checks: compile `hosting_layout.c` and `competition_layout.c` alongside the shared layout, then run `native_hosting.py` with `GLE_TEST_TOWN=0`, `1`, and `2` in separate QA directories. `hosting_state_test.c` links the state, competition core and hosting-state sources and covers invitations/rival bids and Kanto settlement.

Ecology checks: generate the ROM candidate header with `generate_ecology_catalog.py`, compile `ecology_layout.c` into `ecology-layout.bin` alongside the shared layout and run `native_ecology.py`. It covers all eight ranks, actual forest wild generation, DexNav display/search caps, RNG/cache behavior and the non-GLE adapter. Run native town and Safari regressions for configured encounter overrides.
