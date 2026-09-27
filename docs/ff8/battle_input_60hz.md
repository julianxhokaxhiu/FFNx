# Battle controller read at 60 Hz

Option: `ff8_battle_input_60hz` (requires `ff8_fps_limiter` >= 1). English release only for now.

## The problem

The PC battle module runs at 15 fps. To keep the battle UI at the PlayStation pace (60 UI ticks per second), every frame runs the battle UI tick 4 times in a row:

| Step | What happens |
|------|--------------|
| 1 | 3 catch-up UI ticks (menu rendering disabled) |
| 2 | the pad is read (`engine_eval_process_input`, the only read of the frame) |
| 3 | battle logic (ATB, actions, effects, models) |
| 4 | the visible UI tick |
| 5 | frame wait (frame limiter) |

Every UI tick advances the battle pad ring from the current pad mask (`engine_mapped_buttons`), and press edges are computed between consecutive ring entries. The 3 catch-up ticks run before the frame's pad read, so they all see the previous frame's reading: only the visible tick can see a change. In practice the controller is read 15 times per second instead of 60:

- a press and release within one frame (~66 ms) is lost, two presses in one frame count as one;
- GF Boost mashing tops out at about 7.5 presses per second (a press needs a release in between, so one press every 2 frames), against 30 per second on the PlayStation;
- Renzokuken trigger presses and Zell's Duel inputs are sampled 4 times less often than the game logic allows.

## What the option does

The pad is also read 3 times during the frame wait, at 1/4, 2/4 and 3/4 of the frame. On the next frame, each catch-up UI tick runs with one of those readings as the current pad mask (the 3 calls to the UI tick in the battle main loop are redirected). The 4 UI ticks of a frame now see 4 readings taken 16.7 ms apart, in time order, exactly like the PlayStation reading the pad at every vertical blank.

The extra reads use the game's own pad read (`engine_eval_keyboard_gamepad_input`), so key and button mapping are the game's. That function also updates the press edges and previous state of each pad, the raw gamepad button words and the auto-repeat: all of it is saved before and restored after each extra read, and auto-repeat is disabled during it. The frame's own read therefore still behaves exactly like in the original game.

Nothing else changes: frame rate, battle speed, ATB, GF Boost phase lengths, Renzokuken timing window, Zell's Duel timer, UI timers are all vanilla. The menu and cursor are still drawn once per 15 fps frame; only the input is more precise. The latency is the vanilla one (the catch-up ticks run at the start of the next frame, as they always did).

## Checking it works

With `trace_gamepad = true` (or `trace_all`), FFNx.log gets one line at the end of each battle:

```
battle pad sampling: 52 presses this battle: 18 / 11 / 12 from the extra reads, 11 from the game's own read; frame work 6.5 ms, extra reads at 16.7 / 33.3 / 50.0 ms
```

- presses counted on the extra reads could never register in the original game;
- the extra reads can only be taken while the frame limiter waits: if the frame's own work takes longer than a quarter of the frame, the first reads happen late and the spacing becomes uneven (the log shows it).

## Addresses

All resolved in `ff8_data.cpp` from functions FFNx already knows:

| Name | How | FF8_EN.exe 1.2 |
|------|-----|----------------|
| Catch-up UI tick call sites | `battle_main_loop` + `0x142`, `0x14C`, `0x156` | `0x47D0A2`, `0x47D0AC`, `0x47D0B6` |
| Battle UI tick | `sub_4A84E0` (existing) | `0x4A84E0` |
| Game pad read | `engine_eval_keyboard_gamepad_input` (existing) | `0x467D10` |
| Pad masks (2 pads, 28 DWORDs apart: edges, current, -, previous) | `engine_mapped_buttons` (existing) - 1 | `0x1CD01F8` |
| Auto-repeat interval | `engine_eval_keyboard_gamepad_input` + `0x49D` | `0x1CD02F0` |
| Raw gamepad button update | call at `engine_eval_keyboard_gamepad_input` + `0x4BE` | `0x468790` |
| Raw gamepad button words (edges[8], current[8], previous[8]) | raw button update + `0x11`, - 8 DWORDs | `0x1CD0394` |

The offsets are identical in the JP and JP_NV executables; the option is limited to the english release until other versions are tested in game.
