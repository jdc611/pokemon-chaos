# Runtime integration status — November 2, 2026

## Requested deliverable

One complete, integrated Gym Leader Edition ROM. No player-facing partial ROM
release is approved as completion. The current binary is internal QA only.

## Implemented and checked

- Official ARM GCC 13.2.1/binutils/newlib/graphics toolchain works locally.
- Compiled FireRed target: 32 MiB padded; current linked payload ~27.23 MB.
- Pointer-free 1,344-byte GLE state uses an anonymous union with Hoenn secret-base
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
- Gym Guide uses native menus for status, healing, team registration and defenses.
- Registration stores PID/OT identities. Defenses locate the real Pokémon in party
  or PC storage, temporarily assemble the team, and restore the traveling party.
- Normal battle EXP/moves/evolutions persist for defenders within the rank cap.
  Over-cap defenders temporarily normalize and retain their original level/EXP.
- Procedural novice challengers use real trainer battles, native moves, rank XP,
  victory/loss returns, healing and a nonempty continuing queue.
- A permanent protected signature TM is granted in KEY ITEMS. Native TM/party
  screens teach compatible Pokémon and retain the reusable item; natural level-up
  compatibility is supported alongside ordinary TM compatibility.
- All four native inspection gates use fixed traditional Elite Four rosters,
  cap-scaled teams, honor dialogue, preserved retry XP/queue and promotion results.
- Player/rival towns replace their events and construction layout while retaining
  native map IDs, connections and the unselected town. Plot signs call persistent
  construction transactions; Centers/Marts have native functional interiors.
- Housing capacity, gradual population, visitor NPCs and passive deposits are
  connected to exploration activity. Constructed Habitats generate rank-scaled,
  type-matching ordinary wild Pokémon. Other ordinary wild levels follow rank.
- GLA/home travel and a home-Gym whiteout return prevent early recovery dead ends.
- Stone Axe set to 60 BP and 95 accuracy, retaining its effect.

## Validation evidence and limits

Host scenarios pass with address/undefined-behavior sanitizers (leak scanning is
unavailable in this container and disabled). Actual mGBA ARM-call fixtures pass:
rank cap and inspection transitions, all signature move typings, all 18 starter
pool counts >= 9, basic Bulbasaur/Charmander/Squirtle present and their next
stages absent, Glastrier assignment, tested Corviknight/Annihilape/Palafin stat
totals, and actual flash save/reload of the full new state.

The controller-driven opening matrix passes all 18 type menus and drafts,
rotating through all three hometowns. It exercises signature selection, native
Summary profile/stats/moves, three starters with moves, GLA tutorial, hall exit
and native Viridian Gym doorway redirection. Optional native town naming passes.
Player/rival naming is bypassed; the real Oak naming-to-new-game path still needs
checking. This is not a complete campaign or collision/visual audit.

The Gym Guide fixture registers a defender through the native party selector,
then wins a real battle using Fight/Powder Snow input with no altered enemy HP or
forced outcome. It verifies travel-party restoration, healing, queue continuity,
PC identity lookup, EXP writeback, loss bookkeeping, temporary cap normalization,
invalid/missing team refusal and registered identities through flash save/reload.
Additional adapter scenarios use direct ARM calls and explicit outcomes; they are
transaction tests, not claims of full battle or balance coverage.

A separate native signature fixture opens the real Key Items bag, uses the TM,
selects a compatible off-type Pokémon, verifies the move is learned and that the
one-off-type legality rule accepts it, and confirms the single copy is retained.
Four-move replacement, repeated teaching, cancellation and all signature options'
compatibility/balance still need broader UI coverage.

Fixed issues: enemy-party previews triggered Summary's link-opponent branch and
copied an uninitialized link trainer name, corrupting memory. A separate preview
array now displays ability/memo correctly and safely returns through the normal
field callback. National Pokédex setup fixes non-Kanto zero numbers. Modern
CreateMonWithIVs does not assign moves; previews, starters and challengers now
explicitly receive their native initial moves, checked by the fixtures.

The construction fixture passes all three hometowns: actual sign purchase, money,
Center door/collision, native Nurse healing, dynamic exit, housing capacity,
gradual population/deposits and actual flash/map restoration. The Saffron fixture
also passes home-Gym whiteout and actual walking-generated Habitat wild encounter.
The inspection fixture checks all gates and deterministic retries with explicit
transaction outcomes, plus an actual unassisted Lorelei loss and safe retry menu.
These are not complete campaign/balance/collision coverage.

Registration currently selects from the traveling party; registered Pokémon may
subsequently be deposited and found in the PC. Direct PC selection at registration,
all ranks' native selection screens, full evolution/held-item scenarios and a
long balance playthrough remain to be verified. Draft palettes require refinement
and overworld palette integration. Permanent history storage is not implemented.

A pool-generation timeout in the first native test was removed by using raw
species catalog stats/evolution data and bounded evolution traversal. Future
catalog changes must continue running the native test. Two viewed vouchers can
no longer reserve the same nonrepeatable reward simultaneously.

The older compiler blocker and lack of an emulator are now resolved. Any earlier
status document saying those are current blockers is superseded by this file.

## Still required before release

- Complete opening polish, cancellation paths, real Oak naming entry, and
  save/load at each opening stage.
  Signature choice and its protected reusable item/teaching UI are implemented.
- Further town visual/collision polish, all building
  service roles and tiers, and wider route-edge/plot/interior audits.
- Direct PC registration, later-rank challenger composition and balancing,
  inspection victory balance coverage, recruitment queue filtering and richer rewards.
- Environment-wide species redistribution and deterministic full-species coverage,
  remaining universal/research encounter events and postgame distribution.
- All research event scripts/NPCs, form upgrades, encounter progress and rewards.
- General GLA tutor/supply services, recruitment/rival/loyalty story integration,
  practice difficulty balancing and optional Elite Four rematches.
- Honor, Black Market, Rocket/NPC quests, town requests and passive-income hooks.
- Tournaments, championship scheduling/history/team snapshots, news and betting.
- Existing L-screen BP/PWR display and the remaining existing-feature/menu audit.
  Native Trainer Card rank/EXP bar, Gym records and safe front/back flips pass.
- Legacy badge/travel/story lock removal, end-to-end progression, save/load and
  failure-path testing and final release packaging.

Do not rename the currently compiled internal ROM as the completed edition.

## GLA and landmarks checkpoint

Native voucher desk/first-view locking, Summary, milestone rewards, cosmetic GLA
Ball, actual party/PC delivery, full-storage refusal and save migration pass the
new native service fixture. Actual Training Room battles retain Pokémon EXP but
award no money (including Pay Day), official progress or town activity. Native
landmark purchases and relocated entrance/exit pass for all three hometowns.
Silph's public lab avoids the old Rocket campaign; Game Corner opens independently
of Erika's badge. Research notebook displays persisted evidence; research quest
events still require integration. Owner Safari species configuration is now implemented
and tested as described in MASTER section 23.
See MASTER section 22 for delegated tuning and artwork limitations.

## Travel and Safari checkpoint

Native rank-one League/Saffron gate crossings, equipment/PokéRider permissions,
no manufactured badges/Champion flag, and all-rank native trainer scaling pass.
The owner Safari desk, tiered slots, settlement cost confirmation, distinct forms,
persistence/migration, cancellation and actual wild-generation adapter pass.
Save state is now version 6, 1,344 bytes; previous offsets remain unchanged.
These targeted tests do not replace the remaining complete overworld/story audit
or all Safari walking/biome/collision checks. MASTER section 23 records tuning.

## Exclusive encounters and native career card checkpoint

The four actual map encounters now resolve all 72 established assignments and
rank/cap rules. Native save prompting, capture-only consumption, species sprites
and retries are integrated; a real Glastrier capture and flash persistence pass.
The GLA expedition guide reaches each area's entrance. The native Trainer Card
now renders rank EXP and career records; a discovered back-flip invalid-species
crash is fixed. All 17 Memory/Plate typing pairs and stale-form handling pass.
Old free editors and Train to Cap cannot bypass the mode's reward progression.
See MASTER section 24 for fixture boundaries and the current memory budget.

## Supplies, research placement and refused-action recovery

GLA stock now uses the native Buy/Sell interface, rank tiers and honorable discount.
Type: Null placement, native friendship-evolution reporting and Memory unlock are
integrated. Special Slaking's rank-five awakening preserves ordinary Slaking and
blocks pre-research Ability Patch access. Native tests cover actual purchasing,
all stock tiers, one-time delivery, full-storage/PC recovery and flash persistence.
An implicit-waitstate rejection softlock is fixed; native empty-voucher, locked
practice and unregistered-defense interactions return control normally. MASTER
section 25 records the delegated tuning, test boundaries and remaining systems.

## Wings of Kanto

All six universal birds now have independent field encounters and notebook
sightings, with ranks 3–8. Actual Articuno capture/save/no-respawn passes and does
not consume Ice's separate Glastrier record. All six native approach/decline,
placement, rank and retry checks pass. MASTER section 26 records the new map
choices, capture-order fixtures and dungeon-traversal coverage limits.

## All-type habitats and evolution access

Initial habitat construction now offers all 18 types independently of Gym type;
cancel is free and upgrades retain the theme. Native selected-type encounter,
all-type catalog and flash tests pass. GLA evolution stock covers regional
apples/pots/teacups, sweets, Galarica items, armor and other ordinary evolution
items; its actual native long list survives legacy shop filtering. See MASTER
section 27 for tuning and remaining ecology/coverage limits.


## Permanent tutor lessons and save version 7

Thirty native move lessons unlock once with money, rank and party compatibility;
subsequent party/PC teaching is free, including after demotion. Actual native
party and PC teaching, payment/refusal and flash persistence pass. Relearner
access respects current level, owned TMs and unlocked tutors. State version 7
is 1,348 bytes in the unchanged save union; exact version-6 preservation and
older Safari/voucher migration regressions pass. MASTER section 28 records the
complete lesson/price catalog and coverage boundaries.

## Basic recruitment and staff loop

Existing defeated trainers can offer recruitment with seeded willingness,
Potential, loyalty and paid hiring. The native staff office assigns/reserves,
shows teams in Summary, runs real practice and confirms dismissal. Staff are
visible in the home Gym/town; departed rival staff and conditional returns retain
records. Active staff filter ordinary queue entries with their own statistics,
without player rank gains or consuming the final/important challenger. Native
hire/save/relocation/preview/practice/dismissal and host progression tests pass.
Prices are now visible on native final confirmations. MASTER section 29 records
exact tuning, fixture boundaries and still-separate rival/Rocket/quest systems.


## Rival friendlies and legacy ending protection

The visitable rival Gym now offers a native friendly using current registered-team
rules. A persisted basic starter and deterministic ace identity survive rank
changes; all eighteen specialties and current caps/team limits pass fixtures.
An actual controller-played win, first-win reward, repeat/loss/demotion guards,
party restoration and flash persistence pass. Save version 8 is 1,356 bytes;
version-seven tutor licenses and every prior field are preserved. Shared Summary
preview memory leaves 4,520 EWRAM bytes free. The Indigo guard and all six old
ending-room warp targets now prevent the inherited rival-Champion ending in
this edition. MASTER section 30 records exact tuning and test limitations;
GLA doubles, broader rival events and the intended final competition remain
separate release requirements.

Normal title/New Game/Oak/player keyboard/rival preset to hometown selection now
passes a controller-only test without debug or injected-name bypasses. Actual
party/PC tutor regression passes; its test refreshes the native relocated PC
pointer after returning to the field.


## Permanent research access after demotion — November 3, 2026

Wings of Kanto roost visibility, approach permission and battle preparation now
use the highest earned rank as well as current rank. Demotion preserves earned
research access; encounters continue using the current cap. The notebook can
also be opened for the first time after demotion once rank 3 was earned.
Unreached survey ranks remain locked, and captured birds remain consumed.

The rebuilt native fixture passes all six original rank/placement/decline/retry
checks, all six demotion-access checks, future-rank refusal, current-cap levels,
flash-reload access, delayed notebook activation, nonsequential capture records,
and a real Articuno Master Ball capture/save/no-respawn regression. No save layout
or version change is needed; EWRAM remains 257,624 bytes. This checkpoint fixes
research persistence and does not certify the remaining campaign/release gates.
