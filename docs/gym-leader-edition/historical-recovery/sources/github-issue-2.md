# GitHub issue 2 and comments

# Pokémon Chaos: Gym Leader Edition — Master Work Mode Handoff

This issue is the consolidated implementation brief for the separate Gym Leader Edition development branch. Treat user-approved decisions below as requirements. Items explicitly delegated/deferred remain implementation decisions for Work mode; do not repeatedly ask the user to choose balancing numbers.

## Project and branch safety
- Gym Leader Edition is a separate game mode/project and should ultimately produce its own ROM.
- Work on branch `gym-leader-edition` only. It was created from `master`.
- Do not modify `master`, `custom-build`, `oaks-draft`, or other existing branches while implementing this mode.
- The new branch currently starts from `master`; audit/reconcile useful existing changes from other branches before implementation. Do not assume all features from `custom-build` or `oaks-draft` are present.
- Preserve all other modes' code/assets/progress. Adapt shared features specifically for Gym Leader Edition when needed, without changing their behavior in other modes.
- Gym Leader Edition startup should present only Gym Leader Edition. Remove Oak's Draft, Custom Build, and other mode choices from this ROM's startup menu, without deleting their underlying code or other builds. See issue #1.
- Prefer a separate Gym Leader Edition save structure; test save/load/reset and multiple playthroughs.

## Core premise and world
- Keep the Kanto map and open-world exploration; remove original FireRed linear progression, badge locks, and rival/story triggers that conflict with this mode.
- Choose one Kanto town to rebuild from an empty/blank slate as the player's Gym town. Candidate towns discussed: Saffron, Celadon, Fuchsia. Town selection/initial start flow is defined in the project design notes; preserve the chosen town's rebuild concept and allow town renaming.
- Other towns remain normal. Gym Leader Edition is separate from Custom Build and does not use the original eight-badge player progression.
- Player starts at Oak's Lab, chooses hometown, then chooses Gym type and signature move, begins with a blank Gym, and proceeds through GLA HQ tutorial.
- Open-world access: areas including Victory Road and the Pokémon League are accessible from the beginning; do not impose traditional badge gates. Wild levels scale to Gym Rank.

## Gym identity, roster, rules
- Choose Gym type first. Initial starter must already have the chosen type.
- Initial team of three comes from a 3x3 draft. Starter draft reveals the randomly assigned normal ability before selection.
- Start with normal abilities only; hidden abilities can be obtained later through approved systems.
- Choose one signature move from three similarly valued randomized options. The chosen signature move becomes the permanent signature TM given to challengers after a loss. It is one attack, one strategy, and one utility option in the defined signature-move system; preserve prior approved details.
- Roster legality: Pokémon must have the Gym type, except one off-type Pokémon allowed if it knows the Gym's signature move. Dual-types qualify.
- One signature TM copy remains permanent/available as designed; do not duplicate Gym TMs in the overworld or shops.
- Free use of the roster outside official battles; official Gym battles enforce type restrictions, current Gym Rank level cap, and team-size restrictions.
- Gym layouts/puzzles are upgradeable. Puzzle choice is independent of Gym type: an Ice Gym may use switches, etc. Choose puzzle/layout when desired; changing it later costs money. Puzzles are optional/free to use; presets are preferred. Gym Guy in the Gym manages layout/puzzle selection. No penalty for leaving the Gym open without immediately battling challengers.
- Use existing FireRed screens/menus/UI wherever possible. Create a new FireRed-style screen only when an existing screen cannot support the feature. No plain-text replacement UI.

## Gym Rank and official challenges
- Rank determines global level cap and official Gym team size; cap equals the global cap. Rank bar is visible.
- Rank progression combines automatic progress with required League Inspections at designated promotion gates. Losing an inspection does not demote the player; allow retry later. Early ranks are protected; demotion is possible at later ranks according to delegated balancing.
- Work mode owns numerical balancing: rank caps/team sizes/XP thresholds, promotion/demotion tuning, challenger frequency, recruitment rates, honor thresholds, town population/income, side-quest frequency, Rocket pacing, legendary availability lists/triggers, building costs/gating, rival cadence, and inspection gates. Propose/test/tune; do not repeatedly ask the user for numbers.
- Reliable NPC challenger queue. Recruited Gym Trainers filter challengers; their own wins/losses do not directly change player's Rank. Queued challengers can be battled consecutively, with full healing between matches.
- Optional friendlies against other Gym Leaders: once per rank per leader. Rival is an independent Gym Leader and follows the player's restrictions when battling the player.
- Gym Guy is the primary in-Gym NPC contact. Challengers may be left queued; no requirement to battle immediately.
- Gym trainers have a Potential rating. They may leave if the player loses frequently; some are more loyal, especially after useful quests. Players can sometimes persuade departed trainers to return. Trainer departure can include joining the rival's Gym. Trainer recruitment/retention mechanics should use both performance and relationship/quest loyalty.

## Town development, economy, quests, honor
- Town begins empty/blank and is rebuilt by the player. Plot sizes: small, medium, large, plus a unique landmark plot.
- Buildings include one Pokémon Center and one Mart, houses/hotels (population capacity), dojos, museums, type facilities (e.g. Haunted House), restaurants, and other approved businesses. Restoration is staged.
- Population capacity gates upgrades. Population grows gradually; income scales with population. Some businesses yield higher income but lower desirability. Income is passive (no manual collection).
- Currency funds buildings/upgrades, stones/items, and GLA-related costs. NPC suggestions can grant bonuses.
- Side quests/events include NPC delivery tasks, missing persons, bully fights, Team Rocket invasions/heists, and later Giovanni/rival tie-ins. Rocket events scale with town size and persist until cleared; some heists can succeed. Trainers help repel invasions; stronger recruits reduce required battles.
- Honor system: Honorable / Neutral / Notorious. NPC explains it. Choices affect honor. Benefits/penalties include GLA discounts for good honor, Ace trainers rejecting notorious players, and a Black Market (hidden or town) for notorious players. Elite Four dialogue reflects honor.
- Safari Zone in Fuchsia has configurable species; rarer/stronger species cost more. Celadon Game Corner uses updated mini-games/prizes. Game Corner is also an approved source for Pokémon availability.
- Passive income and town reputation/population should make the town visibly grow and attract visitors/residents.

## GLA Headquarters and rewards
- Viridian Gym becomes GLA Headquarters; displaced leader is moved. Oak is the final badge/inspection-style challenge at the Lab as already specified in the master design.
- GLA double battles against Rival are optional.
- GLA rewards include move tutors (party-eligible), rare Pokémon vouchers (no repeats), Gym upgrades; later legendary/Ultra Beast vouchers only where species allocation allows. Stones/items unlock at certain ranks.
- Black Market supports hidden abilities, shinies, egg moves, and approved special nicknames.
- GLA Headquarters core building remains consistent; unlock additional rooms, services, NPCs as Rank increases rather than rebuilding the exterior. Reuse existing FireRed interiors and transitions where possible.
- GLA Pokémon vouchers are automatically stored in the player's GLA account, not Bag items. Redeem only at GLA Headquarters in Viridian. Never expire. Voucher reward tier is fixed when earned; saved vouchers do not upgrade. Pokémon level scales to current Gym Rank on redemption.
- Voucher categories: Standard, Rare, Elite, Shiny, Shiny Elite. Pools are Gym-type-specific, rank-appropriate, and randomly draw three choices. No rerolls; user must accept one of the original three. Choices permanently lock the first time the voucher is viewed; viewing does not consume voucher until selection.
- Show species, level, ability, and inspectable moveset before selection. Use existing FireRed/Oak starter-selection presentation where practical.
- Shiny voucher: all three choices shiny; selected Pokémon guaranteed shiny. Shiny Elite exists but is extremely rare; all three choices shiny, from eligible Elite pool. Legendary is possible but not guaranteed.
- Elite pools mix powerful non-legendaries with eligible unused legendary/mythical/Ultra Beast species. Do not overlap any legendary species already assigned to prior exclusive encounters or universal research quests (including the previously planned 72 type-exclusive encounter group and established legendary research groups). Maintain a master species allocation list across all encounter/reward sources.
- Elite Voucher frequency: combine guaranteed major milestone rewards with rare repeatable rewards from prestigious competitions/GLA challenges; Work mode tunes rates.
- Species uniqueness: proposed rule from assistant: ordinary voucher categories prevent duplicate species claims; shiny vouchers may offer a species previously claimed in normal form; once shiny claimed, remove from future shiny voucher pools. This was deferred to assistant recommendation and should be treated as proposed default, verify against full master design before hard-coding.
- GLA Ball: one consistent exclusive ball for all voucher categories; dark navy/silver with gold GLA emblem, FireRed-compatible sprite/animation dimensions. Cosmetic only, no special capture effect; visible in summary and when sent into battle; not purchasable or used for wild captures.
- Party full: send voucher Pokémon directly to PC. Use normal gift Pokémon flow if party has room.

## GLA Training Room
- Training Room unlocks progressively with Rank.
- Rank 1–2: Basic; Rank 3–5: Basic + Advanced; Rank 6–8: Basic + Advanced + Expert.
- Opponent strategy selection: user can choose a strategy or choose Random. Strategies include offense, defense, weather, status, and balanced; more advanced teams at higher difficulty.
- Training battles use current Gym Rank level cap and official team-size/type restrictions; fully heal after each battle.
- Award experience but no Pokédollars. Repeatable.
- No permanent win/loss record and no effects on Rank, reputation, Honor, or trainer loyalty.

## Elite Four inspections and rematches
- Inspections occur at the player's own Gym; an Elite Four member visits to inspect it. Player requests inspection when eligible/ready. Inspector comments on Gym/development/performance, then battle.
- Losing inspection allows retry later with no additional penalty; winning approves promotion.
- Inspector assignment follows traditional Elite Four order by Rank: Lorelei first, then Bruno, Agatha, Lance last/highest. Existing rank-gate table from the conversation: Lorelei for Rank 2→3, Bruno 4→5, Agatha 5→6, Lance 7→8. Other promotions can be automatic when requirements met.
- Inspectors retain traditional specialties/types (Lorelei Ice, Bruno Fighting, Agatha Ghost, Lance Dragon); do not tailor teams to counter the player's Gym type. Teams scale to inspection's level cap and official team-size restrictions.
- After passing a member's inspection, optional rematches unlock by visiting that member at the Pokémon League. League and Victory Road are accessible from the beginning; rematch access is gated individually by having passed that inspector's inspection.
- Rematches follow player's current Gym Rank official restrictions (team size, level cap, type requirements), not unrestricted 6v6 before Rank 8.
- Every victory grants normal experience and Pokédollars. First victory against each unlocked member at each Gym Rank gives a special bonus reward. No Gym Rank XP.
- Bonus rewards are themed by Elite Four member but must be useful to the player's Gym type/roster; do not give irrelevant type-specific items (e.g. Dragon item to Ghost Gym). Higher ranks offer better rewards. Exact reward pools are delegated to Work mode.
- Rematches have no appointment or cooldown; simply visit League.

## Wild encounters, species availability, Pokédex and DexNav
- Encounter system is environment-based, not location-strength-based. Curated pools for caves, grass/open areas, forests, water, fishing, and other environments. Locations draw somewhat randomized selections from the appropriate environment pool.
- Larger areas can support more species than small areas; different floors within multi-floor cave systems can have different encounter pools.
- Randomized assignments are generated once per new playthrough and remain stable when revisiting. Do not reroll encounters on every visit.
- Every Pokémon species in the hack must be obtainable in every playthrough, at least in base form, through wild encounter, voucher, Game Corner, quest, gift, or another reliable source. A voucher that may randomly never offer a species is not by itself sufficient to guarantee availability. Include a validation pass over all species and evolution paths; trade evolutions must be obtainable without link trading.
- Ordinary species can generally be obtained repeatedly. Some unique/rare/legendary/quest species may be one-time per playthrough.
- All wild Pokémon scale to current Gym Rank so encounters remain on par with the player, independent of location. Victory Road is not inherently stronger; it draws from the relevant cave/environment pool.
- Legendary encounters/research quests previously designed remain intact and must not be duplicated by voucher pools.
- Pokédex habitat information reveals actual randomized locations in the current save after encounter/discovery; floors tracked separately. Habitats must not reflect original FireRed tables if randomized assignment differs. Newly discovered habitats update.
- DexNav must use the same encounter data as the wild encounter engine and Pokédex. It must work with randomized environment pools, floor changes, surfing, fishing, and other environment transitions. Preserve existing DexNav features/settings, including the existing menu option controlling whether species are revealed or hidden until encountered. Do not create a duplicate reveal toggle. Search only species actually available in the current location; discovered/hidden states and search levels/chains/hidden abilities must remain functional. Test across multi-floor caves and other environments.

## Full compatibility/UI/QA audit
- Before implementing, audit all existing Chaos features, systems, menus, and settings in the actual repository/branch. Reuse existing features rather than rebuilding unnecessarily.
- Preserve existing features and adapt them specifically for Gym Leader Edition if they conflict; keep other modes' behavior unchanged.
- Audit every existing and newly added feature/menu for consistency with the Gym Leader Edition vision.
- Preserve FireRed/GBA-native UI: reuse actual screens, menus, tiles, sprites, transitions, and workflows. Only add a new FireRed-style screen when existing screens cannot support the feature. No generic plain-text UI.
- Audit map scripts, NPCs, warps, collisions, doors, transitions, battle triggers, menu navigation, save capacity/persistence, save/load/reset, mode isolation, and progression locks.
- Test fresh boot, full playthrough milestones, Rank transitions, team legality, queued challengers, recruitment/loyalty, town growth, quests/events, all voucher categories, species availability, DexNav/Pokédex, Elite Four inspections/rematches, and regressions.
- Fix discovered problems and retest; do not only produce a list of issues.
- Produce a gap report mapping each approved requirement to implementation status and tests. Clearly mark deferred decisions and assumptions. Do not claim completion without evidence.

## Work mode authority / deferred tuning
Work mode is authorized to choose and playtest numerical balance and implementation details already delegated above. It should make sensible proposals and test them rather than repeatedly asking the user to pick values.
For unresolved design details, consult the existing project handoff/spec and ask only when a meaningful user-facing design choice is genuinely blocking.


## Comment 6568898038

https://github.com/jdc611/pokemon-chaos/issues/2#issuecomment-6568898038

Created 2026-11-02T04:14:55Z; updated 2026-11-02T04:14:55Z

## Completeness clarification — do not treat this issue as the entire historical archive

This master brief consolidates the Gym Leader Edition decisions recovered so far, but it does **not** yet contain every exact roster/species selection, legendary allocation, research quest, or detailed encounter trigger from prior design work.

Before implementation, retrieve and reconcile the original project handoff and prior conversation artifacts. In particular:
- The Library file `Pokemon-Chaos-Gym-Leader-Edition-Handoff.md` is an earlier concept handoff and contains additional context/open questions; it predates many decisions in this issue.
- The separate legendary sprite-sheet asset documents four special legendary-caliber concepts: Awakened Slaking, Razorstone Kleavor, Uncrowned Bisharp, and Overcharged Electivire. Do not mistake these four for the complete legendary roster.
- Existing Kanto battle/story master documents contain detailed traditional Gym leader rosters and story beats, but those are not automatically the Gym Leader Edition player's starter roster or the full Gym Leader Edition legendary/quest allocation.
- The 72 type-exclusive legendary encounter group and research-quest system are referenced in the design history, but the exact complete species list, quest chains, unlock conditions, and allocation ledger are not reproduced here.

Required pre-implementation action: build a source-of-truth appendix listing every exact species/roster/legendary/quest allocation that can be recovered, with source references and status (approved, proposed, deferred, or unresolved). Flag missing information rather than inventing it. Preserve unique legendary allocations across wild encounters, quests, vouchers, and all other sources.

## Comment 6568934756

https://github.com/jdc611/pokemon-chaos/issues/2#issuecomment-6568934756

Created 2026-11-02T04:17:53Z; updated 2026-11-02T04:17:53Z

## Recovered detailed decisions — voucher/legendary, research, honor, recruitment, rival

This supplement records additional approved decisions recovered from prior design conversations. Treat these as binding additions to the master brief. Do not replace them with generic versions.

### Type-exclusive legendary encounters
- There are **18 unique Gym-type sets, with four distinct legendary encounters assigned to each set (72 total)**.
- Each species is assigned to only ONE Gym type, even if it has multiple types. Do not duplicate a dual-type legendary across two Gym pools.
- Species assignments are decided type-by-type. The exact species-by-type ledger must be recovered from the original design conversations/assets if present; do not invent or silently substitute missing assignments.
- These are separate from Elite Voucher species pools. Elite vouchers must not overlap legendary species already assigned to these encounters or existing research quests.

### Research and legendary discovery
- Legendary discovery uses NPC/research-triggered, multi-stage questlines and research breakthroughs.
- The player can select lab/research projects.
- No real-time timers/waiting are required.
- Encounters must not become permanently missable; preserve a way to complete them later.
- Rank balancing and progression pacing are implementation/balance work.
- Preserve the existing legendary research groups and unique allocation; exact quest chains, NPCs, stages, rewards, and triggers must be reconstructed from prior project materials before implementation.

### Vouchers and GLA reward pools
- GLA uses Pokédollars. Vouchers are earned directly and automatically stored in the player's GLA account, not in the Bag.
- Redeem only at GLA Headquarters in Viridian. Vouchers never expire.
- Reward tier is fixed when earned; stored vouchers do not upgrade as Gym Rank increases. The Pokémon's level scales to the player's current Gym Rank when redeemed.
- Voucher pools are Gym-type-specific and rank-appropriate. Each voucher presents three randomized options. No rerolls; player accepts one of the original three.
- Choices permanently lock the first time a voucher is viewed. Viewing does not consume the voucher; selecting a Pokémon redeems it.
- Display species, level, ability, and inspectable moveset before selection. Use existing FireRed/Oak starter-selection UI where possible.
- Elite voucher pool: mix powerful non-legendaries with eligible unused legendaries/mythicals/Ultra Beasts. No guaranteed legendary; a legendary is possible, not guaranteed.
- Shiny Elite voucher: rare. All three randomized choices are guaranteed shiny and come from the eligible Elite pool; a legendary is possible, not guaranteed. No overlap with the 72 type-exclusive encounters or established research allocations.
- Regular Shiny voucher: all three choices are shiny; selected Pokémon is guaranteed shiny.
- Major milestones can award guaranteed Elite vouchers; prestigious competitions/GLA challenges can rarely award repeatable Elite vouchers. Tune rates in implementation.
- Voucher categories include Standard, Rare, Elite, Shiny, and Shiny Elite. Preserve the agreed type-specific pool behavior and exclusions.
- One consistent exclusive GLA Ball is used for all voucher categories: dark navy/silver with gold GLA emblem, FireRed-compatible sprite/animation dimensions; cosmetic only, no special capture effect. It is not purchasable or used for wild captures; show it in summary and when the Pokémon enters battle.
- If party is full, send the voucher Pokémon directly to PC. If there is room, use normal gift Pokémon flow.
- Species uniqueness/duplicate-removal rules between normal and shiny voucher claims were not clearly user-finalized in recovered context. Keep flagged as proposed/deferred until the original conversation can confirm.

### Honor system
- Three displayed standings: **Honorable / Neutral / Notorious**.
- Summary/status display shows only the current label, not a detailed meter.
- A designated town NPC explains the player's exact standing, how close they are to changing category, and the consequences.
- Good Honor can reduce GLA prices.
- Bad/Notorious Honor can cause Ace Trainers to reject recruitment.
- Elite Four members comment on the player's Honor.
- Honor does NOT change Gym Rank progression or battle difficulty.
- The underlying value/thresholds and exact actions that change it were not finalized in the recovered context. Earlier suggestions (e.g. handling gangs, punishment/banishment, rival infiltration/sabotage) must not be treated as approved actions unless confirmed from source material. Tune numerical thresholds only after auditing the prior design.

### NPC recruitment, loyalty, and departures
- Gym Trainers have a Potential rating.
- Trainer departures must not happen suddenly: require sustained losses defending the Gym AND declining Town Reputation, with warnings before a departure.
- Some trainers can be more loyal; completed useful quests can improve/establish loyalty.
- Departed trainers may either return to their original location/recruitment pool or join the rival's Gym. Both outcomes were accepted. It must remain possible to recruit/persuade them again later.
- Rival can recruit unattached trainers and trainers currently working at the player's Gym.
- Preserve trainer-specific histories/relationships where practical; rival recruitment should create meaningful stakes, not permanently erase access to a trainer.
- Exact numerical Potential/loyalty values, role list, recruitment rates, and return/persuasion conditions remain Work mode tuning or require recovery from the original design.

### Rival Gym and interactions
- Rival is an independent Gym Leader with their own Gym, roster, Gym Rank, town reputation, and development; rival progression occurs independently.
- If the rival gets ahead, their battle challenge is adjusted/drops to the player's current Gym Rank so the player is not forced to fight above current restrictions.
- Rival Gym is freely visitable and supports special rivalry events.
- Rival's Gym type is selected as a counter to the player's chosen Gym type.
- Rival operates in one of the remaining available hometowns (not the player's rebuilt town); the exact town selection should follow the prior approved randomized/selection rule if recovered.
- Rival can recruit unattached trainers and trainers from the player's Gym.
- Rival battles follow the player's current Gym Rank restrictions (level cap/team size/type rules); rival remains a distinct Gym Leader with their own type.
- Rival remembers battle wins/losses and rivalry history.
- GLA double battles against the Rival are optional, not mandatory. They scale to the player's current Rank and can award XP, money, and rewards.
- Rival cadence, exact event scripts, recruitment timing, and exact reward schedules should be reconciled against the original discussion before coding.

### Implementation guardrail
The current issue's earlier broad descriptions are not a substitute for the exact decisions above. Work mode must create a detailed decision ledger and mark each item as approved, assistant-proposed, deferred, or not recovered. Never fill gaps by guessing. Preserve all unique species allocations across the 72 encounters, research quests, wild encounter pools, Game Corner, vouchers, and special rewards.

## Comment 6568940677

https://github.com/jdc611/pokemon-chaos/issues/2#issuecomment-6568940677

Created 2026-11-02T04:18:22Z; updated 2026-11-02T04:18:22Z

## Exact legendary assignment recovered so far — source-of-truth status

One approved Gym-type set was recovered with exact locations and species:

**Dragon Gym (approved)**
- Seafoam Islands — Latias
- Power Plant — Latios
- Mt. Ember — Reshiram
- Cerulean Cave — Rayquaza

The overall system remains **18 unique Gym-type sets × four species = 72**, with each legendary species assigned to only one Gym type even if dual-typed.

**Normal Gym is not finalized:** an earlier assistant proposal listed Type: Null, Meloetta, Regigigas, and Arceus, but Type: Null was then removed because it was assigned to a universal research quest. No approved replacement for Type: Null was recovered. Do not treat the remaining proposal as a user-approved final Normal lineup.

An earlier assistant stated that a 72-slot audit had been done, but the actual table was not available in the retrieved record. Do not recreate or infer the other 17 type lists. Continue source recovery and mark them unverified until exact approved mappings are found.

## Comment 6568962545

https://github.com/jdc611/pokemon-chaos/issues/2#issuecomment-6568962545

Created 2026-11-02T04:20:07Z; updated 2026-11-02T04:20:07Z

## Required historical-decision recovery workflow

Do not begin full implementation from this issue alone. The user requires recovery of **every decision made across the original Gym Leader Edition conversations**, including exact choices and details, not just high-level summaries.

Create a decision ledger with one entry per decision and these fields:
1. System/category
2. Exact decision and values/species/names/locations/rewards
3. User wording or explicit approval evidence
4. Status: approved / proposed / deferred / unresolved / unrecovered
5. Original conversation or file source
6. Dependencies/conflicts with other systems

Audit the original conversation threads systematically by topic: initial town/type/roster/signature selection; all Gym-type legendary sets; all legendary research quests and NPCs; voucher categories and exact pools; honor actions/thresholds/NPC reactions; recruit roles, potential, loyalty, departures/returns; rival type/town/Gym/rank/recruitment/events; tournaments, spectators and betting; town buildings/population/economy; GLA services; wild encounter pools/Pokédex/DexNav; Elite Four inspections/rematches; and all smaller decisions.

Use exact conversation evidence. Do not infer a user's approval from a short “yes” unless its preceding question is recovered. Do not conflate the separate Kanto Battle & Story Master (traditional player-mode story rosters) with Gym Leader Edition's gym-leader roster or voucher/legendary species allocations.

The Library handoff currently available is dated Oct 31 and predates later design decisions. It is a useful source, not a complete archive. Existing Kanto Battle & Story Master documents are separate project sources and must only be imported where explicitly approved for GLE.

Implementation gate: no major feature implementation until the recovered decision ledger is reviewed for gaps, conflicting versions, and exact species uniqueness across the 72 type encounters, research quests, vouchers, Game Corner, wild encounters, and special rewards.

## Comment 6568985440

https://github.com/jdc611/pokemon-chaos/issues/2#issuecomment-6568985440

Created 2026-11-02T04:21:56Z; updated 2026-11-02T04:21:56Z

## Additional recovered user-approved decisions — competitions, opening draft, research

This note adds exact user answers recovered from the November 1–2 design conversation. Where the preceding assistant question is not available, preserve the user's answer verbatim as a recorded choice and do not invent a more specific interpretation than the evidence supports.

### Competitions / tournaments / spectator system
The user explicitly selected or approved the following:
- Competition participant pool should include **Gym Leaders**, consistent with the GLA's focus on Gyms.
- Competitions are **somewhat randomized**, not wholly fixed.
- Competitions should include **other participants as well**, not only the player.
- Competitor strength/eligibility should reflect the **current Gym Rank**.
- Some competition design choices were explicitly **deferred** to Work mode.
- User approved that the player can **choose** in the relevant competition setup, and that a **permanent** option/feature would be desirable; recover the exact preceding question before assigning these to a specific subfeature.
- The player can attend/view competitions **as a spectator**.
- Spectating includes a small in-game betting activity: the player can wager on **themselves or other participants**.
- Betting payout is **winner only** (no place/show payouts).
- The user approved both of the two alternatives offered in a subsequent competition choice; exact referent needs original question recovery.
- Competitions should occur at **major events**, not constantly.
- The user approved both of the offered event types/approaches at that point; exact referent needs original question recovery.
- User approved the system direction overall ("That covers it") but later asked for assistant recommendation on one remaining design detail; that detail was deferred/accepted for Work mode recommendation.
- Competition prizes discussed include **decorations, money, reputation, and possibly a TM**.
- TM prize should be selected from a **pool**.
- User likes winning being the qualification requirement.
- Some competition/entry details were deferred; user suggested perhaps waiting before entering/starting in one context, and that the system could operate throughout the relevant period; exact scheduling rules require source-question recovery.
- Player can choose a strategy or choose random in the GLA Training Room; keep this separate from tournament choices unless the source confirms they are the same menu.

### Opening roster draft
- Gym type is chosen first.
- Initial team is a three-round draft: in each round, choose one Pokémon from three offered options (a 3x3 draft).
- No rerolls for the three choices in each round.
- The starter draft reveals the randomly assigned **normal ability** before the player selects.
- Initial draft pool must be appropriate to the chosen Gym type and similarly valued/balanced as designed.
- Normal abilities at start; hidden abilities are obtained later through the established GLA systems.
- This initial three-Pokémon draft is Gym Leader Edition-specific; do not import Oak's Draft's six-crate/no-catching rules.

### Research / legendary unlocks
- Legendary unlocks may come from research/NPC quests, final research stages, or Gym Rank progression.
- User explicitly deferred the detailed choice of research projects to the assistant/Work mode.
- Keep research project selection player-facing; no real-time waiting/timers were approved.
- Do not replace legitimate legendary species with custom creations merely to fill a type pool. User specifically said to keep Latios and Latias in the Dragon lineup and move away from replacing legitimate legendaries with custom creations.
- Exact project choices, quest stage sequencing, NPC names, and rewards remain to be recovered from source conversation; Work mode may design the deferred project selection but must preserve species decisions already approved.

### Evidence and status handling
- The above answers are user-approved as written. Any inferred mapping from a short answer to a particular subfeature must remain marked “referent needs source recovery” until the preceding assistant prompt is recovered.
- Do not import the separate Oak's Draft six-Pokémon draft, Custom Build mechanics, or the Kanto Battle & Story Master traditional Gym rosters into this edition unless specifically approved.

## Comment 6569010862

https://github.com/jdc611/pokemon-chaos/issues/2#issuecomment-6569010862

Created 2026-11-02T04:23:55Z; updated 2026-11-02T04:23:55Z

## Asset recovery note — four custom “Special Legendary Encounters”

I inspected the Library asset **Pokémon Chaos Legendary Encounter Sprite Sheet.png** (and the matching Legendary Sprite Showcase). It documents these custom/special encounter concepts and their visible stats/abilities:

| Concept | Type | BST | Ability |
|---|---|---:|---|
| Awakened Slaking | Normal | 670 | Vital Spirit |
| Razorstone Kleavor | Bug/Rock | 500 | Technician |
| Uncrowned Bisharp | Dark/Steel | 580 | Sharpness |
| Overcharged Electivire | Electric | 615 | Tough Claws |

The asset also shows shiny palette variants for each. These are real documented Pokémon Chaos design assets, but the recovered evidence does not establish whether they are assigned to Gym Leader Edition's 72 type-exclusive legendary encounters, universal research quests, special voucher pool, or a separate Chaos feature. Record them as **existing special encounter concepts — GLE placement unresolved**. Do not silently classify them as ordinary legendary species or insert them into a type pool. Work mode should inspect their existing implementation/assets and reconcile their intended acquisition source with the unique species-allocation ledger.

## Comment 6569014949

https://github.com/jdc611/pokemon-chaos/issues/2#issuecomment-6569014949

Created 2026-11-02T04:24:15Z; updated 2026-11-02T04:24:15Z

## Further recovery — legendary set rules and Normal proposal history

Additional prior-conversation evidence clarifies:
- **APPROVED:** 18 unique type sets, four matching species/location assignments per set.
- **APPROVED:** Mythicals, Ultra Beasts, and special Pokémon may be included in the type sets.
- **APPROVED:** A dual-typed species belongs to only one Gym-type set.
- **APPROVED Dragon set remains:** Seafoam Islands—Latias; Power Plant—Latios; Mt. Ember—Reshiram; Cerulean Cave—Rayquaza.

**Normal type — proposal history, NOT approved/final:**
An assistant proposed:
- Seafoam Islands — Meloetta
- Power Plant — Regigigas
- Mt. Ember — Terapagos
- Cerulean Cave — Arceus

Type: Null / Silvally were reserved for a universal research quest. The later Regigigas assignment was disputed. Do not treat this Normal list as approved, and do not put Regigigas into a final allocation without resolving the dispute. The user's instruction to retain legitimate legendaries and keep Latios/Latias in Dragon stands.

The other 16 type groups and all exact voucher/universal research species pools remain unrecovered in available context. Continue recovery; do not infer.