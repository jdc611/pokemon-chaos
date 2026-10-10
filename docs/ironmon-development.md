# IronMON internal development checkpoints — October 10, 2026

**Not a release. The complete IronMON directive is not implemented. Do not merge or publish this branch as a playable IronMON release.**

Repository: `jdc611/pokemon-chaos`. Target branch: `master`. Internal branch: `ironmon-development`.

Master was at `dffbcb88c6431262284e4c043221919b437699a6` (Build #230). Its descendant `chaos-v3` was at `2421e94573a9b98f76cac488930ff18a89a0ed44` (Build #234). This branch starts from master and includes that existing descendant, preserving the completed Egg Move, learnset, type-hint and FireRed Summary/Growth fixes. It does not use the abandoned Emerald/Hoenn intro project. No public workflow run or release was requested for this partial checkpoint.

## Code present at this checkpoint

- Two setup difficulty values, locked preset on creation and save load, persistent mode/seed gated by a version magic. IronMON uses the existing normal Chaos difficulty profile rather than indexing trainer tables with the new mode values.
- Mode description in setup; IronMON skips editable randomizer/filter pages while retaining seed selection. Full UI traversal still needs verification.
- Seeded starter Pokémon with one held item and an actual usable level-5 damaging move. Hardcore's Oak/quick-start scripts automatically assign slot zero, bypassing choice confirmation. Starter reentry does not regenerate an already awarded main. Quick-start no longer marks the lab rival defeated in IronMON.
- Uniform rank selection within eligible species/item/ability pools. Trainer generation is isolated from curated moves, items, abilities and IV/EV fields, with seeded personality/OT identity. Existing trainer levels are retained. Trainer table identity enters the seed; copied override templates now receive the original trainer identity explicitly.
- Capture replaces the sole main; a local snapshot prevents alias corruption; the previous main is written to protected Retired storage. Provisional badge floors are 10/18/25/32/39/46/52/58/64. Retired history currently retains 30 snapshots and overwrites the oldest after that; the total retirement count persists. This is a bounded history, not an unlimited archive.
- Existing central scripted-gift and egg APIs refuse IronMON acquisition. Individual event dialogue, payment/reward and progression paths have NOT all been audited; do not treat the gift/vendor requirement as complete.
- MGM is forced and reuses existing 31-IV/zero-EV enforcement. Native tests check all six stats. Level caps are bypassed; the EXP command skips wild battles.
- Normal Center authorizations are transient/map-section-bound; persistent bits are spent only when HP, status or PP needs restoration. Hardcore denies healing. Unauthorised centralized party healing is blocked. Direct healing paths and every unique Center identity still need auditing.
- Recovery-tools guard blocks Portable PC/Pokévial access. Medicine, PP recovery, revival, Rare Candy, Mint and EV-item field entry points are blocked. In-battle use was not rewritten. All item paths still need coverage.
- Summary and existing direct free Nature/Ability/Gender setters are blocked. The party relearner offers only level-up moves in IronMON; relearner state changes are gated and current level is enforced. NPC tutor/cost and pre-evolution mappings still need audit.
- Destiny Bond is excluded from the generated IronMON learnset permutation, including its source entries, to preserve the restricted permutation without introducing a duplicate mapping. Other move candidates are deliberately not blanket-banned. Starter guarantee excludes self-sacrifice and conditional attacks from its fallback; these moves are not globally banned.
- The faint command marks the main ended before party restoration. Battle/field/load callbacks route ended runs to a dedicated RUN OVER screen; it saves the terminal state without deleting the save. Screen includes seed, mode, final nickname/level, badges, retirees, trainer victories and play time. A/B returns to title after key release. Native callbacks, rendered UI, flash persistence and reload blocking are verified; an actual Hardcore lab loss also passes using the generated starter/opponent and controlled 1-HP injury. Other loss battles remain untested.
- Existing type icons, discovery rules, effectiveness arrows/immunity calculations were not changed.

## Save and memory findings

`SaveBlock3` grows from 1564 to 1616 bytes; capacity is 1624. The existing field offsets stay unchanged, leaving only 8 bytes. New IronMON state starts at offset 1564 and occupies 52 bytes. The old Chaos records initializer previously cleared the entire tail; it now stops before the IronMON extension. Old saves do not become IronMON simply because appended bytes are zero or 0xFF.

SaveBlock1: 15756/15872 bytes; SaveBlock2: 3884/3968; Pokémon storage: 34144/35712. Tracker observations have NOT been allocated. An appended storage journal is a possible approach, but capacity, SRAM checksums, initialization, old-save migration, run-seed resets and bounded-history behavior must be designed and tested before use. Do not overwrite existing boxes, fusion data, or journey records to claim full tracking.

Current linker usage: EWRAM 252140/262144 bytes; IWRAM 29072/32768; ROM payload 27204996/33554432. Rendering a tracker must respect the remaining RAM and preserve battle callbacks, graphics buffers and input state.

## Verification actually performed

An internal FireRed ROM was compiled with ARM GCC 13.2.1/newlib. It is a development checkpoint, not an accepted playable IronMON release.

Native mGBA checks execute ROM functions and genuine new-game callbacks:

1. `qa_ironmon_core.py`: normal padding does not activate IronMON; record initialization preserves its tail; preset/MGM/no cap; held item and starting attack; starter reentry; central gift refusal; capture pivot/alias/floors; protected Retired box; identical Brock party bytes after global RNG changes; centralized healing denial; full-HP/status/PP Center use costs no token; depleted main heals once; Hardcore denial; actual GBA flash write/load preserves state; end observer and reset isolation.
2. `qa_ironmon_pool.py`: all **1,183 eligible species at three seeds = 3,549 starters** have a functional starting attack, an item, nonzero PP for equipped moves, six 31 IVs and six zero EVs. Seeds: `0x1234abcd`, `1`, `0xffffffff`.
3. `qa_ironmon_startup.py`: actual new-game callback for regular Chaos, Nuzlocke, IronMON Normal and Hardcore; regular modes still enter the lab; Hardcore automatically awards exactly one level-5 starter. Fixture reaches nickname UI, not a completed rival battle.
4. `qa_ironmon_migration.py`: an actual older `.sav` was loaded through both Build #234 and this ROM; SaveBlock1, SaveBlock2, original 1564-byte SaveBlock3 prefix, full Pokémon storage and player party match byte-for-byte. IronMON remains disabled.
5. `git diff --check`: passes.
6. `qa_ironmon_terminal.py`: queued terminal callback finishes flash programming, renders verified screen, preserves main, persists ended state and rejects resume. Short-frame screenshots initially captured the middle of saving; fixture now waits for the input callback. BG control/scroll/palette cleanup was corrected before accepting the screen.
7. `qa_ironmon_world.py`: all eight canonical trainer lists, last-trainer gating, internal warp allowance, Dig/Escape denial, victory unlock, boss counts and 256 seeded non-TM/HM rewards.
8. `qa_ironmon_dungeons.py`: seven area boundaries/prerequisites/completion fixtures; Gym TM award authorization, non-Gym/shop denial, removal and ordinary-mode isolation.
9. `qa_ironmon_singles.py`: Koga Singles setup and simulated Jessie/James victory callbacks preserve sole-main bytes, advance identities and set separate defeat flags. Packed trainer fields use byte access in the harness to avoid unaligned emulator bus-read artifacts. This does not test two played battles.

Fixtures explicitly initialize player/rival name terminators before skipping into new-game callbacks. Missing fixture name initialization caused an early test overflow; this was fixed in the fixture, not represented as a gameplay bug.

These tests do **not** establish a complete playthrough, real Delta verification, every trainer, all acquisition/healing/item routes, actual battle EXP/faint scenarios, every map, or fairness. An actual Hardcore lab loss was played with a controlled 1-HP main. No Brock/Misty playthrough or later-game balance test has been completed. No Delta skin exists yet.

## Remaining acceptance checklist

| Directive sections | Remaining work |
| --- | --- |
| 1–3 Architecture/setup | Full setup UI traversal, mode immutability audit, migration across all supported save versions; preserve regular Chaos/Nuzlocke. |
| 4 Starters | Normal three-choice UI tests, same-seed full reset checks, real lab rival win integration and Normal starter-choice traversal; Hardcore lab loss is verified with a controlled injury. |
| 5 Ownership | Audit storage, party scripts, daycare, ranch, trades, fusion, rentals, gifts/captures; block any recovery/reintroduction/second main; test nickname Yes/No after capture; balance floors against EXP. |
| 6 Gifts/Eggs | Audit every vendor, gift, fossil, prize and story reward; deny entry before charging; retain story flags/progression. |
| 7 Randomization | Verify wild mapping, abilities/natures, trainer mappings, held-item generation, seeded evolution branches and non-regression; mandatory evolution cancellation and inaccessible methods. |
| 8 Move safety | Audit Perish Song/Body, OHKO, self-sacrifice, trapping and immunity combinations; document decisions separately from Destiny Bond. |
| 9 MGM | Actual opposing battle generation/evolutions and all alternate IV/EV routes. |
| 10 EXP | Actual trainer/wild EXP, alternate grinding blocks, progression trainer-yield analysis and floor adjustment. |
| 11 Healing | Every Center map/identity, real nurse scripts, free healing NPCs, scripted/direct restoration, lab/story heal callbacks, held-item/move legitimacy. |
| 12 Items/TMs | Uniform deterministic overworld non-TM pool; central item award paths; Gym-only randomized TM rewards; shop/NPC/hidden/Game Corner filtering; all ability consumables; free tutors/editors; daycare. IronMON now uses a uniform ordinary non-TM pickup pool. Eight Gym scripts issue seeded eligible TM rewards; a transient one-award authorization gates AddBagItem and ordinary shop criteria hide TMs. NPC/Game Corner presentation and all reward routes still need audit. |
| 13 Trainers | Explicit override identity is implemented. Remaining: all trainer/pool paths, all levels/moves/PP/items, reload stability and actual battles. |
| 14 Boss/AI | Brock 3, Misty 3, Surge 4, Erika 4 profile; later roster sizes; improved/classic comparison under same seed; meaningful balance playtests. Initial IronMON sizes now implemented and verified by ROM generation fixtures: 3/3/4/4/5/5/6/6. Regular rosters retain their table sizes. |
| 15 Gyms | Eight Kanto Gym commitments, canonical trainer flags, Leader gating, normal warp/escape denial and victory unlock now implemented; native requirements/size fixtures pass. Remaining: real doors, Saffron/Cinnabar puzzles, scripted warps and save/reload integration. |
| 16 Dungeons | Seven commitments now implemented: Mt. Moon, Rock Tunnel, Tower, Rocket Hideout, Silph, Mansion and Kanto Victory Road. Tower requires Scope; Victory Road requires Badge 8 and Strength HM. Internal region warps allowed; progression exits/story flags/key acquisition complete the area. Native fixtures pass. Remaining: actual rooms/puzzles/story trainers, save/reload and all escape paths. No optional dungeon trainers are forced. |
| 17 Singles | Central Singles flags and consecutive paired-trainer callbacks implemented, with no partner party loading/restoration in IronMON. Silph has a mode branch skipping three-mon selection. Koga setup and Jessie/James callback/identity/defeat-flag/main-integrity fixtures pass. Remaining: actual two-battle transitions, individual map scripts, dialogue/rewards/story flags and all mandatory pairs/multis. |
| 18 Run over | Immediate faint marker, terminal UI, flash save and reload block implemented/fixture-tested. Remaining: other trainer/wild/scripted loss integration, form restoration, failure recovery and reset testing. Emulator save states can restore an earlier RAM/flash snapshot; this ROM cannot prevent that external emulator capability. |
| 19 Tracker | Persistent nonspoiling observations across run; player party support in regular Chaos; polished read-only pages; safe battle/field access/callback restoration; no hidden-info leak; preserve existing indicators. Not implemented. |
| 20 Delta | Inspect official supported inputs/skin schema; choose conflict-free chord; native shortcut; original or available skin graphics; centered TRACKER button; `.deltaskin` packaging/import/Delta checks. No dependency is assumed available. |
| 21–22 Integration | Finish technical audits and internal checkpoints. Publish one complete release only. |
| 23 Tests | Real Brock/Misty progression, representative unfavorable seeds, later controlled saves, save integrity/menu/battle/world/Delta regression and fair EXP/AI/resource balance. |
| 24–26 Delivery | Complete ROM + skin + final commits/build identifier + installation/implementation/testing/limitations reports. Not ready. |

Brendan remains master planning only. Do not add his maps/scripts/trainers/rewards.

## Resume instructions

1. Checkout `ironmon-development`, inspect `git log/status`, read this report and the approved directive. Do not use the old Emerald project or assume the core checkpoint fulfills the release.
2. Build with the repository instructions. In the current workspace ARM tools/newlib live under `/workspace/scratch/3923bd38e6ed/deps/root`; use ARM CPP with `-isystem .../usr/include/newlib` instead of exporting that include path into host tool builds. Materialize the existing title as required, then restore the generated tracked PNG/untracked tilemap before committing.
3. Native QA: compile `ironmon-layout.c` with `-DFIRERED -DMODERN=1 -mthumb -mabi=apcs-gnu -march=armv4t -iquote include`; extract `.rodata` to `$CHAOS_QA_DIR/ironmon-layout.bin`. `ironmon-emulator.c` is the source of the previously built mGBA 0.10 harness used in this session; requires development headers/library to rebuild. Existing headers were unavailable during this session; the known prior compiled harness was reused. Set `CHAOS_ARM_NM`, then run core/pool/startup. Migration additionally requires `CHAOS_BASELINE_ROM_DIR` and `CHAOS_BASELINE_SAVE`.
4. Next verify the real consecutive Singles transition and mandatory story scripts, finish remaining acquisition/healing/item/editor routes and commitment integration tests. Implement tracker save architecture/UI and Delta skin, AI comparison/EXP balance, then Brock/Misty and later integration/playthrough verification. Do not activate half-audited locks or publish an incomplete public build.
5. Current generated ROM/logs/fixtures are scratch-only and reproducible; code, test sources, findings and checklist are committed to the internal branch. The final release must be merged into master only after its acceptance checks pass.

## Continuation checkpoint findings

- Gift/trade/daycare/vendor entry guards now explain the IronMON restriction before menus, payment or party selection in the audited Kanto events. Central gift refusal remains the fallback. All acquisition/service routes are not yet covered (Game Corner/custom rewards/facilities remain an audit priority). Ordinary and portable PC access is blocked; Retired history needs a read-only tracker page.
- The existing HM QoL script supplies a field-effect actor from slot zero when the HM is owned and the badge requirement is met. Dungeon prerequisites reuse this mechanism; no HM helper is created.
- Mandatory evolution cancellation is disabled in IronMON; alternate evolution-method accessibility is not yet fully verified.
- The type-discovery/effectiveness code is untouched.
- Initial read-only tracker overlay is implemented for owned Pokémon in regular Chaos and IronMON, with nature colors, stats, moves/PP, current owned battle typing/ability, Chaos Rating and badges. Native overworld and battle action/move-selection restoration tests pass. Persistent opponent observations, history, coverage and comprehensive tracker acceptance remain unfinished. Storage has only 1,568 unused bytes; do not silently call a small evicting journal comprehensive run history.
- Original Delta skin build script produces a supported L+Select mapping, with TRACKER centered immediately below the screen in iPhone/iPad portrait/landscape layouts. Package/schema/geometry checks pass. Actual Delta import/touch verification remains pending.
- No complete public build has been published. Continue on this internal branch.

### Additional lifecycle/AI verification

- `qa_ironmon_lab_loss.py` starts Hardcore through the real new-game callback, declines nicknaming with B, walks down across the lab rival trigger, lowers the generated main to 1 HP before battle initialization, then sends ordinary A inputs until the battle causes fainting. The terminal screen finishes saving; the main remains fainted and the ended bit survives flash reload. This is a controlled loss test, not an unbiased seed-balance run.
- `qa_ironmon_ai.py` compares the existing smart flags with the basic three-heuristic (bad-move/viability/KO) FireRed-style approximation while retaining identical trainer bytes. `aiProfile=1` is an internal fixture switch; no player menu exposes it. New-game/load preset enforcement resets it to smart (`0`). It is not an exact port of retail FireRed AI, nor a completed balance evaluation. Regular Chaos ignores this IronMON field. `GetAiFlags` is exported for this comparison.
- A capture now explicitly refuses a main that already has zero HP before a battle-end observer runs, preventing a late capture from rescuing a lost run. Native core verifies this.
- IronMON's party relearner no longer requires the regular Chaos post-Brock training unlock. Current-level randomized level-up restrictions remain; actual early menu traversal still needs checking.
- RUN OVER footer spacing was corrected and minutes are zero-padded.

Latest internal ROM SHA-256: `f35281f1e7c31ccf8c4c2e1263e83da019ac1dcff8dd27de40d3cb8a96ba84f9`. This identifier does not imply a public workflow build or release.


### Played story battles and service-isolation continuation

- Route 4's Jessie/James script still required two conscious Pokemon before the engine could convert the battle. IronMON now bypasses only this entry requirement; ordinary Chaos retains its Double Battle check.
- `qa_ironmon_played_pair.py` plays both Route 4 and (`--silph`) Silph partner-conversion scripts with ordinary A inputs. Both opponents fight separate Singles battles, distinct trainer flags persist, the original blast-off/result dialogue completes, the story variable changes, one main remains, and movement returns. Combat is deliberately controlled (level-100 main, enemy HP/type/ability fixture adjustments), with no injected victory or end callback. These are transition/story tests, not unbiased balance tests or a complete playthrough.
- `qa_ironmon_played_exp.py` plays a controlled wild victory and trainer victory. At 30,000 starting EXP, the wild victory retains 30,000; the trainer victory awards EXP (30,419 in this seed). It exercises real attack/faint/EXP commands. A first level-5 fixture lost and returned through RUN OVER; its replacement uses a higher-level main to isolate EXP behavior. Neither is an early-game balance result.
- Arcade Pokemon prizes are blocked before selecting/nicknaming/paying, with central replay guards. Arcade categories retain only Battle Items in IronMON, preserving their original category ID (2); TM categories and stale purchases are rejected without spending Coins. Regular Chaos retains all three categories.
- Four Island daycare and the previously missed Cinnabar Tangela trade now have entry guards. Central daycare deposit/withdraw/egg and trade replacement APIs refuse IronMON. The trade-scene fallback resumes its implicit waiting script through a deferred task rather than synchronously, avoiding a waitstate freeze. Cable/Union Room/Mystery Gift entries are guarded. Fusion and unfusion are blocked at the item entry and party execution paths to prevent a second usable Pokemon.
- IronMON hides PC/Pokevial, training and free editor entries from their applicable drawers. The training script is guarded; R+Start cannot open the cheat/debug menu in IronMON. Regular Chaos access remains unchanged. Native fixture writes remain available for controlled later-map testing.
- Two Island's level-up Move Relearner has a free IronMON path without Mushroom costs. The existing party relearner remains available from the beginning. Actual NPC tutoring traversal is still untested.
- Ordinary wild generation now receives a seeded legal held item, a legal shuffled ability slot and MGM, including legendary/script-created wild Pokemon using CreateWildMon. The common slot selector is also used by starters/trainers. Existing trainer generation bytes retain the same selection algorithm.
- DexNav IronMON searches hide move/ability/item/potential previews, do not inject Egg Moves or bonus items/slots, and do not increase encounter level with chain bonuses. Encounter species/method selection remains available. The actual wild is created through ordinary wild generation. The existing opponent type/effectiveness implementation is untouched. Rendered DexNav secrecy/search integration still needs testing.
- `qa_ironmon_services.py` checks both modes' central daycare/trade/arcade guards preserve all party/storage bytes and Coins; it also checks the deferred trade rejection task resumes a stopped script. This does not establish every NPC presentation.
- `qa_ironmon_wild.py` checks representative wild species/seeds in both modes for legal randomized held item and ability slot, MGM, permitted moves, idempotent preparation and an exact no-op in regular Chaos. An attempted private DexNav-function call was unavailable because the compiler inlined it; no success is claimed for that attempted test. HUD tests remain pending.
- Latest startup checks pass in regular Chaos, Nuzlocke and both IronMON modes. The 3,549-starter species/seed pool passed again after slot selection changed. The real old-save migration comparison passed again before the final deferred trade-task-only change; no save structures or offsets changed in this continuation.

Remaining scope is still substantial: tracker persistence/rendering/input safety, Delta skin, full healing/acquisition/evolution/world audits, EXP curve/AI fairness, actual Brock/Misty playthrough and later integration. No complete IronMON release or public workflow build exists. Continue implementation from this checkpoint without treating it as acceptance.


## Tracker and free-healing follow-up checkpoint

- The overlay initially needed about 34 KB while a real Singles battle left only 20,332 free heap bytes. It now renders text in two-tile-high strips and snapshots only the tile region it overwrites. Unchecked allocation refuses access safely on insufficient memory; no fatal allocation path. It borrows one unused window and restores tile memory, map memory, palette, GPU registers, callbacks and main callback state. Existing tasks/sprites/BG allocation are not rebuilt or freed.
- The raw L+Select shortcut is checked only in stable overworld controls and battle action/move selection. It consumes the chord when opening is denied, including L=A input mode. Opening is denied during fades, pending BG DMA, active text printing, link/recorded battles and memory/window-slot exhaustion. B must release before returning to gameplay; held page buttons do not repeat. RNG VBlank burning is paused while the overlay owns callbacks.
- `qa_ironmon_tracker_overlay.py`: native field restoration in IronMON and regular Chaos; low-heap denial; genuine wild-battle action and move menus with L=A; unchanged battle structure/mons, party/save data, tasks, windows and borrowed VRAM after close. This is not opponent-journal, Doubles, device or full-playthrough acceptance.
- `tools/chaos/delta/build_skin.py`: original PNG artwork, six device/orientation representations, `.deltaskin` archive, centered immediate-below-screen tracker mapping, correct GBA screen ratio, supported inputs, in-bounds controls and no control/game-screen overlap. JSON/asset/archive checks and artwork inspection pass. Native L+Select game-side test passes. Actual Delta import/touch testing remains unavailable/pending.
- Mom, Pokémon Tower purified-zone and Ember Spa script paths now explain the IronMON free-healing restriction before playing a false healing sequence. The two zone triggers still set their temporary entered-zone state, allowing ordinary exit/reentry. Regular Mom healing is unchanged. The walking care-package award function independently refuses IronMON even if a script changes the setting.
- `qa_ironmon_healing_events.py`: both modes execute all three refusal scripts with ordinary confirm input, retain byte-identical Pokémon data and return control. Regular Mom restores HP. Four-mode new-game callback tests pass again.
- This checkpoint changes no save structure or Pokémon storage offset. Compiled memory: EWRAM 252,144 bytes; IWRAM 29,072 bytes; ROM 27,208,364 bytes. No public release/workflow has been dispatched. Remaining acceptance work still includes persistent nonspoiling observations, complete acquisition/healing/evolution/world audits, real Brock/Misty progression and seed/EXP/AI balance, later-game controlled progression, save migration recheck, and Delta device verification.


## Starter, evolution and EXP audit follow-up

- `qa_ironmon_normal_choices.py` enters the actual Normal new-game callback, then each lab ball interaction script. All three options award the selected seeded species (seed 24680: 412, 363, 359), with a level-5 usable attack and held item. Ordinary A accepts and B declines nicknaming; rival-pick dialogue returns control. Repeated preview calls after RNG perturbation and repeated award calls do not reroll or replace the main. This is an event fixture, not walking the entire intro.
- `qa_ironmon_evolutions.py` checks 1,605 enabled evolutionary mappings across seeds 1, 123456 and 0xffffffff. Normal/Hardcore mappings agree, RNG changes do not reroll targets, level thresholds are attainable/stable, and there are no canonical BST regressions in this sample. The existing random-evolution branch normalizes original stone/friendship/trade/time/branch methods into seeded level thresholds (18–24 / 28–32 / 34–40 depending on stage); cancellation is already blocked by the IronMON scene guard. Evolution animations, random ability/move refresh and post-evolution flash reload still need explicit gameplay tests.
- `qa_ironmon_exp_audit.py` follows map object references into shared trainer scripts (route trainers are often absent from their own map script files), generates actual seeded teams, reads their species EXP yields and levels, and uses the engine's integer experience-scaling table. Eight seeds × six growth curves × two conservative profiles = 96 modeled outcomes. `ironmon-exp-audit.json` records identities/results and model limitations.
- With all listed initial optional trainers, the sample reaches Brock at levels 11–16 (median 14), and Misty at 26–31 (median 29). The lab/Gym-only EXP lower bound gives 7–13 (median 10) and 13–18 (median 16). The lower bound is not a navigable route: required overworld/story battles also award EXP. Northern optional routes are included before Misty in the optional profile. All outcomes assume victories, no pivots, no evolution growth-curve changes and no optional EXP modifiers. Do not report these as wins, fairness or a completed EXP/playthrough audit. Floors remain provisional and unchanged.
- Actual old-save migration rechecked against the #234 baseline: Save1/Save2, original Save3 prefix, entire Pokémon storage and party match byte-for-byte, and IronMON remains off.
- Owned active-battle tracker pages now read actual battle stats/moves/PP/species/types/ability, including Transform's five-PP maximum. Partner Pokémon cannot be mistaken for a regular owned party member: lookup requires the player's actual party pointer. Stat stages and ability-based effective multipliers are not yet displayed. Opponent journal/persistence remains unfinished.


### Compressed overlay backup and regular Doubles

- Temporary graphics are now backed up losslessly as halfword runs/literal spans, with an exact size pass, bounded encoding and safe allocation refusal. A full uncompressed second character buffer is no longer reserved. No save-data compression or save-format change is involved. Long-run, worst-case literal and mixed VRAM fixtures each restore byte-for-byte in the native test.
- The native tracker test also enters regular Chaos Koga as a genuine four-battler battle, preserving ordinary Doubles. L+Select works at action and move selection with L=A configured; all four battle mons, battle structure, borrowed graphics, windows, tasks, party/save fields and callbacks remain unchanged after closing. IronMON Singles still passes. Controlled battle-only stat/move/PP fixtures render Attack 321 and Tackle 3/35 rather than stored-party values; screenshots were inspected.
- Current compiled ROM SHA256: `da818d127432e4eeaf5e3f77aa7c16ab1e44f39eada61b4642451df8b8261298`. EWRAM 252,144; IWRAM 29,072; ROM 27,209,268 bytes. This is an internal compilation checkpoint, not the accepted complete IronMON release or a public playtest workflow build.
