# Extended kernel abilities

FFNx can load more than the original 116 abilities from `direct/kernel.bin`. A vanilla kernel keeps the original ability code and tables. No configuration switch enables the extension: FFNx detects the section counts when it reads the file.

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

Keep the original entries and add new ones to the appropriate section. The combined count must not exceed **128**: each GF save record has a 128-bit learned-ability mask. The menu group has an additional **32-entry** limit because its learned capabilities use one 32-bit value. The GF group must begin at ID 64 or above. Adding records changes the IDs of every later record; adding in the middle of a group also changes the IDs of its later entries.

Update all ability references to match, especially the 21 learning slots of each GF in section 3. A GF can display at most 22 learned/learnable abilities at once. Adding records does not add learning slots. Names and descriptions belong in the text section linked to their data section; rebuild the file header and text offsets when saving. An editor such as SolomonRing from [FF8UltimateEditor](https://github.com/HobbitDur/FF8UltimateEditor) can maintain these references and offsets.
