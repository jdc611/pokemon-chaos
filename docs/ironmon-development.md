# IronMON internal development checkpoint — October 10, 2026

**Not a release. The complete IronMON directive is not implemented. Do not merge or publish this branch as a playable IronMON release.**

Repository: `jdc611/pokemon-chaos`. Target branch: `master`. Internal branch: `ironmon-development`.

Master was at `dffbcb88c6431262284e4c043221919b437699a6` (Build #230). Its descendant `chaos-v3` was at `2421e94573a9b98f76cac488930ff18a89a0ed44` (Build #234). This branch starts from master and includes that existing descendant, preserving the completed Egg Move, learnset, type-hint and FireRed Summary/Growth fixes. It does not use the abandoned Emerald/Hoenn intro project. No public workflow run or release was requested for this partial checkpoint.

## Code present at this checkpoint

- Two setup difficulty values, locked preset on creation and save load, persistent mode/seed gated by a version magic. IronMON uses the existing normal Chaos difficulty profile rather than indexing trainer tables with the new mode values.
- Mode description in setup; IronMON skips editable randomizer/filter pages while retaining seed selection. Full UI traversal still needs verification.
- Seeded starter Pokémon with one held item and an actual usable level-5 damaging move. Hardcore's Oak/quick-start scripts automatically assign slot zero, bypassing choice confirmation. Starter reentry does not regenerate an already awarded main. Quick-start no longer marks the lab rival defeated in IronMON.
- Uniform rank selection within eligible species/item/ability pools. Trainer generation is isolated from curated moves, items, abilities and IV/EV fields, with seeded personality/OT identity. Existing trainer levels are retained. Trainer table identity enters the seed; copied override templates currently fall back to their template hash and need explicit identity propagation.
- Capture replaces the sole main; a local snapshot prevents alias corruption; the previous main is written to protected Retired storage. Provisional badge floors are 10/18/25/32/39/46/52/58/64. Retired history currently retains 30 snapshots and overwrites the oldest after that; the total retirement count persists. This is a bounded history, not an unlimited archive.
- Existing central scripted-gift and egg APIs refuse IronMON acquisition. Individual event dialogue, payment/reward and progression paths have NOT all been audited; do not treat the gift/vendor requirement as complete.
- MGM is forced and reuses existing 31-IV/zero-EV enforcement. Native tests check all six stats. Level caps are bypassed; the EXP command skips wild battles.
- Normal Center authorizations are transient/map-section-bound; persistent bits are spent only when HP, status or PP needs restoration. Hardcore denies healing. Unauthorised centralized party healing is blocked. Direct healing paths and every unique Center identity still need auditing.
- Recovery-tools guard blocks Portable PC/Pokévial access. Medicine, PP recovery, revival, Rare Candy, Mint and EV-item field entry points are blocked. In-battle use was not rewritten. All item paths still need coverage.
- Summary and existing direct free Nature/Ability/Gender setters are blocked. The party relearner offers only level-up moves in IronMON; relearner state changes are gated and current level is enforced. NPC tutor/cost and pre-evolution mappings still need audit.
- Destiny Bond is excluded from the generated IronMON learnset permutation, including its source entries, to preserve the restricted permutation without introducing a duplicate mapping. Other move candidates are deliberately not blanket-banned. Starter guarantee excludes self-sacrifice and conditional attacks from its fallback; these moves are not globally banned.
- Battle-end observer records a main's zero HP and trainer victories. **Immediate faint termination, RUN OVER UI, save flushing and resume prevention are NOT implemented.**
- Existing type icons, discovery rules, effectiveness arrows/immunity calculations were not changed.

## Save and memory findings

`SaveBlock3` grows from 1564 to 1616 bytes; capacity is 1624. The existing field offsets stay unchanged, leaving only 8 bytes. New IronMON state starts at offset 1564 and occupies 52 bytes. The old Chaos records initializer previously cleared the entire tail; it now stops before the IronMON extension. Old saves do not become IronMON simply because appended bytes are zero or 0xFF.

SaveBlock1: 15756/15872 bytes; SaveBlock2: 3884/3968; Pokémon storage: 34144/35712. Tracker observations have NOT been allocated. An appended storage journal is a possible approach, but capacity, SRAM checksums, initialization, old-save migration, run-seed resets and bounded-history behavior must be designed and tested before use. Do not overwrite existing boxes, fusion data, or journey records to claim full tracking.

Current linker usage: EWRAM 250040/262144 bytes; IWRAM 29072/32768; ROM payload 27196968/33554432. Rendering a tracker must respect the remaining RAM and preserve battle callbacks, graphics buffers and input state.

## Verification actually performed

An internal FireRed ROM was compiled with ARM GCC 13.2.1/newlib. It is a development checkpoint, not an accepted playable IronMON release.

Native mGBA checks execute ROM functions and genuine new-game callbacks:

1. `qa_ironmon_core.py`: normal padding does not activate IronMON; record initialization preserves its tail; preset/MGM/no cap; held item and starting attack; starter reentry; central gift refusal; capture pivot/alias/floors; protected Retired box; identical Brock party bytes after global RNG changes; centralized healing denial; full-HP/status/PP Center use costs no token; depleted main heals once; Hardcore denial; actual GBA flash write/load preserves state; end observer and reset isolation.
2. `qa_ironmon_pool.py`: all **1,183 eligible species at three seeds = 3,549 starters** have a functional starting attack, an item, nonzero PP for equipped moves, six 31 IVs and six zero EVs. Seeds: `0x1234abcd`, `1`, `0xffffffff`.
3. `qa_ironmon_startup.py`: actual new-game callback for regular Chaos, Nuzlocke, IronMON Normal and Hardcore; regular modes still enter the lab; Hardcore automatically awards exactly one level-5 starter. Fixture reaches nickname UI, not a completed rival battle.
4. `qa_ironmon_migration.py`: an actual older `.sav` was loaded through both Build #234 and this ROM; SaveBlock1, SaveBlock2, original 1564-byte SaveBlock3 prefix, full Pokémon storage and player party match byte-for-byte. IronMON remains disabled.
5. `git diff --check`: passes.

Fixtures explicitly initialize player/rival name terminators before skipping into new-game callbacks. Missing fixture name initialization caused an early test overflow; this was fixed in the fixture, not represented as a gameplay bug.

These tests do **not** establish a complete playthrough, real Delta verification, every trainer, all acquisition/healing/item routes, actual battle EXP/faint scenarios, every map, or fairness. No Brock/Misty playthrough or later-game balance test has been completed. No Delta skin exists yet.

## Remaining acceptance checklist

| Directive sections | Remaining work |
| --- | --- |
| 1–3 Architecture/setup | Full setup UI traversal, mode immutability audit, migration across all supported save versions; preserve regular Chaos/Nuzlocke. |
| 4 Starters | Normal three-choice UI tests, same-seed full reset checks, real lab rival win/loss integration. |
| 5 Ownership | Audit storage, party scripts, daycare, ranch, trades, fusion, rentals, gifts/captures; block any recovery/reintroduction/second main; test nickname Yes/No after capture; balance floors against EXP. |
| 6 Gifts/Eggs | Audit every vendor, gift, fossil, prize and story reward; deny entry before charging; retain story flags/progression. |
| 7 Randomization | Verify wild mapping, abilities/natures, trainer mappings, held-item generation, seeded evolution branches and non-regression; mandatory evolution cancellation and inaccessible methods. |
| 8 Move safety | Audit Perish Song/Body, OHKO, self-sacrifice, trapping and immunity combinations; document decisions separately from Destiny Bond. |
| 9 MGM | Actual opposing battle generation/evolutions and all alternate IV/EV routes. |
| 10 EXP | Actual trainer/wild EXP, alternate grinding blocks, progression trainer-yield analysis and floor adjustment. |
| 11 Healing | Every Center map/identity, real nurse scripts, free healing NPCs, scripted/direct restoration, lab/story heal callbacks, held-item/move legitimacy. |
| 12 Items/TMs | Uniform deterministic overworld non-TM pool; central item award paths; Gym-only randomized TM rewards; shop/NPC/hidden/Game Corner filtering; all ability consumables; free tutors/editors; daycare. Current Chaos weighted item pool and TM pickups remain unchanged. |
| 13 Trainers | Explicit identity for overrides/pools, roster sampling, all trainers, all levels/moves/PP/items and reload stability. |
| 14 Boss/AI | Brock 3, Misty 3, Surge 4, Erika 4 profile; later roster sizes; improved/classic comparison under same seed; meaningful balance playtests. Existing rosters currently have original sizes. |
| 15 Gyms | Per-map commitment, all Gym trainer defeat flags, Leader gating, exits/escape/warps, persistence and puzzle safety. Not implemented. |
| 16 Dungeons | Per-area prerequisites/completion, internal floor warps, required trainers only, HMs/key items, exit/escape restrictions and softlock tests. Not implemented. |
| 17 Singles | Audit/convert all required doubles/multis/partners with separate opponent identity, flags/rewards/dialogue. Not implemented. |
| 18 Run over | Immediate main faint hook, terminal screen/statistics, actual save persistence, loading ended runs safely, reset protection and honest save-state limits. |
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
4. Next implement the complete run lifecycle, then central item/gift/healing and ownership safeguards. Audit and test all maps before commitment locks. Finish trainer profiles/Singles, tracker and skin, then integration/playthrough verification. Do not activate half-audited locks or publish an incomplete public build.
5. Current generated ROM/logs/fixtures are scratch-only and reproducible; code, test sources, findings and checklist are committed to the internal branch. The final release must be merged into master only after its acceptance checks pass.
