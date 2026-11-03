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


## 32. Ancient Titans and Ancient Awakening — November 3, 2026

The following locations, ranks and inscription choices are implementation tuning
under Jack's delegated authority, not newly recovered historical decisions.

- Tanoby's Monean/Liptoo/Weepth/Dilford/Scufib chambers hold Regirock, Regice,
  Registeel, Regieleki and Regidrago respectively. Each opens its native battle
  at earned rank 4 after a short three-choice inscription puzzle about stone,
  ice, steel, electricity or dragons. Wrong answers and cancellation permit
  retries without payment or consumption. Correct answers persist.
- Entering each of those five chambers records an independent discovery even
  before encounter eligibility. Any order works. Rixy holds Regigigas at
  earned rank 6 and requires all five discoveries, never the other captures or
  victories. Each titan uses the player's current cap and standard species data,
  initial moves, native overworld sprite and ordinary legendary capture flow.
- The native research notebook lists all six chambers, their species, discovery
  or capture status and survey rank. The GLA expedition guide adds free rank-4
  transport to Tanoby's outdoor shore; it does not teleport into the chambers.
  Earned expedition access and titan encounter access survive demotion.
- Save prompts precede battle confirmation. Only a native capture consumes a
  titan; defeat/flee leaves it available. Unique bits 79–84 store captures,
  85–89 store the five discoveries, and 90–94 store solved inscriptions.
  Existing bird, Type: Null and exclusive records are preserved. No new save
  version, state size or offsets are required.
- After the five discoveries and the recorded Regigigas capture, the GLA's
  Ancient Awakening entry finds a non-Egg Regigigas in party or PC and permanently
  grants Clear Body. This uses the engine's existing per-Pokemon saved ability
  override and marks project 4 complete. Ordinary Regigigas species data and
  other Pokemon remain unchanged. No duplicate Regigigas is awarded.
- Research object templates share the existing buffer with bird sites, since
  the supported sites are on separate maps. EWRAM remains 257,624 bytes of
  262,144; ROM payload is 27,276,060 bytes, padded to the existing 32 MiB image.

Native mGBA checks pass all five any-order early discoveries, final-temple refusal
before discovery, no-capture final unlock, all six walkable object/approach
placements, all five real inscription menus/wrong-answer retries, cancel
transaction refusal, solved-seal persistence, save/approach declines, all-six
species/levels/moves, defeat/flee retry, demotion/current-cap behavior, flash
persistence, nonsequential capture records and no duplicate respawn. A real
Regirock fight uses a supplied Master Ball without altered enemy HP or injected
outcome and verifies party delivery, flash persistence and no respawn. Awakening
tests exercise the native GLA menu for a party target and an explicit PC-storage
fixture, pre-capture refusal, one-time completion, party/PC flash persistence and
unchanged ordinary species ability. The five-destination expedition guide and
six-bird/actual-Articuno-capture regressions pass.

Tests directly warp to chamber approach tiles and use explicit outcomes for
all-six capture transactions; only Regirock has a controller-played capture in
this checkpoint. These are not full island traversals, six catch-rate/balance
playthroughs or a complete campaign. Other universal projects, story systems,
competitions/history and the remaining release gates still require work.

## 33. Five additional field-research projects — November 3, 2026

Fifteen research-only encounters are now connected to native Kanto maps and the
existing GLA notebook. Locations, rank pacing, inscription questions and narrative
clues below are new implementation tuning under delegated authority.

| Project | Field encounters and initial earned rank |
| --- | --- |
| Cursed Treasures | Wo-Chien in Viridian Forest, Chien-Pao in Seafoam B2F, Ting-Lu in Rock Tunnel 1F, Chi-Yu in Mansion B1F; rank 5 |
| Guardian Shrines | Tapu Koko on Route 10 (4), Tapu Lele on Route 25 (5), Tapu Bulu in Safari West (6), Tapu Fini on Route 19 (7) |
| Lakes of the Mind | Uxie on Route 22 (3), Mesprit in Safari East (4), Azelf on Route 12 (5) |
| Toxic Pact | Okidogi on Route 15 (5), Munkidori on Route 8 (6), Fezandipiti on Route 18 (7) |
| Jungle Guardian | Standard Dark/Grass Zarude deep in Viridian Forest; rank 4 |

Each encounter uses an individual three-choice native inscription/approach menu,
then the existing save prompt, battle confirmation, standard species/moves,
current-cap legendary battle and capture-only consumption. Wrong answers,
cancellation, defeat and fleeing preserve retries. Earned access survives
demotion. Toxic Pact requires discovering its three separate route sites in
any order; no other capture is required. Its clues connect the three poisonous
chains. The other four projects have independent encounters.

The native notebook provides each project's contextual lead, individual species,
location, discovery/seal/capture status and survey rank. Ordinary map objects
remain intact. The shared research map buffer now appends multiple sites:
Viridian's Wo-Chien, Zarude and Galarian Moltres coexist with distinct IDs.
Field captures reserve bits 95–129, discoveries 130–164 and solved seals 165–199;
fifteen entries currently use each reservation. No state version/size change.

Native checks pass all fifteen linked species/reservations, dry walkable object
and adjacent approach tiles, actual actors, early discoveries, rank refusal,
Toxic Pact evidence gate, all fifteen real seal menus/wrong-answer retries,
cancel transaction refusal, saved solutions, declined saves/battles, moves,
defeat/flee retry, current-cap demotion behavior, flash persistence, all-fifteen
nonsequential capture records and no respawn. All five actual GLA notebook/report
return flows pass. A real Uxie fight/capture uses a supplied Master Ball without
altered enemy HP or injected capture outcome and verifies party delivery,
flash persistence and no respawn.

For deterministic interaction testing, ordinary random encounters are disabled
and native trainers are marked previously fought in the fixture only. Gameplay
retains them. Fixtures warp to approach tiles; they do not certify complete
walking routes, all species' catch-rate balance or full campaign traversal.
The six-bird and six-titan regressions, including actual Articuno/Regirock capture
and Regigigas party/PC awakening, pass after sharing the map buffer.

## 34. Finite resident requests, decisions and accessible Honor — November 3, 2026

Home-town resident speakers now offer eight persistent optional stories as
population brings their existing native actors into town. First four requests
accept already-built services and never require rebuilding or collecting items.
These prices, rewards and choices are delegated implementation tuning.

| Speaker | Request / decision | Honest contribution or reward | Alternative |
| --- | --- | --- | --- |
| 1 | Open a Center | 500 | Optional, can claim later |
| 2 | Open any type of Habitat | 1,000 | Optional, can claim later |
| 3 | Open a Restaurant | 800 | Optional, can claim later |
| 4 | Open a Museum | 1,200 | Optional, can claim later |
| 5 | Visitor's lost purse | Return: 400 | Keep: 1,500 |
| 6 | Young food thief offers amends | Second chance: 400 | Banish: 800 |
| 7 | An unearthed relic | Public preservation: 1,000 | Unrecorded sale: 5,000 |
| 8 | Merchant requests an unfair monopoly | Refuse payment | Accept private payment: 2,000 |

Construction support grants 10 Honor and 5 Town Reputation. Honest decisions
grant 40 Honor and 5 Reputation. Shady decisions grant -60/-50/-80/-50 Honor
respectively and reduce Reputation by 3. All transactions saturate safely and
use ordinary Pokédollars. Each speaker can settle once per run; deciding later,
declining a contribution or missing a requested building leaves it available.
Other towns cannot settle the player's requests. These are finite resident
stories, not a completed procedural errand/Rocket/personal-bond quest system.

Speaker 1 also explains the current Honorable/Neutral/Notorious standing in
native dialogue and the actual existing GLA-price/Ace-recruitment/Black-Market
construction effects. No honor bar or extra popularity meter is added. These
choices make both positive and negative Honor reachable through normal play.
Dialogue does not award Gym XP, defense records or research captures.

Reserved ledger bits 200–207 persist settlements independently from research.
State remains version 8, 1,356 bytes with unchanged offsets. Host ASan/UBSan
scenarios pass missing-objective refusal without mutation, valid/repeated/invalid
transactions, honest and shady thresholds and unchanged research bytes. Native
checks pass actual pending Center dialogue, declined and accepted support,
repeat anti-farm, decide-later retries, other-town refusal, all four honest and
all four shady choice menus, ordinary-money settlement, Honor transitions and
flash persistence. Population/rank and wandering-NPC positions are explicit
fixtures; actual Center plot purchase is covered by the separate town regression.
Native registered defense/PC/EXP and town construction/Nurse/housing/income/
whiteout/Habitat regressions pass.

Current linked EWRAM is 257,632/262,144 bytes; ROM payload is 27,285,424 bytes,
padded to one 32 MiB ROM. Complete remaining research, Rocket/competition/history,
full species ecology, native BP/PWR and campaign/device release gates remain.

## 35. Paradox of Evolution: live study and two independent encounters

Implemented November 3, 2026. Locations/ranks/seal wording below are delegated
implementation choices, not recovered prior approvals of specific coordinates.

- A rare Cyclizar visits Route 16's study site at earned rank 2. It uses a real
  native encounter, ordinary catching and a one-time research capture record.
  This supplies a supported route to the already-reserved study species, rather
  than making the player search ordinary pools that deliberately exclude it.
- Open Paradox of Evolution at GLA with a non-Egg Cyclizar in party or PC.
  The researcher records a non-invasive study. The actual Pokemon's identity,
  stats, moves and form remain byte-for-byte unchanged; it is never consumed,
  transformed, cloned or awarded again. Study completion persists after storage,
  release or demotion.
- Koraidon appears at the ancient trace in Mt. Moon B2F at earned rank 5 after
  the live study. Miraidon's Power Plant signal opens at earned rank 6 after
  that study and discovery of the ancient trace. Early evidence persists.
  Miraidon does not require catching or defeating Koraidon. Both are separately
  capturable partners; neither evolves from or replaces Cyclizar.
- All three sites use the existing native field clue/seal/save/confirm/battle
  flow, standard species data and moves, current-rank cap, capture-only
  consumption, defeat/flee retries and demotion-safe earned access. The native
  GLA notebook gives live-study instructions and the ancient/future leads.
  Power Plant's separate exclusive and bird research remain intact.
- Field entries append at slots 15–17 without renumbering the prior fifteen.
  Capture bits are 110–112, discovery bits 145–147 and solved-seal bits 180–182.
  Bit 208 records the Cyclizar study independently from resident settlements
  (200–207). Eighteen entries now use the existing field reservations.
  State remains version 8, 1,356 bytes with unchanged save offsets.

Native checks pass the actual GLA notebook with and without a study partner,
pre-study refusal, early remembered ancient/future evidence, all three real
actors and dry walkable approach tiles, reserved-species policy, a real Cyclizar
fight/Master Ball capture without altered HP or injected outcome, non-consuming
native party study with exact Pokemon-byte preservation, and native flash.
Both legendary seals/species/moves/current caps, independent capture transaction
records, no Koraidon-capture prerequisite, defeat/flee retries, demotion and
no-respawn flash records pass. A native PC-held study preserves the actual
captured Cyclizar's complete box record through flash reload without delivering
a gift or changing the traveling-party count. Nearby original trainer completions
and disabled ordinary random grass battles are interaction-test fixtures only.

The PC test initially restored an older RAM snapshot against newer emulated
flash. Flash is not part of this emulator helper's raw RAM snapshots; its newer
save counter must be retained for an independent restored branch, otherwise a
subsequent load correctly selects the newer preexisting flash save. The fixture
now keeps flash generation monotonic. No gameplay PC/save change was required.

The fifteen earlier field encounters and six-bird/actual-Articuno-capture
regressions pass after adding the new sites. Current EWRAM is 257,632 bytes and
ROM payload 27,287,096 bytes before 32 MiB padding. This is targeted native
integration coverage; full route traversal, legendary difficulty/catch-rate
balance and the separate remaining campaign/release gates remain outstanding.

## 36. Kanto colleague friendlies, Elite Four rematches and rank obedience

Implemented November 3, 2026. Battle rewards, current-rank rosters and relocation
details below are delegated implementation choices.

- Brock, Misty, Lt. Surge, Erika, Koga, Sabrina and Blaine offer optional native
  friendlies in their original Gyms. When the player's or rival's town replaces
  Celadon/Fuchsia/Saffron, its displaced Erika/Koga/Sabrina appears at GLA HQ
  instead. Both displaced leaders are present for all six town pairings.
- Indigo's lobby now has an optional Elite Four rematch coordinator. Lorelei,
  Bruno, Agatha and Lance can each be fought independently using current-rank
  rules. This does not reopen the old Elite Four/Champion/Hall-of-Fame ending,
  and does not substitute for milestone inspections or the final Oak challenge.
- All eleven opponents use their traditional type, current Gym level cap and
  official team allowance. They use actual standard species data, native moves,
  normal abilities and a developing classic ace. Remaining partners come from
  their rank/type catalog, with distinct species when possible and no reserved
  research/exclusive species. IV tuning is 16 plus rank; rank-4+ aces hold Sitrus.
- These battles use actual registered party/PC identities, existing temporary
  over-cap normalization, normal Pokemon EXP, healing and traveling-party
  restoration. Results do not change Gym XP, rank, defense/loss records, queue
  or activity. Normal prize/blackout/Pay Day money is suppressed.
- Each opponent permits one victory per rank; losses can be retried. Persistent
  per-opponent rank masks prevent repeat and demotion/re-promotion farming.
  First Kanto-leader wins pay 400 times rank; Elite Four wins pay 600 times rank.
  Both grant 2 Town Reputation. Each Elite Four victory also earns a Standard
  voucher fixed to the rank at which it was earned. A full account preserves
  that voucher in a pending mask; speaking to that member's coordinator entry
  collects it when space exists, without repeating cash or battle rewards.

A real battle exposed two inherited-rule leaks. Legal current-cap partners could
disobey because the career has no vanilla badges, and generated opponents could
automatically use battle gimmicks because they bypass normal trainer-generation
blocking sentinels. Within-cap partners now obey in Gym Leader mode independent
of badges/met-level/trade ownership. The career battle adapter sets the ordinary
AI no-Dynamax/no-Tera sentinels on generated opponents. Controlled career battles
also refuse Dynamax on both sides. Other modes retain their existing paths.

Save version 9 appends eleven rank-win bytes and four pending-voucher bytes,
aligned after the exact previous 1,356-byte prefix. State size is 1,372 bytes
inside the unchanged 3,200-byte union. Version-8 migration preserves every prior
byte except the version marker, including the former tail padding; it clears
only the new masks. A newly avoided migration hazard: saved rival starter,
results and friendly masks reset only when upgrading saves older than version
8, not on every future version change.

Native tests pass all eleven opponents at ranks 1/4/8 for traditional typing,
distinct species, current caps/team limits, normal abilities, native moves and
reserved-species exclusion. A controller-played Brock win uses three supplied
level-15 Torterra with Giga Drain, without altered enemy HP or injected outcome;
it verifies decline, invalid registration refusal, one-time cash, healed real
identities, rank/queue/record/activity neutrality and repeat refusal. Direct
ARM fixtures exercise every opponent's loss/retry and independent per-rank
settlement, pending Elite voucher/full-account/earned-tier/flash behavior and
demotion guards. Real battle calls verify cap obedience without badges or the
temporary debug bypass and reject automatic opponent gimmicks. Actual League
menu decline/cancel and both relocated leaders across all town pairings pass.

Exact version-8 prefix and native flash tests preserve seeded prior rival,
tutor, exclusive, bird/titan, field, resident and Cyclizar-study records while
initializing new masks. Host ASan/UBSan state scenarios pass with the new size.
Registered defense/PC/EXP and all eighteen rival specialties/actual rival
battle/legacy-ending guards regress successfully after the battle-rule fixes.
Tutor/PC teaching and Paradox capture/study regressions also pass the version-9
integration. These remain targeted integration checks, not all-eleven-opponent
difficulty playthroughs or a completed campaign.

Linked EWRAM is 257,640/262,144 bytes; ROM payload is 27,290,664 bytes before
padding to the existing single 32 MiB ROM.

## 37. Persistent Rocket town incidents

Implemented activity-triggered street, habitat and museum incidents in the player's rebuilt hometown. Rank 2, population 6 and two constructed buildings unlock incidents. Active incidents reduce revenue; active available staff Potential reduces the required fights, with at least one fight retained. Native contact placement, refusal and settlement work in all three hometowns.

These informal town defenses accept a healthy traveling party of up to six Pokémon of any type without Gym registration. Normal Gym defenses still require the registered legal team. Town battles preserve identity and earned EXP, restore/heal the traveling party, and do not change official queues, rank, defense records or activity. Generated opponents use ordinary abilities and native moves, with reserved research species excluded and career battle gimmicks disabled.

After the final fight, the player can defer the persistent decision, turn Rocket over to the GLA for cash/Honor/reputation, or accept hush money at an Honor/reputation cost. Museum heists can succeed independently of a battle victory. Missing artifacts remain recorded; after three cleared incidents at rank 7, a later Giovanni raid can recover them through the honorable resolution. These are abstract museum artifacts, never confiscated player Pokémon. Resident dialogue exposes the persistent security record.

Save version 10 appends the incident state after the exact version-9 prefix. State size is 1,392 bytes inside the unchanged 3,200-byte union. Native flash tests preserve pending decisions, theft/recovery records and the exact prior prefix, including colleague wins and pending vouchers. Host ASan/UBSan scenarios pass activation, staff assistance, retry and one-time settlements.

A controller-played native street victory uses an unregistered mixed-type six-member fixture party without changing enemy HP or injecting its result. Native checks cover decline/unhealthy refusal, deferred choice/reload, settlement cash and contact removal, loss/retry, all four roster kinds at ranks 2/5/8, theft persistence and later boss recovery. Direct ARM fixtures cover the latter outcomes; they are not full battle difficulty playthroughs. All-three-town contact/collision checks and registered defense, resident and colleague regression suites pass.

Linked EWRAM is 257,664/262,144 bytes; ROM payload is 27,296,108 bytes before 32 MiB padding. This completes the town-incident slice, not the wider rival/Rocket narrative, remaining research, competition/history systems or full campaign validation.
