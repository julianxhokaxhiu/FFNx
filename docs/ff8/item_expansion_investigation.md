# AddMoreItems investigation

Status: design investigation; item expansion is not implemented by this branch yet.

Investigated on 2026-10-21. Branch `AddMoreItems` starts at `aa5fea54`
(`fork/codex/add-more-abilities`), which includes the typed kernel file reader,
Magic, learned-ability and battle-command extensions. The existing worktrees
were left in place, including their uncommitted changes.

## Feasibility and initial scope

Adding item definitions beyond vanilla ID 198 looks feasible within the existing
byte-sized ID format. The candidate range is 199-255: 57 additional IDs, with 0
remaining the empty item. This is a target, not a verified compatibility promise:
all consumers, especially possible `0xFF` sentinels, still need auditing.

Keep the existing 198 inventory slots for the first implementation. Slots store
`{ item_id: uint8_t, item_quantity: uint8_t }`; a slot can represent a new ID
without enlarging the save record. Having 255 item definitions does not allow
holding 255 different items simultaneously. Increasing inventory capacity is a
separate change to saves, menus and their consumers.

The first milestone should cover new field-menu items using existing effect
types, correct names/descriptions, sorting and save/reload. Shops and battle use
must be made safe before calling even that milestone ready for normal play.
New effect code, extra magazine pages, new refine recipes and new battle
animations are separate features.

## Evidence and limits of the investigation

Primary executable evidence comes from the connected IDA database for 2013 EN
`FF8_EN.exe`, image base `0x400000`, SHA-256
`064d466b5fe2ba901fd44abf19f37c0fd6a2db40aabd95c9e5959195b6589570`.
Addresses below identify functions in that build only; they are not portable
patch addresses. No IDA database edits or game deployment were made.

The local extracted game assets were read independently to verify these counts:

| Resource | Vanilla layout | Consequence |
| --- | --- | --- |
| Kernel section 7, battle items | 33 records x 24 bytes = 792 bytes | IDs 0-32, including the empty row |
| Kernel section 8, non-battle items | 166 records x 4 bytes = 664 bytes | Global IDs 33-198, indexed by `id - 33` |
| `menu/mitem.bin` | 796 bytes = 199 four-byte records | Field effect metadata is indexed by global item ID |
| `menu/price.bin` | 796 bytes = 199 four-byte records | New IDs need explicit price data and extended consumers |
| Saved inventory | 198 two-byte slots | Capacity is independent of the maximum ID |
| Saved battle order | 32 bytes | Cannot index it directly with a new global item ID |

Section numbers in this document are **zero-based**. Battle/non-battle item text
lives in sections 38/39. The [kernel mapping](kernel.md) and
[FF8 Modding Wiki](https://hobbitdur.github.io/FF8ModdingWiki/technical-reference/main/kernel/)
describe the record formats; the wiki's displayed section numbers are one-based.
Counts above were checked against the extracted files, not inferred from names.

## Verified consumers and required changes

| Function or source | Observed behavior | Required work |
| --- | --- | --- |
| `getTextBattleItem`, `0x47EA30` | IDs below 33 use battle-item name offsets; other IDs use the fixed `id - 33` split and non-battle text | Read full extended records with bounds checks; preserve existing global IDs |
| Description getter, `0x47EA90` | Same fixed split as the name getter | Extend alongside names; handle missing strings safely |
| `AddItemToInventory`, `0x47ED00` | Searches 198 slots; does not reject IDs above 198; writes the ID into one byte | Keep the capacity limit; audit all acquisition paths and reject IDs without definitions |
| `SCRIPT_ADDITEM`, `0x522380` | Pops script arguments and calls `AddItemToInventory` | Candidate acquisition path for test fixtures; runtime validation still required |
| `sub_4C3150` | Clears zero-quantity/zero-ID slots over 198 entries; no observed upper-ID filter | Do not mistake this slot-count loop for a definition limit |
| Item controller, `0x4F81F0`, Sort arm at `0x4FB430` | Sorts through a 200-byte ID-indexed stack array, then emits only IDs 1-198 | Replace the sort operation before exposing new IDs |
| `Mitem_GetType`, `0x4F74F0`, and related helpers | Directly index the loaded four-byte `mitem` records by global ID | Supply and validate rows through the highest supported ID |
| `Menu_Item_DrawListRow`, `0x4FD130` | Reads the global-ID `mitem` type and calls the shared name getter | Extended metadata must also cover rendering paths |
| `MenuReadFiles`, `0x4A1BF0` | Loads `mitem.bin` into three distinct buffers for different menu consumers | Cover all copies, including their reloads and ownership |
| Main menu initializer, `0x4C0B30` | Reloads `mitem.bin` into an existing buffer | File growth/reload must never overrun a previous allocation |
| `setBattleItemFromInventory`, `0x48C6E0` | Only admits IDs 1-32 and uses the saved order to select a live slot | New battle IDs need a different eligibility and ordering path |
| `updateBattleItemData`, `0x48C670` | Reads target/selectability flags directly from `K_ITEM[id]` | Redirect battle-record access; do not reinterpret non-battle name rows as effects |
| `computeCommandAction`, `0x48D200` | Reads item attack type/animation and other battle data; some summon items have ID-specific behavior | Audit every item read and preserve special vanilla behavior |
| Battle exit writeback, `0x4868C0` | Iterates the live battle inventory and writes quantities to the 198 saved slots | Extending the live list also requires extending writeback consumers |
| Battle ordering helpers, `0x4AD0C0` and `0x4F8180` | Assume 32 order entries; use a 32-bit occupancy mask / 32-byte temporary array | Replacing an ID comparison alone is unsafe |
| `MenuComputeItemPrices`, `0x4EDC50` | Iterates a hardcoded 200 entries and builds native buy/sell tables | Extend/pad inputs safely and relocate all dependent tables/loops |
| `src/ff8_data.cpp` | Exposes shop staged quantities as a 200-byte span | Audit the native table and every indexed reader/writer before admitting IDs 200+ |
| `src/ff8_opengl.cpp`, `ff8_add_item_to_player` | FFNx already replaces inventory addition on the achievement path | Integrate with this hook instead of silently overriding it |

### Sort is an immediate correctness and memory-safety blocker

The native sort collects `amounts[item_id]` in a 200-byte local array, clears all
198 saved inventory entries, then rebuilds them with `for (id = 1; id <= 198; ++id)`.
ID 199 is collected but never emitted. IDs 200+ index outside the declared array;
IDs sufficiently far past it can corrupt adjacent stack state. Simply growing
kernel text or allowing an acquisition opcode is therefore insufficient.

Replace this operation with a 256-entry amount table, unsigned IDs, and output
bounded by the existing inventory capacity. Preserve the menu's normal state
transition and cursor/help refresh. Test the code that actually replaces the
inline Sort arm, not only an isolated sorting helper.

### Kernel loading already provides the foundation

`src/ff8/kernel.h` defines typed battle/non-battle item records, section enums,
record size assertions and the native compatibility image. `ff8_kernel_file`
reads section spans from the file header.

`ff8_kernel_load_hook` in `src/ff8/kernel_magic.cpp` currently permits changed
counts only for Magic, learned abilities and battle commands. It copies other
data sections into their fixed native capacities and reports unsupported size
changes. An extended non-battle section is therefore truncated in that image.

Add a dedicated `kernel_item` module following the existing extension lifecycle:
read validated spans, retain the full item records in owned storage, integrate
with the shared load hook, then arm verified native consumers. Keep the fixed
native layout for consumers that have not been relocated. Define behavior for
vanilla/extended reloads and for invalid input; avoid leaving partially installed
hooks or stale extended tables.

### Field effects and battle effects are separate contracts

For an initial field-item format, retain the existing global ID boundary at 33.
Append non-battle name/description records to section 8 and their strings to
section 39. Supporting global ID 255 this way requires 223 non-battle records
(`255 - 33 + 1`). Extend `mitem.bin` through the same global ID; 256 rows occupy
1024 bytes. Existing field effect types can then be reused after their consumers
have been audited. Growing a table does not define a new effect implementation.

Do not derive the non-battle base ID from a newly enlarged battle-record count:
that would renumber vanilla items beginning with Tent (33). New battle items need
an explicit mapping from global ID to battle effect record, or a documented
sparse/global-index format with padding. Choose that contract before implementing
battle-item loading or editor support. Merely appending rows to section 7 cannot
resolve the native 32-entry menu and saved ordering limitations.

The current field metadata helpers have many inline consumers as well as callable
accessors, and three `mitem.bin` buffers. Replacing only `Mitem_GetType` would miss
those reads. Validate whole-record lengths and cross-file coverage, and account
for reloads into existing allocations.

### Shops must be included in the first safety audit

Even an item awarded by a field script may later appear in a shop's sell list.
Postponing custom shop stock does not make the native price and staged-quantity
arrays safe for a new ID. Audit the sell path and either fully support extended
IDs there or explicitly exclude them before any out-of-range indexing.

The extracted `price.bin` has 199 rows although the observed price-builder loop
runs 200 times. Recheck the loader/allocation padding and preserve vanilla
behavior; do not assume the loop bound proves a 200-record on-disk format.

## Implementation sequence

1. Finish the consumer survey: field item use/sort, shops, refine inputs/outputs,
   ammo, monster drop/mug/rewards, text expansion, scripts, battle item reads and
   signed-byte / `0xFF` sentinel handling. Record complete patch sets per build.
2. Implement the shared item module, names/descriptions and input validation,
   plus a sort replacement and safe shop handling. Keep the existing save size.
   Verify all `mitem` copies and reloads with extended and vanilla inputs.
3. Prove one new field item, ID 199, with a distinctive name and a known existing
   effect. Exercise acquisition, menu display/use, Sort, save/reload and shop
   entry/selling. Repeat at IDs 200 and 255 after the sentinel audit.
4. Settle the battle-data contract, relocate/replace the live list and consumers,
   define deterministic ordering for extended IDs without indexing the 32-byte
   saved order, and integrate battle consumption/writeback. If persistent custom
   ordering or more saved inventory slots are wanted, design their storage first.
5. Port verified anchor/operand layouts to other supported executables and test
   compatibility with the existing Magic/Ability/Command extensions. EN addresses
   must never be applied unchanged to other language/version builds.

For each feature, verify the complete patch set before activating it; keep vanilla
behavior when no extension is present. An unsupported executable or inconsistent
asset set must produce a clear diagnostic before any unsafe native consumer runs.

## Validation required before claiming support

- Vanilla kernel and menu files: names, sorting, item use, shops, battle items,
  achievement wrappers and saves behave as before.
- Boundary IDs 198/199/200/254/255: correct text, effects, quantities, acquisition,
  removal, manual rearrangement and Sort; no signed-byte truncation or sentinel
  collision. ID 0 remains empty.
- Mixed inventory and a full 198-slot inventory: preserve quantities, refuse an
  additional distinct item cleanly and keep neighboring save data intact.
- Save/reload, field/battle transitions, repeated menu opens and kernel/menu file
  reloads: retain valid IDs and avoid stale pointers or smaller old allocations.
- Shops: sell-list rendering, staging, buy/sell prices, stock, overflow handling
  and exits for new IDs. Refine and drop/mug paths need equivalent coverage.
- Invalid/truncated files, missing `mitem` rows, missing strings, unsupported
  counts and mismatched resources: safe rejection before native indexing.
- Battle milestone: all original battle items plus a new ID, more than 32 eligible
  types, selection/targeting, action execution, consumption and exit writeback.

This investigation used source review, read-only IDA decompilation/cross-references
and independent binary record-count checks. No runtime item patch was added,
compiled, deployed or tested in game.
