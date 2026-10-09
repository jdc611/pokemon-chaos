#ifndef GUARD_CONSTANTS_TYPE_HINTS_H
#define GUARD_CONSTANTS_TYPE_HINTS_H

// Preserve saved numeric values: old Always becomes Revealed; old Caught
// is read as Seen. Off stays 3, so existing saves require no migration.
#define TYPE_HINTS_REVEALED 0
#define TYPE_HINTS_ALWAYS TYPE_HINTS_REVEALED
#define TYPE_HINTS_SEEN 1
#define TYPE_HINTS_CAUGHT 2 // Legacy saved value; no longer offered.
#define TYPE_HINTS_OFF 3
#define TYPE_HINTS_COUNT 3

static inline u8 NormalizeTypeHintsMode(u16 mode)
{
    if (mode == TYPE_HINTS_REVEALED || mode == TYPE_HINTS_OFF)
        return mode;
    return TYPE_HINTS_SEEN;
}

static inline u8 CycleTypeHintsMode(u16 mode, bool32 reverse)
{
    mode = NormalizeTypeHintsMode(mode);
    if (reverse)
        return mode == TYPE_HINTS_OFF ? TYPE_HINTS_REVEALED
             : mode == TYPE_HINTS_REVEALED ? TYPE_HINTS_SEEN : TYPE_HINTS_OFF;
    return mode == TYPE_HINTS_OFF ? TYPE_HINTS_SEEN
         : mode == TYPE_HINTS_SEEN ? TYPE_HINTS_REVEALED : TYPE_HINTS_OFF;
}

#endif // GUARD_CONSTANTS_TYPE_HINTS_H
