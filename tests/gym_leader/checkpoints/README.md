# Internal campaign checkpoints

These are emulator snapshots from the earned-career development run, not completed releases or distributed player saves. Load them only with the exact ROM SHA-256 recorded beside each snapshot. Base64-decode and gzip-decompress the `.state.gz.b64` file, then verify its uncompressed SHA-256 before using it with the native QA emulator. Snapshots do not establish that flash sectors survive a restart; normal Save/Continue verification remains separate.

Rank three follows actual defense, paid evolution, practice and Lorelei controller battles. Read-only instrumentation chooses inputs. Development resumes were used while repairing the runner; a clean replay without `GLE_CAMPAIGN_RESUME` is still required before claiming an uninterrupted campaign.
