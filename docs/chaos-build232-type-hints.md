# Build 232 — shared Type Hints

Source: `68ea0c0efcaa97e740ad050c1ab9840f819efef6`.
Workflow: https://github.com/jdc611/pokemon-chaos/actions/runs/37937077097
ROM SHA256: `abf8211a2c153ed71356ac27a16d1661774ce7ded249df46f05419ba0b2325fc`.

## Behavior

- Off hides opponent type icons and effectiveness previews.
- Seen enables both only when the visible species has a Pokédex Seen flag.
- Revealed enables both immediately, including unknown species.
- Both displays share ChaosBattleTypesKnown. Visible type icons still use Illusion's disguise, and current battle typing is otherwise preserved.
- Saved IDs remain 0 (old Always, now Revealed), 1 (Seen), and 3 (Off). Legacy Caught value 2 is treated as Seen. Both option entry points cycle only Off / Seen / Revealed.
- No Pokémon, PC, or save-structure definitions changed. The ROM download ZIP contains only pokefirered.gba.

## Verification

- GitHub Build Playtest ROM #232 succeeded, including its existing regression scripts.
- Compiled-ROM checks passed for unknown/seen species under all four old saved option values and the shared visibility gate.
- Preview checks passed for Earth Eater, Well-Baked Body, Wonder Guard, Tera Shell, Purifying Salt, and Mind's Eye, including preserving Ghost immunity under Tera Shell.
- Native Options input checks passed for left/right cycling and acceptance, including legacy Caught handling. The Options screen was rendered for inspection.
- Four generated normal #231 flash saves imported into the #232 native flash loader. All three SaveBlocks and the complete PC storage matched byte-for-byte; full parties and settings restored. Test saves included a full party, a boxed gift, and a nonzero world seed/randomizer setting.

## Limits

The player's actual Delta save was not provided and was not tested. A frontend Continue probe using generated quick-start saves did not reach the overworld in the headless harness on either #231 or #232, so full title-screen Continue compatibility is not claimed verified. Keep the #231 ROM and an exported normal-save backup while checking #232 in Delta. No old emulator save states were used for the cross-build import tests.

The initial SaveBlock3 comparison read an unused optimized pointer; correcting the test to read the actual gSaveblock3 storage resolved that test error. No ROM change was needed.
