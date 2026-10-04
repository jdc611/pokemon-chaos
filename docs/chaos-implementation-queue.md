# Chaos Kanto implementation decisions

## Locked direction

Native FireRed/Kanto is the primary game. Preserve seeded Random BST, randomized evolutions and evolution-line-aware Type/Ability eligibility.

Setup order: Play Style → Randomizer settings → Seed → viable Type/Ability filters → Review. Both filter orders use the same confirmed seed. Changing seed or dependency settings invalidates eligibility and clears filters.

Ordinary gifts and statics use seeded Random/Scaled replacement. Major legendary/mythical rewards use original BST ±50 in both modes; custom story species remain fixed. An empty eligible pool falls back to the original species instead of producing an invalid Pokémon. Starters remain controlled by starter settings.

Overworld item balls and hidden pickups use the saved world seed. Key items, HMs, fossils and Mega Stones stay protected until intentional reward placements are decided. A full bag retry cannot reroll a pickup.

Rod progression: Old Rod at Viridian waterfront, Good Rod in Vermilion fishing hut, Super Rod in Fuchsia fishing hut. Route 12 retains the Magikarp record interaction without another rod reward. Existing saves retain already acquired rods.

Already Mega-evolved party Pokémon announce their form when sent out or switched in, for either side. This announcement does not change evolution mechanics.

## Pewter Museum fossil hub

The Museum will offer fossil Pokémon from all supported generations, expanding by badge count, with purchase/restoration rather than requiring every fossil item. Rewards participate in the ordinary gift/static randomizer. **Badge unlock order and prices remain undecided.** Do not introduce live purchase rewards using guessed values. Retain current resurrection access until the replacement is playable. Cinnabar can retain Chaos/Mega/Strange Fossil story work. Physical fossil rewards are candidate Mega Stone sites, not approved placements.

## Mega acquisition

Use `chaos-content-inventory.md` generated from the actual enabled build. Hitmonorris is a separate Pokémon and gets no Mega Stone. Exact stone placements are undecided. Arcanite needs an item implementation before acquisition planning.

## Legendary, mythical and Ultra Beast direction

Major legendaries should use meaningful statics, including appropriate native Kanto locations and new chambers/branches in existing areas. Late field abilities gate exploration: revisit an earlier inaccessible area, open a branch, complete a short exploration/puzzle, then encounter its legendary. Rock Smash is intended as a late exploration key; Strength, Surf and Waterfall can diversify gates. Exact locations and Rock Smash acquisition are undecided.

Ultra Beasts activate as late-game roamers after Badge 8. Persistent HP/status is preferred. Which mythicals roam remains undecided.

**User correction:** do not list current routes for roamers after unlocking them and do not flash current-route markers on the PokéRider map. The exact discovery/tracking interface still needs a design decision; do not implement the old explicit route list.

Chaozar remains the fixed story/box legendary, separate from randomized rewards and roamers. The current custom Chaos species are protected from gift/static randomization.

## Story direction

Oak notices and genuinely investigates increasingly impossible appearances, Rocket/Mega phenomena and weather/world anomalies. He does not secretly know all answers. Near the end, Oak urgently summons the player; a storm/distortion hits Pallet, displaces/destroys the player and rival homes, and Mom and Daisy disappear. The player enters a distorted maze toward Chaozar, whose unstable Water/Fire nature causes Chaos without deliberate evil. Continue integrating Oak, Rival, Giovanni, Cole, Vesper, Jessie and James.

## Pending design

- Fossil badge unlock order and prices.
- Mega Stone placements.
- Legendary chamber assignments.
- Mythical encounter formats.
- Rock Smash acquisition timing/location.
- Postgame structure and story breadcrumbs.
- Full normal-mode Pokémon availability across Kanto.

Do not fill these gaps with assumed final decisions.
