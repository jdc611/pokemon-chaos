# Locked October 4 addenda

These requirements supplement the current handoff. Implementation status is recorded separately; this document is not a claim that every feature already works.

## Pokémon Ranch

- Rear Pokémon Center door leads to a physical Pokémon Ranch representing PC storage. Do not rename the underlying Pokémon Storage System.
- Select a PC box/pasture; reuse one dynamically populated pasture. Every occupied slot represents its actual stored Pokémon, with natural wandering and existing overworld sprites. Empty slots remain empty.
- Interactions: Summary (normal summary interface), Withdraw (exact slot, party space required), Take Item (actual held item, bag space required), Cancel.
- No stat, EXP, friendship, or item generation bonuses. Preserve existing Nuzlocke storage restrictions, including the Grave box.
- Audit enabled sprites before creating assets, including persistent custom forms.
- Sign: “POKÉMON RANCH / Pokémon stored in your PC Boxes can roam freely here.”

## Availability before expansion

Build a complete enabled-family matrix of natural acquisition: Grass, Cave, Surf, Fishing, Gift/Static, NPC Trade, Game Corner, Fossil Museum, Legendary Static, Roaming, Special Event, or another deliberate method. Randomizer/custom starter coverage is not a natural placement. Distinguish verified gaps from unresolved scripted sources.

Use the audit before placing new encounters or finalizing roughly 12–20 meaningful trades. Avoid duplicating common wild species, fossils, Game Corner rewards, and major gifts/statics.

Preferred habitat pockets: Viridian/Route 2 meadow; Mt. Moon exterior/crater; Cerulean Cape; Vermilion coast; Rock Tunnel ledges; Lavender memorial garden; Celadon urban garden; Fuchsia/Safari preserve; Seafoam exterior islands; Cinnabar volcanic field; Route 23/Victory Road foothills; Sevii postgame habitats. Use day/night where appropriate.

Trade themes: Pewter minerals; Cerulean water; Vermilion electric/coastal; Lavender Ghost/Dark; Celadon urban/exotic; Fuchsia animals; Saffron Psychic; Cinnabar Fire/science/ancient; Sevii regional/hard-to-place families. Exact lists remain pending the audit.

## Signature ability fallbacks

Native species retain native behavior. Non-native users receive the following Chaos behavior; do not exclude an ability merely because it needs a species-specific form. Battle states are per party member and reset at battle end.

| Ability | Non-native behavior |
|---|---|
| Zero to Hero | First switch-out activates Heroic. Each subsequent entry gives +1 higher offense and +1 highest other boostable stat (never HP). Ordinary switching clears stages; the package reapplies without accumulating from repeated switches. First activation popup/message and persistent HERO marker. |
| Battle Bond | First KO activates BOND once per battle: +1 higher offense and +1 Speed. Further KOs do not stack. Persistent state/marker; ordinary switching clears stages. |
| Schooling | Above 25% HP: higher offense, Defense, Sp. Def ×1.2. At/below threshold bonuses disappear; healing can restore them. |
| Shields Down | Above 50% HP: Defense/Sp. Def +1 equivalent. First drop to at/below 50% breaks shield for the battle, removes defense benefit, grants +1 higher offense/+1 Speed. No reshield from healing. |
| Disguise | First damaging hit takes approximately 25% normal damage, then broken for the battle. |
| Ice Face | First physical damaging hit negated or heavily reduced; special hits do not consume. Snow/Hail can restore protection once. |
| Power Construct | At/below 50% HP activate COMPLETE once: approximately 25% effective max HP increase, +1 Defense/+1 Sp. Def; battle-only. |
| Stance Change | Shield: Defense/Sp. Def ×1.3, offenses ×0.85. Damaging move changes to Blade before attack: offenses ×1.3, defenses ×0.85. Only protect-style moves restore Shield. Ordinary status/recovery moves do not. Switching resets Shield. True multipliers, no stage stacking. SHIELD/BLADE marker. |
| Hunger Switch | Start FULL; alternate each end turn; switching resets FULL. FULL: self-recovery/healing berries ×1.2. HANGRY: outgoing and incoming move damage ×1.1. Ordinary STAB remains. Earlier STAB/non-STAB concept rejected. FULL/HANGRY marker. |

Palafin retains its native persistent Hero form until battle ends. Morpeko retains its native form cycling and Aura Wheel type behavior.

## Requires deliberate review before implementation

Forecast, Flower Gift, Gulp Missile, Commander, Embody Aspect, Multitype, RKS System, Tera Shift, Tera Shell, Teraform Zero, Zen Mode, and other species/form/partner-dependent cases found by a full ability-table audit. Do not invent unapproved fallback rules.
