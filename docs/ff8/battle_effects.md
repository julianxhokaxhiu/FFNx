# Native battle effects

FFNx can run its own C++ version of FF8's battle effect code instead of the game's. It covers the GF summons, the spells, the limit breaks, the items and the enemy attacks (330 effects). The goal is the same picture as the game, frame for frame, from code that can be read, fixed and extended.

It is off by default.

## Options

In `FFNx.toml`:

```toml
# Run the C++ version of the battle effect code
ff8_battle_fx_native = true

# For testing: also run the game's own code on every effect tick, keep the game's result on any difference and log it
ff8_battle_fx_verify = true

# For testing ff8_battle_fx_verify: write the compared memory of the first difference of each effect to ffnx_fx_dumps\
ff8_battle_fx_dump = true
```

It works on the 2013 releases: English, French, German, Italian, Spanish and Japanese. On the 2000 releases the options do nothing.

## How it works

Every battle effect is a set of tasks in queues that the game runs once per battle tick (15 per second) through its task queue executor. Each task function that has a C++ version jumps to it, so the game's own executor runs the C++ code; every other task runs the game's code as before. A task keeps the game's function address, so effects that are not ported, and mods that look at the queues, see no difference.

The effect code names the game functions and data it uses (`game::...`). At startup FFNx finds each of them in the running game, starting from the effect table (`ff8_externals.func_off_battle_effects_C81774`) and following the calls and operands of the game's own code, so every release gets its own addresses.

The code is in `src/ff8/battle/fx`:

- `fx_dispatch.cpp`: installation
- `fx_port.h` / `fx_port.cpp`: the shared task types, the engine functions the effects call and the list of ported effects
- `fx_address.h` / `fx_address.cpp`: the game functions and data the effect code uses, and how each one is found
- `fx_modules.cpp`: per effect, the memory it owns (used by the verifier)
- `fx_verify.cpp`: the verifier
- `fx_primplayer.cpp`, `fx_glint.cpp`, `fx_sprite.cpp`, `fx_cast.cpp`: code shared by many effects
- `gfc_engine.cpp`: the engine the cinematic GFs share (Ifrit and the GFs built like it)
- `act_engine.cpp`: the engine of the actor-based effects (limit breaks, enemy attacks)
- `magNNN_*.cpp`: one file per effect, named after the effect id (the magic id of kernel.bin `+0x04`)

## Verifier

With `ff8_battle_fx_verify`, each tick of a ported effect runs twice:

1. the game's own code, with its calls outside the effect (sound, file loading, battle results) recorded;
2. the C++ code from the same starting state, with those calls replayed.

Then FFNx compares the memory the effect owns, its draw packets and the recorded calls. On a difference it keeps the game's result and logs `FF8 battle fx: <effect> tick <n> MISMATCH: ...`. At the end of each effect it logs a summary line (`... ended: ticks=... matched=... mismatched=...`).

A difference that comes back every run of an effect is a bug in its C++ version. Please report it with the log lines (and the dump files if `ff8_battle_fx_dump` was on).
