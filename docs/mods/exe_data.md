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

`battle_actor_sounds.bin` overrides the complete actor sound table, including
new monsters through c0m199. Enable `save_exe_data` to generate a starting file
in `direct/exe/` (or your configured `direct_mode_path`). Existing files are
never overwritten. Language-specific `direct/<language>/exe/` overrides take
precedence, as for the other EXE data files.

The file is exactly **6,048 bytes**: 216 rows of seven little-endian unsigned
32-bit world sound IDs, with no header. Rows 0-15 are characters; a monster's
row is its c0m file number plus 16. The original 160 rows are preserved by
default; the new rows 160-215 start at zero. A missing or incorrectly sized
file leaves the default table intact. Zero is the original table's unused-slot
value; this feature does not change how the game handles that value.

To set slot `slot` for monster `c0m`, write at byte offset
`((c0m + 16) * 7 + slot) * 4`, where slots are 0-6. For example, copying row 87
(c0m071, G-Soldier) to row 161 (c0m145) reuses its sounds. G-Soldier slot 0
contains world sound ID 400000 (`80 1A 06 00`), not audio archive index 1230.
Animation actor-sound commands continue to address local slots; global sound
commands are unchanged. This adds table capacity and an override, not new
audio decoding or automatic sound inheritance from DAT files.

The extension validates the executable's sound-routine layout before patching;
unsupported layouts log a warning and keep the original table.
