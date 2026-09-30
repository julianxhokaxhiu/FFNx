# Battle at 60 FPS with the original battle pace

Enabled with `ff8_fps_limiter = 4`. Outside battles this mode behaves like `ff8_fps_limiter = 1`. Also available on the remastered edition (it runs the same 1.2 executable).

## The problem

The PC battle module runs at 15 fps. To keep the battle UI at the PlayStation pace (60 UI ticks per second), every frame runs the battle UI tick 4 times in a row:

| Step | What happens |
|------|--------------|
| 1 | 3 catch-up UI ticks (menu rendering disabled) |
| 2 | the pad is read (the only read of the frame) |
| 3 | battle logic, effects, models, camera and draw (`BdLink`) |
| 4 | the visible UI tick |
| 5 | frame wait (frame limiter) |

The menus, cursors and gauges are drawn 15 times per second, and the pad is read 15 times per second. Simply running the battle at 60 fps (`ff8_fps_limiter = 3`) makes the whole battle run faster, because every frame advances the battle by one of its 15 fps steps.

## What FFNx does

The battle runs at 60 fps. One frame in 4 is a *real tick*: every battle subsystem advances exactly like in one original frame. The 3 frames in between are *held*: nothing advances and the last real tick is drawn again. The battle itself (ATB, animations, effects, camera, timers, damage) keeps its original pace of 15 steps per second, without interpolation.

The battle UI runs its tick once per frame instead of 4 times in a row (the 3 catch-up calls in the battle main loop are skipped), and the pad is read every frame: menus, cursors, gauges and inputs run 60 times per second, like on the PlayStation. The UI "fresh input" latch, armed once per battle frame, becomes armed every frame (the engine's UI ticks per frame constant becomes 1). The UI counters that count latches as time are brought back to their original pace (1 latch in 4): the HUD blink counter, GF Boost's safe and danger phases, and the Renzokuken trigger timeline. GF Boost, Renzokuken and Zell's Duel read the pad on every UI tick, as on the PlayStation.

What a held frame does:

| Part | Held frame |
|------|------------|
| Magic, GF, limit break and Draw effects, hit effects | not run: their draws from the last real tick (SSIGPU execution nodes) are copied at the end of the real tick and linked again into the held frame's ordering table. VRAM transfers are not repeated. |
| Screen feedback request (screen ghost, e.g. Eden) | armed again |
| Models | the animation frame and the choreography (AnimSeq VM) do not advance, the geometry is rebuilt from the current pose |
| Status visuals (colour pulse, Float, status sprites) | drawn from the state of the last real tick |
| Death / escape / appear fades | the colours of the last real tick are put back (fades also trigger events and roll the battle RNG) |
| Camera animation and camera script | not run; the camera shake of the last real tick is applied again |
| Damage numbers, screen fade | drawn from the state of the last real tick |
| Active character marker (rotating triangle) | drawn from the state of the last real tick |
| Description window transparency blink | its counter advances on real ticks only |
| Choreography tasks (wobble, texture blink, footsteps, camera shake, run-up, camera return) | not run |
| Drag to bone, detached / restored model parts | drawn without counting |
| Battle messages | shown without counting their display time |
| Stage | drawn with the engine's own freeze bits (sky rotation, texture animation, stage scripts) |
| Status timers, Gilgamesh / Angelo countdown, AI text and waits, game time, end of battle countdown | not counted |

The cursor fingers (hands) are recorded by each UI tick in the slots of its position in the frame (tick phase 0 to 3) and drawn once per frame, which then resets the phase: the original frame shows the fingers of its 4 UI ticks together (a hand on each target of a multiple target selection) and the finger blink toggles on its 4th UI tick. With one UI tick per frame, the phase keeps counting over 4 frames and every frame draws the fingers of the last 4 UI ticks.

## Why held frames do not flicker

A held frame is a complete frame: the stage, the models, the effects and the UI are all drawn again, only nothing advances. Skipping a subsystem on a held frame is not enough, because many battle functions draw and advance in the same call, and some per-frame state is reset after each draw. Every part is handled so that a held frame shows exactly the picture of the last real tick:

| Risk | What FFNx does |
|------|----------------|
| Effects (magic, GF, limit breaks, Draw) and hit effects draw from the same code that advances them, so they cannot be run on a held frame | at the end of the real tick, every SSIGPU execution node the effect queue created (arena cursor before / after the queue) is copied, and the ordering table bucket of each node is found by walking the bucket chains. Held frames link the copies again into their own ordering table, in the same bucket, so the depth order and the blending are those of the real tick. The whole ordering table is scanned (all `0x1122` buckets from the render list base), including the first layers drawn over the 3D scene, such as the black bars of Zell's Duel |
| VRAM transfers inside effect packets (framebuffer copies of mirror / warp effects) | not repeated: they would copy whatever the screen holds by then. The rest of the packets is redrawn |
| Models | the animation frame and the choreography do not advance, but the geometry is rebuilt from the current pose every frame, so no model disappears on a held frame |
| Status visuals reset the model palettes every frame, then the colour pulse is applied again | held frames apply again the palettes and colours of the last real tick |
| Fades (death, escape, appear) | the colours of the last real tick are put back after the call |
| Camera shake offsets are cleared after being applied | held frames apply again the shake of the last real tick |
| Screen feedback request (screen ghost) is cleared after each frame | armed again on held frames while the effect still wants it |
| Stage (sky rotation, texture animation, stage scripts) | drawn with the engine's own freeze bits set for the held frames |
| Damage number and screen fade tasks count their time in the same call that draws them | drawn from the state of the last real tick |
| Animations that advance once per draw (active character marker, description window blink) and the cursor fingers, drawn once per frame | brought back to their original pace, so they do not blink 4 times faster |
| A fault while copying or redrawing one effect | caught: that effect is no longer redrawn on held frames, the battle goes on |

## Known limitation

The full-screen flash task (used by a few effects) is only reachable from the effect opcode handlers, so it is not paced: it plays 4 times faster.

## Checking it works

With `trace_battle_animation = true` (or `trace_all`), FFNx.log says `battle pacing: battle at 60 fps, battle logic at 15 ticks per second` at startup.

## Addresses

All resolved in `ff8_data.cpp` from functions FFNx already knows (`get_relative_call` / `get_absolute_value` chains):

| Name | FF8_EN.exe 1.2 |
|------|----------------|
| Catch-up UI tick call sites (`battle_main_loop` + `0x13D` to `0x156`) | `0x47D09D` - `0x47D0B6` |
| Battle UI update | `0x4A8E30` |
| GF Boost update | `0x56DD70` |
| Renzokuken trigger update | `0x4BA6C0` |
| Game time tick | `0x4701B0` |
| Battle read animation / build bone matrices | `0x508F90` / `0x508C90` |
| AnimSeq update entity / advance animation by 1 | `0x504290` / `0x5094F0` |
| Camera animation / camera script / camera operations | `0x5035E0` / `0x509610` / `0x5033E0` |
| Screen feedback request | `0x47CF50` |
| Status timers / Gilgamesh and Angelo countdown | `0x483470` / `0x482F80` |
| Execute task queue | `0x508420` |
| Effect tick / hit effect queue / stage queue call sites (`BdLink` + `0x3A` / `0x23` / `0x7E`) | `0x50093A` / `0x500923` / `0x50097E` |
| Status visuals / entity fades call sites (battle entity task + `0xAD` / `0xC4`) | `0x502B5D` / `0x502B74` |
| AI text management call site (`sub_47CCB0` + `0xB4B`) | `0x47D7FB` |
| Stage 142 fade call site | `0x5129A7` |
| Tasks: damage number, text, screen fade, camera oscillation, stage 147 | `0x5069B0`, `0x506F70`, `0x501D10`, `0x509930`, `0x511EF0` |
| Tasks: wobble, texture blink, footstep, camera shake, move, drag, restore part, detach part | `0x501F90`, `0x5057D0`, `0x50F830`, `0x50F6C0`, `0x50F750`, `0x50F500`, `0x50F0E0`, `0x50F2E0` |
| Stage texture animation / stage render | `0x51B0D0` / `0x500FD0` |
| Cursor finger draw (`battle_main_loop` + `0x1EB`) / UI tick phase / finger slots | `0x4A78E0` / `0x1D74EA8` / `0x1D6D4B0` |
| Active character marker (`BdLink` + `0x60`) / animation state | `0x4BB090` / `0x1D76708` - `0x1D76713` |
| Description window draw (`battle_pause_sub_4CD140`) / blink counter (+ `0x17`, JP + `0x13`) | `0x4C8B70` / `0x1D77311` |
| Battle UI context / UI ticks per frame | `0x1D6D490` / `0xB8A3E4` |
| Screen feedback request value / end of battle countdown | `0x1CFF6F4` / `0x1D27B0C` |
| Camera setting / camera shake | `0x1D99A34` / `0x1D97710` |
| Battle update flags / tasks busy / stage freeze | `0x1D96A9C` / `0x1D96A88` / `0x1D9898C` |
| Detached part matrix / render list base | `0x1D99BF8` / `0x1D8E04C` |
| SSIGPU execution arena start / cursor | `0x1C48828` / `0x1CA8828` |

The same chains were verified to resolve to the same functions in the EN, FR, DE, ES, IT, JP and JP_NV executables. Tested in game on the english Steam release.
