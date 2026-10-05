# Chaos FireRed continuation — Build 213

Repository: jdc611/pokemon-chaos. Branch: master. Continue from actual current master; preserve existing systems and locked custom trainer rosters.

## Latest changes

Build 213 code commit: a8cbc56c8608e396a3f238e3d5e4928b487e37c2.
Workflow: https://github.com/jdc611/pokemon-chaos/actions/runs/37252547866 — green.

- Center-only illegal-party departure rejection now explicitly closes the message, faces the player north, then calls DoDoorWarp for the normal upward step and door open/close animation. It no longer uses the instant DoWarp in the normal just-left-Center path. Fallback recovery paths remain available. First warning line wrapped to fit dialogue box. Host regression check passes.
- Bike Shop greeting now uses its own persistent VAR_CHAOS_BIKE_SHOP_SCENE (0x40E4), rather than skipping its scene based on Bicycle ownership. Entry greeting should occur once even on an existing/debug save already owning a Bicycle, without adding a duplicate. No voucher or money requirement. Previous source skipped entry scene when bike already owned; whether this explains the user's exact failed save has NOT been reproduced.

## Verification still required before calling these visually confirmed

Build 213 compiled successfully and host Center departure tests pass. Native emulator runtime was unavailable in this resumed workspace and was being restored when the user requested saving/handoff. Do NOT claim these latest paths have had emulator verification yet.

1. Actual Center animated exit with illegal type/current ability party: full message fits, no text bump/artifact; dismiss; face north; walk back through door; enter same Center; controls usable and PC accessible. Valid-party exit stays normal. Only Center exits enforce rules; Labs/Marts do not. Test before first heal, Hard and Nuzlocke.
2. Actual Cerulean Bike Shop doorway entry, both no-bike and already-own-bike saves: forced greeting once, Bicycle acquisition if missing, no duplication, no repeat on reentry, no lockup.
3. If rejection still has visual bumps, inspect normal message window rendering/timing rather than assuming wrapping alone fixes it.

## Previously verified work to preserve

Build 212: Cerulean badge NPC offers three-question knowledge challenge (24 draft questions, 8 per hidden difficulty tier); all three correct unlock Nature/Ability/Gender changers. Draft bank authorized by user. Native menus, wrong/cancel/retry, widths and persistent unlock tested. Bike Shop native direct map entry tested there; latest user still reports ordinary entry missing, so retest actual doorway.
Build 211: 567 ordinary Kanto trainer teams modernized (1402 slots, 339 species); 66 locked teams untouched. DexNav R registration toggles off on same selection; hold R + tap START opens Debug and ends any active search HUD, preserving registration.
Build 208: actual Surf needs owned HM Surf tool and badge, no learned move; debug Give All HM Tools exists; early fishing works with rod.
Build 207: DexNav original green Land border restored, ghost text removed, fishing row/cursor centered using shared affine scale. Do not revert.

## Design constraints

Preserve passing Random BST, randomized evolutions, evolution-line type/ability encounter eligibility. Starters must have CURRENT filtered ability and be distinct. Illegal catches remain in pools; permit withdrawing and changing ability inside Centers; require legal current ability at Center departure.

Pewter fossil unlock order/prices, exact Mega placements, legendary locations, Mythical behavior and several shop/challenge details remain TBD. Do not invent them. Master already contains settled Mega Arcanine and Mega Nidoking assets. NG+ options are deferred until normal game stable. Water encounter table consolidation and availability audit are still pending; flexible capacity and non-overlapping rod tiers are required.

Latest locked world directions are in user handoffs: PokéRider after Cerulean rival; first Center heal PC/Pokevial; Teachy TV man also grants auto repel; Mega Ring/Nidokingite Oak event after Sabrina, unordered Brock/Misty/Surge rematches then Koga; stones rewarded starting those rematches, not Champion-gated. Blaine stone alternative TBD. Roamers must NOT reveal literal current routes or flash their exact routes on map.
