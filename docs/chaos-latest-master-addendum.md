# Chaos FireRed latest master addendum
Updated October 5, 2026. Authoritative design direction alongside existing Work handoffs.
These are requirements, not claims of completed implementation. This file supersedes conflicting older story/placement notes.

## Naming and simulator clarification
The box legendary is **Chaossal**. All Chaozar references in the supplied addendum mean Chaossal.
The simulator is the existing unexplained machine in Oak's lab, visible beforehand without revealing its purpose. Do not add a separate replacement machine.
Oak eventually explains the simulation as a required trial/obstacle that prospective Gym Leaders must overcome: "Overcoming Yourself."
The opponent uses the player's own sprite and six Pokemon representing that save's journey.
The player **cannot use any of those six recorded Pokemon** for the trial. Enforce this at selection/start and retries, including retrieving them from storage; never delete/confiscate them.
Track actual Pokemon identity across party/storage/evolution so a nickname change or slot movement cannot bypass exclusion. The user has not specified a blanket species ban or a duplicates policy; do not silently ban every Pokemon of the same species.
Record the best/most meaningfully used Pokemon, their moves and held items as faithfully as feasible, together with species/form, nickname, nature and ability.
Weight Gym, boss and major story participation heavily; meaningful repeated active battle use also counts. Passive HM usage and simply remaining in the party must not dominate.
Retain recognizable snapshots even when a tracked Pokemon is no longer in the party. Save-size/versioning and identity handling require an implementation audit.
The simulation team must have six members. Handling saves without six eligible recorded individuals needs a deliberate design before launch.

## Random BST revision
BST Shuffle preserves the species' normal total exactly, redistributing points across HP/Attack/Defense/Sp. Atk/Sp. Def/Speed; different archetypes are welcome.
Random BST rolls each stat independently: minimum 5; ordinary range 5–160; roughly 5% per-stat extreme roll 161–220; absolute ceiling 220.
Do not impose normal archetypes or preserve the random total. Weird distributions are intended; values must fit engine storage.
Evolution should generally feel like an upgrade, avoiding objectively worse outcomes. Technical progression safeguards may be chosen during implementation.
These changes already have a preceding master implementation commit (986d29ff); verify behavior rather than claiming this design note implements them.

## Cerulean Cave: Giovanni and autonomous Mewtwo
Giovanni reaches Mewtwo first; Mewtwo rejects/deflects his Poke Ball in a cutscene, then Giovanni initiates the boss battle.
Use a true allied partner format: player chooses three available Pokemon, controls one active slot, while an autonomous AI Mewtwo occupies the other. Never add Mewtwo temporarily to the player's party.
Giovanni has six competitive Pokemon, two active at a time, strong items/moves/intelligent AI, Dark/Ghost/Bug or strong neutral pressure, 1–2 bulky members and useful speed control/priority. Mega Nidoking is a centerpiece.
Normal Mewtwo must faint through real combat, not scripted fake damage. Target pacing has normal Mewtwo fall mid-battle with roughly 2–4 foes remaining unless player dominance prevents this.
On its first faint, pause for a short refusal-to-stay-down event and revive once as Mega Mewtwo. Choose X/Y from matchup/battle state if practical; a single scripted form is acceptable if necessary. A subsequent Mega faint is final.
Giovanni retreats. Mewtwo stays independent and awards both Mewtwonite X and Y, telling the player to return when worthy.
A later visit provides the actual normal static catch opportunity, with a defeat/failed-capture respawn fail-safe. Exact respawn trigger remains TBD.
Both stones are exclusive story rewards, never generic pickups or shop stock.

## Scyther specialist and placeholders
Optional Safari Zone researcher/stone collector in a small hut/research room lets a normal Scyther choose supported branches.
Branches: Scizor, Kleavor, Ice, Ghost, Electric, Fire when suitable art exists; more only if excellent designs justify them. Do not force all 18 types.
Ice placeholder: Insurgence Delta Scyther/Scizor, ROM-ready front/back/shiny, source Ice/Fighting (Chaos typing can be revisited).
Ghost placeholder: Ghost Grey Reapor line, GBA-style, source Bug/Ghost.
Electric preferred placeholder: FusionDex Raither (#26.123 Raichu/Scyther), preferred over Scychu, still open to better art.
Fire remains TBD. Do not overwrite live assets or promote placeholders without explicit approval.
Preserve docs/scyther-variant-placeholders.md (10e6748602902d680515df6c2b6d568a7e8fe8fd).
Scizorite placement remains open; Safari specialist/mastery reward are possibilities. Rocket Hideout placement is no longer locked.

## Mega Stone acquisition direction
Audit actual ROM-supported stones/forms before placing anything. The following are favored directions, not final decisions where alternatives remain:
- S.S. Anne Captain: Pidgeotite.
- Rock Tunnel: Steelixite.
- Diglett's Cave: Excadrillite.
- Route 16 Fly clearing: Staraptite.
- Rocket Hideouts: several stolen/collected stones, Houndoominite a strong candidate; not assumed Scizorite.
- Silph: Alakazite and Metagrossite.
- Power Plant: Manectite or Ampharosite, exact choice open.
- Seafoam: Glalitite and Froslassite.
- Tower/Lavender: Gengarite or Banettite, possible pickup/NPC split.
- Cinnabar Mansion/Lab: Cameruptite/Pyroarite/other supported fire/science stone; exact choice open, avoid duplicate Houndoominite.
- Victory Road: Tyranitarite and Salamencite.
- Cerulean Cave: both Mewtwonites only from the story battle.
Keep 6–10 species-enthusiast NPC rewards, not half the roster. NPC has the normal species nearby; showing the matching species awards its stone while player keeps Pokemon. Candidates include Kangaskhan, Medicham, Mawile, Heracross, Pinsir, Absol, Lopunny and Audino.
Preserve existing locked rematch rewards until explicitly reconciled with favored pickup ideas; avoid duplicate acquisition decisions.

## Arcade competitive items
Primary repeatable source for Choice Band/Specs/Scarf, Life Orb, Focus Sash, Leftovers, Eviolite, Rocky Helmet, Assault Vest and other premium competitive held items is the Chaos Arcade coin ecosystem.
Exact prices remain TBD, not automatically finalized by this addendum.

## Endgame sequence and Viridian succession
Giovanni increasingly pursues anomaly power and abandons his Gym priorities.
Cerulean Cave confrontation -> player + autonomous Mewtwo boss battle -> one Mega revival -> Giovanni retreats -> both Mewtwonites awarded.
Later Mewtwo senses catastrophic psychic pressure/distortion around Pallet and urgently directs the player HOME. Its unusual urgency should contrast with its normal detached independence; it need not yet know Chaossal's identity.
Pallet is in a violent storm/distortion; Oak and rival become involved; player and rival homes are displaced/swallowed; Mom and Daisy are missing.
The huge hole where the houses were leads to a dangerous, reasonably short maze that changes/randomizes with the Chaos theme.
Player reaches Chaossal, whose instability causes distortions rather than intentional evil. Resolve the crisis with visible residual world damage.
Oak then nominates/encourages the player to become Viridian's new Gym Leader. It is earned through League qualification trials, not handed over.
Final and hardest trial is Overcoming Yourself at Oak's existing machine, following the sprite/team recording/exclusion requirements above. Victory proves qualification. Other trials remain open to development.

## Breadcrumb system and progression checkpoints
Most NPCs stay ordinary; short optional oddities build toward an escalating pattern. No dialogue mentions randomizer, seed, ROM or mechanics.
Oak investigates honestly and does not know the answer from the start. Giovanni interprets evidence as exploitable power. Mewtwo senses more than humans without dumping the whole solution. Protect Chaossal's identity/name until late.
Early: occasional misplaced wildlife comments from Bug Catchers, Hikers and Fishermen; Oak initially suspects migration/environment/history. Mt. Moon Strange Fossil -> Pewter revival fails -> material is abnormal -> optional Nidoking-party reaction -> Oak researches it.
Cerulean: subtle post-quiz research comment; keep Bike Shop comedy ordinary; rival gives PokeRider before Nugget Bridge. New habitat pockets can include migration/research/disturbed-nest hints without answers.
Rocket/Celadon: strange stones and research collected; grunts see value, senior members know Giovanni wants abnormal energy. Arcade gets optional electronics-glitch flavor, not central plot involvement.
Lavender: restless Pokemon/ghosts react to something distant; Fuji or sensitive NPC hints without naming Pallet/Chaossal. Optional Mega Arcanine 1v1 awards Arcanite and mentions undocumented stones.
Silph: unclassified energy spikes resembling but not matching Mega energy; Giovanni wants research. Rock Smash field-research dialogue can reference recently disturbed geology. Oak's concern grows.
**After Sabrina/Marsh Badge**, Oak gives Mega Ring and in the SAME event reveals Strange Fossil is Nidokingite, not a fossil. Player receives Nidokingite; Oak is surprised such a stone exists and sees more impossible discoveries.
Nidokingite is not a Giovanni, Cinnabar or postgame reward. This locked reveal supersedes older adaptive-first-stone-only design as the required story direction; existing runtime must still be audited/updated.
Sabrina precedes Fuchsia/Koga. Brock/Misty/Surge stronger rematches unlock after Oak's event, any order, retaining their planned first-win Mega Stone rewards.
Safari: ecological migration and unfamiliar-stone evolution research; Scyther specialist knows only undocumented responses, not Chaossal.
Cinnabar: strong clues from historically impossible species, geological ages and unexplained Mega signatures. Do not repeat Nidokingite reveal.
Late Rocket/Silph: missing/damaged notes connect Mega readings, species/geological anomalies and psychic disturbances. Giovanni hunts the source and abandons Viridian; Mewtwo may help locate/control it.
Mewtwo's escalating concern -> urgent HOME warning -> Pallet catastrophe -> maze -> unstable Chaossal reveal. Earlier clues gain retrospective meaning: wildlife, impossible stones, failed fossil, Safari branches, Silph spikes, Rocket interest and empty Gym.

