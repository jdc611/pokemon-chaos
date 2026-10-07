# October 6 stabilization directive — verified playtest build

Completed October 7, 2026. All 26 requested implementation items are included in one native FireRed ROM. The excluded late-game story remains outside this batch.

ROM SHA-256: `c904e825ec912c5ae43a400804133192ab71f9666592b4b95ea775bd24e78907`.
ROM is padded to 32 MiB; linked ROM usage is 27,167,432 bytes. EWRAM usage is 248,164 / 262,144 bytes; IWRAM is 29,072 / 32,768. Expanded SaveBlock3 uses 1,564 bytes within its existing 1,624-byte reservation. Existing Pokemon structures and quest variable allocations are preserved.

## Implementation and evidence

| # | Result | Verification |
|---|---|---|
| 1 | Ability choices sorted alphabetically, including custom abilities; special choices pinned first. | Host menu ordering and selection checks. |
| 2 | Post-Brock aide unlocks Train to Cap and Move Relearner, acknowledges existing shoes and relays Mom's greeting. | Actual aide script and saved unlock in mGBA. |
| 3 | PokéRider unlocks at the Cerulean rival event before Nugget Bridge. | Actual rival battle, victory, gift, field control and no Fame Checker item. |
| 4 | Viridian grass right of Center/below Mart, separate early pool and Nuzlocke area. | Native tile behavior, real encounter generation and area consumption. |
| 5 | Saved named-area Nuzlocke allowances, full-family dupes, gifts/shinies exempt; starter plus five Balls activates early tracking. | Production family fixtures; native consumed/caught/failed/duplicate/shiny checks and save/reload. |
| 6 | Free party/individual training to displayed cap, no move prompts, dead mons skipped, chained evolutions retained. | Caterpie → Metapod → Butterfree; party/one/cancel/above-cap/dead tests. |
| 7 | Pewter flower habitat with its own early pool and area. | Native flower behavior and real encounter generation/consumption. |
| 8 | Records attendant in Centers, ongoing run pages, saved League snapshot and automatic Hall of Fame summary. | Actual attendant navigation, native FireRed Hall of Fame, ten summary pages, final-team pages and flash persistence. |
| 9 | Existing Move Relearner gated behind aide unlock; move functionality retained. | Existing relearner checks and native unlock. |
| 10 | Battle cleanup/retry stabilization and regression coverage. | Genuine Brock loss → healing → re-entry → actionable retry with Normal/Hard and MGM off/on. |
| 11 | Themed anti-sweep tools across Gym teams and early Mega rematches. | Team/source checks; actual seven reward battles using Hard AI. Balance needs playtesting. |
| 12 | Obsolete single-use TM dialogue corrected. | Native script text audit. |
| 13 | Terrain HUD alongside weather, immediate refresh, duration or infinity and inactive hiding. | Native timed/permanent/cleared HUD screenshots reviewed. |
| 14 | Competent Normal AI and stronger Hard AI layered over Basic; no selected-player-move access. | Existing AI behavior/heuristics and source input-boundary checks; Hard-AI native battles. |
| 15 | Independent optional, curated one-time Care Packages at progression milestones. | Native OFF/ON, first-badge contents and repeat prevention; host transaction/dead-heal checks. |
| 16 | Universal trainer RUN block with standard message. | Actual battle action-menu RUN leaves trainer battle active. |
| 17 | Independent EZ Catch guarantees legal throws and consumes the Ball. | Actual full-HP Mewtwo caught with one Poké Ball; used Nuzlocke area still blocked. |
| 18 | Clean Jessie/James double-battle entry. | Actual approach script enters native double battle without blank-screen hang. |
| 19 | Doubles target-dependent hints and independent spread hints. | Native target selection toggle; different results per opponent; reviewed dual-arrow rendering. |
| 20 | Jessie/James automatic defeat/blast-off/completion and clean loss retry. | Actual loss/whiteout/no completion → retry/win → automatic exit/control → no re-entry trigger. |
| 21 | Mach Punch and Vacuum Wave reusable TM gifts with physical/special banter. | Actual gifts, saved one-time receipt and repeat checks. |
| 22 | Fame Checker reward replaced by PokéRider. | Same actual rival-event test as #3; Bicycle remains separate. |
| 23 | Clear missing/insufficient rod feedback across fishing entry paths. | Native all three rod-script missing-rod feedback and host path checks. |
| 24 | Complete TM acquisition audit and progression replacements; no ordinary duplication of Arcade's 28 TMs. | Reachable native-script catalog and regression uniqueness checks. |
| 25 | Complete supported Mega Stone distribution audit, preserving intentional rewards. | 94 items classified; all 15 actual overworld item-event pickups tested; shop/reward disjointness checked. |
| 26 | Protected Gym/rematch stones work after actual victories and cannot be claimed twice. | Seven real battle/script/reward paths plus full-bag retry and saved claims. |

## Regression results

All 21 host suites passed against final source. Native mGBA suites passed: boot, field/events, Jessie/James, training, core rules, battle/capture/HUD, doubles, Brock retry matrix, item/reward/save transactions, seven Gym/rematch battles, Hall of Fame, early progression and Cerulean rival/PokéRider.

Protected battle rewards tested: Brock rematch → Steelixite; Misty rematch → Gyaradosite; Surge rematch → Manectite; Koga → Beedrillite; Sabrina → Alakazite; Blaine → Pyroarite (approved); Giovanni → Garchompite. Arcanite and Oak's post-Sabrina Nidokingite remain protected in their existing systems.

The TM catalog contains 82 supported TMs, 76 with deliberate acquisition methods. Six retired original rewards remain random-item-pool-only, explicitly documented in the TM audit. The Mega catalog contains 67 Marsh Badge shop stones, 15 thematic overworld stones, nine protected rewards and three deliberate reservations. Scizorite remains unassigned pending the Scyther system; Mewtwonite X/Y remain reserved for the excluded Giovanni/Mewtwo story. These three were not silently placed or duplicated.

See [TM audit](chaos-tm-audit.md), [Mega Stone audit](chaos-mega-audit.md), and the reproducible native harness in `tools/chaos/native/`. Raw text results are retained beside that harness.

## Practical limits and next QA

The reported original Delta Brock freeze was not independently reproduced in this environment. The new build passes all four genuine loss/retry conditions; this supports the fix but does not prove every emulator-specific cause eliminated.

Victory fixtures lower opponent HP and preserve player HP to exercise battle completion and reward scripts deterministically. They verify integration, not difficulty balance or a full unassisted campaign. AI prediction uses known battle information and heuristic estimates; it does not read the move selected by the player that turn. Most-used move tracking is bounded/approximate rather than an unlimited history.

A full Delta playthrough is still the primary QA pass for map navigation, pacing, balance and event sequences beyond targeted tests. Load normal in-game saves with the new ROM; old emulator save states can retain obsolete code/runtime state.

Not implemented in this batch, as directed: Giovanni/Mewtwo partner revival story, Pallet storm/Chaossal crisis, Viridian succession/simulator, Scyther variant assets/system and final availability expansion. No unfinished entrances were added for those designs.
