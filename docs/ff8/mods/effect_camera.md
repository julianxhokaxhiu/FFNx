# Battle effect cameras

Most battle effects (magic, items, enemy attacks, limit breaks...) start a camera track stored in the exe when they begin.
FFNx can load that track from a file instead, one file per effect. This lets a mod:

- change the camera of an effect, with no size limit;
- give a camera to an effect that has none.

Mods only ship the files of the effects they change, so several mods can be combined, and every effect without a file stays vanilla.

## File path

```
<direct_mode_path>/exe/effect_NNN.bcam
```

Like the other [EXE data](../../mods/exe_data.md) files, `<direct_mode_path>/<language>/exe/effect_NNN.bcam` takes precedence when it exists.

- `<direct_mode_path>` is the `direct_mode_path` option of `FFNx.toml` (`direct` by default), inside the game folder.
- `NNN` is the effect id, zero padded to 3 digits: the effect id that `kernel.bin` gives to the magic, item or ability (offset `0x04` of a magic entry), the same id that FF8UltimateEditor shows. `1` is Cure, `35` is Life, `5` is the Phoenix Down item effect.

For example, `direct/exe/effect_005.bcam` is the camera of the Phoenix Down effect.

Camera tracks are the same in every language, so one file in `direct/exe/` works with every exe language.

Files are read the first time the effect is used in a game session. Restart the game after changing one.

## File format

The file is a camera track exactly as the exe stores it. All values are little-endian.

| Offset | Size | Value |
|---|---|---|
| `0x00` | u16 | pointer count, always `2` |
| `0x02` | u16 | camera script offset, always `8` |
| `0x04` | u16 | keyframe collection offset |
| `0x06` | u16 | track size, must equal the file size |
| `0x08` | ... | camera script byte-code, then an optional zero padding byte |
| keyframe offset | ... | keyframe collection |

The keyframe collection is:

| Offset (from the collection start) | Size | Value |
|---|---|---|
| `0x00` | u16 | keyframe set count `N` (at least 1) |
| `0x02` | u16 × N | offset of each keyframe set |
| `0x02 + 2 × N` | u16 | collection size, must equal the file size minus the keyframe collection offset |
| ... | ... | keyframe sets |

Every offset is relative (to the track start, or to the collection start) and the script jumps are relative too, so a track can be copied out of the exe as it is.
A file that does not follow these rules is ignored, and the effect keeps its vanilla camera. Set `trace_battle_animation = true` in `FFNx.toml` to log which files are used, and why a file is ignored.

## How it works

When a magic cast starts its effect, FFNx:

1. runs the effect start code of the game;
2. if the effect starts a camera track during its start, plays the file track instead (override);
3. if the effect started no camera at all, plays the file track right after its start (add). This is skipped when the game asked the effect not to move the camera.

After that, the game handles the camera like a built-in one: the action waits for the end of both the effect and the camera, then the view is restored.

Notes:

- No file means vanilla behaviour. FFNx never forces a camera on an effect without a file: many effects that have no camera track move the camera with their own code (Irvine's Shots, Duel, Zell's finishers, Angelo, several GF summons, Gilgamesh...). Adding a file to one of them makes both move the camera at the same time.
- A camera longer than its effect makes the action last longer, since the action waits for the camera to end.
- The camera script decides what plays from the battle state. Camera variable 0 holds the opening shot that the attacker played before the effect: 0-5 for a character, 8 for a monster's Magic, -1 when there was none. Most exe effect tracks (175 of 221, every spell and item among them) end the script at once when variable 0 is below 0, so copied as they are, they play nothing when there was no opening shot. For those actions, use a script that does not stop there, or one of the tracks that handle it (limit breaks and Quistis' Blue Magic, for example White Wind).
- There is no opening shot:
  - for commands that never play one, like Item;
  - for some Magic casts (also Draw, Card, Doom, Absorb, LV Up/Down, Mad Rush, Treatment, Recover, Revive). Each cast rolls for it, with odds set by the **Camera Movement** option of the Config menu: with the slider fully to the right every cast plays the opening shot, fully to the left none does, and in between it is random. A few battles never play it.

  So with a copied exe track, a Magic camera shows on some casts only, exactly like the vanilla spell cameras. Push Camera Movement fully to the right to test a camera file.
- Only the cameras started by the effect start are replaced. Cameras that an effect starts later from its own code, GF summon cinematics (their tracks come from the summon files), the victory camera and the battle intro camera are not affected.
