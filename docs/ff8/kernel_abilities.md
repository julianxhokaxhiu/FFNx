# Extended kernel abilities and battle commands

FFNx can load more than the original 116 abilities from `direct/kernel.bin`. FFNx also supports additional battle commands and command ability data. A vanilla kernel keeps the original ability code and tables. The shared battle-dispatch hook is installed during initialization and forwards commands unchanged while the extensions are inactive. No configuration switch enables the extension: FFNx detects the section counts when it reads the file.

The shared [kernel mapping](kernel.md) defines every data record and reads file offsets and counts from the header. The executable still receives its fixed compatibility layout.

## Limits and IDs

Kernel data sections 12 through 18 contain eight-byte ability records. Ability IDs run across all seven sections in order:

| Section | Group           | Original first ID | Original count | Minimum count | Absolute maximum* |
| ------- | --------------- | ----------------: | -------------: | ------------: | ----------------: |
| 12      | Junction        |                 0 |             20 |             1 |               127 |
| 13      | Command         |                20 |             19 |             0 |               126 |
| 14      | Stat percentage |                39 |             19 |             0 |               126 |
| 15      | Character       |                58 |             20 |             0 |               126 |
| 16      | Party           |                78 |              5 |             0 |               126 |
| 17      | GF              |                83 |              9 |             0 |               126 |
| 18      | Menu            |                92 |             24 |             0 |                32 |

\* Each maximum assumes all other ability sections are reduced to their minimum counts. Only junction ability 0 (None) must remain; other groups may be empty. Junction's maximum includes None. These maxima cannot be used simultaneously. The original 116 abilities leave 12 shared slots; removing original abilities frees more.

The first six sections together can contain at most 127 entries because the menu group must start below ID 128, even when empty. This makes the maximum 126 for any one non-junction, non-menu section after retaining None. A menu entry can occupy the remaining ID 127: for example, 1 junction entry + 126 GF entries + 1 menu entry = 128.

Ability sections may grow or shrink; keep entry 0 in the junction section because ID 0 means no ability. The combined count must not exceed **128**: each GF save record has a 128-bit learned-ability mask. The menu group has an additional **32-entry** limit because its learned capabilities use one 32-bit value. Group start IDs must stay below 128; when the menu group is empty, the preceding groups can therefore use at most 127 entries. Adding records changes the IDs of every later record; adding in the middle of a group also changes the IDs of its later entries.

The junction menu builds command candidates from section 13 and passive candidates from sections 14-16, using the current section boundaries during initialization and each menu refresh. Empty ranges produce no candidates.

Update all ability references to match, especially the 21 learning slots of each GF in section 3. A GF can display at most 22 learned/learnable abilities at once. Adding records does not add learning slots. Names and descriptions belong in the text section linked to their data section; rebuild the file header and text offsets when saving. An editor such as SolomonRing from [FF8UltimateEditor](https://github.com/HobbitDur/FF8UltimateEditor) can maintain these references and offsets.

## Battle commands

These tables have independent limits; they do not consume the shared ability budget:

| Section | Table                | Original count | Maximum count | Extra records |
| ------- | -------------------- | -------------: | ------------: | ------------: |
| 1       | Battle commands      |             39 |           176 |           137 |
| 11      | Command ability data |             12 |           255 |           243 |

Keep the original rows first and append new rows. Battle command IDs `0xB0` and above belong to engine actions. An eight-byte battle command names its effect-data row in byte 4 (`0xFF` means none). Each effect-data row is 16 bytes. An added command requires valid effect data; byte 7 selects its vanilla **Execution type**. Zero defaults to Mad Rush. Supported families are Defend, Mad Rush, Treatment, Recover, Revive, Doom, Kamikaze, Absorb, LV Down and LV Up. These reuse the game's existing handlers; effect data controls the attack type, power, statuses, animation and hit count.

For a new Treatment-style command in SolomonRing:

1. In **Command Ability Data**, select Treatment and add an entry; edit the copied effect data.
2. In **Battle Commands**, select Treatment and add an entry. Name it and select the new effect-data row; keep the same built-in **Execution type** (Seq anim 11, initial cam anim 0-5).
3. In **Command Abilities**, add an ability, set its AP and point it at the new battle command.
4. Assign that ability's unified ID to a GF learning slot in **G-Forces**, then save `direct/kernel.bin`.

The GF learns the section 13 ability; that ability grants the section 1 battle command; the command uses the section 11 effect data. Only the section 13 record consumes the shared ability budget.

## Removing entries

SolomonRing can remove original abilities in sections 12-18, clear GF references to the removed ability and shift later IDs. Cleared learning slots (ability 0, None) do not appear in the GF Learn list. Learned bits beyond the current kernel are also ignored. If the game reloads a changed kernel, the ability records and group bounds refresh with its GF slots and text. This frees shared IDs for other ability sections. Empty groups are supported; GF effects still work when their IDs move below 64. Use a fresh or migrated save after changing IDs.

Original battle commands use **Disable command / Reactivate command** in SolomonRing. They retain their slots, settings and command-ability references. Disabled commands remain visible with a locked editing panel; reactivation restores editing and player access. Disabling does not free capacity or shift later IDs. Command 0 remains reserved as the empty action.

Added battle commands use **Delete command**: the row and its text are removed, later added IDs shift down, and the editor updates kernel references. References to the deleted command are cleared. References in other files and saves must be updated separately.

The disabled marker remains `0xFE` in battle-command byte 7. Original command ability data rows retain reserved slots when removed; their marker remains `0xFE` in padding byte 2. The editor clears references to removed data rows, which retain their payload for engine-internal actions. Added effect-data rows are removed and compacted. Disabled command slots and reserved data slots do not provide extra capacity.

FFNx filters disabled commands (including default Attack/Mug and limit commands) after character command setup and turns stale player selections into the empty action. An original fixed-effect command whose data was removed is unavailable until it is given valid effect data. Active added commands still require valid effect data when saving. Reactivation does not reconstruct references cleared by older editor versions.
