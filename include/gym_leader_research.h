#ifndef GUARD_GYM_LEADER_RESEARCH_H
#define GUARD_GYM_LEADER_RESEARCH_H
struct MapHeader;
struct MapEvents;
struct ObjectEventTemplate;
void GymLeaderConfigureResearchMap(struct MapHeader *header);
u32 GymLeaderPrepareWingsBattle(void);
u32 GymLeaderPrepareTitanBattle(void);
void GymLeaderConfigureFieldResearch(struct MapEvents *,struct ObjectEventTemplate *,u32 capacity);
u32 GymLeaderPrepareFieldBattle(void);
#endif
