# Chaos FireRed V2 implementation checkpoint — October 8, 2026

**Status: incomplete; not a release or a verified V2 ROM.**

The execution environment stopped responding during final integration testing.
Repeated attempts to start a shell, recover existing execution sessions and
write a local checkpoint failed or remained stalled. This is an execution
failure, not an approval requirement.

## Source and last build

Repository: jdc611/pokemon-chaos.
Implementation started from master commit
`edd207acd8df4574c97e1fddcd630c1ade172adf`.

The implementation is in the uncommitted working tree at:
`/workspace/scratch/3923bd38e6ed/fire-red`.

This checkpoint branch contains **documentation only**, not the uncommitted V2
implementation. Local files must be recovered before continuing; do not assume
this branch contains the changes.

The last successful compile produced:
`/workspace/scratch/3923bd38e6ed/fire-red/pokefirered.gba`.

That ROM predates the final fixes to Ability-window graphics, changed-item berry
cleanup and strict level-cap EXP handling. It must not be handed off as completed
V2. Last successful linker totals: EWRAM 248180 bytes; IWRAM 29072 bytes; ROM
27179912 bytes before padding.

## Requirement status

| Section / requirements | Implemented in working tree | Remaining verification / work |
|---|---|---|
| 1: full-party capture and later-route failures | Initialize battle party-order mapping before catch replacement; correct naming callback to rebuild battle graphics; nonblank mandatory Nuzlocke naming | Repeat Nuzlocke full-party captures across routes, Surf/fishing, randomized settings and full-box/all-storage-full cases; finish cry/message and save/reload stress checks |
| 1: Mt. Moon Center black screen | Capture/menu state cleanup repaired; Center transition path inspected | Black screen not reproduced in 24 native entry/heal/exit cycles; root cause remains unproven; Delta testing still needed |
| 1: fishing/surf border | Reload message-box/border graphics for native multichoice menus | Repeated fishing/surf/map-transition palette checks |
| 2: Scyther | Correct normal palettes from existing indexed artwork; shiny assets unchanged | Normal front and icon reviewed; verify back/shiny battle appearances |
| 2: Play Style | Move NEXT upward to fit | Native new-game layout review |
| 2: Records Nurse | Move to x3/y3 in applicable Centers | Pewter reviewed; check remaining layouts and access |
| 2: Viridian/Pewter grass | Preserve patch geometry; replace tree-overlay/flower graphics with native clean tall grass | Viridian reviewed; final Pewter encounter/animation review |
| 2: Pewter Ranch door | Remove blocking NPC | Entry/exit regression |
| 2: Train to Cap | Remove black fade before native field menu | Native menu background review; whole/one/cancel/chained evolutions already pass |
| 3: EXP All / MGM | Default independent EXP All ON; Options toggle; full participant / half shared EXP; zero MGM EVs, perfect IVs; reject EV-increasing items | Inspect quantitative battle EXP test log; rerun after strict-cap fix; verify all EV item sources and save persistence |
| 3: nicknames / Rename | Mandatory acquisition prompts, capture/hatch nonblank guard, party RENAME; Arcade draft nickname before delivery | Starter/gift/hatch/Arcade script and rename integration; Nuzlocke capture matrix needs corrected keyboard navigation |
| 3: party Relearn | Post-Brock RELEARN; exact Level Up / TM / Egg Moves choices; selected member passed directly | Egg list and native return reviewed; test actual teaching in all three categories |
| 3: Ability Changer | Full current species ability slots, current mark/descriptions, incompatible filter entries disabled | Final VRAM placement fix needs rebuild; filter-selection and native script continuation tests |
| 3: TM details | Native framed move data/description/priority/recoil and six actual compatibility icons | Mach Punch screen reviewed; complete all-TM description/long-text/known-move and reusable teaching audit |
| 3: fishing | Ignore premature A; retain automatic hook; reduce failed-bite frequency | All three rods, failure sampling and DexNav integration |
| 3: Surge puzzle | Preserve first switch across wrong second choices and map re-entry | Actual trash-can interaction/completion regression |
| 4: evolutions | 64 data changes; Link Cable held-item methods; friendship level replacements; regional item methods; cable alternatives for awkward methods; Celadon evolution-stock additions | Compiled roster audit passed; representative native methods passed; finish full item availability/prices, day/night, move/party/gender/special condition tests |
| 5: Evolution & Growth | Separate native Summary page, live requirements, branch navigation, next species icon, growth/EXP/egg groups | Scyther page reviewed; test all branches, final forms, random evolutions and long conditions |
| 5: Chaos Rating | Current-form stats, actual ability, types, IVs and complete supported level/TM/egg movepool heuristic; level-independent | Representative calibration and level invariance passed; finish randomized/ability/evolution/permanent-Mega recalculation tests and formula documentation |
| 6: eggs | Three curated Center vendors, $5000, one purchase each, random species, safe party/PC delivery, gift semantics | One-time charges and full-party PC delivery pass; actual hatch, full-storage failure and nickname/pool tests |
| 7: Care Packages | Exactly three automatic area milestones; exact supported item lists; 999 Rare Candy top-off; atomic rollback; receipts and pagination | Exact contents/top-off/replay pass for all three; finish actual area entry, OFF/ON return behavior and overflow/retry tests |
| 8: berries | Battle-scoped consumed bits; original holder refund only; no overwrite/theft/Knock Off refund; suppress repeat activation | Native tests exposed old cleanup clearing changed items; fix is written but uncompiled; rerun consumption, switching, Harvest/Unnerve, doubles and item-changing effects |
| 9: aides | Remove quotas; preserve original rewards/one-time flags; unique jokes | Corrected Route 11 edit to preserve trade NPC; actual five reward/replay tests still needed |
| 10–12: regression/build/delivery | Most code is implemented; several host and native checks passed | Final build, full required check matrix, initial gameplay/load verification, commit/push and ROM delivery remain |

## Passed checks observed before execution failure

- All 21 existing host regression scripts passed.
- Native Brock genuine loss, whiteout, re-entry and second battle initialization.
- Native Jessie/James genuine loss, retry, win, automatic continuation/blast-off,
  completion and no re-entry retrigger.
- Native doubles target-effectiveness indicators.
- Post-Brock unlock script; Whole Party and One Pokémon training; cancel;
  Caterpie → Metapod → Butterfree; above-cap unchanged; dead Nuzlocke member skipped.
- Full-party capture chooser rendered six distinct actual species correctly.
- Every replacement slot (0–5) retained untouched identities, transferred the
  exact selected original Pokémon to PC and delivered the catch into that slot.
- Canceling replacement stored the catch while preserving all six original members.
- MGM perfect IVs, zero direct-write/battle EVs and vitamin rejection; ordinary
  vitamin use still worked.
- EXP All default ON and independent OFF/ON setting.
- All three Mystery Egg purchase receipts, exact charges, replay prevention and
  full-party PC delivery.
- All three exact Care Package item lists, Rare Candy maximum and replay guards.
- Native Kadabra Link Cable; Scyther with Metal Coat + Link Cable; Slowpoke with
  King's Rock + Link Cable; Eevee Ice Stone; Exeggcute Alolan Sun Stone.
- Compiled evolution audit: 673 evolution entries across supported forms, with
  no detected trade-only target lacking a non-trade alternative and no remaining
  mandatory friendship, trade-partner or foreign-region-only conditions.
- Current-stage rating level invariance for Caterpie, Magikarp, Scyther, Haunter,
  Gengar, Snorlax, Charizard, Mewtwo and Rayquaza.
- 24 native Mt. Moon Center entry/healing/exit cycles without the reported black
  screen. These do not prove the intermittent Delta issue is fixed.
- Native visual review of corrected Scyther front/icon, Viridian grass, Pewter
  employee placement, Options, three Relearn categories/egg list, Mach Punch
  details and Evolution & Growth page.

## Important failing / unfinished tests

- Nuzlocke stress fixture correctly rejected an empty nickname; its subsequent
  keyboard navigation selected whitespace and did not complete naming. The test
  was being corrected, not counted as a capture pass.
- Native berry cleanup test proved a legitimately changed nonberry item could be
  cleared by older cleanup. The subsequent fix skips that old branch for original
  berries, but it has not been rebuilt or verified.
- Ability selector screenshot showed lower-window VRAM overlap. Base block was
  moved from 0x200 to 0x100, but that final fix has not been rebuilt or reviewed.
- Direct synchronous test calls into flash-saving stalled the native harness;
  verify save/load through the native Save UI rather than that shortcut.
- Quantitative EXP test was running when execution failed. Its result was not
  read. Do not claim the quantitative battle result.
- No full playthrough or Delta test was performed.

## Recovery instructions

1. Recover the working tree before cloning or resetting anything.
2. Inspect `git status` and all uncommitted changes.
3. Read scratch `v2-*.log` and `qa_v2_*.py` files.
4. Rebuild using `/workspace/scratch/3923bd38e6ed/build-v2.sh`.
5. Regenerate every native save-state fixture after rebuilding; they embed code
   addresses. Native helper now supports six-argument ABI calls.
6. Finish the remaining matrix above; do not equate compilation with fixes.
7. Add reproducible V2 tests and a final verification report to the repository.
8. Commit/publish verified source, save one final V2 ROM and deliver its exact path.

Do not implement the deferred Forest expansion or unfinished Chaossal endgame.
