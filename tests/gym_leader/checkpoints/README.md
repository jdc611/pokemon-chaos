# Internal campaign checkpoints

These are emulator snapshots from the earned-career development run, not completed releases or distributed player saves. Load them only with the exact ROM SHA-256 recorded beside each snapshot. Base64-decode and gzip-decompress the `.state.gz.b64` file, then verify its uncompressed SHA-256 before using it with the native QA emulator. Snapshots do not establish that flash sectors survive a restart; normal Save/Continue verification remains separate.

Rank three follows actual defense, paid evolution, practice and Lorelei controller battles. Read-only instrumentation chooses inputs. Development resumes were used while repairing the runner; a clean replay without `GLE_CAMPAIGN_RESUME` is still required before claiming an uninterrupted campaign.

## Earned Rank 4

`earned-rank-four.state.gz.b64` continues the earned Rank 3 development route through 16 real defenses. `earned-rank-four.sav.gz.b64` contains the 128 KiB flash written by the native Start > Save command, independently verified through power reset and title-screen Continue. Both are gzip-compressed base64; validate SHA-256 against their adjacent metadata. The flash save is genuine game output, not fabricated progress. This remains a resumed development campaign, not a completed clean playthrough.
