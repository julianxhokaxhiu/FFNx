# Kernel mapping

`src/ff8/kernel.h` maps the header and all 31 data sections of `kernel.bin`.
`kernel_enums.h` defines the stored flags and selectors with explicit byte widths.
The layouts follow the [FF8 Modding Wiki](https://hobbitdur.github.io/FF8ModdingWiki/technical-reference/main/kernel/), SolomonRing and the verified 2013 EN IDA records.

## Reading the file

The header has a 32-bit section count followed by 56 file-relative section offsets.
`ff8_kernel_file` validates the complete header before exposing any section: offsets
must be ordered and inside the file, and data lengths must contain whole records.
Equal adjacent offsets represent an empty section. The last section ends at EOF.

Use the section type to read records and obtain their count from the header:

```cpp
ff8_kernel_file file(data, size);
if (!file.valid()) return;
auto commands = file.section<ff8_kernel_section::battle_commands>();
uint8_t data_id = commands[command_id].ability_data_id;
```

Check an index against the returned span's size before indexing it. Text sections
32-56 are byte streams, available through `bytes(section)`; the 16-bit text offsets
in data records are relative to their corresponding text section. `0xFFFF` means no
string. Section enums are zero-based; the wiki/editor numbers below are one-based.

## Data records

| Section | Data                       | Record type                            | Bytes |
| ------- | -------------------------- | -------------------------------------- | ----- |
| 1       | battle commands            | `ff8_kernel_battle_command`            | 8     |
| 2       | magic                      | `ff8_kernel_magic`                     | 60    |
| 3       | junctionable gfs           | `ff8_kernel_junctionable_gf`           | 132   |
| 4       | enemy attacks              | `ff8_kernel_enemy_attack`              | 20    |
| 5       | weapons                    | `ff8_kernel_weapon`                    | 12    |
| 6       | renzokuken finishers       | `ff8_kernel_renzokuken_finisher`       | 24    |
| 7       | characters                 | `ff8_kernel_character`                 | 36    |
| 8       | battle items               | `ff8_kernel_battle_item`               | 24    |
| 9       | non battle items           | `ff8_kernel_non_battle_item`           | 4     |
| 10      | non junctionable gfs       | `ff8_kernel_non_junctionable_gf`       | 20    |
| 11      | command ability data       | `ff8_kernel_command_ability_data`      | 16    |
| 12      | junction abilities         | `ff8_kernel_junction_ability`          | 8     |
| 13      | command abilities          | `ff8_kernel_command_ability`           | 8     |
| 14      | stat percentage abilities  | `ff8_kernel_stat_percentage_ability`   | 8     |
| 15      | character abilities        | `ff8_kernel_character_ability`         | 8     |
| 16      | party abilities            | `ff8_kernel_party_ability`             | 8     |
| 17      | gf abilities               | `ff8_kernel_gf_ability`                | 8     |
| 18      | menu abilities             | `ff8_kernel_menu_ability`              | 8     |
| 19      | temporary character limits | `ff8_kernel_temporary_character_limit` | 24    |
| 20      | blue magic                 | `ff8_kernel_blue_magic`                | 16    |
| 21      | blue magic parameters      | `ff8_kernel_blue_magic_parameter`      | 8     |
| 22      | shot                       | `ff8_kernel_shot`                      | 24    |
| 23      | duel                       | `ff8_kernel_duel`                      | 32    |
| 24      | duel parameters            | `ff8_kernel_duel_parameters`           | 100   |
| 25      | rinoa limits part1         | `ff8_kernel_rinoa_limit_part1`         | 8     |
| 26      | rinoa limits part2         | `ff8_kernel_rinoa_limit_part2`         | 20    |
| 27      | slot array                 | `ff8_kernel_slot_array`                | 60    |
| 28      | slot sets                  | `ff8_kernel_slot_set`                  | 16    |
| 29      | devour                     | `ff8_kernel_devour`                    | 12    |
| 30      | misc                       | `ff8_kernel_misc`                      | 60    |
| 31      | misc text offsets          | `ff8_kernel_misc_text_offset`          | 2     |

The Duel parameters record contains 25 four-byte move-graph entries. Slot arrays
contain 60 set IDs, and each Slot set contains eight magic-ID/quantity pairs. Misc
text offsets are individual 16-bit entries, not native pointers.

## Field conventions

- **Attack animation** is a 16-bit visual-effect ID, separate from a Magic-section ID.
- **Target hit animation** selects the target's reaction; it is separate from the actor's Seq anim and camera.
- **Status 1** uses a two-byte `ff8_kernel_status1` mask; **Status 2** uses a four-byte `ff8_kernel_status2` mask. Record order can differ. Non-junctionable GFs store four Status 2 bytes followed by the low Status 1 byte.
- Junction/Character ability flags occupy three bytes and retain three byte-sized enum fields.
- Entry IDs remain numeric so the records can reference reordered or extended tables; flag enums describe fixed bit meanings.
- GF learning-slot byte 0 is the summon status accuracy only in slot 0. Byte 1 is the minimum level (0-100) or prerequisite slot + 101. Byte 2 is an excluded slot, checked only with a prerequisite; `0xFF` means none. Byte 3 is the unified ability ID.
- Misc `+0x30/32/34/36` contains Duel durations; `+0x31/33/35/37` contains initial move-graph indices. Crisis levels are 1-4 while a Limit Break is available. Duration bytes represent raw / 15 seconds at normal game speed.

## Native compatibility image

`ff8_kernel_vanilla_data` describes the executable's fixed data layout. Its offsets
are derived with `offsetof`, and every record has a compile-time size check. The
header-based file view reads changed files; the loader copies supported sections
into the native compatibility image and keeps full extended tables in FFNx-owned
storage. Copies stop at each source section's boundary and zero-fill unused native
space. Text header entries point into the persistent file buffer.

Mapping all sections does not extend every native consumer. The currently supported
count changes include Magic, Battle commands, Command ability data, the seven
learned-ability groups and the item sections. Their limits and references are described in
[Extended kernel abilities and battle commands](kernel_abilities.md) and
[Extended items and inventory pages](items.md). Other tables
retain the counts expected by the game; a changed unsupported table is reported.
The item extension imports legacy inventories and persists its extra slots in the save-image tail.
