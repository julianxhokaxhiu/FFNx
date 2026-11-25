# Native battle effects

FFNx runs its own C++ version of FF8's battle effect code instead of the game's. It covers the GF summons, the spells, the limit breaks, the items and the enemy attacks (330 effects). The goal is the same picture as the game, frame for frame, from code that can be read, fixed and extended.

It is always on. It works on the 2013 releases: English, French, German, Italian, Spanish and Japanese. On the 2000 releases the game runs its own effect code.

## How it works

Every battle effect is a set of tasks in queues that the game runs once per battle tick (15 per second) through its task queue executor. Each task function that has a C++ version jumps to it, so the game's own executor runs the C++ code; every other task runs the game's code as before. A task keeps the game's function address, so effects that are not ported, and mods that look at the queues, see no difference.

The effect code names the game functions and data it uses (`game::...`). At startup FFNx finds each of them in the running game, starting from the effect table (`ff8_externals.func_off_battle_effects_C81774`) and following the calls and operands of the game's own code, so every release gets its own addresses.

The code is in `src/ff8/battle/fx`:

- `fx_dispatch.cpp`: installation
- `fx_port.h` / `fx_port.cpp`: the shared task types, the engine functions the effects call and the list of ported effects
- `fx_address.h` / `fx_address.cpp`: the game functions and data the effect code uses, and how each one is found
- `fx_primplayer.cpp`, `fx_glint.cpp`, `fx_sprite.cpp`, `fx_cast.cpp`: code shared by many effects
- `gfc_engine.cpp`: the engine the cinematic GFs share (Ifrit and the GFs built like it)
- `act_engine.cpp`: the engine of the actor-based effects (limit breaks, enemy attacks)
- `magNNN_*.cpp`: one file per effect, named after the effect id (the magic id of kernel.bin `+0x04`)
