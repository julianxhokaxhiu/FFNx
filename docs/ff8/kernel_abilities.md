# Extended kernel abilities

FFNx can load more than the original 116 abilities from `direct/kernel.bin`. A vanilla kernel keeps the original ability code and tables. No configuration switch enables the extension: FFNx detects the section counts when it reads the file.

## Limits and IDs

Kernel data sections 12 through 18 contain eight-byte ability records. Ability IDs run across all seven sections in order:

| Section | Group | Original first ID | Original count |
| --- | --- | ---: | ---: |
| 12 | Junction | 0 | 20 |
| 13 | Command | 20 | 19 |
| 14 | Stat percentage | 39 | 19 |
| 15 | Character | 58 | 20 |
| 16 | Party | 78 | 5 |
| 17 | GF | 83 | 9 |
| 18 | Menu | 92 | 24 |

Keep the original entries and add new ones to the appropriate section. The combined count must not exceed **128**: each GF save record has a 128-bit learned-ability mask. The menu group has an additional **32-entry** limit because its learned capabilities use one 32-bit value. The GF group must begin at ID 64 or above. Adding records changes the IDs of every later record; adding in the middle of a group also changes the IDs of its later entries.

Update all ability references to match, especially the 21 learning slots of each GF in section 3. A GF can display at most 22 learned/learnable abilities at once. Adding records does not add learning slots. Names and descriptions belong in the text section linked to their data section; rebuild the file header and text offsets when saving. An editor such as SolomonRing can maintain these references and offsets.

## Example: HP+90% on Quezacotl

Insert a stat-percentage record after HP+80%, set its percentage to 90, give it a name and description, and assign its new ID to a Quezacotl learning slot. With an otherwise vanilla layout it becomes ID 42; character abilities now begin at 59, GF abilities at 84, and menu abilities at 93. AP and level/prerequisite requirements come from the kernel. This grants a character percentage ability learned by Quezacotl.

To increase the GF's own HP instead, add a record to section 17. Byte 5 enables Boost; byte 6 selects summon damage (0) or GF HP (1); byte 7 is the bonus percentage. For GFHP+90%, use 0/1/90 for those three bytes. This GF effect applies after learning and needs no character ability slot.

## Saves and testing

Use a new game or a disposable test save when changing IDs. Existing saves store learned bits, equipped abilities and the current learning ID; FFNx does not migrate them to a different ability layout. Keep the kernel layout stable for saves you intend to retain.

The extension keeps full ability records in FFNx memory and hands the game its original data-section layout. It also moves the junction and menu candidate buffers. Patch locations are resolved from existing game anchors, with separate Latin and Japanese layout details, and validated before any ability patch is installed. Diagnostics for invalid sections or unexpected game code require `trace_all`.

Native regression tests are documented in [tests/ff8](../../tests/ff8/README.md). Tests exercise retail functions in a private process, including learn lists, names, GF effects and both menu-list builders. IDA-derived checks cover EN, DE, ES, FR, IT, JP and JP_NV; they do not replace a full interactive playthrough of each language.

Install a development runtime as `AF3DN.P` together with its matching shaders and `FFNx_steam_api.dll`. Preserve `FFNx.toml` and back up the previous runtime and kernel.
