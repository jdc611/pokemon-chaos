# Runtime integration status — November 2, 2026

## Requested deliverable

One complete, integrated Gym Leader Edition ROM. No player-facing partial ROM
release is approved as completion. The current binary is internal QA only.

## Implemented and checked

- Official ARM GCC 13.2.1/binutils/newlib/graphics toolchain works locally.
- Compiled FireRed target: 32 MiB padded; current linked payload ~27.23 MB.
- Pointer-free 1,280-byte GLE state uses an anonymous union with Hoenn secret-base
  storage. Compile-time fit assertion; following SaveBlock1 offsets preserved.
- Rank XP, promotion and four inspection gates, protected demotion floors;
  current GLE cap queried through existing global cap function when active.
- Challenger queue, pending important arrival, gradual population, construction
  transactions, recruit bookkeeping and research discovery state are core APIs.
- Five voucher policy classes: fixed tier, persistent first-view options,
  normal/shiny claim separation, reservations across simultaneous opened offers.
- 72 exclusive runtime assignments with no repeats; 49 reserved research species
  with no overlap. Glastrier replaces Ice Articuno. Birds stay in research.
- 18 isolated custom species records, including protected original species,
  selected abilities/stats, four-move initial sets and draft palette data.
- Implementation-authored same-type signature pools: 18 x 3 categories x 3 options.
- Runtime species-pool builder with rank/category filtering and basic starter pool.
- Native single-mode new-game hook, Oak hometown selection, optional native town
  naming, 18-type menu, signature confirmation and persistent nine-candidate draft.
- Draft candidate ability text, Summary preview and deterministic starter IVs;
  three selected level-5 Pokémon are added to the real party and Pokédex.
- Separate basic Gym and GLA interiors with healing NPC, dynamic town exits and
  redirection of the selected hometown Gym and Viridian Gym entrances.
- Stone Axe set to 60 BP and 95 accuracy, retaining its effect.

## Validation evidence and limits

Host scenarios pass with address/undefined-behavior sanitizers (leak scanning is
unavailable in this container and disabled). Actual mGBA ARM-call fixtures pass:
rank cap and inspection transitions, all signature move typings, all 18 starter
pool counts >= 9, basic Bulbasaur/Charmander/Squirtle present and their next
stages absent, Glastrier assignment, tested Corviknight/Annihilape/Palafin stat
totals, and actual flash save/reload of the full new state.

The controller-driven opening fixture passes Saffron/Grass, signature selection,
Summary entry/return, all three starter picks, GLA tutorial, hall exit and native
Viridian Gym doorway redirection. Optional native
town naming was also exercised through confirmation. Name/player/rival entry is
still bypassed by the fixture; real Oak naming-to-new-game flow needs testing.
These checks are not a complete campaign playthrough or a collision/visual audit.

Known presentation issue: the native Summary profile panel currently shows empty
ability/memo text and a zero regional number for non-Kanto candidates. The draft's
own detail message displays the candidate's ability. Summary display refinement,
all-page review, and national Pokédex setup remain required. A Summary-to-field
crash exposed a cleared map header; the draft return now reloads it from the saved
map and reconstructs its layout before resuming the field. Draft palettes require
visual refinement and overworld palette integration. Permanent history storage is
not implemented.

A pool-generation timeout in the first native test was removed by using raw
species catalog stats/evolution data and bounded evolution traversal. Future
catalog changes must continue running the native test. Two viewed vouchers can
no longer reserve the same nonrepeatable reward simultaneously.

The older compiler blocker and lack of an emulator are now resolved. Any earlier
status document saying those are current blockers is superseded by this file.

## Still required before release

- Complete opening polish, native Summary text repair, all hometown/type and
  cancellation paths, real Oak naming entry, save/load at each opening stage.
  Signature choice is recorded; its reusable item/teaching UI is not yet granted.
- Actual blank town maps, safe plots, building/interior transitions, restoration,
  collision/visual verification, three landmarks and habitat encounters.
- Registered official teams, battle adapters, safe demotion normalization and
  restoration, generated challengers/inspectors, queues/healing/rewards in game.
- Wild rank scaling, environment draws, deterministic full-species coverage,
  static encounter scripts, save prompts/retries and postgame distribution.
- All research event scripts/NPCs, form upgrades, encounter progress and rewards.
- GLA voucher UI and gifts, protected reusable signature teaching, tutor/supply
  services, recruitment/rival/loyalty story integration and Training Room.
- Honor, Black Market, Rocket/NPC quests, town requests and passive-income hooks.
- Tournaments, championship scheduling/history/team snapshots, news and betting.
- Existing L-screen BP/PWR display and complete existing-feature/menu audit.
- Legacy badge/travel/story lock removal, end-to-end progression, save/load and
  failure-path testing and final release packaging.

Do not rename the currently compiled internal ROM as the completed edition.
