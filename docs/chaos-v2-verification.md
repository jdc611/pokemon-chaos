# Chaos FireRed V2 playtest — October 8, 2026

One comprehensive playtest build from `jdc611/pokemon-chaos`, based on master `edd207acd8df4574c97e1fddcd630c1ade172adf`. The unfinished Chaossal endgame and deferred Viridian Forest expansion were not implemented.

**ROM:** `/workspace/scratch/3923bd38e6ed/Chaos_FireRed_V2_Playtest.gba` (33,554,432 bytes).

**SHA-256:** `604f6f65afc33fce13bed845a03e3367d2ce17a14c2ded2124fd0128b0d844c1`.

**Important unresolved report:** the intermittent black screen at the Pokémon Center outside Mt. Moon did not reproduce in 24 native entry/heal/exit cycles. Its original cause remains unknown; this build must not be described as proving that issue fixed. No forced restart workaround was added.

## Completed implementation, by directive section

| Section / items | Implemented behavior |
| --- | --- |
| 1 — Stability (1–4) | Full-party capture initializes the actual party order and safely restores battle graphics after nickname entry. Six real party slots are displayed; the selected original is stored before replacement. Naming/party allocation guards added. First-eligible Nuzlocke full-party captures tested on Route 9 and later routes. Fishing/surfing multichoice menus reload native frame graphics/palettes. Mt. Moon Center investigated and stress-tested; its reported intermittent failure remains unconfirmed. |
| 2 — Visuals (5–11) | Correct Scyther normal palettes without changing shiny art; Play Style NEXT fits; Records attendants repositioned across Centers; Viridian patch underlay cleaned while preserving shape; Pewter Ranch blocker removed; Pewter encounter tiles use standard tall grass; Train to Cap keeps the field visible behind its native menu. |
| 3 — Gameplay / QoL (12–18) | EXP All defaults ON, has an Options toggle, and distributes participant/full and eligible nonparticipant/half EXP plus normal-mode EVs. Fainted Pokémon receive none and strict caps award zero EXP. MGM maintains 31 IVs/zero EVs and blocks EV gains. Nuzlocke nickname decline/blank-name paths guarded; party RENAME added. Selected-mon RELEARN provides LEVEL UP / TM / EGG MOVES after the existing unlock. Ability changer shows current/all legitimate slots, descriptions, and disables filter-incompatible choices. Native TM details show mechanics and six party icons with compatibility/known markers. Fishing ignores premature A, automatically hooks, and retains failed bites. Surge's first switch survives wrong guesses and re-entry. |
| 4 — Evolution accessibility (19) | Complete compiled evolution-table audit; 64 source-table changes. Trade-with-item evolves through Link Cable plus the required held item. Friendship conditions replaced with appropriate levels and day/night conditions. Foreign-region-only branches get deliberate item alternatives. Hard-to-represent special conditions retain their original method with a Cable alternative. Celadon sells the supported stones, Cable and required evolution items, including Gimmighoul coins. Protected Mega distribution remains separate. |
| 5 — Evolution & Growth (20–21) | Separate native Summary page with current/next species, next-form icon, actual requirement, branch browsing, final-stage label, growth rate, next-level EXP and egg groups. Supports all 63 Milcery branches. Current-stage-only estimated Chaos Rating uses actual randomized stats/ability, current-form legitimate level/TM/egg movepool, IVs, typing/distribution and curated synergies; independent of level and future forms. Rating appears only here. |
| 6 — Mystery eggs (22) | Pewter baby/early pool (7 species), Vermilion supported starters (27), Celadon regional forms (8). Each costs $5,000, once per vendor/save, random at purchase, no preview, gift encounter semantics, safe full-party PC delivery. Failed delivery does not charge or mark purchased. Nuzlocke hatch nickname guard included. |
| 7 — Care packages (23) | Exactly three automatic milestones: north Route 2, east Route 4 and north Route 10. Named supported Balls/stones and exact quantities implemented; Candy tops off to 999. Package 1 includes 16 supported healing/status berries and 18 resistance berries. Paginated native notification; transactional inventory award; one-time receipts. Disabled milestones are recorded and can be claimed when returning to that area with the option enabled. Old interactive bundle awards disabled. |
| 8 — Held berries (24) | Original consumed held berry restored after battle only when safe. Once-per-battle consumption state survives switching/Harvest; no midbattle regeneration. Knocked-off/stolen berries are not refunded and changed held items are not overwritten. Bag-used berries retain ordinary consumption. |
| 9 — Oak aides (25) | All five species-count checks removed, original rewards/locations/one-time flags retained, distinct short jokes added. |
| 10 — V1 protection (26) | Existing filters, randomizers, progression unlocks, Nuzlocke/EZ Catch, trainer RUN restriction, Jessie/James, doubles indicators, Gym teams/rewards, TM/Mega distribution, weather/terrain, Ranch, DexNav, PokéRider, healing and saved records preserved and regression-tested as described below. |

## Verification and evidence

The final ROM compiled successfully and booted in native mGBA. All **21 host suites** and **27 final native suites** passed. Two supplementary native suites passed for all 63 Milcery evolution branches and actual Ranch/Train-menu entry paths. The final ROM above includes the last strict-cap controller fix; the foundation fixtures were regenerated before the final suite.

Results and UI screenshots: [native V2 evidence](../tools/chaos/native/results/v2/). Reproduction instructions and harness: [native README](../tools/chaos/native/README.md). The manifest lists each final native suite and exit status; host results are in `host-checks.log`.

### Native gameplay / UI exercised

- All six full-party replacement slots, six distinct species/personalities, correct stored original, untouched other identities, and cancel-to-PC. Fresh Nuzlocke full-party captures on Routes 9/10/12 with naming, spent encounter and Ball consumption.
- EXP All ON/OFF, full/half/no EXP, normal EV allocation, MGM zero EVs, and participants/nonparticipants already at cap receiving exactly zero EXP.
- Brock actual loss/retry for Normal/Hard with MGM ON/OFF. Jessie/James actual loss/retry/win, automatic departure and no retrigger. Outcome fixtures adjust HP/moves to reach engine paths; they do not establish difficulty balance.
- Train to Cap whole party, one Pokémon, cancel, chained Caterpie evolutions and dead-Nuzlocke skip; native field-background menu. Post-Brock unlock, rival PokéRider reward without Fame Checker, starter plus five Balls.
- Nuzlocke opportunity consumption, evolutionary dupes, shiny exception, gifts and independence from AI/EZ Catch; trainer RUN blocked; legal EZ Catch at full HP while preserving other restrictions.
- Native ability screen, Options, Play Style, TM detail/return, Summary/final-stage/branch pages, Center placement, city habitat visuals and Pewter Ranch door/return. All 82 TM descriptions were checked for existence and native text fit; evolution requirement text fits three lines.
- Actual Surge first switch, wrong second, leave/re-enter, correct second. All five aide reward scripts with empty Pokédex, and no duplicate rewards.
- Three vendors: charge-on-success, once-only, insufficient funds, full-party PC delivery, and full-party/all-boxes-full rejection without charging or setting receipt.
- Three package exact contents, one-time receipts, Candy 999, OFF-to-ON deferred eligibility. Full-bag transaction rollback is code-reviewed rather than separately stress-tested for every package.
- Actual held-Oran activation/removal, repeat activation prevention, safe postbattle refund, idempotence, changed held item preservation, and Knock Off/theft exclusions.
- Five trials per rod with early A inputs: Old Rod 5 bites/0 misses, Good Rod 3/2, Super Rod 4/1; actual automatic encounter transition. This small sample confirms behavior, not precise odds calibration.
- Representative native Cable, trade-held-item, stone and regional evolution targets. Compiled table audit covers 673 entries. All 63 Milcery branch displays cycled through and wrapped correctly. Rating calibration checks level invariance and representative low/mid/high examples.
- Weather/terrain timed/permanent indicators; immediate doubles target effectiveness and independent spread evaluations; seven actual Gym victory/reward continuations and repeat prevention; 15 overworld Mega events; Hall of Fame summary and ongoing records.
- Native flash save/load of the unchanged 1,564-byte SaveBlock3 and settings/records. Twenty-four Mt. Moon Center entry/heal/exit cycles, Records use and emulator-state reload.

### Host/source audits

Existing randomizer/stat/filter, arcade economy/prize transactions, cosmetics/Ranch, menus, Nuzlocke and reward suites passed. BST test covers 25,200 profiles, preserves Shuffle totals, and measured the intended roughly 5% extreme Random BST rolls. Evolution changes and rating examples are recorded in [evolution audit](v2-evolution-audit.json), [table changes](chaos-v2-evolution-changes.json) and [rating calibration](v2-rating-calibration.json).

## Unresolved issues and limits

1. **Mt. Moon exterior Center black screen:** not reproduced; root cause unconfirmed. It could still recur in Delta. No claim of a definitive fix.
2. No full playthrough or iOS/Delta background/resume test was performed. Repeated-cry audio symptoms were not independently verified with audio playback. Native interface/storage tests passed, but do not prove every intermittent emulator symptom eliminated.
3. Not every evolution family was manually evolved. Complete table/item audits plus representative engine tests support accessibility. Every mystery-egg species was validated against supported data; not every egg was hatched in live gameplay.
4. Berry behavior is generic and representative native tests passed; every berry in doubles and every complex Trick/Recycle/item-transfer combination was not exhaustively playtested.
5. Ability filter combinations and permanent Mega ratings are implemented against actual current-form data but not exhaustively exercised form-by-form. Chaos Rating is an estimate, not a definitive competitive ranking.
6. Save layout is preserved and native roundtrip passed; a player's pre-V2 save was not supplied for migration testing. Keep a backup before the playtest.

## Fallbacks and scope

The preferred party-swap flow was retained because the six-slot regression paths passed; automatic-only PC storage fallback was not needed. Difficult species-specific evolution requirements retain native identity with practical single-player item alternatives. Supported items were used without overwriting unrelated IDs. No new infinite Candy mechanic, unrelated story work, or Forest expansion was added.

The ROM is a **V2 playtest**, ready for the requested full playthrough QA with the unresolved Center report explicitly carried forward.
