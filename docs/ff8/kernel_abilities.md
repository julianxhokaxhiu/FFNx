# Extended kernel abilities and battle commands

FFNx can load more than the original 116 abilities from `direct/kernel.bin`. FFNx also supports additional battle commands and command ability data. A vanilla kernel keeps the original ability code and tables. No configuration switch enables the extension: FFNx detects the section counts when it reads the file.

## Limits and IDs

Kernel data sections 12 through 18 contain eight-byte ability records. Ability IDs run across all seven sections in order:

| Section | Group           | Original first ID | Original count |
| ------- | --------------- | ----------------: | -------------: |
| 12      | Junction        |                 0 |             20 |
| 13      | Command         |                20 |             19 |
| 14      | Stat percentage |                39 |             19 |
| 15      | Character       |                58 |             20 |
| 16      | Party           |                78 |              5 |
| 17      | GF              |                83 |              9 |
| 18      | Menu            |                92 |             24 |

Ability sections may grow or shrink; keep entry 0 in the junction section because ID 0 means no ability. The combined count must not exceed **128**: each GF save record has a 128-bit learned-ability mask. The menu group has an additional **32-entry** limit because its learned capabilities use one 32-bit value. Group start IDs must stay below 128; when the menu group is empty, the preceding groups can therefore use at most 127 entries. Adding records changes the IDs of every later record; adding in the middle of a group also changes the IDs of its later entries.

Update all ability references to match, especially the 21 learning slots of each GF in section 3. A GF can display at most 22 learned/learnable abilities at once. Adding records does not add learning slots. Names and descriptions belong in the text section linked to their data section; rebuild the file header and text offsets when saving. An editor such as SolomonRing from [FF8UltimateEditor](https://github.com/HobbitDur/FF8UltimateEditor) can maintain these references and offsets.

## Battle commands

These tables have independent limits; they do not consume the shared ability budget:

| Section | Table                | Original count | Maximum count | Extra records |
| ------- | -------------------- | -------------: | ------------: | ------------: |
| 1       | Battle commands      |             39 |           176 |           137 |
| 11      | Command ability data |             12 |           255 |           243 |

Keep the original rows first and append new rows. Battle command IDs `0xB0` and above belong to engine actions. An eight-byte battle command names its effect-data row in byte 4 (`0xFF` means none). Each effect-data row is 16 bytes. An added command requires valid effect data; byte 7 selects the vanilla command it **behaves like**. Zero defaults to Mad Rush. Supported families are Defend, Mad Rush, Treatment, Recover, Revive, Doom, Kamikaze, Absorb, LV Down and LV Up. These reuse the game's existing handlers; effect data controls the attack type, power, statuses, animation and hit count.

For a new Treatment-style command in SolomonRing:

1. In **Command Ability Data**, select Treatment and add an entry; edit the copied effect data.
2. In **Battle Commands**, select Treatment and add an entry. Name it and select the new effect-data row; keep **Behaves like: Treatment**.
3. In **Command Abilities**, add an ability, set its AP and point it at the new battle command.
4. Assign that ability's unified ID to a GF learning slot in **G-Forces**, then save `direct/kernel.bin`.

The GF learns the section 13 ability; that ability grants the section 1 battle command; the command uses the section 11 effect data. Only the section 13 record consumes the shared ability budget.

## Removing entries

SolomonRing can remove original abilities in sections 12-18, clear GF references to the removed ability and shift later IDs. This frees shared IDs for other ability sections. Empty groups are supported; GF effects still work when their IDs move below 64. Use a fresh or migrated save after changing IDs.

Original battle commands and command ability data use fixed engine IDs. Removing one clears its editor references and hides it from lists and pickers, but retains a reserved slot. Removed effect-data rows retain their payload for engine-internal actions. Other rows keep their IDs. These reserved slots are shown in the entry counter and do not provide extra command/effect capacity. Added rows beyond the original tables are removed and compacted normally. Command 0 remains reserved as the empty action.

The removal markers are `0xFE` in battle-command byte 7 and command-ability-data padding byte 2. FFNx filters removed commands (including default Attack/Mug and limit commands) after character command setup and turns stale player selections into the empty action. An original fixed-effect command whose data was removed is unavailable until it is given valid effect data. Added commands still require valid effect data when saving.
