# FF8 item expansion summary

Implemented on branch `AddMoreItems`. Runtime implementation commit: `07c50093`.

| Area | Original game | This branch |
| --- | --- | --- |
| Usable item definitions | IDs 1-198 | IDs 1-255: 57 additional IDs |
| Player inventory | 198 slots | 255 slots |
| Player item menu | 18 pages of 11 | 24 pages; final page has two usable rows |
| Battle item types | 32 | 88: original 32 plus 56 added types |
| Shop sell menu | 25 pages of eight | 32 pages of eight |

Original item IDs remain unchanged. New IDs 199-254 can have field and battle
behavior; ID 255 is field-only because the native random battle-item helper uses
255 as its failure result. Existing field-effect types and stack limit 100 remain.
Irvine's eight ammunition types and the original battle-order editor retain their
existing type counts.

The implementation extends kernel item records, name/description lookup, menu
metadata and prices. It expands acquisition/removal, inventory lookups, player
paging, both manual-rearrangement cursors, sorting, refinement inventory access,
shop staging/purchase/selling, battle quantities/consumption/writeback, and
magazine achievement inventory scans. Native signed-ID bookkeeping is corrected
so high IDs cannot index before the original battle-order table.

The raw save image remains 8192 bytes. Its original 5028-byte savemap and CRC
layout are preserved. First-198-slot inventory projection is refreshed before CRC
generation; all 255 slots are stored in a versioned, independently checksummed
522-byte block at raw offset 0x1600. Older or invalid extensions import the native
inventory and clear extra slots. A stale extension cannot overwrite native
inventory edits made by another tool.

This first implementation requires the verified 2013 English executable. Before
activation it validates 843 native operands and all hook signatures. Vanilla
kernels do not activate item expansion. Other executable variants reject extended
item kernels instead of applying unverified addresses.

The pure inventory tests and isolated x86 native integration suite pass. The
native suite executes selected retail machine code in its own process, including
page wrapping, all final-page cursor paths, swapping, native acquisition/removal,
shop transactions, battle state/consumption/writeback, native save building and
both CRC copies, legacy/corrupt/stale save migration, and bounded metadata loads.
The full DLL and 172 matching shader binaries build successfully. The linker
emits the upstream build's generic LNK4088 warning for its /FORCE:MULTIPLE policy.
Interactive in-game graphics/input validation remains to be done.

The active IDA database received 40 findings comments and eight descriptive names
for previously anonymous helpers. Native executable bytes and native savemap
types were preserved. The SHA-256-guarded replay script is
[ida_annotations.py](../../tests/ff8_items/ida_annotations.py); it preserves the
findings independently of the live IDB and can reapply the disassembly comments
and names. Save the open IDB through IDA to persist its live changes.

Generated test fixtures define IDs 199-255 as Extra Potion, with field metadata
derived from Potion; IDs 199-254 also have Potion-derived battle records. They do
not grant starting inventory or add shop stock. The local test bundle is separate
from installed games and includes the DLL, matching shaders, fixtures and test log.

See [the item format and implementation details](items.md) and
[the reproducible test instructions](../../tests/ff8_items/README.md).
