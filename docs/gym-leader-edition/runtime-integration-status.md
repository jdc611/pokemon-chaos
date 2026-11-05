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

## 38. Regional Cups, Kanto brackets and permanent archives

Implemented eight-entrant single-elimination Regional Cups and Kanto Championships through a native GLA event desk. Activity-based notices give 64 activities for entry; later scheduling varies by seeded gameplay activity. Every third event is Kanto, including before player eligibility. Regionals require Rank 2; Kanto requires current Rank 6 and a retained Regional victory. Player entry preserves the rival and reigning NPC champion. A reigning player receives eligibility under the same current-rank rule and must enter to compete. NPC events proceed without the player; entered player rounds wait between matches. Brackets and NPC outcomes are deterministic, not reload-rerolled, and the rival is not forced into the final.

Competition battles use the actual registered party/PC identities and existing official type, size and current-cap checks. Traveling-party restoration, healing, earned EXP and demotion normalization use the career battle adapter. Event results do not consume Gym challengers or alter official defenses, losses, rank XP or activity. Regional victory permanently retains qualification. Kanto reigning status changes on each final; former titles remain counted. Money/reputation rewards and one pre-event 500-Pokédollar winner wager settle once. A winning wager returns 4,000 total; entry must precede betting so wagers cannot replace their selected entrant.

Native results pages preserve entrants, quarter-finals, semi-finals and final winners in append-only records. Championship teams are read-only snapshots of species/form, nickname, level, personality and shiny appearance flag; no usable Pokémon are awarded from history. The current Hall viewer shows nickname/species/level/shiny status through native messages; a full visual Hall presentation is still polish work. NPC winning teams use the same deterministic native colleague/rival roster generators. Player snapshots use the actual final-match team.

Permanent storage is finite: 128 bracket records in the unchanged SaveBlock1 reservation and 32 Kanto winning-team snapshots in the two legacy Hall flash sectors. Full archives stop new events instead of deleting older records. Each archive generation fits one checksummed sector, alternates banks, verifies writes, and retains the previous generation. A final remains pending until the team archive succeeds; main-save rollback can ignore an orphan newer archive entry and safely reuse that next slot. These are concrete hardware limits, not a promise of unlimited history. Version 11 appends competition data after the exact 1,392-byte version-10 prefix; total state is 3,072/3,200 bytes. Other game modes retain their normal Hall of Fame path.

Kanto titles earn a choice of eight reusable TM licenses: Earthquake, Ice Beam, Thunderbolt, Flamethrower, Psychic, Shadow Ball, Energy Ball and Dazzling Gleam. Bag-full failures retain entitlement; duplicates cannot consume another title. Exhausted licenses convert subsequent title prizes into 5,000 extra Pokédollars, including previously uncollected excess titles. Research bits 209–216 hold licenses; 217–222 hold three cosmetic trophy choices. Regional/Kanto collections are permanent, swaps are free, and Landmark tiers 1/2 unlock the center/right displays. Native statue objects occupy preset Gym positions and provide no battle boosts.

Host ASan/UBSan checks pass three-round player victory, elimination, rank qualification, deterministic unattended events, wagers, one-time settlements, pending archival and full-capacity history preservation. Native tests pass exact version-10 prefix migration/flash, event menu and results navigation, missing-registration refusal, all three registered-team round adapters, Kanto archival/reload, independent NPC championships, native prize selection/reusable licensing/duplicate refusal, and trophy placement/upgrade gating/flash. A controller-played quarter-final against Brock uses three supplied level-25 Torterra with Giga Drain without altered enemy HP or an injected outcome. Remaining round and archive fixtures inject outcomes to test integration; they do not certify tournament difficulty. An invalid alternate archive bank preserves the valid prior teams. This is not a real-device power-cut test.

Registered defense/PC/EXP, all colleague rematches and Rocket incident regression suites pass the integrated version-11 ROM. Linked EWRAM is 257,676/262,144 bytes; payload is 27,309,424 bytes before 32 MiB padding. Town showcase/defense-format competitions, richer news/recognition, hometown celebrations and a visual Hall viewer remain unfinished competition work alongside the remaining research, rival narrative, final Oak battle and full campaign validation.

## 39. Swords, Echoes and Creation Trio field research

Appended thirteen universal field sites without renumbering the previous eighteen: Cobalion, Terrakion, Virizion, Keldeo; Raikou, Entei, Suicune, Raging Bolt, Gouging Fire, Walking Wake; and Dialga, Palkia, Giratina. Exact maps, coordinates, ranks and seal answers are recorded in field-research-sites.json as delegated implementation choices. All are excluded from ordinary species/voucher pools through the existing special-species reservation.

Swords of Justice records three independent teachings before Keldeo's training-pool encounter opens. Echoes of the Past links each modern beast's field discovery to its own ancient counterpart; the six captures remain independent, with no evolution or consumed partner. Fractures in Time and Space links temporal evidence in Pokemon Tower 7F and spatial evidence in Seafoam B3F to Giratina's distortion seam in Lost Cave Room 10. This currently uses the existing cave room and native field presentation, not a finished custom Distortion World dungeon.

Native GLA notebooks provide locations, clues and per-species progress for all three groups. Exploration is remembered at early ranks, earned access survives demotion, encounters scale to the current cap, and each site offers the established save/battle confirmation flow. Failed encounters retain their opened seal and can be retried; only an actual caught outcome records exhaustion. No other capture is a prerequisite. This uses the existing capture/discovery/seal reservations, with 31 of 35 slots occupied, and leaves save version 11/state size unchanged at 3,072 bytes.

Native tests pass all thirteen template placements and dry walkable approaches, avoiding original objects/warps/events; early discovery/rank refusal; independent Keldeo/ancient-pair/Giratina evidence gates; controller-driven wrong/correct seal menus and save/battle declines; generated species/current-cap checks after demotion; independent outcome/retry/anti-duplicate ledgers; and native flash persistence. The new species' caught outcomes in this targeted suite are injected integration fixtures, not thirteen played capture battles. Regression suites pass the previous fifteen research sites, actual Uxie capture, all five original notebooks, and actual Cyclizar capture/party-and-PC study plus Koraidon/Miraidon research.

Linked EWRAM remains 257,676/262,144 bytes; ROM payload is 27,313,128 bytes before 32 MiB padding. Four Winds roaming/weather research and conditional form projects remain, along with the broader rival/Rocket narrative, competition presentation, final Oak battle and campaign validation.

## 40. Oak's final examination and native ending

Oak now returns to his actual Pallet Town laboratory after the opening, through a Gym Leader-only object-script override. His final examination requires current Rank 8, no pending inspection and at least one retained Kanto Championship title. Reigning status is not required. Early visits explain the career requirements; declining or bringing an invalid registered team cannot consume the challenge.

The final battle uses the registered Gym team, current Rank 8 cap/team limit, actual party/PC identities, normal career healing/restoration and the no-Dynamax/no-automatic-Tera rules. Oak brings six level-100 partners: Tauros, Exeggutor, Arcanine, Blastoise, Venusaur and Charizard, with ordinary abilities, trained stats, native moves and held items. His roster covers the three Kanto starters and classic Oak partners. Losses remain retryable without rank, queue, official-record or money penalties. Victory awards the actual Earthbadge/game-clear flags, 20,000 Pokédollars and reputation once; career play remains open.

Final completion occupies reserved research bit 223, following championship licenses and cosmetic choices, without increasing state size or save version. Repeat visits retain recognition and offer credits without re-awarding the battle or stipend. Oak comments on current Honor before the examination. Other game modes retain the original Oak script.

The ending enters FireRed's existing credits directly after a successful normal save and map-view capture. It deliberately bypasses the legacy Hall of Fame writer because those sectors hold permanent Kanto team archives. The player can defer credits, and a failed save does not intentionally enter the resetting credits sequence. Credits return through the native title-screen reset; continuing resumes the saved career. This reuses the original FireRed staff presentation, not a newly authored Chaos credits sequence.

Native checks pass lab placement and rank/title gates, decline, missing registration, all six native species/levels/normal abilities/moves, loss/retry, one-time settlement, final Badge/game-clear flags, original healed identities and unchanged rank/queue/official/activity records. A full controller-played Oak victory uses six supplied level-100 Meowscarada, normal Overgrow, Life Orbs, Attack/Speed training, Jolly mint effects, native maximum PP and compatible Brick Break/Knock Off/Thunder Punch/Flower Trick. Battle presentation uses native FAST text, SET style and animations off; no enemy HP or battle result is injected for this victory. Two earlier supplied Grass-team strategies lost; this is one successful integration fixture, not an all-type difficulty certification or a full campaign playthrough.

The native credits entry succeeds after saving. Byte-for-byte checks preserve both complete Hall flash sectors across final settlement and the credits save. Registered defense/PC/EXP and all eighteen rival specialties, real rival battle, migration/flash and legacy E4/Champion/Hall-of-Fame protection regress successfully. Linked EWRAM remains 257,676/262,144 bytes; ROM payload is 27,316,028 bytes before 32 MiB padding. Four Winds, broader rival/Rocket stories, remaining competition presentation and campaign/species/device validation still gate the polished release.

## 41. Four Winds weather surveys and persistent roaming

Added four native scientist weather stations on Routes 9, 11, 23 and 18 for Tornadus, Thundurus, Landorus and Enamorus, with delegated survey ranks 3/4/5/6. Each independent seal releases its wind to roam Kanto; no other capture or completed wind is required. The GLA Four Winds notebook reports each signal and lets the player choose which to track or help a fainted wind recover. The native PokeRider/Fly map displays the selected species and current route in its existing caption window.

The engine still has one native encounter slot. Four separate saved individuals are multiplexed through it: each retains IVs, personality, shiny appearance, HP, status and route. All released uncaught records can move on map transitions, while the selected signal supplies actual native roaming encounters. Changing the tracked signal does not reroll a Pokemon. Old generated roamer visuals are removed when changing individuals so one native index cannot retain another wind's sprite. Movement uses Kanto routes, caches duplicate/same-map loads, leaves an encounter opportunity on arrival, and sometimes moves other signals between maps. Fleeing moves that wind while preserving its condition. Current-cap changes reconstruct its stats and scale retained HP proportionally.

Native catches exhaust the corresponding independent research allocation. Defeat/fainting records a resting wind; the notebook restores its health with the same identity and shiny result. The original Roar/player-teleport deactivation hazard is guarded for active winds. Ordinary non-Gym-Leader native roamer creation and location behavior retain their original path. Legacy unrelated roamer releases are refused inside Gym Leader mode. Weather stations cannot start duplicate static legendary battles.

Save version 12 appends four 20-byte records plus four bytes of selection/map-load metadata after the exact 3,072-byte version-11 prefix. State size is 3,156/3,200 bytes; previous registration, research, competition, license, trophy and final-Oak data remain unchanged. Four final field slots use the previously reserved capture bits 126–129, discoveries 161–164 and seals 196–199; all 35 field reservations are now assigned. Linked EWRAM remains 257,676/262,144 bytes, with a ROM payload of 27,320,672 bytes before 32 MiB padding.

Native tests pass exact version-11 prefix/flash migration, all four weather placements and controller-driven seal/decline/release flows, independent uncaught signals, static-battle refusal, all four reconstructed species/IVs/personalities/shininess/normal abilities, Kanto locations, HP/status retention through switching, Roar survival, faint/recovery without reroll, current-cap access after demotion, duplicate-load/arrival location retention and flash. One real Tornadus roaming encounter is placed deterministically on the test route, then caught through the native battle Bag with a Master Ball; its actual party delivery, original identity, exhaustion and save/reload pass. No caught outcome is injected for this played capture. Other condition/outcome checks are explicit fixtures, not four complete played hunts.

The selected signal is visibly readable in the native Rider screen. A non-Gym-Leader fixture still creates the old native Latias roamer without changing any stored wind record. Version-12 regressions pass the thirteen Swords/Echoes/Creation sites, the Regional/Kanto bracket/archive/reward fixture, and a full played final-Oak victory, Badge, normal save and native credits with both Hall sectors preserved. These do not certify all-type balance, every supported species' acquisition method, or a complete physical-device campaign.

## 42. GLA rival doubles and permanent trainer bonds

The GLA now has a native rival-double coordinator. Optional doubles use the actual registered team, current cap and team limit, require at least two registered partners, and share the rival's generated roster rules. Native trainer/DOUBLE flags produce four battlers; Dynamax and automatic opponent Tera stay disabled. Rank, official records, challenger queue and activity remain neutral; the original traveling party is restored and healed. Singles and doubles retain independent once-per-rank eligibility. A first doubles victory grants 2,000 Pokedollars per rank, four reputation and a Standard/ Rare/ Elite voucher at ranks 1–3/4–6/7–8, retaining the earned rank. Full wallets preserve a pending entitlement for later collection, including after demotion; repeated visits and victories cannot duplicate it. Ledger bits 224–231 store victories and 232–239 pending vouchers.

Staff office and individual trainer actions now include Personal request. A stable trainer-specific story asks for three Gym-filter appearances (wins or losses), a town Dojo, a House/Hotel with population twelve, or a resolved hometown Rocket incident. These are delegated implementation choices within the approved staff/side-request design. Unmet requests explain the goal; complete requests offer a native yes/no reward. Acceptance gives 1,500 Pokedollars, five Honor, three reputation and 25 loyalty, clears warnings and establishes the existing personal-bond protection from poor-season departures. Potential remains intrinsic and unchanged. Away trainers must return before completing a request.

A permanent 256-bit trainer ledger prevents dismissal/rehire reward farming. Rehiring still requires willingness, room and the signing fee, then restores the personal bond and at least 80 loyalty without repeating its reward. The generated 206-entry original-trainer source catalog is now append-only, retaining retired rows and asserting its 256-entry capacity. Save version 13 appends the 32-byte ledger after the exact version-12 3,156-byte prefix; migration seeds existing personal bonds. State size is 3,188/3,200 bytes. Ordinary away-trainer persuasion now varies with each eighty career activities instead of permanently rejecting an identity; a qualified Rank-8 leader cannot be blocked by the random roll. Existing rank, reputation and notorious/Ace requirements remain.

Native tests pass an actual controller-driven four-battler rival victory, registration refusal, decline, gimmick restrictions, party identity/healing, official neutrality and separate singles/doubles rewards. Explicit settlement fixtures cover loss/retry, higher-rank payment, full-wallet pending Elite voucher, demotion, anti-repeat and flash reload. Personal-request tests pass exact version-12 prefix migration; all four unmet gates and completions; actual office decline/accept; native Dojo/House construction; unchanged Potential; one-time claims, paid rehire with restored bond, permanent dismissed history and native flash. Return fixtures cover stable same-window refusal and qualified Rank-8 recovery. These targeted checks do not constitute a full campaign or all-type balance certificate. Build payload is 27,324,968 bytes; linked EWRAM remains 257,676/262,144 bytes.

## 43. Postgame cross-type field research and first linked species inventory

After Oak's final examination, the four original exclusive encounter sites host native field researchers. Each offers the existing eighteen-type selector, followed by the normal save/approach/capture flow for that type's assigned partner at the current site. This implements the approved postgame access guarantee for the other Gym types' exclusive assignments. It is earned through the permanent Oak completion flag, remains accessible after demotion, and uses the current cap. The existing 72-bit allocation ledger remains authoritative: partners already caught during the Gym career cannot be caught twice. Researchers remain after a capture, and caught own-type objects no longer hide the postgame research access point. Oak explains these locations in both his victory and completed-career dialogue. No save layout or version change is needed.

Native tests first rerun all 72 original encounter fixtures plus an actual pre-ending Glastrier capture. Postgame checks then exercise all 72 type/location selections and generated identities, retained own-type exhaustion, current-cap encounters at Rank 1 after an explicit ending/demotion fixture, independent retries and pre-ending refusal. Controller input cancels and selects the native type menu, plays a real other-type Meloetta encounter with Master Ball capture, verifies party delivery, reuses the resident researcher, rejects the captured allocation and preserves both captures through native flash. Ending progress is supplied for this targeted access test; this is not a complete campaign playthrough. Build payload is 27,325,608 bytes; linked EWRAM remains 257,676/262,144 bytes.

The new native species inventory reads actual linked species and voucher pool membership across all eighteen types and three ordinary voucher categories at tier eight, grouped by National Dex. It finds 1,025 supported Dex numbers and 1,245 exact species/form voucher candidates. Five Dex entries currently have neither a voucher candidate nor a dedicated encounter/placement in the inspected source: Cosmog, Cosmoem, Poipole, Meltan and Kubfu. This is a candidate/source inventory, not a completed availability proof: ordinary wild tables, reachable gifts/evolutions, all-type access, forms and deterministic availability still need checks. A family-level reservation review is also necessary because exact-form reservations can leave alternate forms in voucher pools. No full-species or release gate is marked complete.

## 44. Nature's Gratitude, rare lineages and form-family reservations

Nature's Gratitude now supplies Shaymin's separate nature quest. Its Route 25 garden is discoverable early; earned Rank 6, population twelve and an owned Grass habitat upgraded to tier two permit its seal. Any Gym type can construct that habitat. Native notebook guidance, a three-choice gratitude lesson, pre-battle save/decline flow, current-cap wild battle, retry and permanent capture are integrated through the existing field-research system. This is an implementation-authored quest under the approved complete-build discretion, not a recovered exact location or numerical design. It appends site 35 without renumbering the old sites: capture/discovery/seal use free ledger bits 240/241/242, avoiding the original 35-slot bit ranges. The previously unused Form research notebook label becomes Nature's Gratitude; conditional form projects remain proposed rather than claimed implemented.

The GLA research notebook also offers Rare lineages. After Oak's final examination, every Gym type may receive one level-five Cosmog, Poipole, Meltan and Kubfu through independent one-time nursery placements. Gifts use ordinary abilities, cost no money, deliver to party or PC, and consume their allocation only after successful delivery. Full storage retains the placement. Bits 243–246 record these four deliveries; moving or releasing a partner cannot repeat them. Cosmoem is reached through Cosmog's existing native level evolution. This closes the five basic missing-source entries found by section 43 without altering the approved seventy-two exclusive assignments.

Reservation checks now compare native form-family identities. Alternate Shaymin, Keldeo and other reserved forms can no longer leak into ordinary starter/voucher/habitat pools merely because their exact numeric species differs from a reserved form. Custom exclusive species retain their own existing form identity. The generated catalog contains fifty-four exact research reservations, with no exact overlap with the seventy-two assignments. The linked inventory now finds 1,200 exact voucher candidates and a candidate, dedicated source or linked evolution route for all 1,025 supported National Dex numbers. A complete per-Dex snapshot is recorded in species-candidate-inventory.json. Evolution candidates are expressly separated from validated evolution conditions; map, wild/gift reachability, deterministic access and all-form requirements remain unverified, so the full-species release gate remains false.

Native tests pass Shaymin's dry floor/approach and original-event avoidance; early discovery, rank/population/habitat gates, native construction/upgrade, earned access after demotion, wrong/correct seals, battle decline, a real Master Ball capture and exhaustion. Nursery checks pass the ending gate; actual menu decline/accept; all four independent party/PC deliveries; no charge or repeat; native Cosmog-to-Cosmoem evolution lookup; flash persistence; and full-storage refusal followed by delivery into a freed PC slot. A missing script wait on the new nursery menu was corrected during testing. Regression passes the thirteen Swords/Echoes/Creation sites and the full native supply/Type: Null/Silvally/Slaking/storage suite. The supply fixture now reloads field objects after raw LoadGameSave before walking; this matches normal field re-entry instead of mixing restored coordinates with stale live objects. Save version 13 and state size 3,188/3,200 bytes are unchanged. Linked EWRAM remains 257,676/262,144 bytes; ROM payload is 27,327,628 bytes before padding.

## 45. Native FireRed Championship Hall team artwork

The Championship Hall now offers View team alongside older/newer archive navigation. It opens FireRed's actual Hall of Fame PC renderer with the selected archived lineup, including front sprites, stored personality/gender, nickname, level and shiny appearance. Up/down selects each of up to six partners; A or B closes the selected team and resumes the event desk's native field script. The header identifies the archived champion and Kanto competition. Earlier text pages remain available for event context and team browsing.

A narrow read-only entry point in the existing FireRed renderer accepts a copied team and a return callback. It bypasses native Hall-of-Fame save loading/writing, so the separate alternating Championship archive sectors remain authoritative and unmodified. The copy validates archive identity/generation/count, champion, party size, species and levels. Original trainer IDs were not retained in the 18-byte Championship mon format; the archive view therefore hides that field and explicitly labels shiny partners rather than inventing an ID. Original non-archive Hall viewing retains its existing path. The PC initializer's redundant scratch-team allocation was also removed so the already allocated team is not leaked.

Native tests rerun the full competition suite, including a controller-played Regional quarter-final, registered-round fixtures, prize/TM/trophy behavior, alternating flash banks and NPC championships. A further native Rank-8 NPC circuit supplies a six-partner winning team. A known shiny flag and ten-character nickname are explicit archived-display fixtures. Controller tests inspect all six partners, older player teams, repeated opening, A/B exits and return to the event menu. PID/species/nickname/level/shiny data match the stored snapshot. Both archive sectors, the traveling party and career state remain byte-for-byte unchanged by viewing; a subsequent normal save/reload preserves the archives. Rendered native screenshots verify the champion header, highlighted sprite, nickname/species, level and shiny label. These are targeted renderer/save checks, not a physical-device campaign certificate.

No save schema change is required. Linked EWRAM is 257,720/262,144 bytes; ROM payload is 27,328,884 bytes before 32 MiB padding.

## 46. Deterministic postgame placement directory

The GLA research notebook now includes Placement directory. Oak's completed final examination unlocks all eighteen types permanently, including after demotion. The native species lists offer explicit choices rather than random voucher draws: 973 eligible canonical species and regional forms, excluding assigned/research form families, battle-only forms and normal placements already recorded. A selected partner can be inspected in the existing FireRed Summary screen before a separate yes/no payment confirmation. Identity and preview stats remain stable across visits to Summary. Placements arrive at level five with ordinary abilities and fixed fifteen IVs, distinguishing this completion route from developed reward vouchers. Delegate-authored fees are 1,000 plus ten times BST for ordinary species, or 50,000 plus twenty times BST for legendary/mythical/Ultra Beast/Paradox candidates; Honorable standing applies the existing ten-percent GLA discount policy.

The directory shares the normal voucher claim ledger. A partner already claimed by an ordinary voucher cannot be placed again, and a directory placement cannot later be duplicated by an ordinary voucher. Viewed normal vouchers keep their promised candidates: those candidates cannot be taken through the directory while the voucher holds them. The separate shiny ledger is not consumed. Assigned and universal quest partners remain on their dedicated routes. Cosmoem is explicitly reserved with the Cosmog nursery lineage, bringing exact research reservations to fifty-five. Cash and the claim bit commit only after successful party/PC delivery; declines, insufficient funds and full storage preserve both. No new save bytes are used.

Long native list IDs now skip both the B-cancel sentinel 127 and the error sentinel 255; Safari and directory index decoding follow the same skipped values. The directory frees its temporary species list on cancellation, type changes and exit. Native tests browse every actual type list, inspect all entries' type/reservation safety, select a real option across index 127, and verify list cleanup. Controller tests cover cross-type Squirtle placement, native Summary, stable personality, insufficient-funds retry, explicit decline, exact fee, party delivery and ordinary/shiny ledger separation. Direct transaction fixtures cover both directions of voucher exclusion, held-voucher protection, anti-repeat, full-storage refusal, a freed PC slot, native flash and field return. Safari restoration/purchase/cancel/wild-generation and the full gratitude/nursery suites regress successfully with the expanded eighteen-entry notebook.

The per-type directory-access snapshot and updated per-Dex source inventory bind to ROM SHA-256 eb42e0b8c6678751a9d6f154a07f58dd517f7af8567c43c24904ed979a3414f5. Across all 1,025 supported National Dex numbers, there are no remaining entries lacking a deterministic directory source, dedicated encounter/placement, or linked evolution from those roots. Cosmog-to-Cosmoem is also checked through the native evolution lookup. This is a source/access inventory: complete physical travel/campaign paths, every alternate form and evolution condition, and curated world ecology are not all certified. The release/full-species gate therefore remains false. Save version 13/state size 3,188 bytes remain unchanged; linked EWRAM is 257,728/262,144 bytes and ROM payload is 27,331,904 bytes before padding.

## 47. Championship homecomings and Kanto Gazette recognition

Returning to the rebuilt hometown after a new Kanto title now automatically starts a native field celebration. The first welcome includes townspeople, an active recruited trainer when available, a fanfare and a longer congratulatory scene. Later welcomes are shorter. Two native championship memorial statues remain beside the Gym, with the doorway's approach clear. Temporary celebrants leave afterward and cannot respawn through lazy object spawning. Multiple titles earned before a homecoming are honoured together, avoiding a backlog of repeated scenes. The last celebrated count uses free ledger bits 247–252; the finite thirty-two-team championship archive keeps this six-bit counter within range. No save layout/version change is needed.

An active Rocket incident postpones the celebration without consuming it. Resolving the incident refreshes the town and releases the pending welcome; visits to other maps cannot consume a hometown celebration. Scenes award no extra money, reputation, Honor or rank progress. Normal saving preserves the completed welcome. Native town residents also recognize current and former champions. Honor changes the warmth/caution of their reaction; losing the reigning title does not erase the player's championship history.

The Kanto Gazette is available from a native GLA newsreader and selected ordinary civilians in ten Kanto towns/cities. Script replacement is limited to named civilian dialogue, preserving the time/tool giver, tutors, Mega-reward Oak and other special interactions. Rebuilt player/rival towns keep their own resident scripts and recognition. Five native news pages read the actual reigning/retained titles, most recent Regional result, hometown population/reputation/Honor, independent rival career and persistent Rocket/artifact journal. Reading news does not mutate career state or grant rewards.

Controller-driven native tests pass first and shorter later welcomes in all three hometowns, an active Rick representative, statue/approach placement, temporary-actor removal, no-repeat behavior, flash persistence and exact reward/rank neutrality. Further fixtures verify Rocket postponement/resolution, home-only consumption, subsequent titles, current/former champion and all three Honor reactions. Actual loaded templates integrate recognition in ten Kanto locations, while selected tool/tutor/Oak scripts retain their original pointers. The GLA newsreader's menu, all five pages, cancellation and read-only state pass. Native screenshots confirm the homecoming scene and Gazette presentation. Final linked EWRAM is 257,824/262,144 bytes; ROM payload is 27,336,796 bytes before padding. Save version 13/state size 3,188 bytes remain unchanged. Town hosting/showcase/defense competition formats, broader campaign/ecology and full release validation remain work in progress.

## 48. Hometown hosting, invitations and physical competition venues

Implemented under delegated balancing: the GLA hosting director offers town showcases, Gym defense doubles, Regional Cups and Kanto Championships through native menus. Applications and earned invitations are separate from entering a bracket: a town may host while NPCs compete. No preparation errands are required. Invitations can be declined without losing money, reputation or Honor; ignored invitations expire into a rival-hosted event. Rival bids compare actual facilities/population/standing against rival development, so reopening dialogue does not reroll approval.

Qualification thresholds (rank/population/reputation) are showcase 2/9/60, defense doubles 3/12/80, Regional 4/18/120 and Kanto 6/24/180. Showcases require a Dojo, Hotel, Park or Restaurant; doubles require a Dojo; Regionals require a Dojo and Hotel; Kanto requires individually tier-2 Dojo and Hotel plus a tier-2 landmark. Active Rocket incidents block new bookings. Fees are 3,000/5,000/8,000/15,000, discounted 10% at Honor 100. Cancellation, insufficient funds, rejected rival bids and repeated applications do not charge. Approved bookings charge exactly once.

Projected visitor income is shown before approval. Actual income uses population, reputation, Hotel/Restaurant tiers and format; it is capped at twice the base fee and halved during a Rocket incident. Hosting can make a loss. Settlement pays once after the event, with a small format-scaled reputation reward; no extra rank progress is granted. Kanto settlement waits for the existing championship archive transaction. Existing entrant/bracket/history/team archive sizes are preserved by packing venue/format metadata into the existing host byte. Legacy records remain HQ competitions. The new eight-byte hosting tail migrates save version 13 to 14 without changing the preceding 3,188-byte state; total state is 3,196 of 3,200 reserved bytes.

Booked events use their physical hometown, with free native travel to safe ground at (10,9) and a coordinator at (11,9), leaving the Gym door clear. HQ cannot start a booked hometown match. Defense events use actual four-battler doubles and require at least two registered Pokemon. Showcases and Regional/Kanto events use the existing singles bracket with their distinct host format, qualification and economics; showcase is not a newly invented contest minigame. Event notices and archived history display the format and venue. The coordinator is refreshed after field return, never by modifying field objects while battle graphics are active.

ASan/UBSan tests pass qualification, stable rival bids, generated invitations, acceptance/decline/expiry, atomic fees, financial risk, unattended events, Kanto archive settlement, single payment and saturation. Existing state, Rocket and competition core scenarios pass. Native controller tests pass in Saffron, Celadon and Fuchsia: exact v13 migration, director menus, insufficient funds, cancellation, one-time doubles booking, travel/coordinator, wrong-venue and one-registration refusal, native flash persistence, and a real four-battler quarter-final victory without injected outcome or modified enemy HP. Each hometown then checks the remaining round adapters with explicit result fixtures, one-time visitor settlement and immutable history metadata. Existing native Regional/Kanto registration, battle, prize and alternating flash archive regression tests pass. These are targeted fixtures, not a full campaign certification or controller-played Kanto hosting run.

The linked ROM payload is 27,343,184 bytes before padding, EWRAM is 257,824/262,144 bytes and IWRAM is 29,076/32,768 bytes. Padded ROM SHA-256: `4db2a5f39a4c293c1fbf1d8990ec676dcf9953c64a0a0ab46635fe0992efe820`. Hosting is now a tested checkpoint. Curated world ecology, physical travel/inherited-feature audits, remaining Gym upgrade/puzzle coverage, all-type balancing and complete campaign/release validation remain open; no finished V1 ROM is declared.

## 49. Shared world ecology and Gym Rank-safe DexNav

Ordinary Gym Leader wild encounters now use a shared deterministic resolver even when the inherited randomizer setting is off. The ROM catalog contains 973 canonical/regional placement roots from the linked species inventory; runtime filtering excludes legendary/mythical/Ultra Beast/Paradox species, reserved quest families, custom/battle forms and ordinary species above a current-rank BST ceiling of 320 + 45 × rank. This catalog is an input pool, not a claim that every root appears in a particular seed's finite encounter slots. Dedicated encounters, habitats, Safari and the deterministic postgame directory retain their own acquisition policies.

Biomes use actual map identity/type: Viridian Forest, Berry Forest and Pattern Bush favor Grass/Bug/Poison/Fairy; caves and Rock Smash favor Rock/Ground/Steel/Dark/Ghost/Fighting; Seafoam favors Ice/Water; Power Plant favors Electric/Steel; Mansion favors Fire/Poison/Ghost/Dark; all Mt. Ember paths favor Fire/Rock/Ground. Surf and fishing require Water typing. Route grass excludes purely Water and purely Ice species. Land has six seeded groups, Surf three, and the rods share one Old Rod/two Good Rod/two Super Rod groups with native slot weighting. Groups may overlap; they are not promised unique species. Seed, map, method, time and current rank determine slots without consuming gameplay RNG. Rank demotion therefore reapplies the current strength limit.

Actual wild generation, current/map-specific lookup, DexNav encounter lists/search levels and Pokédex area queries use the same resolver. Current configured habitat and Safari lists override ordinary ecology in their matching land/hidden contexts. Native local-species queries and type-attracting lead abilities also consult the shared slots. Inherited mass outbreaks cannot replace the curated GLE encounter. Other modes keep their existing behavior. DexNav's base level follows Gym Rank, and chain/rare level bonuses are capped at the current global Gym Rank cap.

A 64-entry result cache plus a compact rank-eligibility bitset avoids rescanning all reservations for every UI slot. The eight-rank native biome sample covers 272 distinct sampled species and finishes in 0.81 seconds in the emulator environment; this is a test-environment measurement, not a device performance guarantee. Cache keys retain seed/map/method/time/rank isolation, and no new save bytes are required.

Native checks pass all eight ranks across eight representative biome maps, independent type/BST/reservation assertions, deterministic results and unchanged RNG state. Further checks pass actual forest wild generation with randomization disabled, DexNav search-level membership and full-chain cap enforcement, aquatic Surf/rod grouping, seed changes/restoration, and the rendered native DexNav screen. Temporarily disabling GLE confirms ordinary non-randomized wild generation still uses its native table. Existing Safari restoration/menus/transactions/flash/wild adapter and hometown construction/healing/economy/save/whiteout/actual walking Habitat encounter regressions pass. Physical travel to every map, every time-of-day combination, an exhaustive world species census and complete campaign/device testing are not certified by these fixtures.

Save version 14/state size 3,196 bytes remain unchanged. Linked EWRAM is 258,720/262,144 bytes, IWRAM 29,076/32,768 bytes and ROM payload 27,346,928 bytes before padding. Padded ROM SHA-256: `62b4e04e899b5fae6f9ef2b4dff1a32b6b9e6ecfa2bf873bd9780737e92b1b4f`. Physical travel, inherited gifts/prizes and progression bypasses, Gym upgrades/puzzles, all-type campaign balancing and full release validation remain open.

## 50. Inherited Game Corner access and scripted-gift cap safeguards

The inherited arcade scripts already admitted Gym Leaders, but their C game/prize/refill/cosmetic handlers still required FireRed badge four. The prize-room scripts also retained that badge gate. A shared access check now accepts a valid Gym Leader career or the original fourth badge, so GLE does not need an invented badge or game-clear flag to use these services. Original-mode checks remain unchanged. Existing town landmark construction still controls the rebuilt Celadon building's physical entrance; this change is not a free landmark upgrade.

Arcade Pokemon prizes now respect the current GLE level cap and refuse reserved quest families before delivery or coin deduction. The ten existing ordinary prize species, prices, ability/shiny options and minigame rules are preserved. Both native scripted-gift adapters likewise clamp GLE player rewards to the current cap and refuse reserved families. The parameterized adapter copies its input template, preserving the caller's requested level and leaving enemy templates unchanged. Dedicated GLE research/nursery rewards use their existing guarded delivery paths. Ordinary-mode gifts retain their original levels.

Native controller tests pass the actual prize clerk with no original badge: normal Porygon choice/confirmation, cancellation without coins, paid delivery at the current cap, reserved-species refusal without delivery/payment, and flash persistence. They also pass the actual TM detail browser, Reflect purchase, duplicate-owned refusal, bag/coin save persistence, and real Memory cabinet launch/return without a stake or fabricated completion flag. These checks do not certify winning every inherited minigame. A separate fixture verifies the two scripted-gift adapters, input-template preservation, unchanged enemy levels and reserved refusals; the actual Eevee ball/givemon script, nickname cancellation, one-time flag and flash reload pass at the current cap. Temporarily disabling GLE preserves the ordinary level-25 gift and original badge requirement.

Save version 14/state size 3,196 bytes remain unchanged. EWRAM is 258,720/262,144 bytes, IWRAM 29,076/32,768 bytes and linked ROM payload 27,347,252 bytes before padding. Padded ROM SHA-256: `ef1556c8d55887aa0faf4daa3294e608ee79b0834b21b03064a42cdde463b7e9`. Remaining audits include actual Sevii ferry/research travel, other inherited story triggers, Gym layout/puzzle/theme controls, all-type campaign balancing and full release validation.

## 51. Native Sevii ferry access through the GLE travel permit

The travel audit found that GLA expeditions could reach selected Sevii research entrances, while the ordinary ferry still depended on the inherited Bill/Celio storyline. The GLE travel permit now provisions the Rainbow Pass and initializes the native Vermilion/full-Sevii destination state independently of the prior one-time permit bit. Existing careers with that bit already set receive the repair. Bill's mandatory One Island Center introduction is suppressed for this separate career. No badges, championship titles, legacy game-clear flag, quest rewards or landmark tiers are granted.

Native tests deliberately remove the new pass and reset the three inherited travel variables after a completed GLE opening, then verify permit repair and exact badge/game-clear neutrality. Controller input opens both ferry destination pages, selects Five Island, and runs the actual departure/sailing/arrival scene to Five Island Harbor. Pass and location persist through native flash save/reload. Entering One Island Center afterward does not restart the mandatory introduction. This certifies that transport path, not a walked route to every Sevii research chamber or every ferry pairing. Those physical paths and the broader campaign audit remain open.

Save version 14/state size 3,196 bytes and RAM budgets remain unchanged. Linked ROM payload is 27,347,316 bytes before padding; padded ROM SHA-256: `6a15a7d679426c69c572d991b88585ca8e6c5e49c5f9b29cd5f602ab2d4813f5`. The original ferry storyline remains unchanged outside GLE because permit provisioning exits unless a completed GLE opening is active.

## 52. All-type native opening matrix and research terrain access audit

A controller-driven opening matrix now passes all eighteen Gym types. Each independent fixture chooses its type through the native menu, inspects the first candidate's ability/profile/stats/moves in Summary, completes the three-member level-5 draft, checks official registration legality and the current cap, finishes the GLA tutorial and walks out/back through the actual doorway. Hometowns cycle Saffron/Celadon/Fuchsia across the eighteen fixtures. This is one fixed opening fixture per type, not every type-town/seed combination or eighteen complete campaigns. The reproducible native runner and ROM-bound results are committed separately from release certification.

The research terrain audit covers all thirty-six field sites. It checks source warp/connection graph reachability with explicit ferry-edge assumptions, then loads each native map, verifies the map identity and exact dimensions, and traces collision-grid components from actual inbound warp/connection entry tiles to the site's approach. Ordinary trainers are marked defeated as an inspection fixture. Native directional ledge rules resolve Route 9's wind-station approach; all thirty-six approaches connect. Early audit samples were discarded after discovering that short map-load waits could inspect the previous map's grid. The corrected run waits for loading and checks dimensions. No encounter placements were changed on the strength of that invalid intermediate sample.

This audit does not simulate every actor, elevation change, field action, puzzle condition or story prerequisite, and it directly warps to load maps for inspection. It therefore does not certify a complete walked campaign. The actual Vermilion-to-Five-Island ferry and targeted field encounter/controller checks remain separate evidence. Both audit snapshots bind to ROM SHA-256 `6a15a7d679426c69c572d991b88585ca8e6c5e49c5f9b29cd5f602ab2d4813f5`; no gameplay/save layout or ROM bytes changed for this checkpoint. Gym layout/puzzle/theme controls, broader progression/balance and end-to-end release validation remain open.

## 53. Native Gym layouts, optional switch trials, independent themes and upgrades

The Gym Guy now manages design through the existing FireRed menu system. Layout, puzzle and theme are independent: four native-map presets (Classic, Twin courts, Training ring, League arena), four puzzle choices (none, two switches, relay sequence, master sequence) and six palette themes (classic stone, forest, flame, ocean, mystic, steel). Reused native rock tiles and item-ball console markers keep the room within the existing tileset/sprite system. No puzzle is required for a valid Gym. Presets preserve trainer stations, the guide and doorway.

Three permanent paid upgrade tiers unlock at earned ranks 3/5/7 for 5,000/15,000/30,000. Layout changes cost 1,000, puzzle changes 1,500 and themes 750; Honor 100 discounts these fees by 10%. Upgrade ownership and installed choices survive demotion. Locked selections, cancellation, insufficient funds and duplicates do not charge or overwrite a design. Gym upgrades also expand cosmetic trophy slots while preserving the prior landmark-based entitlement. Removing a puzzle is a separate paid design change and preserves layout/theme/upgrades. Puzzle filtering adds a modest 2/4/6 percentage points to active staff chances and never directly grants rank XP.

Trial progress is temporary and resets on re-entry. Two-switch puzzles toggle left/right; relay uses left/center/right; master uses left/right/center/right/left. The native consoles and rock barrier respond to actual interactions. Reset is available through the guide. Loading a saved position above the barrier provides an owner exit. The final terrain refresh runs after FireRed restores its saved map view, preventing stale gate tiles from overriding the installed design. Themes recolor the Gym's secondary background palette; classic restoration is checked. Individual trainer station selection remains separate work; these presets currently retain the five established safe stations.

Save version 15 appends exactly four design bytes at offset 3,196, filling the 3,200-byte reserved state without changing the preceding prefix or SaveBlock size. Native migration fixtures preserve every v14 prefix byte except the version update and initialize only the new tail, then pass flash save/reload. State, competition, Rocket and hosting ASan/UBSan regressions pass. Native design checks pass all three hometown choices: guide menus, locks/fees/refusals, independent design choices, controller-operated two-switch trial and walking through its gate, re-entry reset, upper-room owner safety, relay/master adapters and puzzle removal. All four presets coexist with five active staff, three trophies and three consoles; native templates and visible actor spawning are checked separately to account for camera-based lazy spawning. Native screenshots confirm the map and menus.

Existing real defense/registration, all four inspection gates, hosted doubles, recruitment/Summary/practice/return/dismissal and eighteen Gym-type opening fixtures pass with the v15 state. The current build additionally passes a controller-played Oak victory, one-time final badge/reward, normal save/credits and both championship archive sectors. Later-tier/preset/coexistence setup and some inspector/round outcomes remain explicit fixtures, not a claim of an uninterrupted full campaign. The title bootstrap is aligned with the already approved tracked image so it cannot replace that artwork with an older encoded copy on a fresh build.

Linked EWRAM is 259,484/262,144 bytes, IWRAM 29,076/32,768 bytes and ROM payload 27,352,212 bytes before padding. Padded ROM SHA-256: `f4a7265bd0fd7fd1b17a4044b09aabb6671fff3450690a5d1d3afedb9f68317b`. Remaining work includes individual trainer stations, broader balance/progression and an uninterrupted campaign/save/release pass. No finished-release declaration is made by these targeted checks.


## 54. Individual trainer stations and distinct native staff actors

Gym Guy → Gym design → Staff stations now selects an active trainer and assigns Automatic, Upper left, Upper right, Lower left, Lower right or Rear platform. The five established floor positions remain safe in all four layouts. Each explicit station can be reserved by one active trainer. Automatic staff fill the other positions; conflicting requests and cancellation preserve the prior choice and charge nothing. Changes take effect on the next Gym visit. This controls physical placement within the native room; it does not add simulated trainer-path battles or change rank rewards.

The existing recruitment active byte now encodes reserve (0), automatic (1), or a requested station (2–6). Existing active=1 saves retain automatic placement. Repeat assignment preserves an installed preference. Moving to reserve, capacity-based demotion removal, departure and dismissal release the preference; retained active staff keep theirs. The renderer reserves explicit positions before filling automatic spaces and safely handles duplicate or malformed encodings without placing two staff on the same tile. No new persistence or EWRAM allocation is required: save version 15 and the exact 3,200-byte reserved state remain unchanged.

Native validation exposed the old shared actor-ID range: later recruitment slots could collide with Gym trophies or a town Rocket contact. Staff now use dedicated IDs 210–219. The normal Continue script-restoration path translates former staff IDs in saved templates and visible/saved actor state, scoped to the rebuilt header's actual staff and matching trainer graphics. It preserves the saved position and avoids changing trophy or Rocket actors. A legacy-ID flash fixture passes the actual Continue callback without a repair warp, including visible staff identity checks.

ASan/UBSan host suites pass station reservations, atomic conflicts, automatic fill, repeat assignment, reserve/reactivation, actual activity-based departure, demotion, all one-byte malformed preference values and the unchanged state budget. Existing state, Gym design, competition, Rocket and hosting suites pass. Native station/design runs pass all three hometown choices: actual guide/station menus, refusal/cancellation/no fees, flash persistence, all five positions in all four presets, and visible actors alongside three trophies and three consoles. A ten-record ledger fixture verifies that active records in its later five slots have distinct actors and correct record selection. Native recruitment/Summary/practice/defection/return/dismissal, defense/PC registration/all inspection gates and permanent personal requests also pass. The personal-bond migration test now expects the current save version rather than the historical v13 value. Screenshots confirm the actual native menus and room.

These later-tier, currency, contract, title and legacy-ID scenarios remain explicit fixtures, not an uninterrupted campaign. The audit is recorded in `native-stations-audit.json`. Linked EWRAM remains 259,484/262,144 bytes and IWRAM 29,076/32,768 bytes; ROM payload is 27,354,412 bytes before padding. Padded ROM SHA-256: `05b769f7875608d0d5c749facf0f2b05a9fa56efe8dc35e613832f358c07bef6`. Broader progression/balance, remaining story/physical-feature coverage and uninterrupted campaign/save/release validation remain open. No finished-release declaration is made.
