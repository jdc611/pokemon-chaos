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
- Stone Axe set to 60 BP and 95 accuracy, retaining its effect.

## Validation evidence and limits

Host scenarios pass with address/undefined-behavior sanitizers (leak scanning is
unavailable in this container and disabled). Actual mGBA ARM-call fixtures pass:
rank cap and inspection transitions, all signature move typings, all 18 starter
pool counts >= 9, basic Bulbasaur/Charmander/Squirtle present and their next
stages absent, Glastrier assignment, tested Corviknight/Annihilape/Palafin stat
totals, and actual flash save/reload of the full new state.

Fixture new-game entry is diagnostic. It explicitly initializes GLE state;
there is no finished player-facing GLE opening yet. These are API/integration
checks, not a full playable route, device test, balance test or complete
collision/visual audit. Draft palettes require visual refinement and overworld
palette integration. Permanent history storage is not implemented.

A pool-generation timeout in the first native test was removed by using raw
species catalog stats/evolution data and bounded evolution traversal. Future
catalog changes must continue running the native test. Two viewed vouchers can
no longer reserve the same nonrepeatable reward simultaneously.

The older compiler blocker and lack of an emulator are now resolved. Any earlier
status document saying those are current blockers is superseded by this file.

## Still required before release

- Single-mode startup; Oak opening, town naming/selection, identity, move selection
  and playable 3x3 draft with native ability/summary inspection.
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
