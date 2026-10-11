# Chaos tracker controller skin

This is original artwork and configuration; it does not depend on an existing custom skin.

Build with Python 3 and Pillow:

```sh
python tools/chaos/delta/build_skin.py --output /tmp/chaos-delta-skin
```

The resulting `Pokemon-Chaos-Tracker.deltaskin` includes portrait and landscape representations for standard iPhone, edge-to-edge iPhone, and standard iPad. TRACKER is centered immediately below the 3:2 GBA screen. Its input list sends the supported `l` and `select` inputs together. The game handles raw keys so this also works with L=A selected, and consumes the chord when opening is temporarily unsafe.

The Dark Comfort skin has a near-black background, light gray controls with dark labels, and red A/B buttons. Its layout uses almost the full portrait width and places the game screen lower. The four directions are separate buttons with unmapped gaps, and the main controls sit closer to the bottom for thumb reach. The game picture retains its native proportions; filling most of a tall portrait display would require stretching or cropping. A distinct Dark Comfort skin identifier allows it to coexist with the previous skin. Installing it does not require a new ROM or modify save data.

Install from Delta Settings → Controller Skins → Game Boy Advance → the desired orientation → +, then choose the `.deltaskin` file. Select the imported skin. A ROM containing the tracker hooks is required. B closes the overlay; L/R change pages; Up/Down select an owned party Pokémon.

The build script verifies archive integrity, assets, identifiers, screen ratio, tracker placement, input names and control bounds. The game shortcut has native mGBA action/menu-state tests. **Actual Delta import, multi-touch dispatch and device testing are pending. The package is supplied for device playtesting; its import and touch behavior must not be described as device-verified.**

Primary schema reference: https://noah978.gitbook.io/delta-docs/skins (including Using Multiple Inputs). Official installation reference: https://faq.deltaemulator.com/using-delta/controller-skins.

## Fire / Water Neon reference skin

Build the illustrated variant with `python tools/chaos/delta/build_neon_skin.py --output /tmp/chaos-neon-skin` (Pillow and NumPy). It produces `Pokemon-Chaos-Fire-Water-Neon.deltaskin`, named **Pokemon Chaos Fire Water Neon** in Delta. This is a skin-only package; it does not change the ROM or save. The source artwork is retained in `assets/fire-water-dragons.png`.

The user's reference is recreated as obsidian fire/water dragons, dark glass controls, red-to-blue luminous rims, red A, blue B and separated arrow buttons. The working portrait layout retains lower thumb reach and TRACKER immediately beneath the real 3:2 viewport. Menu is provided alongside Select/Start. The reference's painted title screen is replaced by actual game output. Its dragon artwork is regenerated rather than pixel-identical, and proportions adapt to the taller phone. Glow is static, not an animated LED effect. Six validated device/orientation layouts are provided; physical iOS import and touch feel remain unverified.

Artwork uses the built-in image-generation tool with the user's reference: a tall 9:19.5 background only, no screen/buttons/text, black obsidian dragon heads in the lower half, red glowing eye and lava on the left, blue glowing eye and luminous water/lightning on the right, dark obsidian in the upper viewport area. Native deterministic code draws all neon controls, labels and the transparent live viewport, and verifies that touch regions do not overlap.
