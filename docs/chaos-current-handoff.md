# Chaos FireRed continuation — Build 214

Repository: jdc611/pokemon-chaos. Branch: master. Continue from actual current master; preserve existing systems and locked custom trainer rosters.

## Latest changes — Build 214

- Custom starters with Random Abilities and an Ability filter can choose any otherwise eligible species. Type restrictions still apply. The individual starter receives the filtered ability in normal slot 0, and keeps it through boxing, saving/copying and evolution. Ordinary members of its species retain the seeded ability table. Normal Abilities keep the natural normal-slot eligibility rule; Hidden Abilities are not granted by filters.
- The individual ability uses nine previously unused encrypted Pokémon bits; no Pokémon or save structure grew. Party/box legality, battle entry/switching and Summary resolve that individual ability. Explicit Ability changes replace the override.
- Center rejection hides the map-name popup before opening dialogue. The popup had been scrolling the shared BG0 layer while the warning printed. Animated return follows the exterior doorway's actual interior warp ID instead of always selecting warp 0, so native Kanto return lands centered on the exit mat (7,8 in Cerulean).
- Teachy TV gift scene checks ownership and completion before displaying the gift. Older saves owning the TV with a stale scene value are repaired to completed state.
- DexNav displays only LAND and WATER panels. WATER contains Surf and each rod method with one exact method per displayed entry, not combined rod labels. There is space for all 15 source slots as needed (18 display positions), so previously truncated water/fishing choices are retained. LAND stays green; WATER stays cyan; right info panel, R register/toggle and Nuzlocke view-only behavior are preserved. This is display consolidation; existing wild tables and randomized encounter generation were not rewritten. Broader aquatic availability/non-overlap audit remains pending.

## Validation

Local FireRed ROM compiled successfully. All ten existing/new host regression scripts pass.
Native mGBA checks passed:
- Actual Cerulean Center exit with an illegal current ability: full readable warning, popup removed, turn/door return and centered (7,8) arrival on the red mat.
- Actual Oak acquisition: selected custom Pikachu with Random Abilities + Huge Power filter is given to party slot 0 with Huge Power immediately.
- Encrypted starter override resolves in party and box, survives copying and species evolution, retains normal slot 0 and passes filter legality.
- Teachy TV ownership guard repairs stale scene state without repeating the gift/dialogue.
- DexNav two-panel rendering visually inspected; five-row/six-column cursor navigation wraps; R registration toggles off; Nuzlocke A remains blocked within DexNav.

Bike Shop Build 213 ordinary doorway greeting still needs verification for both no-bike and already-own-bike saves. No new Bike Shop changes in Build 214. Keep the Random Items user-reported pickup bug pending until native pickup behavior is traced and verified; source regression success alone is not proof of the user's runtime case.

## Previously verified work to preserve

Build 212: Cerulean badge NPC offers three-question knowledge challenge (24 draft questions, 8 per hidden difficulty tier); all three correct unlock Nature/Ability/Gender changers. Draft bank authorized by user. Native menus, wrong/cancel/retry, widths and persistent unlock tested. Bike Shop native direct map entry tested there; latest user still reports ordinary entry missing, so retest actual doorway.
Build 211: 567 ordinary Kanto trainer teams modernized (1402 slots, 339 species); 66 locked teams untouched. DexNav R registration toggles off on same selection; hold R + tap START opens Debug and ends any active search HUD, preserving registration.
Build 208: actual Surf needs owned HM Surf tool and badge, no learned move; debug Give All HM Tools exists; early fishing works with rod.
Build 207: DexNav original green Land border restored, ghost text removed, fishing row/cursor centered using shared affine scale. Do not revert.

## Design constraints

Preserve passing Random BST, randomized evolutions, evolution-line type/ability encounter eligibility. Starters must have CURRENT filtered ability and be distinct. Illegal catches remain in pools; permit withdrawing and changing ability inside Centers; require legal current ability at Center departure.

Pewter fossil unlock order/prices, exact Mega placements, legendary locations, Mythical behavior and several shop/challenge details remain TBD. Do not invent them. Master already contains settled Mega Arcanine and Mega Nidoking assets. NG+ options are deferred until normal game stable. Water encounter table consolidation and availability audit are still pending; flexible capacity and non-overlapping rod tiers are required.

Latest locked world directions are in user handoffs: PokéRider after Cerulean rival; first Center heal PC/Pokevial; Teachy TV man also grants auto repel; Mega Ring/Nidokingite Oak event after Sabrina, unordered Brock/Misty/Surge rematches then Koga; stones rewarded starting those rematches, not Champion-gated. Blaine stone alternative TBD. Roamers must NOT reveal literal current routes or flash their exact routes on map.
