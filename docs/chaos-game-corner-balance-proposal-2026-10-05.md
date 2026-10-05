# Pokémon Chaos FireRed — Game Corner balance proposal

Updated October 5, 2026. **Starting economy accepted by the user on October 5, 2026. Prices/payouts/odds are approved starting targets; actual timing and measured returns still require prototype testing.** Build 216 remains the current ROM; this document does not change runtime behavior.

## Locked decisions carried forward

- An entrance NPC blocks the entire Game Corner until Erika's Rainbow Badge, saying it is under construction with exciting changes coming. All games and prize categories open together afterward.
- Restore the Coin Case requirement. A permanent, one-time first-entry event has a friendly NPC walk to the player, briefly introduce the arcade/prizes, automatically give the Coin Case and joke about the shady fellow in back. Preserve the Rocket/poster/hideout progression. Ownership, permanent event flags and full key-item-pocket handling must be safe on existing saves; the old generic `FLAG_GOT_COIN_CASE` is currently defined as zero and must not be used as a working permanent flag.
- Keep coins. Ordinary useful prizes should take approximately 10–15 minutes; premium prizes approximately 30–45 minutes at typical play. Longer games and higher difficulty pay more per completed game. Wager games offer higher potential rewards. Skilled play should be competitive with gambling.
- Free entry: Checkers, Type Match, Memory Match and reflex. Proposed Voltorb Flip entry is also free; this was not separately settled in the discussion. Wagers: slots, high-roller slots, Lucky Type Wheel, Rocket Risk and High-Low.
- Replace the once-daily allowance direction with a periodic real-time refill; two hours is the proposed interval. No balance threshold, no missed-refill accumulation. A small, poor-value coin purchase remains available with no purchase limit or separate earned/purchased balance. Avoid easy clock toggling; accept that emulator/save-state manipulation cannot be prevented completely. Daily Challenge remains a distinct activity; its reset/reward details below are proposals.
- Pokémon roster: Porygon, Rotom, Dratini, Zorua, Larvesta, Jangmo-o, Toxel, Bagon, Beldum, Dreepy. Offer only supported Rotom forms: base, Heat, Wash, Frost, Fan, Mow.
- Prize tiers: Normal, Hidden Ability, Shiny, Shiny + Hidden Ability. Hide HA tiers if there is no distinct supported natural HA. With random abilities, Normal follows the run's randomized normal ability; HA grants the actual species HA, clearly displayed. Apply MGM's existing IV/EV rules when enabled; otherwise ordinary generated stats, no extra perfect-IV or ideal-Nature grant. Premiums are exclusive to newly purchased Game Corner Pokémon.
- TMs only, no move tutor. Reusable TMs. Earthquake and similar top-tier moves remain for deliberate story placements. Expanded attack/support list is a proposal pending actual TM compatibility and availability audit.
- Three categories: Pokémon; TMs / Battle Items; Cosmetics. Buy/Owned cosmetic menus, preview before purchase, immediate application, persistent ownership, free reselection and free Default theme/color. Purchased decorations automatically use approved preset locations and can be toggled, without a placement editor.
- Initial proposed cosmetic lineup accepted: Forest/Beach/Night/Game Corner PC wallpapers; Classic/Midnight/Rocket PokéRider themes; bench/flowers/statue/fountain Ranch props; Forest/Beach/Snow/Night Ranch environments; black-red/blue-white/purple-gold outfits. Add white, black and premium gold outfits. Statue candidates: Rhydon, Lapras, Snorlax, Venusaur, Charizard, Blastoise; candidate species and preset locations still need final review. Reuse existing decorations/tiles where suitable; doll graphics need adaptation into statues rather than being mislabeled finished statue assets.
- Checkers: 8×8, mandatory capture, complete mandatory multiple jumps, short-range kings moving/capturing in both directions, promotion ends the turn. Same rules at every difficulty, with difficulty-dependent real AI/search and payouts; no cheating.
- Separate outstanding bug: Random starters with no filters must be base-stage Pokémon; Kingdra being offered is a regression to trace. Do not restrict an explicitly selected Custom starter by this rule. Other filtered-starter behavior remains intact.

## Economy targets and existing limits

**Typical target: 100 net coins per active minute**, after entry fees/wagers. Competent mixed arcade play targets roughly 80–120 net/minute; strong play around 150. Individual gambling sessions can lose coins. These are design targets, not measured prototype results. Playtesting must measure payout, elapsed active time, accuracy/win rate and loss streaks before final approval.

At 100 net/minute, 1,000–1,500 coins take 10–15 minutes; 3,000–4,500 take 30–45. Strong play at 150 reduces the latter to 20–30 minutes. Longer matches award larger individual prizes rather than rewarding deliberate stalling.

Current `MAX_COINS` is **9,999** (`include/constants/coins.h`). Every proposed single purchase fits. Rewards must handle the cap without overflow, wrapping, or charging for unavailable rewards. Explain any capped reward before it is credited. Current `I_REUSABLE_TMS` is already TRUE (`include/config/item.h`); preserve it.

| Refill / purchase | Proposed value | Notes |
|---|---:|---|
| Free periodic refill | 100 coins per eligible claim | Every two real-world hours, any balance, no accumulation; start cooldown on successful claim |
| Emergency purchase | 50 coins for ₽5,000 | Only this small bundle; unlimited purchases, no bulk option/discount |
| Daily Challenge | 150 bonus coins on success | Proposed once per real-world day; free entry, about 2–3 minutes, separate from refill |

100 free coins fund five standard spins, two high-roller spins or four 25-coin wager rounds. Buying a 4,500-coin Dreepy would cost ₽450,000 through 90 small bundles; this is intentionally much less efficient than playing. Free games provide immediate recovery from zero.

Clock rules need a tested implementation: backward adjustments never directly refresh a claim; suspicious forward jumps do not generate multiple claims; no backlog payout. Legitimate clock corrections must not permanently lock a save out. Exact thresholds and recovery rules remain open and should be tested alongside RTC behavior in Delta. Do not treat highest-seen future time as an irreversible permanent lock.

## Proposed arcade rewards

Rewards are **gross coins credited**, with entry cost separate. Ordinary rounds pay only on completion; leaving loses the round reward and any wager. Checkers forfeits pay zero. Any draw bonus requires a genuinely completed legal draw, not a cancel/restart shortcut.

| Activity | Entry | Proposed reward | Intended typical round / rate |
|---|---:|---|---|
| Standard Slots | 20 per spin | Outcome table below | About 10 spins/minute; expected +100 net/minute |
| High-Roller Slots | 50 per spin | Outcome table below | About 10 spins/minute; expected +150 net/minute, much higher variance |
| Voltorb Flip | Free | Completed board: 150 / 250 / 400 at levels 1 / 2 / 3; bust 0 | Roughly 2 / 3 / 4 minutes; skill progression toward 75–100/minute |
| Memory Match | Free | 6 / 8 / 10 pairs: 30 / 50 / 75; efficient-clear bonus 10 / 15 / 25 | About 30–75 seconds; streak/accuracy unlock harder boards; no payout for repeated individual pair flips |
| Type Match | Free | Ten questions: 10 per correct, +30 for 10/10 | About 60–90 seconds; 8/10 pays 80, perfect 130 |
| Reflex: Berry Timing | Free | Twenty prompts: +5 per correct hit, +20 for a perfect round | About 60 seconds; maximum 120, ordinary 60–90 |
| Checkers Easy | Free | Win 450, legal draw 50, loss 0 | Estimate 6-minute completed game: 75/minute on a win |
| Checkers Normal | Free | Win 900, legal draw 100, loss 0 | Estimate 8-minute completed game: about 113/minute on a win |
| Checkers Hard | Free | Win 1,500, legal draw 150, loss 0 | Estimate 10-minute completed game: 150/minute on a win |
| Lucky Type Wheel | 25 per spin | Common type win 400; rare type win 800; miss 0 | Proposed 24 equally likely sectors, described below; about 12 spins/minute targets +100 net/minute |
| Rocket Risk | 25 per round | Banked totals after safe draws: 25, 50, 100, 175, 300; bust 0 | Five-draw cap; 20% bust chance per draw; choosing when to bank determines risk/rate |
| High-Low | 10 per round | Banked totals at 1–5 correct: 15, 25, 45, 80, 150; wrong 0 | Ties redraw for free; rates depend on the actual next-species distribution and player decisions, so no fixed earnings claim |
| Daily Challenge | Free | 150 one-time completion bonus | Select from implemented skill games; proposed daily rotation/reset needs review |

Checkers win rates matter: Hard's 150/minute is a winning-match rate, not a guarantee. Losing games lower its realized earning rate. Board sizes and coin rewards for other games require timing tests; add/remove bonus amounts if experts can repeat them much faster than estimated. Do not pad games with tedious delays to force the estimate.

### Slot odds and payouts — proposed aggregate outcome targets

These are proposed overall result frequencies, **not yet tested reel-strip probabilities**. Actual reel design, stopping behavior, visible symbols and payout lines must realize and disclose consistent rules; do not claim these are measured native odds. First prototype/simulation review can adjust the targets before final approval.

| Result | Standard chance | Standard gross payout | High-Roller chance | High-Roller gross payout |
|---|---:|---:|---:|---:|
| No win | 55% | 0 | 65% | 0 |
| Small win | 30% | 40 | 25% | 100 |
| Medium win | 12% | 100 | 9% | 300 |
| Jackpot | 3% | 200 | 1% | 1,300 |
| Total | 100% | Expected 30/spin | 100% | Expected 65/spin |

Subtract the 20 / 50 wager: expected net **10 / 15 per spin**. These intentionally generous arcade returns should make ordinary play rewarding while leaving individual losses and jackpots meaningful. Avoid betting a balance the player cannot afford.

Lucky Type Wheel proposal: six common types each occupy two sectors; twelve other types each occupy one sector, for 24 total. A common type has a 2/24 chance at 400; a rare type 1/24 at 800. Both return 33⅓ coins on average for a 25-coin wager: expected profit 8⅓ per spin, with rare types giving bigger, less frequent wins. Which types are common remains open; show probabilities and gross prizes before betting.

Rocket Risk proposal: 20% independent bust per draw, no fake AI or changing odds based on winnings. The player may bank after any successful draw; after five safe draws the 300 is automatically banked. Banking at five gives expected gross 300 × 0.8⁵ = 98.304, minus the 25 fee = 73.304 per started round. At an illustrative two rounds/minute this is about 147 net/minute. Banking earlier lowers variance; actual throughput and UI timing require testing. The final reveal, no-result cancellation and payment order must prevent duplicate collection.

## Pokémon prices and upgrades

| Pokémon | Base coins | Typical active earning time at 100/minute |
|---|---:|---:|
| Porygon | 1,500 | 15 minutes |
| Zorua | 1,500 | 15 minutes |
| Toxel | 1,500 | 15 minutes |
| Rotom, any supported form | 2,000 | 20 minutes |
| Larvesta | 3,000 | 30 minutes |
| Dratini | 3,500 | 35 minutes |
| Bagon | 3,500 | 35 minutes |
| Jangmo-o | 3,500 | 35 minutes |
| Beldum | 4,000 | 40 minutes |
| Dreepy | 4,500 | 45 minutes |

| Tier | Surcharge added to species base price |
|---|---:|
| Normal | 0 |
| Hidden Ability | +1,000 |
| Shiny | +3,000 |
| Shiny + Hidden Ability | +4,000 total |

Example: Dreepy costs 4,500 / 5,500 / 7,500 / 8,500. The combined surcharge is not added twice. The maximum proposed total is 8,500, below 9,999. Premium gold cosmetics are a separate vanity reward, not a Pokémon stat boost.

No unique-form Rotom surcharge. Native form compatibility/moves must be correct on direct acquisition. Show selected species/form, ability tier, shiny status and final price before confirmation; charge only when delivery succeeds. Use existing party/PC delivery rules without adding duplication or losing a purchase when storage is full. Natural HA eligibility and randomizer/filter legality must be checked before displaying a purchase as legal; owning the case or paying coins does not bypass run rules.

## TM and battle-item prices

A reusable TM is a permanent move unlock, so duplicate purchases should be disabled if already owned. Existing copies found through story events remain valid. The final acquisition audit can remove redundancy with easy existing sources.

| TM group | Proposed coins per TM | Current native TM coverage |
|---|---:|---|
| Reflect, Light Screen, Safeguard, Protect, Aerial Ace | 1,000 | Existing |
| Taunt, Brick Break | 1,200 | Existing |
| Substitute, Thunder Wave, Defog | 1,200 | Needs TM additions |
| Thunderbolt, Ice Beam, Flamethrower, Shadow Ball, Psychic, Sludge Bomb | 1,500 | Existing |
| Will-O-Wisp, U-turn, Volt Switch | 1,500 | Needs TM additions |
| Energy Ball, Flash Cannon, Dark Pulse, Dragon Pulse, Roost | 1,800 | Needs TM additions |
| Trick Room, Tailwind, Encore | 2,000 | Needs TM additions |
| Earth Power, Moonblast, Aura Sphere | 2,200 | Needs TM additions |

The expanded list is implemented in Phase 3: current master has 67 TMs with existing item/HM IDs preserved and native per-species compatibility. Do not teach everything to every species. Surf stays an HM/story acquisition rather than being duplicated as an arcade TM. Earthquake, Close Combat, Draco Meteor and similar top-tier options are reserved for strategic world/story sources, not this draft prize counter.

| Battle item | Proposed coins | Purchase behavior |
|---|---:|---|
| Air Balloon | 250 | Repeatable |
| White Herb | 250 | Repeatable |
| Power Herb | 350 | Repeatable |
| Focus Sash | 500 | Repeatable |
| Choice Band / Specs / Scarf | 1,500 each | Repeatable |
| Life Orb | 1,800 | Repeatable |
| Assault Vest | 1,800 | Repeatable |

Before setting consumable prices, audit whether this run's battle/item restoration rules actually consume these held items permanently. If restored after battle, price them as reusable equipment rather than assuming ongoing replacement costs. Battle items are subject to their existing effects/rules; these rewards do not change them.

## Permanent cosmetic unlock prices

| Category | Proposed unlock | Coins |
|---|---|---:|
| PC wallpaper | Forest / Beach / Night | 400 each |
| PC wallpaper | Game Corner | 800 |
| PokéRider UI | Classic / Default | Free |
| PokéRider UI | Midnight / Rocket | 750 each |
| Ranch decoration | Bench | 300 |
| Ranch decoration | Flower beds | 300 |
| Ranch decoration | Each approved Pokémon statue | 750 each |
| Ranch decoration | Small fountain | 1,200 |
| Ranch environment | Default | Free |
| Ranch environment | Forest / Beach / Snow / Night | 1,000 each |
| Outfit | Default | Free |
| Outfit | Black-red / blue-white / purple-gold | 750 each |
| Outfit | White / black | 1,000 each |
| Outfit | Premium shaded gold | 6,000 |

White, black and gold change outfit colors with shading and readable outlines, preserving skin/hair/face details. Test every supported model's walk/run/bike/surf/fish/field actions and reflections before selling any outfit palette. Premium gold has no gameplay effect; it is an optional roughly 60-minute vanity goal at the typical earning rate.

Ownership is permanent. Buy previews an unowned choice, shows its cost, confirms and immediately applies it. Owned allows free application of themes/wallpapers/colors, or toggling decorations in fixed locations. Standard existing wallpapers stay free; do not put already freely available wallpapers behind a purchase. One active environment, one active PokéRider skin and one active outfit color; wallpapers remain independently assignable per PC Box. New statue assets, tile palettes, concrete decoration coordinates and the final arcade floor plan need visual review before implementation.

## Review and validation still required

1. Starting numerical balance accepted October 5. Confirm technical refill/clock recovery and Daily Challenge reset behavior before implementation; verify prototype economics and revise with playtest evidence.
2. Prototype games and measure net coins per active minute at realistic accuracy/win rates. Verify exact slot odds from actual reel logic, not just target table arithmetic.
3. Audit native TMs, new TM compatibility and alternate acquisition paths before locking the final list.
4. Preview the arcade layout and each cosmetic in all applicable themes/models. Preserve Rocket story tiles/events, entrance movement and Ranch object limits.
5. Verify RTC rollback/forward corrections and recovery, coin caps, interrupted transactions, owned-cosmetic persistence and all run modes (including MGM and ability randomization).

Approval of this proposal is not a claim that the arcade overhaul is already built. Build 216's no-case behavior remains until the replacement first-entry event and restored case checks are implemented together.
