# EXE data

Sometimes texts and textures are stored inside the EXE, instead in the data directory.
In this case it is harder for modders to mod the game.

This feature allows modders to override data from the EXE via the
(Direct Mode)[direct_mode.md] feature.

Use the `save_exe_data` option to dump files to the direct/exe/ directory.
And then FFNx will look for those files directly instead of data from the EXE.

## Supported data

### FF8

- `battle_scans.msd`: Texts in battle scans. Note: this is not exactly the same
  format in the EXE, the msd format is used because it is a well documented format
  of FF8.
- `card_names.msd`: Card names. Note: this is not exactly the same
  format in the EXE, the msd format is used because it is a well documented format
  of FF8.
- `card_texts.msd`: Card module texts. Note: this is not exactly the same
  format in the EXE, the msd format is used because it is a well documented format
  of FF8.
- `draw_point.msd`: Draw point and Disc error messages

### FF8 battle actor sounds

`battle_actor_sounds.bin` contains the sound IDs for characters and monsters through c0m199. Enable `save_exe_data` to create it in `direct/exe/`. Existing files are kept. As with other EXE data, `direct/<language>/exe/` takes precedence.

The file has no header and must contain exactly 6,048 bytes: 216 rows of seven little-endian unsigned 32-bit world sound IDs. The original 160 rows are preserved; rows 160-215 start at zero for mods to fill.

A monster's row is its c0m number plus 16. Slot numbers are 0-6, with byte offset `((c0m + 16) * 7 + slot) * 4`. To reuse G-Soldier sounds for c0m145, copy row 87 to row 161. G-Soldier slot 0 uses world sound ID 400000 (`80 1A 06 00`).

The table uses world sound IDs, not audio archive indices. DAT sound sections are not imported automatically. Missing or incorrectly sized files keep the defaults.
