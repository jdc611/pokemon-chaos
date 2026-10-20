# Custom Build — Golden Ace and boss strategy update

Start a **new Custom Build run** to use the new draft-generation rules. Normal Chaos and The Last Six retain their original rosters, randomization and presentation. Prior Custom Build saves remain loadable and keep their existing individual builds and candidate generation; they receive the boss improvements but cannot retroactively turn a selected sixth companion into a new Golden roll.

## Golden sixth crate

The final pair of storage boxes is gold in Custom Build. Oak introduces the special last choice, a shiny sound plays, and the native comparison screen has a gold shimmer and sparkle reveal. The three familiar categories remain: evolving starter, immediate non-Mega power, and single-stage Mega potential.

- Ordinary potential remains 300–700, subject to existing viable-start/growth constraints.
- Golden potential uses 550–700, with a rare 701–800 jackpot (5% per initial endpoint draw). For the Mega category this is the future Mega endpoint, with its growth reserved backward. The evolving category's normal endpoint remains hidden. A single-stage non-Mega's current total is visible in full.
- In **every new-run crate**, the immediate-power choice has strictly higher current BST than the Mega choice. Endpoint/growth draws remain independent, with bounded rejection to condition the Mega candidate on that ordering; there is no fixed 100-point deduction. Conditioning can change the offered Mega category's jackpot frequency.
- One uniformly chosen slot is guaranteed shiny. Each other slot independently has a 25% shiny roll: theoretical counts are 56.25% one, 37.5% two, 6.25% three.
- Shininess is visible while comparing, deterministic for the run, and separate from personality, base allocation, ability options, moves and growth. It persists through normal evolution and permanent Mega evolution.
- Ability and move pools are unchanged. Gold does not guarantee an elite ability, move or favorable stat floor in addition to its stronger endpoint.
- Rival and Oak receive the two rejected candidates, including their shiny status and original personality. The rival's sixth-crate identity remains his last-slot ace in later full-team encounters; Oak's team remains hidden until its existing reveal.

A version byte occupies former alignment padding in OaksDraftState. The state remains 48 bytes, and no SaveBlock or Pokémon sizes/offsets change. Version-zero saves reconstruct their old rolls. Version one enables the new conditioning, weighted NPC stat spreads and Golden rules. Future generators must preserve these versions or provide migration.

## Bosses and six-Pokémon Gyms

All eight Custom Build Gym Leaders have six members, including Sabrina's previously five-member roster. Every existing authored member remains, and the signature ace remains last. Extra early-team members use the authored encounter's highest level and its IV standard; level caps and story gates are unchanged.

| Leader | Coordinated strategy / new roster additions where needed |
| --- | --- |
| Brock | Sand support, Rock/Ground coverage and screens; adds Roggenrola and Anorith |
| Misty | Rain support, Water pressure and complementary coverage; adds Wingull and Marill |
| Lt. Surge | Electric Terrain and coverage/pivoting; adds Flaaffy and Charjabug |
| Erika | Sun support, Grass pressure and coverage; adds Whimsicott |
| Sabrina | Psychic Terrain, screens and coverage; adds Bronzong |
| Koga | Preserved Double Battle, partner-safe attacks, Protect and Tailwind; retained Mega Beedrill |
| Blaine | Sun support with retained authored roster and Mega encounter |
| Giovanni | Sand support with retained Ground roster and signature Mega encounter |

Rival, Oak, Elite Four, Giovanni's boss encounters and the authored Cole/Vesper/Jessie/James story battles also receive the role-aware builder. Existing non-Gym team sizes, partner formats and signature Pokémon stay intact.

**Randomized stats, not perfect allocations:** ordinary bosses get a deterministic random redistribution of their legitimate species BST. Their BST, level, nature and EVs are not increased by this system. Drafted rival/Oak companions retain their own independently generated draft potential, with random weighted spreads rather than player-controlled allocations. No player allocation is copied. Boss Mega/form changes preserve the same random weights at the new form's legitimate total and retain signature Mega abilities.

Moves are selected after seeing the boss Pokémon's own randomized offensive stats, speed and bulk. Teams combine STAB/coverage, screens, setup, recovery, disruption and pivoting. Early encounters use a lower-power attack tier. Double Battles avoid friendly-fire Earthquake/Surf sets and include Protect/Tailwind coordination. Valid native/signature abilities remain where useful, with limited field-setting and supporting abilities added. Mega Stones remain; modest berries and role-specific held items support early teams.

Boss AI enables the existing engine's move viability, KO/2HKO evaluation, HP awareness, setup/status evaluation, conservative damage estimates, switch choices and anti-stall checks. Hidden move/item/ability/party omniscience and selected-action prediction are disabled. This extends existing AI behavior; it is not a new search engine, guaranteed optimal play or a secret counterteam generator. Teams depend only on the run seed and authored encounter, never the player's current party.

## Validation and limits

See `custom-build-golden-ace-results.json` and `custom-build-golden-ace-source-results.json` for exact build and test output, and `custom-build-golden-ace-files.txt` for changed files.

Native checks use mGBA 0.10.2. Controlled battle fixtures manipulate HP/moves to exercise story, rewards and save behavior; they do not prove natural difficulty balance. On-device iOS Delta testing and a complete natural playthrough remain outstanding. In-game saving is required before switching ROMs; do not carry emulator save states across builds.

The original Draft room exit, rival approach, evolution text fixes, global removed-gimmick restrictions and charcoal Delta tracker skin remain included. Existing source ROM/download files are preserved.
