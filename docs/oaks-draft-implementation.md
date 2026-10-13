# Oak's Draft — integrated FireRed playtest

## Start

Start a new game, then press SELECT on PLAY STYLE until it says OAK'S DRAFT. Choose the existing difficulty, MGM and optional Nuzlocke. Draft options expose only Normal/Shuffle/Random base stats, Normal/Random abilities, and Normal/Random movesets. The mode starts in Kanto. Existing Normal and IronMON saves are not converted into Draft runs.

## Implemented mechanics

- Oak's new back room contains six piles of shipping crates. Choose one level-5 companion from each group of three. Future groups stay sealed; full Summary screens support comparison, IV/stat views and move details. SELECT opens a permanent-choice confirmation.
- Eligible baby/lowest-stage and single-stage official species span the enabled generations. Supported regional forms are included; temporary forms, custom species, legendary/mythical/Ultra Beast/Paradox classes and duplicate evolutionary families are excluded.
- All 18 candidates are deterministic. Names are unique within the initial draft, permanent and attached to personalities; short backstories appear in both the picker and Summary. They have no mechanical effects. An otherwise non-attacking initial moveset receives a usable starting attack.
- The rival chooses between your rejected options using current-form strength and existing team type coverage. His choice is announced. Oak takes the other companion without announcing a future team.
- The opening battle is a regular 6v6 single battle, with both teams at level 5. Winning and losing both continue the story and heal your team. This encounter is exempt from Nuzlocke deaths.
- Later rival battles retain all six identities. Their development uses the authored encounter's ace level, normal supported evolution branches, stronger level-appropriate moves, and tiered held items. The existing Silph partner format uses three of those companions during the team-up.
- Oak replaces the Champion in Draft mode only, with his six retained and developed companions. His name, portrait, overworld presentation and dialogue identify him. Victory enters the existing Hall of Fame sequence and opens the original/current report comparison before entering the Hall of Fame.
- Wild captures and free acquisition routes are blocked centrally. Eggs, Mystery Egg sales, Pokémon prizes, split-evolution extras and link-trade entry cannot add companions.
- Ordinary scripted gifts, Magikarp purchase, Dojo choices and fossils use a confirmed permanent one-for-one exchange. Only a living current party member can be surrendered. The replacement matches its level; existing costs, source flags and prerequisites remain. Items are returned safely before a companion is removed. Existing species-specific NPC trades remain available and keep permanent nicknames.
- Gift preview shows species, assigned nickname, level matching and backstory; randomized battle attributes stay hidden until confirmation.
- The Game Corner Pokémon counter becomes a mystery ability counter after its existing unlock. A reroll costs 2000 Coins, is deterministic and hidden until purchase, and is limited to one purchase per Pokémon. Its consumed marker follows evolution, boxing and saves. Existing legitimate ability-changing systems remain usable.
- Each Gym awards a valid TM compatible with at least one current companion. New compatible rewards are preferred; an exhausted pool has a compatible fallback. Previously acquired TMs are recorded. Awarded Gym TMs are resolved to other valid TMs at non-Gym sources, including pickups, shop lists and Game Corner lists.
- The player may bring the full party to all Gyms. Existing leaders, AI, caps, signatures, Mega events and Sabrina/rematch/Koga gates remain. Koga remains a Double Battle.
- Nuzlocke deaths after the opening battle go to GRAVE and leave vacancies. Vacancies and dead companions cannot be exchanged for gifts. Ordinary runs retain normal healing. Required field HMs still need ownership and badges, never a Pokémon that knows or can learn the move.
- The report preserves the original six, with Summary access and original/current analysis: attack balance, current attack types, shared weaknesses and remaining evolution paths. It is available in the draft room and near the Hall of Fame. It does not reveal unused crates or the hidden Oak team.

## Persistence and compatibility

Draft state is versioned and occupies the mutually exclusive IronMON save tail without changing save sizes or existing field offsets. Two previously unused encrypted Pokémon bits store membership and paid-reroll consumption without growing Pokémon structures. Original candidates are reconstructed from the saved seed/settings and picks. Future changes to the pool or randomization algorithm must preserve this generator version or provide migration before claiming compatibility with earlier Draft saves.

## Validation

The local native FireRed target compiled successfully. EWRAM: 253,896/262,144 bytes; IWRAM: 29,368/32,768 bytes; ROM payload: 27,233,292/33,554,432 bytes. The deliverable is a padded 32 MiB GBA ROM.

Passing checks include all 21 existing source regression suites, six stat/trait combinations, six permanent selections, deterministic identities and names, real flash save/reload mid-draft and after completion, three rival/Oak generation stages, central acquisition blocking, actual gift/reroll interfaces, the mapped room and all Summary screens, opening win/loss with Nuzlocke exemption, all eight mapped Gym battles and compatible rewards, Koga's Double Battle, Oak's Champion/Hall-of-Fame path, required HM commands and real evolution animations. Existing Normal/Nuzlocke/IronMON startup, legacy IronMON profiles, world policy, tracker overlay and terminal lab-loss checks pass. Battle outcomes use controlled HP, levels and moves to exercise transitions; they do not establish natural difficulty balance.

## Limits and follow-up

- A full natural playthrough and actual iOS Delta import/touch/backgrounding tests remain unverified. The ROM runs under native mGBA; the existing Delta skin remains a separate controller asset.
- Early six-member team balance and ability-reroll pricing need human playtesting. No equal-size Gym restriction has been added.
- Mandatory Double Battles still require two living companions. A depleted Nuzlocke team cannot trade a vacancy to replenish itself.
- Legendary difficulty, the relocated rival finale, and the larger Mewtwo/Pallet/Gym-succession story remain deferred by design.
- The compact report's first analysis panel shows up to six attack types and the two largest shared weaknesses; full individual details are available through Summary.

ROM SHA-256: `739e9325f14da79ad55ca0bdc98f3f99eec724c46a6d7f04de8ebd20e3720b2e`

## Changed files

- `data/layouts/OaksDraftRoom/border.bin`
- `data/layouts/OaksDraftRoom/map.bin`
- `data/maps/OaksDraftRoom/map.json`
- `data/maps/OaksDraftRoom/scripts.inc`
- `docs/oaks-draft-final-results.json`
- `docs/oaks-draft-implementation.md`
- `docs/oaks-draft-regression-results.json`
- `docs/oaks-draft-source-results.json`
- `include/oaks_draft.h`
- `src/oaks_draft.c`
- `src/oaks_draft_ui.c`
- `tools/chaos/native/draft-extra-layout.c`
- `tools/chaos/native/draft-layout.c`
- `tools/chaos/native/qa_oaks_draft.py`
- `tools/chaos/native/qa_oaks_draft_battles.py`
- `tools/chaos/native/qa_oaks_draft_gifts.py`
- `tools/chaos/native/qa_oaks_draft_gyms.py`
- `tools/chaos/native/qa_oaks_draft_progression.py`
- `tools/chaos/native/qa_oaks_draft_ui.py`
- `.github/scripts/check_chaos_arcade_items.py`
- `.github/scripts/check_chaos_randomizer.py`
- `.github/workflows/playtest.yml`
- `data/event_scripts.s`
- `data/layouts/layouts.json`
- `data/maps/CeladonCity_GameCorner_PrizeRoom_Frlg/scripts.inc`
- `data/maps/CeladonCity_Gym_Frlg/scripts.inc`
- `data/maps/CeruleanCity_Gym_Frlg/scripts.inc`
- `data/maps/CinnabarIsland_Gym_Frlg/scripts.inc`
- `data/maps/FuchsiaCity_Gym_Frlg/scripts.inc`
- `data/maps/PalletTown_ProfessorOaksLab_Frlg/map.json`
- `data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc`
- `data/maps/PewterCity_Gym_Frlg/scripts.inc`
- `data/maps/PokemonLeague_ChampionsRoom_Frlg/map.json`
- `data/maps/PokemonLeague_ChampionsRoom_Frlg/scripts.inc`
- `data/maps/SaffronCity_Gym_Frlg/scripts.inc`
- `data/maps/VermilionCity_Gym_Frlg/scripts.inc`
- `data/maps/ViridianCity_Gym_Frlg/scripts.inc`
- `data/maps/map_groups.json`
- `data/scripts/trainer_battle.inc`
- `data/specials.inc`
- `include/data.h`
- `include/global.h`
- `include/pokemon.h`
- `include/run_settings.h`
- `src/battle_script_commands.c`
- `src/battle_setup.c`
- `src/cable_club.c`
- `src/chaos_arcade_prizes.c`
- `src/chaos_v2.c`
- `src/evolution_scene.c`
- `src/field_specials.c`
- `src/item.c`
- `src/main_menu.c`
- `src/new_game.c`
- `src/party_menu.c`
- `src/pokemon.c`
- `src/pokemon_storage_system.c`
- `src/pokemon_summary_screen.c`
- `src/scrcmd.c`
- `src/script_pokemon_util.c`
- `src/shop.c`
- `src/shop_criteria.c`
- `src/trade.c`
