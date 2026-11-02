# Repository and Source Audit

## Scope and provenance

Primary source is the user-supplied shared **Choose Rebuild Town** conversation. Its parent chain yielded 856 nonempty visible user/assistant messages: 422 user turns, 434 assistant turns. Original UUIDs and UTC plus America/Chicago timestamps are retained. Source dates span October 31–November 1, 2026 in America/Chicago. Raw downloaded HTML SHA-256: `d60e4077e3a94145ebf3661dfe1df26b3d898ca92a8279b70411ef4d136622c6`. The HTML payload itself is not committed because it includes app transport/irrelevant metadata. Only the visible design text is archived.

The archive follows the supplied share’s active parent chain. It does not claim to recover deleted/private alternative conversation branches. Empty/tool-only/redacted/internal messages are not design evidence. Embedded images are referenced through separately located assets.

## Repository branches and history

| Ref | Inspected tip |
| --- | --- |
| origin/backup-kanto-before-working-baseline-reset | fdd9f1e9c8b1baaf5809333c7e6d53d6482f5427 |
| origin/chaos-firered | dffbcb88c6431262284e4c043221919b437699a6 |
| origin/chaos-v2-checkpoint-2026-10-08 | c22af0a72f12d932046622bf61e4032fb8cbfe3a |
| origin/chaos-v3 | 2421e94573a9b98f76cac488930ff18a89a0ed44 |
| origin/custom-build | 6fff7893220db9b7421ef146a00e9a9bb81f1641 |
| origin/emerald-legacy | f55acb0aa002820af4e506f28721a5756fddc65b |
| origin/gym-leader-edition | c9db7884e17fa86ec109ccde32a7ebcd7da17a65 |
| origin/ironmon-development | 27692a2a0bac1cdbd3748c9c1e39d6a58f96285f |
| origin/master | c682247de55e99009adb692482b2232126c89c03 |
| origin/oaks-draft | 722f6d937c7c6093db1463cc0fffeed3c79fc527 |


All remote branch names and tips were inventoried; tree paths and relevant history were searched. The target GLE branch was at `c9db7884e17fa86ec109ccde32a7ebcd7da17a65`. Its difference from master `c682247de55e99009adb692482b2232126c89c03` was only the existing 255-line `docs/gym-leader-edition/DECISION-LEDGER.md`, added by `02f9d475faf70687f25b98fa6972b6b34f09591a` and amended by `c9db7884…`. No gameplay implementation of the recovered GLE design is established by that branch diff.

Commit history searches for GLE/voucher/legendary research and documentation changes found the existing recovery commits, not an exact hidden 72-species gameplay table. Historical non-GLE changes such as custom-build six-member Gym teams and prior Kanto rosters are not design authorization for GLE. This was a targeted historical/source audit, not a full gameplay QA audit or line-by-line review of every old engine file.

GitHub issues 1 and 2 were read; issue 2 body and all 7 comments archived. Pull-request collection returned no PRs. Issue 2 was generated through the connector and contains recovery summaries, not original design exchanges; exact shared transcript outranks its erroneous or incomplete summaries. Existing issue/ledger were not overwritten.

## Existing gameplay evidence

| File | Observed baseline behavior / relevance |
| --- | --- |
| data/maps/SeafoamIslands_B4F_Frlg/scripts.inc:157 | Articuno level50 static remains in baseline. |
| data/maps/PowerPlant_Frlg/scripts.inc:38 | Zapdos level50 static; separate Electrode34 objects also need future accounting. |
| data/maps/MtEmber_Summit_Frlg/scripts.inc:27 | Moltres level50 static. |
| data/maps/CeruleanCave_B1F_Frlg/scripts.inc:35 | Mewtwo level70 static. |
| src/caps.c | Existing badge-driven global cap code; not the new rank table. |
| src/roamer.c | Existing roamer state, movement and instance handling; usable source, not approved quest assignments. |
| src/dexnav.c / src/pokedex_area_screen.c | Existing encounter/discovery implementation areas to reconcile later. |


The [baseline scripted-species inventory](data/baseline-scripted-species-inventory.csv) records literal species constants in map setwildbattle/givemon/giveegg/setvar commands. It is a candidate source inventory, not a guarantee of reachable acquisitions; indirect/special code paths and dynamic pools require later engine auditing. Ordinary baseline wild tables and traditional trainers must not be treated as final GLE pools.

## Library and conversation retrieval

Library searches used Gym Leader, exact handoff title, legendary/voucher terms and exact named asset titles. Current concept handoff was read in full (153 lines), and its pasted form is archived at T0001. It predates later decisions and is not evidence that later rules are missing. Three named concept assets were read through image-file extraction; extraction limits/stat discrepancies are recorded in appendix 11. Metadata for matching showcase was located. Other-mode Kanto Battle & Story documents were identified and deliberately excluded as roster authority.

Personal-context searches covered: 18 groups/72 assignments; vouchers; Training Room/Potential/Notorious; recruitment/loyalty/return; rival hometown/type; betting; twelve distinctive quest names; exact later pools outside Choose Rebuild Town. Search results mainly returned secondary summaries or the same conversation. No independent later exact voucher pool or complete NPC/stage database was recovered. This does not establish that no other conversation exists.

To expand recovery, share any later thread that enumerated voucher species, named quest NPCs, or exact starter/TM lists. The provided Choose Rebuild Town thread is already archived; it does not need to be reopened or re-shared.
