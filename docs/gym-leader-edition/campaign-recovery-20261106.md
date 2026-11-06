# Campaign execution recovery — November 6, 2026

The execution environment became unavailable while preparing the Agatha retry. No further native execution was possible. This is an infrastructure stop, not a completed-release milestone.

## Durable state

- Tested main branch checkpoint: a7bc490830f4883b295002f4c78d4dc5f0bbd5a0 (MASTER section 55, native five/six registration repair).
- Padded ROM SHA-256: a2bf30748226a4b63f20f096638bcde1d9a2f14b55748c3577b72735c87af4e2.
- Latest durably archived earned career: Rank 6, 61 official wins, 23 losses, 5 rank XP. Decode and verify tests/gym_leader/checkpoints/earned-rank-six.sav.gz.b64 against its JSON metadata; continue through the actual title screen. Its snapshot also exists, but requires the exact ROM hash.
- The latest native Save/Continue check preserves all five registered identities.
- MASTER Library file is version 34, same identity libfile_e2e03241598081919e9eb1fe9aed90bc.

## Observed progress after that archive

The resumed Rank 6 route reached 92 official wins, 28 losses and 780 rank XP; all five registered partners reached level 65. Agatha's actual first inspection defeated that team. Inspection losses do not increment official defense losses. If the workspace survives, the post-loss checkpoint is in career-agatha-corrected-qa/career-agatha-loss.state. No Rank 7 victory was earned.

The continuous fresh title-screen replay defeated Lorelei, earned Rank 4, and reached its Bruno-preparation practice phase without importing emulator snapshots or flash. Its active process was not verified after the environment disconnected. If preserved, outputs are in continuous-fresh-career-qa. Do not claim that this prefix completed Bruno, Agatha or the full campaign.

## Runner repair and next actions

The original long Rank 6 defense was a controller-policy bug: it repeatedly selected a move Disabled by Igglybuff. Compiled volatile-bitfield masks now exclude Disabled, Tormented, Taunted, Encored and Throat Chopped moves. The corrected controller completed that defense and continued to Agatha without game changes or supplied battle outcomes.

The draft retry leads with Lilligant for Stun Spore against Arbok, then Wormadam, Abomasnow, Dhelmise and Arboliva; Dhelmise uses its real Phantom Force against the Ghost opponents. This draft has not executed. Recompile campaign_layout.c to include the added Ice/Fairy constants and status offset, retain volatile-masks.bin, and test the retry with genuine controller actions. If the loss state was lost, restore the durable Rank 6 save and replay the remaining defenses. Do not fabricate the missing progress.

native_lance.py is a prepared, unexecuted continuation. Remaining work includes earning Agatha and Lance victories, real Regional/Kanto wins, Oak, remaining coverage and full campaign/save/release validation. The eleven release gates remain false. No completion percentage or finish-time guarantee is justified.
