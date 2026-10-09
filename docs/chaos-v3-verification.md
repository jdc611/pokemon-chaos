# Pokémon Chaos FireRed V3 — playtest build and verification

Built October 9, 2026 from `jdc611/pokemon-chaos`, base commit `dffbcb88c6431262284e4c043221919b437699a6`.

This is one compiled V3 playtest ROM. It boots and runs in the native mGBA-based test environment, including actual battles, menus, purchases, and a normal in-game save/load. It is **not a claim of complete Delta certification or completion of every visual requirement**. Remaining work is stated below.

## ROM identity

- File: `Pokemon_Chaos_FireRed_V3.gba` — 33,554,432 bytes (32 MiB padded GBA ROM).
- SHA-256: `292de9294cfc3a55754cbdcce0e5b35b7302c7e5bab733c30f01c1f472bdf5cd`
- Header: `POKEMON FIRE`, game code `BPRE`.
- Linker usage: EWRAM 248,308 / 262,144 bytes (94.72%); IWRAM 29,072 / 32,768 (88.72%); ROM payload 27,188,704 bytes.
- SaveBlock3 remains 1,564 bytes; new transformation/activation flags are battle state, not new persistent save fields.

## Brief change log

- Replaced conflicting V2 adaptations with the locked V3 stat multipliers and thresholds for non-original Zen Mode, Stance Change, Schooling, Shields Down, Power Construct, Zero to Hero, Tera Shift, and Hunger Switch. Original species retain native form paths. Battle Bond gains both offenses and Speed once; Embody Aspect and As One use actual current battle stats.
- Added universal temporary Forecast/Plate/Memory typing, first-hit Disguise/Ice Face protection, Commander Doubles support with a targetable holder, Gulp Missile retaliation, and entry/reentry Teraform Zero clearing. Tera Shell uses a single final 0.5 effectiveness multiplier at full HP while preserving type immunities. Non-Terapagos Tera Starstorm remains Normal.
- Added conscious bench support for the approved Singles support abilities, real-item Symbiosis transfer, no duplicate support stacking, and Battery/Power Spot combined special damage of 1.69. Prevented post-battle Symbiosis item duplication. Restored/transferred Berries can activate again without recreating Bag items.
- Ordinary full-party gift/capture storage skips the Nuzlocke Graveyard. Randomized Magikarp-vendor preview, receipt, nickname target, and PC destination use the actual purchased Pokémon.
- Party, Summary, naming, and move-relearning callbacks share key-release filtering: held keys produce one action. Egg Moves has bounded list construction, a 256-entry capacity, corrected sorting bounds, and bounded/cycle-safe ancestry traversal.
- DexNav clears old encounter buffers and keys randomizer caching by the relevant settings. Land, surfing, and fishing methods remain separate.
- Added Summary editing via **SELECT** when the existing changers are unlocked. Up/Down selects Nature, Ability, Gender, Accept, or Discard; Left/Right previews eligible values. Accept commits, B/Discard restores. Existing editor access remains available.
- Growth/Evolution uses a red/blue-gray treatment, black readable text, synchronized page indicators, explicit branch counts and next-evolution icons. Chaos Rating remains the calibrated current-form 0–10 estimate; its methodology was preserved and the unexplained `(est)` abbreviation replaced with an explanation.
- Gloom displays both Vileplume/Leaf Stone and Bellossom/Sun Stone; both native item evolutions work without National Dex.
- Main TM descriptions show move name, category, PP, power, and accuracy. Existing TM compatibility details remain. Strange Fossil text wraps into three short lines; its Sabrina/Nidokingite progression is unchanged.
- Moved the conflicting Cerulean NPC away from the Records Nurse. Added a stationary Egg display on the existing table beside each approved Egg vendor.
- FireRed hatch cycles are uniformly 16 steps instead of 128, preserving normal hatch handling rather than instant hatching.
- Celadon 5F X-item counter now includes Life Orb, Flame Orb, Toxic Orb, Assault Vest, Eviolite, Weakness Policy, Ability Shield, Booster Energy, 17 Plates, and 17 Memories. Prices are ₽3,000 or ₽5,000 for competitive stock; buy/sell pricing is consistent across maps. No duplicate definitions or stock entries were added.
- Added regression tests and compiler-derived fixture offsets. Updated the Doubles preview fixture to reveal opposing typing before asserting different arrows, consistent with V3's no-leak requirement.

## What actually passed

| Test group | Verified result |
|---|---|
| Existing native ability suites | **530 cases in 77 suites passed**, covering the ability-audit names; paired/variant abilities share or use their native suites. |
| Ripen Berry tests | **8 passed**, covering supported stat-raising Berry effects. |
| New Chaos battle suite | **10 passed**: non-original Zen, Disguise, Ice Face, Power Construct, Stance Change, Tera Shift/Starstorm, Gulp Missile, Singles Symbiosis/item restoration, Commander, Teraform Zero reentry. |
| Competitive held-item suites | **51 passed** across Life Orb, Flame Orb, Toxic Orb, Assault Vest, Eviolite, Weakness Policy, Ability Shield, Booster Energy. |
| Existing compiled-ROM regressions | **27 suites passed**: full-party capture/replacement and PC delivery, Nuzlocke routing, menus/egg relearning, TMs, berries, EXP, Doubles target UI, centers, gyms, ranch, progression and story-event checks. |
| Host/source logic regressions | **21 suites passed**, including the production Chaos multiplier boundary harness. These are distinct from ROM execution tests. |
| New save/menu probes | Summary draft changes leave party bytes unchanged; Discard restores the original Pokémon; Accept changes requested fields; 90 held Right frames change a value once; normal native SaveGame and LoadGameSave retain accepted edits byte-for-byte. |
| Storage probes | Full party with boxes 0, 4, or Graveyard selected delivers the new gift to ordinary storage and preserves all six original party Pokémon. |
| Vendor probes | All four combinations of open/full party and nickname Yes/No passed; randomized species 561 used consistently; original party bytes preserved; exactly ₽500 charged. |
| DexNav | Ten reopenings on a land map and ten on a water/fishing map retained identical eligible species/methods while unrelated RNG advanced. |
| Egg counters | Bulbasaur, Pikachu, and Magikarp eggs advance on 16-step boundaries and enter the ordinary pending-hatch path. Existing three-vendor delivery/receipt/payment tests also passed. |
| Battle preview | Unknown identity hides arrows; neutral hits do not reveal; qualifying hits reveal. Earth Eater, Well-Baked Body, Purifying Salt, Tera Shell, Wonder Guard, Mind's Eye, Mold Breaker, and Ability Shield preview behavior passed. |
| Evolution/shop | Native Gloom stone evolutions pass without National Dex; all 63 Milcery branches cycle/wrap; compiled 49-entry Celadon counter is valid and unique, with correct competitive prices and resale limits. |
| Visual inspection | Summary preview/editor, Growth page indicators and both Gloom branches, primary TM description area, Egg tabletop display, and Cerulean NPC placement were rendered for inspection. |

The existing ability suites test native supported mechanics and interactions. They do **not** establish exhaustive testing of every non-original species, every pair of copied/swapped/suppressed abilities, or every Singles support combination. The new probes cover the additional cases explicitly named above. Raw logs, result JSON, and selected screenshots accompany this report.

## Remaining work and omissions

1. **Zen Mode's custom subtle gold aura is not implemented.** Its V3 stats and a `ZEN` battle indicator are implemented and tested. A new aura animation was left out because its palette/OAM integration was not verified for safe coexistence with battle effects.
2. **Stance Change currently displays `BLADE`/`SHIELD` text rather than graphical sword/shield icons.** Its multipliers/reset are implemented and tested. New icon integration remains unverified; no claim is made that the requested graphic treatment is complete.
3. **No real iOS Delta control or device test was performed.** The GBA ROM and normal native save round trip are real; Delta touch controls, emulator backgrounding, audio, and importing an existing V2 Delta save remain unverified.
4. No complete new-game-to-postgame playthrough was performed. Existing progression/center suites passed; the earlier intermittently reported Mt. Moon Center black screen was not reproduced in the center stress test and is not claimed definitively fixed.
5. Not every long custom item description was manually inspected; Strange Fossil and the selected UI screens were checked. Each possible menu/button timing and randomized Egg hatch-animation combination was not exhaustively tested.
6. The Illusion disguise stays visible in type icons; effectiveness previews are hidden while Illusion remains active to avoid exposing the real holder. A speculative preview using disguise typing was not added.
7. **Brendan is intentionally absent from V3**, exactly as directed. Future character/home/signature/arcade/Rocket/Cerulean Cave/Gym ideas are preserved in master-planning documentation; no maps, trainers, rewards, flags, or story scenes were added for him.

No failed test is represented as passing. This ROM is a playtest deliverable with the above known limits, not a promise that all requested acceptance scenarios have been verified.

## Build and test reproduction

Run `python3 .github/scripts/materialize_chaos_title.py`, then `make firered` with an ARM newlib toolchain. The produced ROM is `pokefirered.gba`. Test ROMs use the repository's `make BUILD=firered check` infrastructure; `test/battle/ability/chaos_v3.c` contains the new engine cases. Compiler-produced layout fixtures and `tools/chaos/native/qa_v3*.py` contain the compiled-ROM probes. Test fixtures must be regenerated after ROM relocation; old emulator states cannot be reused with new code addresses.
