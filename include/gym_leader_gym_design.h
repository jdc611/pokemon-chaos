#ifndef GUARD_GYM_LEADER_GYM_DESIGN_H
#define GUARD_GYM_LEADER_GYM_DESIGN_H
#include <stdint.h>
struct GleGymDesign {uint8_t layout,puzzle,theme,tier;};
struct GleState;struct MapHeader;
uint32_t GleGymDesignPrice(const struct GleState *,uint8_t kind,uint8_t choice);
int GleGymDesignPurchase(struct GleState *,uint8_t kind,uint8_t choice,uint32_t *money);
void GymLeaderConfigureGymDesign(struct MapHeader *);
void GymLeaderGymPalette(uint16_t start,uint16_t count,uint8_t skipFaded);
#endif
