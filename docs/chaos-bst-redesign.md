# BST redesign — implemented and verified

October 6, 2026.

- BST Shuffle redistributes the species' exact normal total among all six stats; it does not merely rearrange the original six values. Supported species retain their full total, with stats bounded to 5–220.
- Random BST rolls every stat independently. Ordinary rolls are 5–160; approximately 5% of each stat's rolls are 161–220. No total budget or archetype is imposed. Species/seed results are deterministic.
- Randomized evolution selection now compares actual run BSTs. It scans the correct middle/final/legendary pool from the seeded starting position and prefers a total at least as high as the source. If an exceptional source roll exceeds every eligible target, it uses the strongest eligible target instead of hanging or preventing evolution. This safeguards total power, not every individual stat or matchup.
- Both evolution-graph scans skip disabled species before consulting their evolution tables.

Verification: `python tools/chaos/check_bst.py` executes the actual C implementations with fixture species: 25,200 stat profiles, 7,560 evolution selections, 4.93% extreme rolls. FireRed compile/link succeeded. Native mGBA function checks covered 755 Kanto species/seed profiles and 370 randomized evolutions (368 non-downgrades and two verified strongest-target fallbacks).

These BST changes are independent of the unfinished Cerulean Cave, simulator and Pallet story implementation. A successful BST validation build does not validate those pending changes.
