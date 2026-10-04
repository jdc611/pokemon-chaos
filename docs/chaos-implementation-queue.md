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

## October 3 addendum — current locked acquisition/progression direction

These decisions supersede older acquisition notes above. They are design locks, not claims that every event is implemented.

- **PokéRider:** rival awards it immediately after the Cerulean rival battle before Nugget Bridge. Only visited, valid towns; no indoor use or sequence-breaking destinations. Retain the later Badge-8 tracking interface direction without route listings or flashing location markers.
- **Pewter researcher:** add a restoration-area employee whose badge/progression-dependent dialogue hints at new options and encourages returning, without revealing the future fossil roster. Badge/species order and prices remain TBD.
- **Cerulean knowledge challenge:** repurpose the badge-explanation NPC. Three random questions per attempt, one each from hidden moderate/experienced/expert tiers, roughly 8–12 questions each. All three correct unlock Nature, Ability and Gender Changers. Friendly failure and fresh questions on retry. Final bank remains TBD; do not ship a guessed bank.
- **Bicycle:** first Bike Shop entry forces a short owner scene congratulating the silly millionth customer and awards the Bicycle without a voucher. Remove the fetch-quest requirement.
- **Game Corner Pokémon:** Porygon, Rotom, Dratini, Zorua, Larvesta, Jangmo-o, Toxel, Bagon, Beldum and Dreepy. Rotom purchase offers only forms actually implemented in the native master. Coin prices remain TBD.
- **Game Corner TMs:** premium attacks plus strategy/support, repeatable access and coverage of gaps rather than duplicating every useful TM. Reflect/Light Screen/Safeguard/Substitute/possibly Protect and Thunderbolt/Ice Beam/Flamethrower/Shadow Ball/Psychic/Brick Break/Aerial Ace are candidates, not a finalized list. List/prices remain TBD.
- **Celadon Department Store:** evolution/competitive utility hub based on supported native items; Link Cable, stones, Metal Coat, King's Rock, Upgrade, Dubious Disc, Razor items, Protector, Reaper Cloth and similar supported items are candidates. Ability Capsule may be normal stock; Ability Patch is special stock. Item prices remain TBD.
- **Marsh Badge clerk:** before Sabrina asks to see a real Marsh Badge; after Sabrina celebrates and unlocks Ability Patches and otherwise unplaced Mega Stones. Never sell stones assigned to an overworld placement, story, rematch, trainer challenge or other deliberate acquisition. Exact inventory remains TBD.

### Rematch reward correction — no Champion gate

**Latest user instruction supersedes the addendum's preferred postgame timing:** signature Mega Stones must not wait until Champion. Starting with Brock's rematch, each Gym Leader's first rematch victory gives their signature Mega Stone and the planned specialty TM, once only. Brock, Misty and Surge retain the existing unrestricted rematch order after Sabrina and before Koga. Their already locked signature Megas are Steelix, Gyarados and Manectric. Do not add a Champion gate to these rewards or move the required rematches to postgame. Later rematch availability still needs a progression decision; do not invent exact timing. Rematch stones are excluded from Celadon stock. Final rematch TMs and remaining signature assignments are TBD. **Blaine requires an alternative reward, TBD, because Arcanite is obtained separately in Lavender.** Nidokingite also retains its separate story route.

### Separate custom Mega acquisition stories

**Nidokingite:** find the Strange Fossil early around Mt. Moon (exact tile TBD). Pewter accepts it, then reports failed resurrection and anomalous material. Having Nidoking in the party optionally produces an unexplained glow/pulse; the scientist does not understand it, and the sequence works without Nidoking. Oak researches it and, with the Mega Ring, reveals and returns the impossible Nidokingite as a breadcrumb toward Chaos. Do not gate it behind Giovanni. Preserve the existing post-Sabrina Mega Ring timing unless deliberately revised; the new addendum leaves timing TBD only if not already implemented.

**Arcanite:** optional strong NPC in Mr. Fuji's Lavender house challenges exactly one selected party Pokémon with Mega Arcanine. First win awards Arcanite once; retries are allowed until victory. The player party must be restored correctly after the special format. NPC identity/name and level remain TBD. Arcanite must be implemented as a real supported stone/item; the current native Mega Arcanine uses no stone. Blaine can showcase it later but does not gate acquisition.

Still undecided: quiz bank, coin prices, Game Corner TM list/prices, Celadon item prices and Mega inventory, remaining rematch Mega assignments/TMs, exact Strange Fossil tile, Arcanite challenger identity/level, and later rematch unlock milestones. Do not auto-decide these.

### Auto Repel acquisition

The Viridian Teachy TV old man equips Auto Repel in the same short handoff, without a catching tutorial. It becomes available in the tools menu after the gift; the toggle remains off until the player enables it. Completed older saves retain access through the existing old-man scene state. A failed Teachy TV gift does not advance the handoff.
