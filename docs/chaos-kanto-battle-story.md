# Chaos Kanto battle and story master

Chaos is FireRed/Kanto first. Major teams draw from all generations. Ordinary
random encounters retain each route's native number of species; type/ability
filter behavior remains a separate future decision.

## Implemented progression

| Phase | Player level cap | Boss ace level |
|---|---:|---:|
| Brock | 15 | 15 |
| Misty | 22 | 22 |
| Lt. Surge | 28 | 28 |
| Erika | 34 | 34 |
| Sabrina | 40 | 40 |
| Oak's three rematches, any order | 44 | Brock 42, Misty 43, Surge 44 |
| Koga, after all three rematches | 46 | 46 |
| Blaine | 52 | 52 |
| Viridian Giovanni | 58 | 58 |
| League, provisional | 66 | Native teams pending final redesign |
| Postgame | 100 | Pending final postgame caps |

Both native approaches toward Fuchsia require Sabrina. Route 12's Snorlax and
Route 16's Snorlax explain the Marsh Badge requirement. The Cycling Road gate
also enforces it even if the player owns a Bicycle. Koga checks Sabrina, Oak's
Mega Ring, and all three independent rematch completion bits. Surf is already
Soul Badge gated.

After Sabrina, Oak appears outside Saffron Gym and triggers when the player
exits. The event works on existing saves that already have Sabrina's badge. He
gives the Mega Ring and the first compatible Mega Stone for an eligible current
party member, then checks boxed Pokemon. Eggs are excluded. If the player owns
no Mega-capable species, he provides Gyaradosite and explains that Route 4's fixed
Magikarp seller supplies its evolution line. This fallback does not claim every
random team already has a Mega-capable Pokemon. The remaining overworld stone
locations are deliberately unassigned. Bag-full failures preserve the reward
for a retry and do not unlock the rematches early.

A leader still offers their original badge battle if that badge is missing,
including an optional first Surge challenge after Oak's reward.

Brock, Misty, and Surge's rematches are unlocked by Oak's reward and can be won in
any order. Their Mega aces are Steelix, Gyarados, and Manectric. Completing all
three lifts the player cap from 44 to 46. Koga is a mandatory Double Battle with
six Pokemon and Mega Beedrill. His Glimmora uses Sludge Bomb rather than friendly
fire Sludge Wave; additional Protect coverage supports the format; Galarian
Weezing uses Levitate, not Neutralizing Gas.

## Gym rosters

The exact levels and moves are stored in `src/data/trainers_frlg.party`.

| Leader | Pokemon, in party order |
|---|---|
| Brock | Geodude, Rockruff, Nacli, Onix |
| Misty | Dewott, Lombre, Finizen, Starmie |
| Surge | Charjabug, Luxio, Toxtricity, Raichu |
| Erika | Roserade, Lurantis, Arboliva, Vileplume, Tsareena |
| Sabrina | Indeedee-F 37, Malamar 37, Hatterene 38, Espeon 39, Alakazam 40 |
| Brock rematch | Glimmora, Lycanroc-Dusk, Coalossal, Mega Steelix |
| Misty rematch | Barraskewda, Ludicolo, Azumarill, Mega Gyarados |
| Surge rematch | Kilowattrel, Magnezone, Electivire, Mega Manectric |
| Koga | Glimmora, Crobat, Toxicroak, Revavroom, Galarian Weezing, Mega Beedrill |
| Blaine | Talonflame, Ceruledge, Skeledirge, Volcarona, Houndoom, Arcanine |
| Giovanni | Excadrill, Krookodile, Ursaluna, Rhyperior, Great Tusk, Mega Nidoking |

Sabrina is a singles team at cap 40. Indeedee uses Psychic Surge with Psychic,
Dazzling Gleam, Hyper Voice and Reflect; Malamar uses Contrary with Psycho Cut,
Night Slash, Superpower and Topsy-Turvy; Hatterene and Espeon use Magic Bounce;
Alakazam remains a regular cap-40 ace. All ordinary Gym teams from Koga onward
have six Pokemon. No trainer bag healing items are assigned to these new teams.

## Rocket battles

Cole is the Cerulean Dig thief and the physical Game Corner gatekeeper. Vesper
is the final Tower guardian and the control Game Corner gatekeeper. They retain
native stolen-item, Mr. Fuji, and hideout door progression.

Game Corner Cole, cap 32: Golbat 30, Pawniard 30, Primeape 30, Houndoom 31,
Sandslash 31, Scrafty 32. Vesper: Haunter 30, Persian 30, Sableye 30, Salazzle 31,
Froslass 31, Drapion 32. Both have six Pokemon.

Game Corner Giovanni, cap 33: Persian 30, Honchkrow 30, Krokorok 31,
Kangaskhan 31, Rhydon 32, regular Nidoking 33. Silph Giovanni, cap 39: Persian 36,
Honchkrow 36, Krookodile 37, Rhyperior 37, Nidoqueen 38, regular Nidoking 39.

The Card Key interaction prompts before starting Cole's four-Pokemon team
(Crobat 35, Bisharp 35, Houndoom 36, Scrafty 37), immediately followed by Vesper
(Gengar 36, Salazzle 37, Froslass 37, Drapion 38). There is no healing or field-menu
access between them. Winning Cole records a checkpoint; losing to Vesper lets
the player retry Vesper rather than repeat unrelated story. The Card Key is
awarded only after both wins.

Jessie and James use anime Pokemon; Meowth is dialogue rather than a battle slot.
Their first encounter is a mandatory Double Battle outside Mt. Moon: Jessie has
Ekans 17 and Wobbuffet 18; James has Koffing 17 and Cacnea 18. The electric-rat
reference appears only here. They say "Team Rocket's blasting off again!" after
losing. Cole and Vesper use the approved Black Fragrant Archer/Ariana battle and
overworld art. Jessie and James use separate poses and walking frames extracted
from Monicaccina / Ody-chan’s approved GBA sheet. Meowth uses the native
overworld Pokemon sprite and remains a dialogue character.

Silph's rival encounter becomes a mandatory partner Double Battle. The rival
acknowledges the player's kindness and asks to put their rivalry aside. The
player is healed, selects exactly three Pokemon, and the rival brings Pidgeot
37, Arcanine 38, and the established counter starter evolved normally for level
39. Jessie brings Arbok 37, Yanmega 38, Wobbuffet 39; James brings Weezing 37,
Cacturne 38, Carnivine 39. Party restoration preserves all six original slots,
changes to selected Pokemon, and fainted status before Nuzlocke death handling.
Cancelling selection restores the previous facility context and permits retry.

## Deliberately pending

- Final location for the earlier fossil scientist (Cinnabar is a placeholder).
- Lossless Mega Nidoking source sprites for final pixel polish.
- Other overworld Mega Stone placements.
- Final rival / Victory Road approximately 59-60, Jack approximately 61, Elite
  Four approximately 62-65, Champion approximately 66: exact redesign pending.
- Cerulean Cave's final Jessie/James battle followed by Giovanni: exact postgame
  caps and rosters pending. No provisional final battle is claimed implemented.

## Validation and playtest

`.github/scripts/check_chaos_kanto.py` compiles focused host fixtures around the
actual progression functions. It checks all six rematch orders, cap changes,
stone priority/fallback, party restoration on win/loss-shaped fixtures, and
rival evolution. Native ROM compilation still validates actual engine headers,
trainer constants, and event assembly; it does not replace emulator testing.

Test a fresh run's early Gyms and Mt. Moon doubles; both Sabrina gates; Oak's
one-time gift including bag-full retry; all rematch orders and persistence;
Koga with one versus two usable Pokemon; the Card Key checkpoint; Silph selection
cancel/retry, a win, and a loss with all original party slots restored.


## Custom Mega Nidoking and Strange Fossil

Implemented Poison/Ground Mega Nidoking (81/130/95/115/89/95, BST 605).
Rampage boosts damaging moves by 1.4 and commits the user for three turns;
status moves do not start a commitment and the ability causes no confusion.
The lock expires once per turn (including spread attacks), releases on loss
of the ability, fainting, or depleted PP, and clears on switching.
Viridian Giovanni's Nidoking holds Nidokingite and uses Earthquake, Poison Jab,
Ice Beam, and Megahorn. Earlier Giovanni fights retain regular Nidoking.
Mega Nidoking is excluded from random encounter pools.

Mt. Moon's fossil researcher gives a bonus Strange Fossil after his battle.
This is a protected key item, separate from the Helix/Dome choice. Cinnabar's
fossil scientist is a placeholder identification location: he discovers there
is no ancient life to revive, recognizes Nidoking's energy signature, and gives
Nidokingite. A joking refusal still returns the stone. Bag-full and declined
study paths preserve the fossil for retry. Both events use reusable labels so
the scientist can move earlier without rewriting the reward or save state.
Oak's starter Mega Stone selection skips Nidokingite to preserve this reveal.

Sprites: exact four-view selection from the updated sprite master, attributed
to FYTYNo1, Mega Nidoking GBA sprite v2. Front/back normal and shiny palettes
are shared GBA indexes. The embedded master source is a JPEG screenshot;
lossless source recovery remains desirable for final pixel-level polish.
Party icons retain ordinary Nidoking's icon, as with existing icon palette rules.

## Rocket sprite and shortcut testing

Battle Tests includes Cole (Cerulean and Hideout), Vesper (Tower and Hideout),
Mt. Moon Jessie/James doubles, Silph Jessie/James with three selected player
Pokemon and three rival Pokemon, and the uninterrupted Cole/Vesper Silph
gauntlet. The latter gives six level-38 test Pokemon and does not heal between
battles or award the Card Key. Story checkpoints remain unchanged by the
practice scripts, although native trainer victory flags are recorded as with
other battle tests. Gym rematch shortcuts are also available.

Asset credits: Black Fragrant / Pokemon FireGold (Cole and Vesper source art);
Monicaccina, formerly Ody-chan (Jessie and James GBA-style pack). Source links:
https://github.com/TeamAquasHideout/Team-Aquas-Asset-Repo
https://www.deviantart.com/monicaccina/art/GBA-Jessie-and-James-353030680

Jessie and James battle art is centered on separate 64x64 canvases and indexed
to fifteen opaque colors plus transparency; overworld frames are assembled in
native nine-frame walking order on 144x32 sheets without resizing the artwork.

Debug safety review: Battle Tests has 15 entries, Rocket Tests seven, and Mega
Rematches three. The shared list builder enforces its twenty-entry capacity
and terminates cached labels within their buffers. Input is checked against
the current menu length before indexing. Debug trainer launches initialize
the native battle parameters and clear stale partner state. CI host checks
execute all 23 shortcut selections through the actual input, preparation and
launch functions with memory sanitizers; they also cover oversized menus, long
labels, idle input, cancellation and invalid selections. These are source and
host checks; Delta gameplay remains the final display/runtime test.

## Late Gym Water-matchup adjustment (tester feedback)

Blaine remains six Pokemon with a level-52 ace. Torkoal replaces Talonflame as
his level-48 lead: Drought, Heat Rock, Lava Plume / Solar Beam / Body Press /
Stealth Rock. Houndoom replaces Sludge Bomb with Solar Beam. The other species,
levels and moves stay the same. Sun reduces Water damage, strengthens Fire
attacks and supports immediate Solar Beam; Heat Rock keeps it active longer.
Blaine still uses regular Arcanine until the separately planned custom Mega
Arcanine/permanent-Mega implementation lands.

Viridian Giovanni remains six Pokemon with a level-58 Mega Nidoking ace.
Level-56 Storm Drain Gastrodon-West with Leftovers replaces Rhyperior:
Earth Power / Muddy Water / Ice Beam / Recover. It supplies a Water immunity
and recovery while retaining the Ground theme. The other five sets stay the
same. Game Corner and Silph Giovanni are unchanged.

Both late Gym teams add Smart Switching, HP Aware and Ace Pokemon AI flags.
They can use their defensive options and save Arcanine/Nidoking for last.
Neither team gains omniscient prediction, higher levels, IV increases or EV
buffs. Rampage remains a three-turn commitment, including the risk of an immune
Pokemon switching into the repeated attack.

## Gym teams at their level caps with perfect IVs

Every Pokemon on each Gym Leader roster now uses that battle’s full cap:
Brock 15, Misty 22, Lt. Surge 28, Erika 34, Sabrina 40, Koga 46,
Blaine 52 and Viridian Giovanni 58. Mega rematches use Brock 42, Misty 43
and Lt. Surge 44 for every team member. These levels supersede the earlier
staggered levels above. The story encounters with Giovanni retain their
existing levels. All eleven rosters have perfect 31 IVs in all six stats. Species, moves,
items, abilities and battle formats remain as previously defined; the same native rosters serve the debug shortcuts.
