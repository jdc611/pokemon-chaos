#ifndef GUARD_CHAOS_ARCADE_H
#define GUARD_CHAOS_ARCADE_H
void ChaosArcadeUpdateCoinBanner(void);
void ChaosArcadeEnsureSave(void);
void ChaosArcadeRanchPalette(u16 start,u16 count,bool8 skipFaded);
void ChaosArcadeRiderPalette(void);
void ChaosArcadeRanchScenery(void);
void ChaosArcadeOutfitPalette(u16 tag,u8 slot);
void ChaosArcadeRefreshOutfit(void);
void ChaosRanchOptions(void);
void ChaosRanchThemeMenu(void);
void ChaosRanchDecorationMenu(void);
void ChaosRanchApplyCosmetic(void);
#endif
