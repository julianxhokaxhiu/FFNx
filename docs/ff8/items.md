# Extended items and inventory pages

A `kernel.bin` that appends rows to the item sections adds item IDs **199-255** and grows the
inventory to **255 slots on 24 pages**, instead of 198 slots on 18 pages. With a vanilla kernel,
nothing changes.

It works on the 2013 releases (English, French, German, Spanish, Italian, Japanese) and the 1.2
releases. If the game code FFNx needs to change has already been patched by another mod, the log
says so and items 199-255 are ignored. Items 1-198 keep working as usual.

## Adding items

Section numbers are the one-based ones from [Kernel mapping](kernel.md).

| Item ID | Name and description | Battle use |
| --- | --- | --- |
| 1-32 | section 8 (battle items), row = ID | section 8, row = ID |
| 33-198 | section 9 (non battle items), row = ID - 33 | none |
| 199-254 | section 9, row = ID - 33 | optional: section 8, row = ID - 166 |
| 255 | section 9, row 222 | none (255 is the game's "no item" value in battle) |

- **Section 9** keeps its 166 rows and can append up to 57 rows, one per new ID from 199.
- **Section 8** keeps its 33 rows and can append up to 56 battle records. They map to IDs by
  position, starting at 199. To give a battle use to ID 205 only, append records for 199-204
  with their 20 data bytes (everything after the two text offsets) set to zero: such a record
  means "no battle use".
- `menu/mitem.bin` and `menu/price.bin` stay one 4-byte row per item ID, and can grow up to
  256 rows. A missing row means "refine only" in `mitem.bin` and a price of 0 in `price.bin`.
  New items reuse the existing field effects: `mitem.bin` cannot create a new one.

FFNx checks the row counts and every text offset when the kernel loads. If something is wrong,
the log says what, and items 199-255 are ignored.

## In game

- The item menu, Sort, rearrange, shops (32 sell pages), refine and field scripts all use the
  255 slots. The last page has two rows.
- The battle Item list holds the 32 original battle items, in their saved order, followed by the
  added ones in ID order.

## Saves

Save files keep their original size and checksum. The first 198 slots stay where the game
expects them. All 255 slots are also written to the unused end of the save data (offset
`0x1600`), with their own checksum.

On load, an older save, or one whose first 198 slots were changed by another tool, loads its
198 slots and leaves the extra ones empty. A vanilla game or save editor only sees the first
198 slots, and may erase the extra ones when it saves.
