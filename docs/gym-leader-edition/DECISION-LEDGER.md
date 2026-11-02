# Gym Leader Edition — Historical Decision Ledger

**Repository:** `jdc611/pokemon-chaos`  
**Target branch:** `gym-leader-edition`  
**Purpose:** Recover and preserve the user's decisions from prior Gym Leader Edition design conversations. This is a living ledger, not an implementation claim.

## Evidence and status rules

- **APPROVED** — directly stated or explicitly accepted by the user.
- **DEFERRED** — user explicitly postponed the choice or delegated it to Work mode.
- **PROPOSED** — assistant suggestion without clear user approval.
- **UNRESOLVED** — conflicting, incomplete, or not yet decided.
- **UNRECOVERED** — user confirms the topic was designed, but exact details/source conversation are not currently available.

Do not infer approval from a short “yes” without recovering the preceding question. Keep exact values and species assignments unchanged. Never silently fill gaps with invented choices.

---

## 1. Project separation and preservation

- **APPROVED:** Gym Leader Edition is a distinct experience from Oak's Draft / The Last Six and Custom Build.
- **APPROVED:** Preserve existing Pokémon Chaos modes, branches, ROM progress, and saves.
- **APPROVED:** A dedicated Git branch named `gym-leader-edition` was created from `master`. Implement Gym Leader Edition only on that branch unless the user explicitly directs otherwise.
- **APPROVED:** Gym Leader Edition startup should present Gym Leader Edition only; remove Oak's Draft, Custom Build, and other mode selections from this build's startup flow, without deleting their code/assets/branches. Tracked in GitHub Issue #1.
- **APPROVED DIRECTION:** A separate Gym Leader Edition ROM/build is expected; keep shared code/assets reusable where practical.
- **APPROVED:** Give Gym Leader Edition an independent save structure/progression. Test save/load/reset and multiple playthroughs.
- **REQUIRED:** Reconcile the branch's `master` starting point with useful existing changes on `custom-build`, `oaks-draft`, and other branches before implementation. Do not assume they are already merged.
- **APPROVED:** If an existing Chaos feature conflicts with GLE, adapt it specifically for GLE while preserving behavior in the other modes.

## 2. World, opening flow, and town

- **APPROVED:** Keep the Kanto map as the world.
- **APPROVED:** Choose one existing Kanto town to rebuild from a blank slate; all other towns remain normal.
- **APPROVED:** The selected town's buildings and signature attraction are rebuilt; the other towns are not wiped.
- **UNRESOLVED:** The player has considered Saffron, Celadon, and Fuchsia. No final hometown choice is confirmed in the recovered source.
- **APPROVED:** Town can be renamed.
- **APPROVED:** Opening flow: Oak's Lab → choose hometown → blank Gym/town setup → choose Gym type and signature move → GLA Headquarters tutorial. Preserve exact ordering as specified in the master design; reconcile if the codebase has a different existing opening.
- **APPROVED:** Remove the original eight-badge player progression and FireRed linear progression locks/rival triggers that conflict with GLE.
- **APPROVED:** Areas including Victory Road and the Pokémon League are accessible from the beginning. Do not gate exploration behind traditional badges.
- **APPROVED:** The original Gym Leader of the selected town relocates to Viridian as part of the League/GLA progression.
- **APPROVED:** Oak is the final challenge at the Pallet Town Lab, and is available only at the highest Gym Rank.
- **APPROVED:** Other Kanto towns remain intact/normal, except for planned GLA or story relocations.

## 3. Gym type, signature move, and starting roster

- **APPROVED:** Choose Gym type first rather than choosing a conventional starter.
- **APPROVED:** Signature move selection presents three randomized, similarly valued options; player chooses one.
- **APPROVED:** The chosen signature move becomes the TM awarded to challengers who defeat the player.
- **APPROVED:** The selected signature TM is permanent as the Gym's reward identity. Preserve the agreed one attack / one strategy / one utility move-option structure where applicable; exact pool implementation must follow source decisions.
- **APPROVED:** Initial roster is a three-round 3×3 draft: three rounds, three options each round, choose one Pokémon per round.
- **APPROVED:** No rerolls for the initial three Pokémon draft.
- **APPROVED:** Reveal each offered Pokémon's randomly assigned normal ability before the player chooses.
- **APPROVED:** Normal abilities only at start; hidden abilities are obtained later through GLA systems.
- **APPROVED:** Player roster Pokémon must have the chosen Gym type. Dual-types containing the chosen type qualify.
- **APPROVED:** One off-type Pokémon is allowed if it knows the selected signature move.
- **UNRESOLVED:** Whether the off-type exception applies once to the registered roster or separately to each battle team; whether official-only or exploration restrictions apply; evolution/form/move-forgetting edge cases need explicit enforcement policy.
- **APPROVED:** Player may use their Pokémon freely outside official Gym battles.
- **APPROVED:** Official Gym battles enforce Gym-type legality, Gym Rank level cap, and team-size restrictions.
- **APPROVED:** All supported Pokémon species should be available in each playthrough, distributed through habitat-based encounter pools and other sources as needed. Exact source/availability audit required.
- **DO NOT IMPORT:** Oak's Draft's six-Pokémon draft, no-catching rules, crate logic, or Custom Build stat-allocation mechanics into GLE.

## 4. Gym Rank, challengers, and League progression

- **APPROVED:** Gym Rank determines the global level cap and official battle team size; rank bar is visible.
- **APPROVED:** Rank progression combines automatic progress with required Elite Four League Inspections at designated promotion gates.
- **APPROVED:** Inspection victory is required to qualify for promotion at an inspection gate.
- **APPROVED:** Losing an inspection allows a later retry; no additional loss penalty is required.
- **APPROVED:** Rank demotion/recertification is part of the progression design; early ranks are protected, later-rank demotion is possible.
- **APPROVED:** The selected town's original Gym Leader relocates to Viridian; each traditional Gym Leader can be challenged once per Rank; Oak is reserved for the highest Rank.
- **APPROVED:** Hybrid challenger system: reliable NPC challenger queue plus other challenge types.
- **APPROVED:** Recruited Gym Trainers filter challengers; their own wins/losses do not directly change the player's Gym Rank.
- **APPROVED:** Consecutive queued challengers can be battled with full healing between matches.
- **APPROVED:** Challengers do not need to be battled immediately; queue can be left for later.
- **APPROVED:** Optional Friendlies against other Gym Leaders, once per Rank per leader.
- **APPROVED:** Rival is an independent Gym Leader and follows the player's current restrictions when battling the player.
- **DEFERRED TO WORK MODE:** Rank caps, team sizes, XP thresholds, promotion/demotion tuning, challenger frequency, inspection gates, rival cadence, and all numerical balance.
- **APPROVED:** Elite Four inspections occur at the player's Gym; inspector visits and comments on the Gym/development/performance before battle.
- **APPROVED:** Traditional Elite Four specialties remain: Lorelei Ice, Bruno Fighting, Agatha Ghost, Lance Dragon. Do not tailor them to counter the player's Gym type.
- **APPROVED:** Traditional inspection order by rank; Lorelei earlier, Bruno next, Agatha next, Lance highest. Recovered gate examples: Lorelei Rank 2→3; Bruno 4→5; Agatha 5→6; Lance 7→8. Verify exact full gate table against source before coding.
- **APPROVED:** Elite Four and Victory Road accessible from the beginning; rematch eligibility is individually unlocked after passing that member's inspection.
- **APPROVED:** Optional Elite Four rematches happen by visiting the member at the Pokémon League; no appointment or cooldown.
- **APPROVED:** Rematches continue to obey current Gym Rank team size, level cap, and type restrictions; no unrestricted six-on-six until the Rank allows it.
- **APPROVED:** Every rematch victory gives normal experience and Pokédollars. First victory against each member at each Rank gives a special bonus. No Gym Rank XP.
- **APPROVED:** Bonus rewards are themed by Elite Four member but must be useful to the player's Gym type/roster; higher Rank gives better rewards.

## 5. Town development, economy, and reputation

- **APPROVED:** Town reputation visibly attracts NPC visitors and residents; growth must be visible in the world, not only a menu number.
- **APPROVED:** Population capacity gates upgrades; population grows gradually; income scales with population.
- **APPROVED:** Passive income; no manual collection.
- **APPROVED:** Some businesses may produce more income but lower desirability.
- **APPROVED:** Plots include small, medium, large, and a unique landmark plot.
- **APPROVED:** Buildings include one Pokémon Center and one Mart, houses/hotels for population capacity, dojos, museums, type facilities (e.g. Haunted House), restaurants, and other approved businesses.
- **APPROVED:** Restoration is staged.
- **APPROVED:** Currency funds upgrades, stones/items, and GLA costs.
- **APPROVED:** NPC suggestions can grant bonuses.
- **APPROVED:** Keep town development within the Kanto GBA map/plot constraints; Gym uses mostly preset layouts with limited personalization.
- **APPROVED:** Gym layout/puzzles can be upgraded; puzzle choice is independent of Gym type (e.g. Ice Gym can use switches). Presets preferred. Changing layout/puzzle later costs money. Gym Guy manages layout/puzzle selection.
- **APPROVED:** No penalty for leaving the Gym open without immediately battling queued challengers.
- **DEFERRED TO WORK MODE:** Population/income values, building costs/gates, side-quest frequency, town growth pace, and other numerical tuning.

## 6. NPC recruitment, trainer loyalty, and departures

- **APPROVED:** Gym Trainers have a Potential rating.
- **APPROVED:** Trainer departure should not happen suddenly: require sustained losses defending the Gym AND declining Town Reputation, with warnings before departure.
- **APPROVED:** Some trainers are more loyal; completing useful quests can increase loyalty.
- **APPROVED:** Departed trainers may return to their original location/recruitment pool or join the rival's Gym. Both outcomes are possible.
- **APPROVED:** It must remain possible to recruit/persuade a departed trainer again later; do not permanently erase access.
- **APPROVED:** Rival can recruit unattached trainers and trainers currently working at the player's Gym.
- **UNRESOLVED/DEFERRED:** Exact trainer role list, potential values, loyalty scale, warning sequence, departure thresholds, persuasion conditions, recruitment rates, and how a trainer is assigned to rival vs returns. Tune in Work mode but preserve above guardrails.

## 7. Rival Gym and rivalry interactions

- **APPROVED:** Rival is an independent Gym Leader with their own Gym, roster, Gym Rank, town reputation, and development; rival progression occurs independently.
- **APPROVED:** Rival Gym type is chosen as a counter to the player's Gym type.
- **APPROVED:** Rival operates in one of the remaining towns, not the player's rebuilt town.
- **UNRESOLVED:** Exact rival town selection method/identity; recover source before fixing.
- **APPROVED:** Rival Gym is freely visitable and supports special rivalry events.
- **APPROVED:** Rival can recruit unattached trainers and trainers from the player's Gym.
- **APPROVED:** Rival remembers battle wins/losses and rivalry history.
- **APPROVED:** If rival gets ahead, rival challenge is adjusted/drops to player's current Gym Rank so player is not forced to fight above current restrictions.
- **APPROVED:** Rival battles follow player's current Gym Rank restrictions (level cap/team size/type rules), while rival retains their own Gym identity/type.
- **APPROVED:** Optional GLA double battles against Rival; not mandatory. Scale to player's current Rank and award XP, money, and rewards.
- **UNRESOLVED/UNRECOVERED:** Exact event script/cadence, rival town selection, rival progression formula, interaction choices, dialogue branches, recruitment timing, and event reward schedule.

## 8. Honor system

- **APPROVED:** Three standings: Honorable / Neutral / Notorious.
- **APPROVED:** Main summary/status shows only the current label, not a detailed meter.
- **APPROVED:** A designated town NPC explains the player's exact standing, proximity to changing category, and consequences.
- **APPROVED:** Good Honor can reduce GLA prices.
- **APPROVED:** Notorious standing can cause Ace Trainers to reject recruitment.
- **APPROVED:** Elite Four dialogue reflects the player's Honor.
- **APPROVED:** Honor does not change Gym Rank progression or battle difficulty.
- **UNRECOVERED:** Exact actions that raise/lower Honor, numeric thresholds, full NPC response matrix, and all consequences. Do not invent or infer these; recover original discussion.
- **DO NOT ASSUME:** Earlier assistant examples about gangs, punishment/banishment, sabotage, or rival infiltration are not approved unless source conversation confirms them.

## 9. GLA Headquarters, vouchers, and rewards

- **APPROVED:** Viridian Gym becomes GLA Headquarters; displaced leader is moved.
- **APPROVED:** GLA core building remains consistent; unlock additional rooms/services/NPCs with Rank rather than rebuilding exterior.
- **APPROVED:** GLA double battles against Rival are optional.
- **APPROVED:** GLA rewards include party-eligible move tutors, rare Pokémon vouchers (no repeats as specified), Gym upgrades, later legendary/Ultra Beast vouchers where species allocation allows; stones/items unlock at certain ranks.
- **APPROVED:** Black Market supports hidden abilities, shinies, egg moves, and approved special nicknames.
- **APPROVED:** GLA vouchers are stored automatically in player's GLA account, not Bag items.
- **APPROVED:** Redeem only at GLA Headquarters in Viridian. Vouchers never expire.
- **APPROVED:** Voucher tier is fixed when earned; saved vouchers do not upgrade. Pokémon level scales to current Gym Rank when redeemed.
- **APPROVED:** Voucher categories: Standard, Rare, Elite, Shiny, Shiny Elite.
- **APPROVED:** Voucher pools are Gym-type-specific, rank-appropriate, and randomly draw three choices.
- **APPROVED:** No rerolls; accept one of the original three.
- **APPROVED:** Choices permanently lock on first view. Viewing does not consume voucher until selection.
- **APPROVED:** Show species, level, ability, and inspectable moveset before selection; use existing FireRed/Oak starter-selection UI where possible.
- **APPROVED:** Shiny voucher: all three choices shiny; selected Pokémon guaranteed shiny.
- **APPROVED:** Shiny Elite is extremely rare; all three choices shiny from eligible Elite pool; legendary possible but not guaranteed.
- **APPROVED:** Elite pools mix powerful non-legendaries with eligible unused legendary/mythical/Ultra Beast species.
- **APPROVED:** Elite vouchers must not overlap legendaries already assigned to type-exclusive encounters or established universal research quests.
- **APPROVED:** Guaranteed Elite vouchers at major milestones plus rare repeatable Elite voucher rewards from prestigious competitions/GLA challenges.
- **APPROVED:** One consistent exclusive GLA Ball for all voucher categories: dark navy/silver with gold GLA emblem; FireRed-compatible sprite/animation dimensions; cosmetic only; no special capture effect; visible in summary and when sent into battle; not purchasable or used for wild captures.
- **APPROVED:** If party is full, voucher Pokémon goes directly to PC; otherwise use normal gift Pokémon flow.
- **UNRESOLVED:** Whether ordinary voucher claims prevent future duplicate species; whether Shiny voucher may repeat a normal-form claim; once shiny claimed, whether species is removed from future shiny pool. This was an assistant-proposed default, not clearly user-approved.

## 10. Training Room

- **APPROVED:** GLA Training Room unlocks progressively by Rank.
- **APPROVED:** Rank 1–2 Basic; Rank 3–5 Basic + Advanced; Rank 6–8 Basic + Advanced + Expert.
- **APPROVED:** Player can choose an opponent strategy or choose Random; strategies include offense, defense, weather, status, and balanced.
- **APPROVED:** Training battles use current Gym Rank level cap and official team-size/type restrictions; heal fully after each battle.
- **APPROVED:** Training Room awards experience but no Pokédollars; repeatable.
- **APPROVED:** No permanent win/loss record and no effects on Rank, reputation, Honor, or trainer loyalty.

## 11. Type-exclusive legendary encounters — species allocation ledger

- **APPROVED SYSTEM:** 18 unique Gym-type sets × four legendary encounters = 72 total.
- **APPROVED SYSTEM:** Each legendary species is assigned to only one Gym type, even if it is dual-type. No duplicates across type sets.
- **APPROVED:** Preserve legitimate legendary Pokémon; user explicitly said not to replace legitimate legendaries with custom creations and specifically to keep Latios/Latias in Dragon lineup.
- **RECOVERED EXACT SET — DRAGON (APPROVED):**
  1. Seafoam Islands — Latias
  2. Power Plant — Latios
  3. Mt. Ember — Reshiram
  4. Cerulean Cave — Rayquaza
- **UNRESOLVED — NORMAL:** An assistant proposal listed Type: Null, Meloetta, Regigigas, Arceus. Type: Null was subsequently removed because it was assigned to a universal research quest. No approved replacement was recovered. Do not treat the remaining three as finalized without source.
- **UNRECOVERED:** Exact species and location assignments for the other 16 Gym types. Do not invent or infer them. Recover original conversations or ask the user only if source recovery fails.

## 12. Legendary research quests and special encounters

- **APPROVED:** Legendary unlocks can come through research/NPC quests, final research stages, or Gym Rank progression.
- **APPROVED:** Player selects research projects; detailed project choice was explicitly deferred to assistant/Work mode.
- **APPROVED:** No real-time wait/timer requirement.
- **APPROVED:** Preserve established legendary research groups and unique allocations.
- **APPROVED:** Every species must remain obtainable in every playthrough through some reliable source (wild base form, quest, voucher, Game Corner, gift, etc.); do not make a species obtainable only through a voucher that might randomly never offer it.
- **APPROVED:** Some rare/special Pokémon may be one-time per playthrough; ordinary species generally repeatable.
- **UNRECOVERED:** Exact quest NPC names, dialogue, chains, stage requirements, research projects, field clues, locations, rewards, and which exact species belong to universal research groups. Recover before implementing quest chains.

## 13. Wild encounters, Pokédex, DexNav

- **APPROVED:** Curated environment pools: caves, open grass/grassland, forest, water, fishing, and other environments.
- **APPROVED:** Each location/floor draws a somewhat randomized selection from the appropriate pool.
- **APPROVED:** Larger areas can have more species; individual floors of a multi-floor cave can have different pools.
- **APPROVED:** Assignments generated once per new playthrough and remain stable when revisiting.
- **APPROVED:** All wild Pokémon scale to current Gym Rank, independent of location; Victory Road is not inherently stronger and uses the relevant cave/environment pool.
- **APPROVED:** Every Pokémon species is obtainable in every playthrough, at least in base form, through wild encounters, voucher, Game Corner, quest, gift, or another reliable source.
- **APPROVED:** Trade evolutions must be obtainable without link trading.
- **APPROVED:** Pokédex reveals actual randomized habitats in the current save after encounter/discovery; separate cave floors tracked separately.
- **APPROVED:** DexNav uses same randomized encounter data as wild encounters and Pokédex; works across floors, surfing, fishing, and environment changes.
- **APPROVED:** Preserve existing DexNav menu option for species revealed immediately vs hidden until encountered. Do not add duplicate toggle.
- **APPROVED:** DexNav search only shows/searches species actually present in current location; preserve search levels/chains/hidden ability features.
- **APPROVED:** Test multiple environments and multi-floor caves.

## 14. Competitions, tournaments, spectators, betting

Recovered user answers from Nov 2 conversation; where the preceding assistant question is not available, keep referent marked for source recovery:
- **APPROVED:** GLA competitions focus on Gym Leaders / Gym types.
- **APPROVED:** Competition participants include others, not just the player.
- **APPROVED:** Somewhat randomized participants/competition setup.
- **APPROVED:** Participant eligibility/strength is tied to current Gym Rank.
- **APPROVED:** Player can attend as a spectator.
- **APPROVED:** Spectator activity includes small in-game betting.
- **APPROVED:** Player may bet on themselves or other participants.
- **APPROVED:** Payout is winner-only; no place/show payout.
- **APPROVED:** Competitions are associated with major events, not constant always-on events.
- **APPROVED:** Prize ideas include decorations, money, reputation, and potentially a TM.
- **APPROVED:** TM prize selected from a pool.
- **APPROVED:** Winning is a qualification criterion in the relevant competition context.
- **DEFERRED:** Several competition details were explicitly deferred to assistant/Work mode.
- **UNRECOVERED:** Exact tournament types, entry criteria, number of participants, schedule, event calendar, betting odds/stakes/caps, whether player may enter and spectate at the same event, exact reward pool, and the referents for short answers “both”, “permanent”, “throughout”, “auto”, “simple”, “start empty”, “continue without you”. Recover preceding questions before interpreting these.

## 15. Existing feature compatibility, UI, QA

- **APPROVED:** Audit all existing Pokémon Chaos features, menus, and settings plus all GLE additions against GLE vision.
- **APPROVED:** Adapt shared features for GLE where necessary; preserve other modes' behavior.
- **APPROVED:** Reuse actual FireRed/GBA screens, menus, graphics, tiles, sprites, transitions, and workflows wherever possible. Create a new FireRed-style screen only if an existing screen cannot support the feature. No plain-text substitute UI.
- **APPROVED:** Audit map scripts, NPCs, warps, collisions, doors, transitions, battle triggers, menus, save capacity/persistence, save/load/reset, mode isolation, and progression locks.
- **APPROVED:** Fix discovered problems and retest; do not merely report issues.
- **APPROVED:** Produce a requirement-to-implementation gap report with test evidence; do not claim completion without evidence.

## 16. Explicitly separate projects / do not conflate

- `Chaos_Kanto_Battle_Story_Master_UPDATED.docx` and sprite variants are a separate Kanto-first player-mode battle/story specification with traditional Brock/Misty/Surge/Erika/Sabrina/Koga/Blaine/Giovanni progression, Rocket encounters, and player/rival story. Do not treat those conventional Gym Leader rosters as the player's GLE roster or as GLE's 72 type-exclusive legendary list.
- Oak's Draft / The Last Six and Custom Build are separate modes; do not import their rules automatically.
- The legendary sprite showcase includes special/custom forms such as Awakened Slaking, Razorstone Kleavor, Uncrowned Bisharp, and Overcharged Electivire. These are separate special concepts, not a complete substitute for the 72 legitimate type-exclusive legendary allocation.

## 17. Required next recovery steps

1. Search all prior Gym Leader Edition conversation threads and recover the preceding prompts for short answers.
2. Reconstruct the full 18-type × four-species legendary table, with exact locations and status.
3. Reconstruct every universal research questline and exact species allocation.
4. Reconstruct all voucher category species pools/exclusions and duplicate rules.
5. Reconstruct Honor action/threshold/NPC consequence matrix.
6. Reconstruct NPC recruitment roles, loyalty, warnings, departures, returns, and rival poaching.
7. Reconstruct rival type/town/independent progression/events and battle interactions.
8. Reconstruct tournament/event types, schedule, participant selection, betting and rewards.
9. Reconcile against original Library handoff, other project docs, assets, and actual repository.
10. Keep every unrecovered item visible as a gap; never fabricate.

**Implementation gate:** Work mode must complete and review this historical decision ledger and species-allocation audit before major GLE feature implementation.