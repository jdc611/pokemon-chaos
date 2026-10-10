# IronMON starter, BST and controller update

This update builds on published Playtest #235. It does not change run-over/restart behavior; that request was withdrawn.

## New runs

- Normal uses the existing custom base-species starter picker. Appearance/shiny editing is skipped in IronMON. Ability, nature, moves and the one held item remain seeded/randomized, with MGM and a functional damaging move guaranteed.
- Hardcore assigns one seeded random starter.
- Both modes expose a BST row on the Play Style setup page: Shuffle or Random. Normal defaults to Shuffle, Hardcore to Random. The choice is stored once and reasserted on load; it cannot be changed during a run.
- Shuffle redistributes the species' canonical stat total. Random independently generates all six base stats using the existing Chaos algorithm (95% 5–160, 5% 161–220); it does not preserve the canonical total. The chosen profile applies to all Pokémon, including trainers and evolution selection.

## Compatibility and fixes

Existing #235 IronMON saves keep their three-choice Normal/random Hardcore starters and shuffled stat budgets. The run-state version distinguishes new runs, reusing a previously unused byte without growing or moving save structures. Start a New Game to use the new starter/BST options.

IronMON trainer generation now retains authored Dynamax eligibility. All three lab-rival entries disallow Dynamax; later authored permissions remain intact. Regular Chaos trainer generation is unchanged.

The actual overworld tracker shortcut previously returned the same value as a launched field script. Its caller acquired a script lock after the tracker saved its scene; closing restored graphics/callbacks but left control locked. The shortcut now returns a consumed-overlay result, and the caller neither locks controls nor steps the player. Safely denied tracker openings also consume the chord without acquiring that lock. Real shortcut tests cover repeated opening/closing, movement, denied openings with L=A, and subsequent Start/Cancel input; direct overlay-call tests alone missed this caller bug.

The Dark Comfort Delta skin uses a near-black background, light gray controls, red A/B, lower portrait controls and four separate cardinal buttons with unmapped gaps. Its game viewport remains transparent and proportional. The centered TRACKER chord is unchanged. It works with #235 independently of the new ROM.

## Verification

- Local FireRed build succeeds; EWRAM/IWRAM remain 253,892/29,072 bytes, below their capacities.
- All 21 existing source regression suites pass, including 25,200 species/seed stat profiles.
- Native emulator tests verify all four mode/BST combinations through actual setup button presses, confirmation and new-game initialization.
- Three seeds per combination verify deterministic stats, the locked preset, flash save/reload, MGM, all three lab-rival Dynamax markers and legacy preservation.
- Actual custom selection, B/No cancellation, exact awarded species, item/attack guarantee and reentry protection pass.
- Actual Hardcore lab battle with controlled 1-HP injury verifies Dynamax denial and preserved faint/terminal behavior.
- Regular Chaos/Nuzlocke/IronMON startup, legacy Normal choices, all eight Gym guards and tracker field/battle restoration pass.
- Evolution selection tests cover 3,210 mappings under both budgets. The existing bounded candidate scan can fall back to its strongest sampled eligible target when a random source has exceptional BST; seven sampled random-budget mappings regress by 11–93 points. This is disclosed, not asserted to be a strict no-regression guarantee.
- Skin archive, all six device/orientation layouts, transparent viewport, independent directions, input mappings, bounds and nonoverlap checks pass.

Native tests use controlled fixtures and do not establish natural playthrough balance. iOS Delta import/touch testing and full walking playthroughs remain unverified. Published workflow/ROM identifiers are reported after CI completes.
