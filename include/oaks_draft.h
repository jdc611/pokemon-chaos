#ifndef GUARD_OAKS_DRAFT_H
#define GUARD_OAKS_DRAFT_H
#include "pokemon.h"
struct Trainer;
#define OAKS_DRAFT_MAGIC 0x4F443031u
bool32 IsOaksDraft(void);
void OaksDraftInitialize(void);
bool32 OaksDraftSpeciesEligible(enum Species species);
void OaksDraftGenerateSpecies(u16 species[18]);
void OaksDraftCreateCandidate(struct Pokemon *mon, u32 index, u8 level);
bool32 OaksDraftCommit(u32 choice);
bool32 OaksDraftCreateTrainerParty(struct Pokemon *party, const struct Trainer *trainer);
const struct Trainer *OaksDraftTrainerOverride(const struct Trainer *trainer);
void OaksDraftOpenCrate(void);
void OaksDraftOpenReport(void);
void OaksDraftOpenGift(void);
void OaksDraftOpenAbilityReroll(void);
void OaksDraftGiveGymTM(void);
void OaksDraftRecordTM(enum Item item);
bool32 OaksDraftTMReserved(enum Item item);
bool32 OaksDraftOpeningBattle(void);
void OaksDraftFormatBackground(struct Pokemon *mon, u8 *dest);
void OaksDraftSetNickname(struct Pokemon *mon, u32 index);
bool32 OaksDraftGiftTrade(u32 slot, enum Species species, enum Item item);
bool32 OaksDraftRerollAbility(u32 slot);
void OaksDraftGetRound(void);
void OaksDraftSetChampionGraphic(void);
void OaksDraftFinishOpening(void);
enum Item OaksDraftExternalItem(enum Item item);
bool32 OaksDraftAwardingGymTM(void);
void OaksDraftPreviewGift(enum Species species, u8 *name, u8 *note);
#endif
