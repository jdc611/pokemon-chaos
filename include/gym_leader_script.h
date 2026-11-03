#ifndef GUARD_GYM_LEADER_SCRIPT_H
#define GUARD_GYM_LEADER_SCRIPT_H
// Async specials emit an implicit waitstate after returning. Error paths must
// resume on the next frame, after that waitstate has actually been installed.
void GymLeaderResumeScriptNextFrame(void);
#endif
