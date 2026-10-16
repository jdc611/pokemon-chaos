#ifndef GUARD_CUSTOM_BUILD_H
#define GUARD_CUSTOM_BUILD_H
#include "global.h"
#include "pokemon.h"
// Serialized only into deprecated, unused Dex flag buffers; never enlarge saves.
struct __attribute__((packed)) CustomBuildRecord {
 u32 personality;
 u16 stats[6], finalBST, rootSpecies, builtSpecies, ability;
 u8 megaGain, flags;
};
bool32 IsCustomBuild(void);
void CustomBuildInitialize(void);
struct CustomBuildRecord *CustomBuildFind(u32 personality);
u32 CustomBuildBaseStat(struct Pokemon *mon,u32 stat);
u32 CustomBuildAbility(struct BoxPokemon *mon);
u32 CustomBuildSum(const struct CustomBuildRecord *record);
u32 CustomBuildReserve(enum Species species);
u32 CustomBuildDepth(enum Species species);
bool32 CustomBuildHasMega(enum Species species);
enum Species CustomBuildMegaTarget(struct Pokemon *mon,enum Item item);
void ItemUseOutOfBattle_CustomMega(u8 taskId);
void ItemUseCB_CustomMega(u8 taskId,void (*task)(u8));
void CustomBuildGenerate(u16 species[18]);
void CustomBuildCandidate(struct Pokemon *mon,struct CustomBuildRecord *record,u32 index,enum Species gift,u32 level);
void CustomBuildFloors(const struct CustomBuildRecord *record,u16 floors[6]);
void CustomBuildAbilities(const struct CustomBuildRecord *record,u16 choices[3],bool32 retain);
bool32 CustomBuildGrowth(struct Pokemon *mon,struct CustomBuildRecord *record);
bool32 CustomBuildCommit(struct Pokemon *mon,const struct CustomBuildRecord *record,u32 choice,u32 giftSlot,enum Item giftItem);
void CustomBuildOpenCrate(void);
void CustomBuildOpenGift(void);
void CustomBuildOpenReport(void);
bool32 CustomBuildOpenGrowth(struct Pokemon *mon,void (*callback)(void));
bool32 CustomBuildResumePending(void);
bool32 ChaosRemovedItem(enum Item item);
bool32 ChaosUsableMove(enum Move move);
bool32 ChaosUsableAbility(enum Ability ability);
#endif
