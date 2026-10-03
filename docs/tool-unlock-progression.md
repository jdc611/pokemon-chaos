# Chaos tool unlock progression

- Time Changer: one-time Pallet departure gift after obtaining a starter. Unlocks TIME in Quick Tools.
- Pokevial and Portable PC: unlock together after the first completed Pokemon Center nurse heal. The nurse gives one added explanation; later heals do not repeat it. Declining healing does not unlock the tools. Their existing challenge restrictions still apply.
- Ability, nature, and gender changers: hidden until a later PC upgrade event sets `VAR_CHAOS_CHANGERS_UNLOCKED`. Cerulean is a proposed location, not finalized or implemented. Hide their Quick Tools page until unlocked.
- Configurable quick start still requires the first Center heal for recovery tools.

Unlock state uses persistent event variables so it survives saving and loading. Center unlock applies to the shared native FRLG nurse script, including its whiteout healing path.
