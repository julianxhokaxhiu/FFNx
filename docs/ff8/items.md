# Extended items and inventory pages

A kernel with extended item sections enables **255 inventory slots across 24 pages**, instead
of the original 198 slots / 18 pages. The first 23 pages hold 11 items each; the final page
has two usable rows. Vanilla kernels do not enable the extension or change the number of pages.

| Area | Original game | Extended |
| --- | --- | --- |
| Item IDs | 1-198 | 1-255 |
| Player inventory | 198 slots | 255 slots |
| Player item menu | 18 pages of 11 | 24 pages; the last one has two usable rows |
| Battle item types | 32 | 88: original 32 plus 56 added types |
| Shop sell menu | 25 pages of eight | 32 pages of eight |

It works on the 2013 releases (English, French, German, Spanish, Italian, Japanese) and the
1.2 releases. Before changing anything it checks every native operand and call site it is about
to change; if one differs (another mod patching the same code), the log names it and items
199-255 are ignored, while items 0-198 work as usual.

## File format

Section numbers here are **zero-based**; the kernel wiki/editor displays them
one-based. Preserve all original IDs. Item zero remains the empty item.

| Global item ID | Name/description record | Battle effect record |
| --- | --- | --- |
| 0-32 | Section 7, row = ID | Section 7, row = ID |
| 33-198 | Section 8, row = ID - 33 | No battle effect |
| 199-254 | Section 8, row = ID - 33 | Optional appended section 7 row = 33 + ID - 199 |
| 255 | Section 8, row 222 | Field only; 255 remains the native battle random-item failure sentinel |

Section 7 retains its original 33 records, then optionally appends up to 56
24-byte battle records. The appended rows map to IDs 199-254, rather than moving
the original Tent-and-later IDs. Section 8 retains its original 166 records, then
appends up to 57 four-byte name/description records. Define corresponding section
8 rows for every added battle item. Names and descriptions for all IDs 33-255
come from section 8 / text section 39; original battle names use text section 38.

An appended battle record whose 20 data bytes (everything after its two text
offsets) are zero is a placeholder: that item has no battle use and stays out of
the battle Item list. Because appended records map to IDs by position, this lets a
mod give a battle record to, for example, ID 205 while IDs 199-204 remain field-only
items: records for 199-204 are placeholders. The placeholder's text offsets are
still validated but otherwise ignored.

The extension validates record counts and checks every item text offset and
terminator against its actual text section. `0xFFFF` means no string. Kernel
loading still builds the existing native compatibility image, while item hooks
use the complete extended records.

Both `menu/mitem.bin` and `menu/price.bin` remain flat four-byte records indexed by
**global item ID**. Extend them through the highest defined ID (1024 bytes for all
256 rows including zero). `mitem` selects existing field-use behavior and its
parameters; adding a definition does not create a new effect type. Battle effects
come from section 7. Price records retain the normal discount/sell formulas.
Specialized systems such as Irvine's eight ammunition types and the original
battle-item order editor retain their existing type counts.

All three native `mitem` copies and `price.bin` allocations reserve 256 rows, and
menu reloads are copied through a bounded temporary allocation. Missing `mitem`
rows default to refine-only / no field use; missing prices default to zero before
the native minimum-price rule. Oversized or fractional files are replaced with
inactive defaults and logged. Invalid field-effect types are disabled per row.

## Inventory, menus and battle

The expanded inventory backs item acquisition/removal, field scripts, inventory
lookups, the item menu, manual rearrangement, sorting, refinement and shop selling.
The sell menu has 32 pages of eight slots. Shop staged quantities and buy/sell
price arrays cover all 256 IDs. Magazine achievement checks scan all inventory
slots, including magazines manually moved to a later page.

Slot indices stay within 0-254 because native consumers reserve byte value 255
as a failure sentinel. Item selection and both rearrangement cursors wrap within
the final two-row page. Nine zeroed padding entries make native full-page drawing
safe without exposing extra selectable or saved slots.

Sort intercepts the native Sort state before its 200-byte local buffer is used.
It preserves IDs through 255 and clamps duplicate quantities to the normal 100
limit. Saving also normalizes empty/duplicate slots without reordering the player’s
inventory.

The live battle item list has 96 entries, sufficient for the original 32 battle
items plus 56 added battle IDs. The original saved order still controls the first
32 types. The native order helper reads IDs as unsigned bytes so high IDs cannot
index before that order table. Added types follow the original types in ID order;
they do not index the 32-byte saved
ordering array. The shared battle menu state and reservation array are relocated
as well, and native item quantity/consumption/writeback consumers use the expanded
list. ID 255 can be carried and used in the field but has no added battle record.

## Save compatibility

The native save remains an **8192-byte image** with its original 5028-byte savemap
and CRC. The first 198 slots are projected into the vanilla inventory before the
native save builder computes that CRC. A versioned 522-byte inventory block is
written at image offset `0x1600`, in the formerly unused tail. It contains all 255
slots and its own checksum. This is inside the normal LZSS-compressed save file;
there is no sidecar file or borrowed field-variable storage.

The accepted-load path reads the complete tail and restores the extension only
after native save validation. Preview reads do not change the running inventory.
The extension must have a valid version, slot count, checksum, quantities, unique
IDs and matching first-198-slot projection. An older save, damaged block or stale
block after an external inventory edit imports the native inventory and clears
the additional slots. New-game initialization also clears stale extra slots.

A vanilla executable or save editor can read the native projection, but it cannot
preserve/use every extended item or inventory slot. Saving through software that
clears the tail discards the extension. Keep backups when moving saves between
extended and vanilla environments.

## Native code

The extension moves the native arrays it grows (inventory, battle item records, the live battle
item list, the battle list window's per-row marks, shop prices and quantities) into FFNx and
redirects the instructions that use them. `src/ff8/kernel_item_layouts.h` lists those
instructions: each native function is reached from an FFNx external through calls or absolute
references, and each operand is an offset in that function, per release layout (JP_NV shares
JP's, the 1.2 releases their 2013 counterpart's). Functions whose whole logic changes are
replaced in C++: item names and descriptions, building and refreshing the battle item list,
the random battle item pick and the battle-order reservation (the native one reads item IDs as
signed bytes).

`tests/ff8_items/generate_layouts.py` rebuilds the table from the executables and checks it:
every reference to a moved array in each release must be a listed operand or sit in a function
left alone on purpose (debug menus, the unused `ShopMenu_Enter`, functions replaced in C++).

## Validation and fixtures

`tests/ff8_items/inventory_tests.cpp` covers all 255 item IDs, capacity and quantity
limits, sorting, normalization, save round trips, legacy files and corrupt blocks.

The isolated x86 native harness executes the production item module and private copies of
retail machine code, once per release executable. For each one it resolves every native
function from that release's FFNx externals, checks every operand, rejects a modified operand
or call site, and runs native acquisition/removal/lookup, shop staging and writeback, the
battle item list, battle quantity/consumption/writeback, the battle list window and the
battle-order reservation (against the native function on 64 inventories). On the 2013 English
release it also runs native page wrapping and partial-page cursor bounds, manual rearrangement,
the intercepted Sort state, the native save builder and both vanilla CRC copies, accepted-load
migration, and bounded menu metadata loads.

`make_fixture.py` builds an English test kernel and extended menu files from the
user's extracted vanilla assets, without changing the inputs or deploying to a
game. It defines IDs 199-255 as Extra Potion and adds Potion-derived battle
records for IDs 199-254. Fixtures are generated locally and are not committed.

See [test instructions](../../tests/ff8_items/README.md). Native harness checks
are not an interactive in-game graphics/input validation; test the resulting DLL
with a copied save and modded assets before using this branch for a playthrough.
