"""Replay AddMoreItems comments/names in the verified 2013 EN IDB.
Run through IDA's File > Script file menu. Native bytes and data layouts are
unchanged. Save the IDA database after applying if automatic saving is disabled.
"""
import ida_nalt
import idc

INPUT_SHA256 = "064d466b5fe2ba901fd44abf19f37c0fd6a2db40aabd95c9e5959195b6589570"
ANNOTATIONS = [
    [
        "0x1CF7778",
        "AddMoreItems: native kernel section7 (zero-based), 33 battle-item records x24 bytes. Runtime full table is indexed by GLOBAL byte ID: original rows0..32 unchanged; appended file rows33..88 map to global IDs199..254. IDs33..198 retain their original nonbattle meaning. Global255 has no added battle record (random helper failure sentinel). Native compatibility kernel image is not resized."
    ],
    [
        "0x1CF7A90",
        "AddMoreItems: native kernel section8 (zero-based), original166 name/description pairs x4 bytes for global IDs33..198. Append up to57 records for global199..255. Runtime name/description hooks use row=ID-33 for all IDs>=33, text section39. IDs0..32 use battle records/text section38. Both offsets are u16 relative to text-section base; FFFF=no string; all offsets/terminators validated."
    ],
    [
        "0x1D28E78",
        "Native live battle inventory: 32 records x5 bytes {u8 global_id,quantity,battle_flag,target_info,selectability}. End0x1D28F18 aliases the start of unrelated monster data: relocate real list references/end comparisons only. AddMoreItems uses separate96-entry storage (480 bytes): original32 slots follow saved order; added IDs199..254 occupy slots32..87; remaining8 empty. Never index the 32-byte saved order with an added ID."
    ],
    [
        "0x1D768D0",
        "Native shared battle-menu state: 52-byte control prefix +32 item reservation bytes (84 bytes). AddMoreItems relocates the WHOLE state plus all634 verified code operands to148 bytes=52+96. Merely enlarging the item list is unsafe: native list initialization clears count reservation bytes. Regression calls actual BattleMenu_ListWindow_Init with96 and verifies neighboring buffers stay intact."
    ],
    [
        "0x1D8CD18",
        "Native buy-price table: 200 u32 entries. AddMoreItems relocates it to256 u32 entries, preserving native price/discount formulas and minimum-price behavior. price.bin remains4 bytes per GLOBAL ID, padded/allocated to1024 bytes through ID255."
    ],
    [
        "0x1D8D120",
        "Native sell-price table: 200 u32 entries. AddMoreItems relocates it to256 entries and extends native price-building loops to256. Shop sell inventory has32 pages x8, logical inventory bound255; native selection guards reject the final blank row."
    ],
    [
        "0x1D8D058",
        "Native shop staged quantities: 200 bytes indexed by GLOBAL ID. AddMoreItems relocates to256 bytes, changes clear lengths50->64 dwords, and updates FFNx's external span. Inventory staging/writeback loops198->255; ID purchase loop200->256. Native regression verifies high-ID staging, last-slot quantity update, and insertion beyond original capacity."
    ],
    [
        "0x47EA90",
        "Item description lookup; same vanilla ID split at33 as name getter0x47EA30. AddMoreItems replaces both getters: IDs<33 read full battle records/text38, IDs>=33 read section8[ID-33]/text39. Returns empty for undefined IDs/FFFF; appended battle rows never renumber Tent-and-later original IDs."
    ],
    [
        "0x4AD0DA",
        "VERIFIED signed-ID hazard: original movsx eax,byte ptr[edx] allows IDs128..255 to become negative and pass the signed ID<33 filter before indexing battle_order[ID-1]. AddMoreItems guarded opcode patch at0x4AD0DB: BE->B6 (movzx). Keep original bytes in IDB; correction is applied by FFNx at runtime. Regression: inventory containing ID255 must not reserve a vanilla battle-order position."
    ],
    [
        "0x4C0A30",
        "Native row navigation (__cdecl, pad bits,count,current row): Down4000 increments/wraps, Up1000 decrements/wraps. Generic helper remains unchanged. AddMoreItems replaces only item-list calls4F831B,4FB8E0 and rearrangement-target call4F8C50 with count=min(11,255-page*11), giving two rows on page24. Target/ability/yes-no menu calls keep original counts."
    ],
    [
        "0x4F831B",
        "AddMoreItems partial-page hook: primary item-use list; cursor_item at menu+54h. Pass actual final-page row count2 to native4C0A30; prevent selecting display padding. Production menu wrapper clamps cursor before/after native update."
    ],
    [
        "0x4F8C50",
        "AddMoreItems partial-page hook: manual rearrangement DESTINATION list, state98/99; cursor_target=menu+58h. Source uses+54h. Destination must also wrap only within two usable rows on page24."
    ],
    [
        "0x4FB8E0",
        "AddMoreItems partial-page hook: manual rearrangement SOURCE list, state96/97; cursor_item=menu+54h. Both source and destination are clamped before native swap state100. Regression executes native swap with a stale padded-row target and verifies padding stays zero."
    ],
    [
        "0x4F90A8",
        "AddMoreItems main item paging: native18 pages becomes24; wrap-last17->23 also at4F920F. Native page size11 retained. Page24 covers logical slots253,254; nine extra physical entries are zero display padding, not selectable/saved slots."
    ],
    [
        "0x4FBA6D",
        "AddMoreItems rearrangement destination page-up last-page constant17->23. Page-down18->24 at4FBC12. Source navigation uses the main item paging path. New cursor clamping is required when page switching from a row beyond1 into final partial page."
    ],
    [
        "0x4D72A9",
        "AddMoreItems refinement menu page count18->24; equivalent counts at4D72D3,4D72FD,4D7327. Inventory traversal consumers extend198->255. Existing refinement recipes/effect types are unchanged; this extends item inventory access."
    ],
    [
        "0x4EC309",
        "Shop sell-menu page count25->32 (8 rows/page) for255 usable inventory slots. Price/quantity tables separately cover all256 IDs. Do not confuse inventory slot capacity with number of possible byte IDs."
    ],
    [
        "0x4BC91C",
        "BattleMenu_ExecuteSelectedCommand item-list count32->96. Shared menu state/reservation buffer also relocated to52+96 bytes; all native control-state references must follow the relocated prefix. The original32 saved order values control original battle IDs only."
    ],
    [
        "0x48C6E0",
        "AddMoreItems replaces native live-battle-item builder. Relocated96 slots: saved battle_order[ID-1] for original IDs1..32; stable slot32+ID-199 for defined added battle IDs199..254. Each5-byte live record is populated from full battle metadata indexed by global ID; zero/zero-quantity slots skipped. ID255 remains field-only."
    ],
    [
        "0x48C670",
        "AddMoreItems replaces metadata refresh to visit all96 live records and use full global-ID battle table. Selectability is derived from low attack_flags bits80h/20h; target_info and battle_flag copied using original layout."
    ],
    [
        "0x483CA0",
        "Native random battle-item selector: nonzero input is returned unchanged; input0 chooses item whose randomSelectFlag bit0 is set; none returns255. AddMoreItems replaces fixed32-slot/wrap1Fh search with eligible-ID selection over96 live slots using native RNG48F020. This sentinel is why added battle records stop at global254."
    ],
    [
        "0x485E60",
        "Native battle quantity lookup by global item ID. AddMoreItems relocates live-list base/end to96-entry storage; isolated regression reaches added ID254 in slot87."
    ],
    [
        "0x486B40",
        "Native battle quantity adjustment/consumption. AddMoreItems relocates all audited live-item references/end bounds; native regression consumes added ID254 and verifies the added record changes."
    ],
    [
        "0x486C10",
        "Native live-battle-to-player inventory writeback. AddMoreItems uses96 live slots and255 player slots; retain ID with zero quantity until matching saved slot is updated. Save normalization subsequently clears zero/duplicate slots without reordering the inventory."
    ],
    [
        "0x47ED00",
        "Native acquisition has fixed198-slot bound and calls original battle-order bookkeeping. AddMoreItems patches native inventory pointers/bounds to255. When FFNx Steam-achievement hook replaces this function, ff8_add_item_to_player routes to the extension-aware helper instead; signed quantity adjustment and100 cap are preserved. Magazine checks use the full inventory span."
    ],
    [
        "0x47ECD0",
        "Native single-unit quantity decrement for a global item ID; scans entire inventory and does not immediately clear zero-quantity ID. AddMoreItems relocates base/end (including quantity-byte end+1) to255 slots. Actual machine-code regression decrements a late added slot without touching its neighbor."
    ],
    [
        "0x56DAF0",
        "Native hasItem returns presence by scanning IDs (not a quantity getter). AddMoreItems relocates inventory start/end so field presence checks reach IDs/slots beyond original198; actual machine-code test reaches ID255."
    ],
    [
        "0x4B9530",
        "Menu file allocator. AddMoreItems intercepts mitem.bin and price.bin loads, allocates256x4=1024 bytes using game allocator, copies bounded native data, pads missing rows, frees original allocation. All three native mitem copies must be covered. Missing mitem rows default type10 (no field use); invalid type>21 disabled. Price minimum rules remain native."
    ],
    [
        "0x4BE8C0",
        "Read menu file into existing allocation. AddMoreItems intercepts mitem.bin/price.bin reloads through temporary native allocation then bounded1024-byte copy; avoids overflowing the existing expanded destination on oversized or fractional4-byte files. Other assets use original function."
    ],
    [
        "0x4C0BCC",
        "Direct mitem.bin read call also intercepted by AddMoreItems bounded reload helper. This path bypasses generic4BE8C0, so allocation hook alone is insufficient. Native metadata helpers4F74F0(type),4F7510(parameter),4F7530(flags) verified with added IDs."
    ],
    [
        "0x4E3BB0",
        "AddMoreItems save-builder call hook (also4E3C11): normalize inventory; project first198 slots into native savemap BEFORE original4E2EF0 computes CRC; then write full255-slot extension into unused raw image tail at1600h. Raw image remains8192 bytes, native savemap5028 bytes and both original CRC copies stay unchanged."
    ],
    [
        "0x4E3C11",
        "Second AddMoreItems save-builder call hook. Tail block at raw image+1600h is522 packed bytes: u32 magic4D495846h ('FXIM'), u16 version1,u16 slots255,u32 FNV-1a inventory checksum,255x{u8 ID,quantity}. FNV seed2166136261,multiplier16777619. Vanilla CRC does not cover tail; block has its own checksum. Tail stays inside existing LZSS save, no sidecar/field-variable use."
    ],
    [
        "0x4E4DE3",
        "Accepted save-load read length5120->7808 so raw image tail is available when reading from image+384 through8192. Native savemap copy remains5028 bytes. Preview paths do not import extension or modify running inventory."
    ],
    [
        "0x4E4F1E",
        "Accepted-load native Menu_CopyBytes call, after vanilla validation, is wrapped for item import. First import198 native slots and clear extras; load full255 only if extension magic/version/count/checksum/unique IDs/quantities valid AND first198 exactly match native projection. Corrupt/old/editor-stale block falls back to native198. Wrapper source points to raw image+384."
    ],
    [
        "0x4E4401",
        "New-game initialization call to56DA10 wrapped by AddMoreItems: execute native defaults, clear full255-slot runtime inventory, import198 native defaults, normalize. Prevents extra slots from surviving a new game."
    ],
    [
        "0x4F74F0",
        "Native field mitem type getter indexed by global byte ID. With expanded1024-byte mitem allocation, ID255 lookup is valid. No new effect dispatch types are introduced: new definitions select existing field behavior via4-byte mitem records; battle effects are separate kernel section7 records."
    ],
    [
        "0x4F7510",
        "Native field-item parameter getter; extended mitem rows retain original record layout. Native harness verifies Potion-derived added ID199 parameters and flags through this getter and4F7530."
    ],
    [
        "0x4FB430",
        "Native Sort state79: fixed198 inventory scan, 200-byte stack quantity table indexed by item ID, rebuild only IDs1..198. Unsafe for added IDs. AddMoreItems intercepts this state BEFORE native arm executes; replacement merges/sorts all byte IDs1..255, caps stacks100 and clears remaining slots. State then returns to3 with tab0. Original main-controller state-machine documentation is preserved at function entry."
    ],
    [
        "0x4F81F6",
        "FFNx AddMoreItems audit (07c50093, 2026-10-22): verified 2013 EN only. Native inventory198 slots/18 pages. Runtime255 usable slots/24 pages (last2 rows), plus9 zero display-padding records. Global item ID255 is valid, but slot index255 is a not-found sentinel. Wrapper handles Sort state79 outside its200-byte local table and bounds both rearrangement cursors. Runtime patch map src/ff8/kernel_item_patches.h validates843 operands plus hook signatures before activation; docs/ff8/items.md describes format and tests. Native machine-code regression covers paging, three final-page navigation paths and swapping."
    ],
    [
        "0x1CFE79C",
        "Native player inventory:198 records x{u8 item_id,u8 quantity}, end1CFE928h. AddMoreItems relocates consumers to255 usable records (510 bytes) followed by9 zero display-padding records. ID0 empty, IDs1..255 valid, stack cap100. Preserve native savemap layout: first198 projected here before save CRC; full255 stored in save-tail block. Do not enlarge this native IDB array across adjacent save variables."
    ]
]
RENAMES = [
    [
        "0x47EA90",
        "sub_47EA90",
        "Item_GetDescription"
    ],
    [
        "0x4C0A30",
        "sub_4C0A30",
        "Menu_NavigateRows"
    ],
    [
        "0x4AD0C0",
        "sub_4AD0C0",
        "Inventory_ReserveBattleOrderPosition"
    ],
    [
        "0x4EBA80",
        "sub_4EBA80",
        "Shop_StagePlayerInventory"
    ],
    [
        "0x4EB9F0",
        "sub_4EB9F0",
        "Shop_CommitGilAndInventory"
    ],
    [
        "0x483CA0",
        "sub_483CA0",
        "BattleItems_SelectRandomEligible"
    ],
    [
        "0x47ECD0",
        "sub_47ECD0",
        "Inventory_RemoveOne"
    ],
    [
        "0x4BE8C0",
        "sub_4BE8C0",
        "MenuAsset_ReadIntoBuffer"
    ]
]


def apply_annotations():
    digest = ida_nalt.retrieve_input_file_sha256()
    actual = digest.hex() if isinstance(digest, bytes) else str(digest).lower()
    if actual != INPUT_SHA256:
        raise RuntimeError("This annotation map requires the verified 2013 EN IDB.")
    for address, comment in ANNOTATIONS:
        if not idc.set_cmt(int(address, 16), comment, 0):
            raise RuntimeError("Cannot annotate " + address)
    for address, original, replacement in RENAMES:
        ea = int(address, 16)
        current = idc.get_name(ea)
        if current == replacement:
            continue
        if current != original:
            print("Preserving existing name at", address, current)
            continue
        if not idc.set_name(ea, replacement, idc.SN_CHECK):
            raise RuntimeError("Cannot rename " + address)
    print("Applied", len(ANNOTATIONS), "AddMoreItems comments and",
          len(RENAMES), "verified function-name mappings.")
    print("Native bytes and native savemap layouts are unchanged.")


if __name__ == "__main__":
    apply_annotations()
