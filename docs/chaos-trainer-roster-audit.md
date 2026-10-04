# Kanto trainer modernization

567 ordinary native FireRed trainer teams, 1,402 party slots, and 339 distinct replacement species. All levels, party sizes, trainer identities, battle formats, IVs and event IDs were preserved. Species are fixed authored rosters, independent of the encounter seed.

Early teams use basic species. Midgame and later teams advance through thematic evolutionary lines; 1,137 slots have moves selected from their actual level-up learnsets at their existing level. Late ordinary trainer aces may carry a Sitrus Berry. Existing ability assignments were cleared when changing species so the native generator assigns valid abilities.

The following 66 established or major story teams were excluded and verified byte-for-byte against the pre-change master. This protects rival starter counter logic, Gym Leaders, Giovanni, named Rockets, Elite Four, Champion and previously customized battles.

- `TRAINER_BOSS_GIOVANNI`
- `TRAINER_BOSS_GIOVANNI_2`
- `TRAINER_CHAMPION_FIRST_BULBASAUR`
- `TRAINER_CHAMPION_FIRST_CHARMANDER`
- `TRAINER_CHAMPION_FIRST_SQUIRTLE`
- `TRAINER_CHAMPION_REMATCH_BULBASAUR`
- `TRAINER_CHAMPION_REMATCH_CHARMANDER`
- `TRAINER_CHAMPION_REMATCH_SQUIRTLE`
- `TRAINER_CHAOS_COLE_SILPH`
- `TRAINER_CHAOS_JAMES_MOON`
- `TRAINER_CHAOS_JAMES_SILPH`
- `TRAINER_CHAOS_JESSIE_MOON`
- `TRAINER_CHAOS_JESSIE_SILPH`
- `TRAINER_CHAOS_VESPER_SILPH`
- `TRAINER_CRUSH_GIRL_CYNDY_2`
- `TRAINER_CRUSH_GIRL_SHARON_3`
- `TRAINER_CRUSH_GIRL_TANYA_3`
- `TRAINER_CRUSH_KIN_MIK_KIA_3`
- `TRAINER_CRUSH_KIN_RON_MYA_4`
- `TRAINER_CUE_BALL_PAXTON`
- `TRAINER_ELITE_FOUR_AGATHA`
- `TRAINER_ELITE_FOUR_AGATHA_2`
- `TRAINER_ELITE_FOUR_BRUNO`
- `TRAINER_ELITE_FOUR_BRUNO_2`
- `TRAINER_ELITE_FOUR_LANCE`
- `TRAINER_ELITE_FOUR_LANCE_2`
- `TRAINER_ELITE_FOUR_LORELEI`
- `TRAINER_ELITE_FOUR_LORELEI_2`
- `TRAINER_LEADER_BLAINE`
- `TRAINER_LEADER_BROCK`
- `TRAINER_LEADER_BROCK_REMATCH`
- `TRAINER_LEADER_ERIKA`
- `TRAINER_LEADER_GIOVANNI`
- `TRAINER_LEADER_KOGA`
- `TRAINER_LEADER_LT_SURGE`
- `TRAINER_LEADER_LT_SURGE_REMATCH`
- `TRAINER_LEADER_MISTY`
- `TRAINER_LEADER_MISTY_REMATCH`
- `TRAINER_LEADER_SABRINA`
- `TRAINER_NONE`
- `TRAINER_PKMN_PROF_PROF_OAK`
- `TRAINER_RIVAL_CERULEAN_BULBASAUR`
- `TRAINER_RIVAL_CERULEAN_CHARMANDER`
- `TRAINER_RIVAL_CERULEAN_SQUIRTLE`
- `TRAINER_RIVAL_OAKS_LAB_BULBASAUR`
- `TRAINER_RIVAL_OAKS_LAB_CHARMANDER`
- `TRAINER_RIVAL_OAKS_LAB_SQUIRTLE`
- `TRAINER_RIVAL_POKEMON_TOWER_BULBASAUR`
- `TRAINER_RIVAL_POKEMON_TOWER_CHARMANDER`
- `TRAINER_RIVAL_POKEMON_TOWER_SQUIRTLE`
- `TRAINER_RIVAL_ROUTE22_EARLY_BULBASAUR`
- `TRAINER_RIVAL_ROUTE22_EARLY_CHARMANDER`
- `TRAINER_RIVAL_ROUTE22_EARLY_SQUIRTLE`
- `TRAINER_RIVAL_ROUTE22_LATE_BULBASAUR`
- `TRAINER_RIVAL_ROUTE22_LATE_CHARMANDER`
- `TRAINER_RIVAL_ROUTE22_LATE_SQUIRTLE`
- `TRAINER_RIVAL_SILPH_BULBASAUR`
- `TRAINER_RIVAL_SILPH_CHARMANDER`
- `TRAINER_RIVAL_SILPH_SQUIRTLE`
- `TRAINER_RIVAL_SS_ANNE_BULBASAUR`
- `TRAINER_RIVAL_SS_ANNE_CHARMANDER`
- `TRAINER_RIVAL_SS_ANNE_SQUIRTLE`
- `TRAINER_TEAM_ROCKET_GRUNT_16`
- `TRAINER_TEAM_ROCKET_GRUNT_17`
- `TRAINER_TEAM_ROCKET_GRUNT_21`
- `TRAINER_TEAM_ROCKET_GRUNT_5`

## DexNav controls

R in DexNav registers a selection; pressing R again on the same species and encounter method clears registration. Holding R and tapping Start in the overworld prioritizes Debug over the DexNav search, including when a species is registered. Plain R continues to start the registered search.
