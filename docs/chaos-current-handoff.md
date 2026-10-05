# Chaos FireRed continuation — Build 216

Repository: jdc611/pokemon-chaos. Branch: master. Continue from actual current master; preserve existing systems and locked custom trainer rosters.

## New Game Corner direction — October 5 (design only)

Read `docs/chaos-game-corner-balance-proposal-2026-10-05.md` for the full locked discussion and starting economy accepted October 5. Read `docs/game-corner-layout/README.md` for the newly requested arcade and fixed Ranch decoration layout proposals; their arrangement is awaiting visual review. Entire arcade is blocked by a construction NPC until Erika; everything opens together afterward. Restore Coin Case checks alongside an automatic permanent first-entry gift/intro, preserving Rocket/poster story. Build 216 is still the runtime ROM and still has case checks removed; do not describe the new event or games as implemented. Cosmetic purchases immediately apply, permanently unlock and can be freely reselected through Buy/Owned with Default. TMs are reusable (already TRUE); tutor service rejected. HA premiums give actual native HA even with randomized abilities; all prize tiers follow MGM. Checkers uses approved standard forced-capture/multi-jump/promotion rules. Current working coin cap is 9,999.

User reported Kingdra from Random starters with NO filters: these must use base-stage species. Trace and fix this separately without restricting explicit Custom choices or reverting established filtered behavior. Starting prices/payouts/odds were accepted after the draft. Detailed RTC behavior, visual layout approval and new TM assignments still need review. Diagrams are planning artifacts; no arcade overhaul or new ROM has been implemented by these planning commits.

## Latest changes — Build 216

- User revised DexNav to exactly two rows of six visible entries for LAND and WATER. Full-size native Pokémon icons and cursor restored (no affine shrink). All baked-in old WATER pixels under the registration strip are cleared; the R button's bottom is rebuilt without old header lettering. Water retains all 15 possible source slots internally, using L to flip between twelve-slot pages only when more than twelve unique species/method entries exist. Empty page-two cells safely return no species. Existing wild tables, exact Surf/rod labels, R toggle registration and Nuzlocke view-only behavior remain intact.
- Removed the shared Center wall poster/map above and beside the Ranch rear door. Door, warp and PC remain functional.
- Ranch door and pasture-sign scripts close the message frame before opening the dynamic box menu; this prevents standard-menu graphics from overwriting a still-visible sign frame. The box menu now occupies the screen by itself, and Cancel leaves a clean field.
- Removed Coin Case gates from coin sellers, gamblers, slot machines and all prize counters in native Celadon and the shared Hoenn Game Corner/roulette scripts, plus hidden coin pickups. Currency remains coins; existing prices, payment checks and MAX_COINS limits are unchanged. Coin collection and capacity messages no longer claim a required case. The optional Restaurant Coin Case gift remains available.

Build 216 compiles successfully. All 12 host regression scripts passed. Native mGBA screenshots confirm two rows per panel, original icon scale, clean R/header background, cleared Center poster and clean Ranch selector/cancel. Native checks passed exact overflow identities, safely empty page-two selections and repeated L page redraws; ordinary Ranch exact-slot withdrawal, held-item handling, Grave guard and rear-door return still pass. On a native save without the Coin Case, coin purchase increased the balance from 0 to 50, the slot-machine callback started, and the Pokémon prize clerk opened the redemption menu. Existing starter override and Center return checks remain green.

## Latest changes — Build 215

Locked requirements are preserved in `docs/chaos-locked-addenda-2026-10-04.md`. Build 214 changes below remain intact.

- Pokémon Ranch: rear doors in all 19 native Kanto/Sevii Centers lead to a reusable pasture for the selected PC Box. Normal PC Storage naming remains unchanged. Up to 30 actual boxed Pokémon wander; viewport spacing keeps active objects within the native limit. Summary uses the normal box Summary interface. Withdraw transfers the exact slot without healing or changing its data, only if party space exists. Take Item adds the actual held item to the bag before clearing it; full bags, Mail and Nuzlocke Grave boxes are guarded. Empty slots remain empty. Ranch provides no encounters, bonuses or farming mechanics. Return uses the originating Center's actual rear doorway tile.
- Nine locked species-dependent ability fallbacks implemented: Zero to Hero, Battle Bond, Schooling, Shields Down, Disguise, Ice Face, Power Construct, Stance Change and Hunger Switch. All forms in their native Pokédex families retain the existing native implementation. Per-party battle state survives switching where specified and resets at battle end. Small name/HP markers show the current state.
- Zero to Hero: first switch-out activates HERO; each subsequent entry gives +1 higher offensive stat and +1 highest remaining boostable stat. Re-entry cannot accumulate switch bonuses. Bond triggers only once after a qualifying KO; its stages clear normally on switching while its activated state remains.
- Schooling scales the higher offense and both defenses by 1.2 above 25% HP. Shields Down supplies defensive equivalents until its one-way break at 50%. Non-native Disguise and Ice Face reduce the qualifying first hit to 25%; special attacks do not consume Ice Face, and snow/hail can restore it once. Substitute damage does not consume either protection.
- Power Construct temporarily grants +25% max HP and +1 both defenses, with normalization during stat recalculation, battle cleanup and capture. Stance Change uses stat multipliers, resets to Shield on switching, and changes back only for protect-style moves (including Endure), not generic status moves. Hunger Switch alternates FULL/HANGRY at turn end and resets FULL on switching. FULL boosts HP-healing berries and self-recovery hooks (Recover-family, Rest, weather recovery, Swallow) by 20%; drain, Wish and passive recovery are not boosted. HANGRY modifies outgoing and incoming damaging moves by 10%.
- Complete enabled-family acquisition audit and full ability-table audit saved in `docs/chaos-availability-matrix.{md,json}` and `docs/chaos-signature-ability-audit.json`; regenerate with `.github/scripts/audit_chaos_availability.py`. The current source audit groups 1,573 enabled species/form records into 540 families: 99 have confirmed acquisition sources, 2 need roamer activation-script review, and 439 have no audited natural path. These are audit findings, not new encounter assignments or proof that all variable-producing C paths are reachable. Kanto/FireRed sources are separated from Hoenn/LeafGreen tables. No new habitat tables or exact trade list were invented.
- Sprite audit: 1,453 enabled records have explicit overworld images; 120 temporary/form records resolve through existing compatible same-Dex images. No family lacks a compatible sprite. No enabled record named Chaozar, Chaossal, Hitmonorris or Dragon Eevee was found in this checkout, despite older handoff references; resolve those custom-species discrepancies before claiming implementation.

## Build 215 validation and continuation

FireRed ROM links successfully (32 MiB). All 12 host regression scripts pass, including actual-module battle-state tests and Ranch transaction/object-limit checks. Native mGBA confirms the rear-door selector and actual entry, correct Center return (rear doorway tile), normal Summary and return to Ranch, exact-slot withdrawal/item removal, full-party and Grave protections, and the actual first Zero to Hero switch-out/re-entry sequence (+1/+1 with visible HERO marker). Native arithmetic checks also cover Schooling threshold and Disguise first-hit reduction. Full turn-by-turn emulator coverage of all nine fallbacks remains a playtest task; host checks cover all nine and native-family guards.

Pending deliberate decisions: Forecast, Flower Gift, Gulp Missile, Commander, Embody Aspect, Multitype, RKS System, Tera Shift, Tera Shell, Teraform Zero, Zen Mode and other form/partner-dependent candidates in the full audit. Do not exclude abilities or invent their fallback rules without review. Use the matrix to propose roughly 12–20 gap-filling trades and the locked habitat pockets before implementation. No exact list has been finalized. Preserve the user-reported Random Items pickup bug and ordinary Bike Shop doorway greeting verification below as pending.

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

Pewter fossil unlock order/prices, exact Mega placements, legendary locations, Mythical behavior and several shop/challenge details remain TBD. Do not invent them. Master already contains settled Mega Arcanine and Mega Nidoking assets. NG+ options are deferred until normal game stable. The acquisition matrix is now built; habitat/trade assignments and a broader aquatic non-overlap audit remain pending; flexible capacity and non-overlapping rod tiers are required.

Latest locked world directions are in user handoffs: PokéRider after Cerulean rival; first Center heal PC/Pokevial; Teachy TV man also grants auto repel; Mega Ring/Nidokingite Oak event after Sabrina, unordered Brock/Misty/Surge rematches then Koga; stones rewarded starting those rematches, not Champion-gated. Blaine stone alternative TBD. Roamers must NOT reveal literal current routes or flash their exact routes on map.
