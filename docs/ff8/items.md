# Extended items and inventory pages

AddMoreItems supports the verified 2013 English executable. A kernel with extended
item sections enables **255 inventory slots across 24 pages**, instead
of the original 198 slots / 18 pages. The first 23 pages hold 11 items each;
the final page has two usable rows. Vanilla
kernels do not enable the extension or change the number of pages.

The first implementation checks all native operands and hook signatures before
installing any item patches. Extended item kernels on another executable, or on
an incompatible patched executable, are rejected with an AddMoreItems diagnostic.
The existing Magic/Ability/Command features retain their own version support.

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

## Validation and fixtures

`tests/ff8_items/inventory_tests.cpp` covers all 255 item IDs, capacity and quantity
limits, sorting, normalization, save round trips, legacy files and corrupt blocks.

The isolated x86 native harness executes the production item module and private
copies of retail machine code. It verifies the complete guarded patch set,
unsupported/mismatched executable rejection, native later-slot lookup/removal,
high-ID battle-order exclusion, shop staging/purchase/writeback,
battle quantity/consumption/writeback, native page wrapping and partial-page
cursor bounds, manual rearrangement, Sort interception,
the native save builder and both vanilla CRC copies, accepted-load migration,
and safe extended/missing/invalid/oversized menu metadata.

`make_fixture.py` builds an English test kernel and extended menu files from the
user's extracted vanilla assets, without changing the inputs or deploying to a
game. It defines IDs 199-255 as Extra Potion and adds Potion-derived battle
records for IDs 199-254. Fixtures are generated locally and are not committed.

See [test instructions](../../tests/ff8_items/README.md). Native harness checks
are not an interactive in-game graphics/input validation; test the resulting DLL
with a copied save and modded assets before using this branch for a playthrough.
