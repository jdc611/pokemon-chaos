# Chaos tracker controller skin

This is original artwork and configuration; it does not depend on an existing custom skin.

Build with Python 3 and Pillow:

```sh
python tools/chaos/delta/build_skin.py --output /tmp/chaos-delta-skin
```

The resulting `Pokemon-Chaos-Tracker.deltaskin` includes portrait and landscape representations for standard iPhone, edge-to-edge iPhone, and standard iPad. TRACKER is centered immediately below the 3:2 GBA screen. Its input list sends the supported `l` and `select` inputs together. The game handles raw keys so this also works with L=A selected, and consumes the chord when opening is temporarily unsafe.

Install from Delta Settings → Controller Skins → Game Boy Advance → the desired orientation → +, then choose the `.deltaskin` file. Select the imported skin. A ROM containing the tracker hooks is required. B closes the overlay; L/R change pages; Up/Down select an owned party Pokémon.

The build script verifies archive integrity, assets, identifiers, screen ratio, tracker placement, input names and control bounds. The game shortcut has native mGBA action/menu-state tests. **Actual Delta import, multi-touch dispatch and device testing are pending. The package is supplied for device playtesting; its import and touch behavior must not be described as device-verified.**

Primary schema reference: https://noah978.gitbook.io/delta-docs/skins (including Using Multiple Inputs). Official installation reference: https://faq.deltaemulator.com/using-delta/controller-skins.
