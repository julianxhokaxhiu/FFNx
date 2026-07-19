/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 HobbitDur                                          //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#include "kernel_magic.h"

#include "../ff8.h"
#include "../patch.h"
#include "../globals.h"
#include "../common.h"
#include "../log.h"

#include <stdint.h>
#include <string.h>

// -------------------------------------------------------------------------
// AddMoreMagic - extended kernel.bin magic section (EN/FR/DE/SP/IT/JP/JP_NV
// retail 1.2 exes)
//
// The exe ignores the kernel.bin header for data access: 31 data-section
// labels are baked at vanilla buffer offsets (K_MAGIC = buffer+540, 57*60B).
// Only TEXT sections (file sections 31..55) are resolved through the header
// at runtime. Spell ids are bytes; ids >= 64 are routed to GF handling
// (GFs are 64..79), which caps vanilla magic at 64 ids.
//
// This module arms itself only when the loaded kernel.bin has a grown
// section 1. It then:
//  1. Serves the game a vanilla-LAYOUT image of the file (magic truncated
//     to 57 in-buffer) so every baked data address stays valid, keeps the
//     full file stashed, and points every text-section header offset into
//     the stash (text sections may thus grow freely too).
//  2. Copies ALL magic entries (up to 256) into an FFNx-side table and
//     rewrites every code displacement targeting [K_MAGIC, K_MAGIC_END)
//     to the table (the exe never bounds-checks these indexings).
//  3. Fixes the 4 magic-vs-GF classification branches (cmp id,64) so only
//     64..79 take the GF path: name getter, description getter, draw-list
//     visibility, Draw-command execution.
//  4. Replaces 3 functions that cannot be branch-patched:
//     - linkedStockFieldCharData (Draw->Stock setup; split cmp/jcc),
//     - menu_reorder_magic (writes a 64-byte STACK buffer indexed by
//       spell id -> would corrupt the stack with extended ids),
//     - sub_4BE790 (per-char held-magic + junction validate; writes a
//       64-bit STACK bitfield indexed by id/32 -> stack smash for id >= 64).
//  5. Relocates the savemap drawn-once bitfield (blind-scanned by value,
//     same technique as step 2) to persisted free savemap space once any
//     magic id >= 96 exists, since 5 native access sites index it by
//     (id-1)/32 with no bounds check.
//
// File contract for modders: kernel.bin's section 1 must contain entries
// for ids 0..N-1 *including* 32 dummy 60-byte rows for ids 64..95 (GFs 64-79,
// 80-95 reserved for a future 32-GF exe patch) when N > 64 (id == entry index
// everywhere). mmagic.bin (standalone menu file, 4B/spell, loaded with
// size-from-file) must be extended to cover the highest id; magsort.bin can
// stay vanilla (new spells sort last).
//
// Per-version addresses are resolved once in ff8_data.cpp (ff8_find_externals)
// into ff8_externals.magic_* fields, following this codebase's standard
// convention for version-dependent addresses - see that file for how each
// one was derived (EN researched directly in IDA; FR/DE/SP/IT/JP/JP_NV via
// byte-signature matching + a verified per-language data-segment delta,
// cross-checked against 5 independent anchors per language). This module
// only consumes ff8_externals.magic_*; the drawn-once relocation target is
// not one of those fields - it is computed below from the already-resolved
// field_vars_stack_1CFE9B8 external (the field-script variable block base)
// plus 753, the slot the drawn-once bitfield relocates to.
// -------------------------------------------------------------------------

#define CHAR_STRIDE             152         // FF8CharacterData record size
#define CHAR_MAGIC_OFF          16          // 32 x {id:u8, amount:u8}
#define CHAR_JUNCTION_OFF       92          // 20 stat slots, each = a junctioned magic id
// Relocation target for the drawn-once bitfield when extended magic (id >= 96)
// is present: field-script variable 753. Vars 753-1023 (271 bytes) are
// verified unused on all three axes - no field script (all 882 *.jsm
// scanned), no EXE code reference, zero in every real save - AND they sit
// inside the save's CRC span, so the game serializes and checksums them on a
// normal save. Pointing the drawn-once accesses here gives native
// persistence for a full 256-bit table with no save/load file hooks.
#define SCAN_CODE_START         0x401000u
#define SCAN_CODE_END           0x520000u

#define VANILLA_KERNEL_SIZE     37992u
#define VANILLA_MAGIC_COUNT     57
#define MAGIC_ENTRY_SIZE        60
#define MAX_MAGIC_ID            256
#define GF_FIRST_ID             64
#define GF_LAST_ID              79          // the exe's real 16 GF ids (SG_ARRAY_GF_DATA[16]);
                                            // classification trampolines use this, unchanged.
// Reserve ids 64..95 (32 slots) for GFs - 16 used today, 16 kept free for a
// future 32-GF exe patch. Extended (mod-added) magic therefore starts at 96,
// leaving 80..95 as an unused hole. Bump GF_RESERVED_COUNT (and GF_LAST_ID,
// once the exe supports it) when that future change lands.
#define GF_RESERVED_COUNT       32
#define EXTENDED_MAGIC_FIRST    (GF_FIRST_ID + GF_RESERVED_COUNT)   // 96
#define KERNEL_SECTION_COUNT    56
#define KERNEL_MAGIC_SECTION    1           // data section that may grow
#define KERNEL_FIRST_TEXT_SEC   31          // sections 31..55 resolved via header
#define KERNEL_FILE_MAX         (1024 * 1024)

// Vanilla EN data-section offsets (sections 0..31; index 31 = first text
// section, used as the end bound of data section 30). Data section sizes are
// language-independent, so this table is the same for every retail build.
static const uint32_t vanilla_data_offsets[32] = {
	228, 540, 3960, 6072, 13752, 14148, 14244, 14640, 15432, 16096,
	16416, 16608, 16768, 16920, 17072, 17232, 17272, 17344, 17536, 17656,
	17912, 18424, 18616, 18936, 19036, 19052, 19152, 19212, 19468, 19660,
	19720, 19976,
};

// ---- state --------------------------------------------------------------
static uint8_t ff8_magic_table[MAX_MAGIC_ID][MAGIC_ENTRY_SIZE];
static int ff8_magic_count = VANILLA_MAGIC_COUNT;
static bool ff8_magic_armed = false;
static char *ff8_kernel_stash = nullptr;     // full grown kernel.bin image
static char *ff8_magic_text = nullptr;       // stash + grown offsetMagicText

typedef int(__cdecl *load_file_to_buffer_t)(const char *, char *);

// ---- classification stubs (no inline asm) --------------------------------
// 4 sites in the exe do "cmp reg,40h / jcc gf_path" to route ids >= 64 to GF
// handling; none of them sit at a call instruction we could redirect with
// replace_call, and their containing functions are either too large
// (computeCommandAction) or too dependent on undocumented struct layouts
// (manageMonsterSpellVisibility) to safely reimplement whole in C. Each site
// is instead replaced with a `call` into a tiny stub - built here as literal
// machine-code bytes, not compiler-assembled inline asm - that widens the
// check to the GF range (64..79) via an ordinary __cdecl C function, then
// either falls back into the original instruction stream (magic path) or
// jumps to the original jcc's target (GF path). This is a pure-C-callable
// design: the only non-C part is the handful of opcode bytes needed to save/
// restore the register around the call and act on its return value.
static int __cdecl ff8_is_gf_id(int id)
{
	id &= 0xFFFF; // the draw-execute site classifies on BX (16-bit); harmless elsewhere
	return (id >= GF_FIRST_ID && id <= GF_LAST_ID) ? 1 : 0;
}

// Bump-allocates from a single RWX page, lazily created on first use. FFNx
// has no existing "trampoline buffer" utility (every other patch either
// rewrites an existing instruction in place, via patch_code_*/memcpy_code, or
// redirects a call/function entry point, via replace_call/replace_function);
// these stubs are genuinely new, tiny pieces of machine code, so they need
// their own executable memory rather than driver_malloc's plain heap.
static uint8_t *ff8_trampoline_alloc(uint32_t size)
{
	static uint8_t *page = nullptr;
	static uint32_t used = 0;
	if (!page)
		page = (uint8_t *)VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	uint8_t *stub = page + used;
	used += size;
	return stub;
}

// x86 register encodings used below (push/pop opcode = base + index).
#define REG_EAX 0
#define REG_EBX 3

// Builds: push reg ; call ff8_is_gf_id ; test al,al ; pop reg ; jnz gf_target ; ret
// `reg` (32-bit) holds the spell id on entry and is fully restored before the
// final `ret`, so falling through to the site's next instruction (the magic
// path) sees the exact same register state the original "cmp/jcc" would have
// left it in; taking the jnz instead reproduces the original jcc's GF jump.
static void *ff8_build_classify_stub(uint8_t reg, uint32_t gf_target)
{
	uint8_t *stub = ff8_trampoline_alloc(16);
	uint8_t *p = stub;
	*p++ = 0x50 + reg;                                       // push reg
	*p++ = 0xE8;                                              // call rel32
	*(uint32_t *)p = (uint32_t)&ff8_is_gf_id - (uint32_t)(p + 4);
	p += 4;
	*p++ = 0x84; *p++ = 0xC0;                                 // test al,al
	*p++ = 0x58 + reg;                                        // pop reg
	*p++ = 0x0F; *p++ = 0x85;                                 // jnz rel32
	*(uint32_t *)p = gf_target - (uint32_t)(p + 4);
	p += 4;
	*p++ = 0xC3;                                              // ret
	return stub;
}

// Verify the expected "cmp reg,40h" + jcc encoding, save the jcc target,
// then overwrite the whole compare-and-branch with "call stub" (+NOPs).
//
// The compare may carry a 0x66 operand-size prefix ("cmp bx,40h" instead of
// "cmp ebx,40h"): the DRAW-execute site is `66 83 FB 40 0F 83 rel32`. The site
// MUST start at that prefix - patching one byte late leaves the 0x66 in front
// of our 0xE8, which the CPU decodes as `66 E8` = a 16-bit CALL that pushes a
// 2-byte return address and corrupts the stack. So handle the prefix here.
static bool install_id_trampoline(uint32_t site, uint8_t reg, const char *what)
{
	const uint8_t *p = (const uint8_t *)site;
	uint32_t pre = (p[0] == 0x66) ? 1 : 0;   // optional operand-size prefix

	// cmp reg,imm8: (66) 83 /7 ib  (modrm F8=eax/ax, FB=ebx/bx)
	if (p[pre] != 0x83 || p[pre + 2] != 0x40)
	{
		ffnx_warning("AddMoreMagic: unexpected bytes at %s site 0x%X (%02X %02X %02X %02X), skipping patch!\n",
			what, site, p[0], p[1], p[2], p[3]);
		return false;
	}

	uint32_t cmp_len = pre + 3;           // (prefix) + opcode + modrm + imm8
	const uint8_t *j = p + cmp_len;       // the following jcc
	uint32_t patch_size, gf_target;
	if ((j[0] & 0xF0) == 0x70)            // jcc rel8 (2 bytes)
	{
		gf_target = site + cmp_len + 2 + (int8_t)j[1];
		patch_size = cmp_len + 2;
	}
	else if (j[0] == 0x0F && (j[1] & 0xF0) == 0x80) // jcc rel32 (6 bytes)
	{
		gf_target = site + cmp_len + 6 + *(int32_t *)(j + 2);
		patch_size = cmp_len + 6;
	}
	else
	{
		ffnx_warning("AddMoreMagic: unexpected jcc at %s site 0x%X (%02X), skipping patch!\n", what, site, j[0]);
		return false;
	}

	void *stub = ff8_build_classify_stub(reg, gf_target);

	// call stub (5 bytes) + NOP fill for the remainder of the region.
	uint8_t code[16];
	code[0] = 0xE8;
	*(uint32_t *)&code[1] = (uint32_t)stub - (site + 5);
	for (uint32_t i = 5; i < patch_size; ++i) code[i] = 0x90;
	memcpy_code(site, code, patch_size);

	return true;
}

// ---- blind value-scan relocation (shared by K_MAGIC and drawn-once) -----
// Scans SCAN_CODE_START..SCAN_CODE_END for any dword equal to `from` and
// repoints it to `to`. One important false-positive class must be excluded: a
// `call rel32` (E8) or `jmp rel32` (E9) whose opcode+displacement bytes happen
// to form a value equal to `from`. Real example (EN 1.2, K_MAGIC): the call to
// getAICON_SP1_DATA @0x49A483 is E8 48 CF 01 00, and the dword at 0x49A483 is
// 0x01CF48E8 - squarely in the K_MAGIC range. Rewriting it corrupts the call
// (E8 -> part of a table address, decoding as `pop esp`), which crashes the
// field menu (only reached when dword_1D6BC4C==0). Such a match has the branch
// opcode as its low byte AND a target that lands in real .text - a genuine
// data operand never does - so skip those.
static uint32_t relocate_scan(uint32_t from, uint32_t to, uint32_t range_end, const char *what)
{
	uint32_t rewritten = 0, skipped_branch = 0;

	for (uint32_t addr = SCAN_CODE_START; addr < SCAN_CODE_END - 4; ++addr)
	{
		uint32_t value = *(uint32_t *)addr;

		if (value >= from && value < range_end)
		{
			uint8_t op = *(uint8_t *)addr;
			if (op == 0xE8 || op == 0xE9) // call/jmp rel32?
			{
				uint32_t target = addr + 5 + *(int32_t *)(addr + 1);
				if (target >= SCAN_CODE_START && target < 0x600000u)
				{
					++skipped_branch; // real branch instruction - never touch it
					continue;
				}
			}

			patch_code_dword(addr, (DWORD)(to + (value - from)));
			++rewritten;
			addr += 3; // skip the rewritten dword
		}
	}

	ffnx_trace("AddMoreMagic: %s: relocated %u displacement(s), skipped %u branch false-positive(s).\n",
		what, rewritten, skipped_branch);
	return rewritten;
}

// ---- replaced functions -------------------------------------------------
// Draw->Stock action setup (replaces linkedStockFieldCharData).
// Vanilla routed every id >= 64 to the GF branch; only 64..79 belong there.
static void *__cdecl ff8_linked_stock_field_char_data(int char_slot, int spell_id)
{
	uint8_t *chr = (uint8_t *)(ff8_externals.magic_f_char_data + 464 * char_slot);
	const uint8_t *draw_cmd = (const uint8_t *)(ff8_externals.magic_k_battle_command + 8 * 10);

	chr[0] = 10;          // battle command id: Draw
	chr[1] = draw_cmd[5]; // command menuFlags
	chr[2] = draw_cmd[6]; // command targetInfo
	chr[3] = 0;
	chr[4] = 9;

	if (spell_id >= GF_FIRST_ID && spell_id <= GF_LAST_ID)
	{
		chr[5] = 0;
		chr[6] = 0;
		chr[7] = 2; // stock target type: GF
	}
	else
	{
		const uint8_t *magic = ff8_magic_table[spell_id & 0xFF];
		chr[5] = magic[9];  // statusWindowFlags
		chr[6] = magic[10]; // targetInfo
		chr[7] = 0;
		if (magic[11] & 0x40) // ATTACK_FLAG_REVIVE
		{
			chr[7] = 1;
			chr[3] |= 1;
		}

		// Inventory: full-stock flag (amount 100) / no-free-slot flag.
		uint8_t *inventory = chr + 130; // FF8FieldMagicData[32], stride 5, {id, amount, ...}
		int slot = 0;
		while (inventory[5 * slot] != (uint8_t)spell_id)
		{
			if (++slot >= 32)
			{
				for (slot = 0; slot < 32 && inventory[5 * slot]; ++slot);
				if (slot >= 32) chr[3] |= 2; // inventory full
				return chr;
			}
		}
		if (inventory[5 * slot + 1] == 100) chr[3] |= 2; // already at max stock
	}

	return chr;
}

// Magic menu sort (replaces menu_reorder_magic). The vanilla function fills a
// 64-byte stack array indexed by spell id; extended ids would corrupt the
// stack. Ids absent from the sort preset (all extended ones with a vanilla
// magsort.bin) are appended in ascending id order instead of being silently
// dropped.
static int __cdecl ff8_menu_reorder_magic(int character_id, int sort_preset)
{
	// magsortbuffer is an array of pointers (MenuReadFiles passes it as
	// Menu_GetFile((void**)magsortbuffer, ...) - no '&' - so magsortbuffer[0]
	// itself holds the loaded-file pointer, confirmed by decompiling
	// MenuReadFiles @ 0x4A1C31). Must dereference before indexing by preset.
	const uint8_t *preset = (const uint8_t *)(*(uintptr_t *)ff8_externals.magic_magsort_buffer) + 64 * sort_preset;
	if (!preset[0]) return 0;

	uint8_t amounts[MAX_MAGIC_ID];
	memset(amounts, 0, sizeof(amounts));

	uint8_t *inventory = (uint8_t *)(ff8_externals.magic_sg_chara_data + 152 * character_id + 16); // 32 x {id, amount}
	for (int i = 0; i < 32; ++i)
	{
		uint8_t id = inventory[2 * i], amount = inventory[2 * i + 1];
		if (id && amount) amounts[id] = amount;
	}

	memset(inventory, 0, 64);

	int write_slot = 0;
	for (int i = 0; i < 64 && preset[i]; ++i)
	{
		uint8_t id = preset[i];
		if (amounts[id] && write_slot < 32)
		{
			inventory[2 * write_slot] = id;
			inventory[2 * write_slot + 1] = amounts[id];
			amounts[id] = 0;
			++write_slot;
		}
	}

	for (int id = 1; id < MAX_MAGIC_ID && write_slot < 32; ++id)
	{
		if (amounts[id])
		{
			inventory[2 * write_slot] = (uint8_t)id;
			inventory[2 * write_slot + 1] = amounts[id];
			++write_slot;
		}
	}

	return 1;
}

// SG_MAGIC_KNOWN_DRAWN_ONCE is a 64-bit (2-dword) savemap bitfield indexed by
// (id-1)/32 in 5 places (1 read + 4 writes). ParseBattleParty does exactly this
// for every held party spell on battle start; for a held id >= 65 the index is
// >= 2 and it writes past the field into adjacent savemap memory (the enemy
// scanned-once bitfield) -> corruption that manifests as a later NULL-pointer
// crash in battle stage setup.
//
// Since magic ids 64-95 are GF-reserved (never magic), the first magic id that
// overflows the native field is 96. So we only touch the drawn-once field when
// the kernel actually declares a magic id >= 96 (ff8_magic_count > 96). When it
// does, we relocate the whole field, for ALL ids, to a 256-bit store in
// verified-free savemap space (field-script variable 753) via the same blind
// value-scan used for K_MAGIC, so every (id-1)/32 access up to id 256 stays
// in bounds AND the state persists through a normal save (that region is
// inside the save CRC span).
//
// Why move ALL ids rather than split (vanilla ids native / extended ids new):
// a single access site computes base[(id-1)/32] from ONE base displacement, so
// routing low vs high ids to different bases would need a per-site conditional
// trampoline at all 5 sites (each with different registers) - fragile, for only
// a cosmetic gain. A stock (unmodded) game never reaches here (id < 96 => no
// relocation => drawn-once stays byte-for-byte vanilla), which is the
// compatibility guarantee that actually matters. In a modded game the new
// location is invisible in play and held spells are re-marked drawn every
// battle by ParseBattleParty, so nothing is lost by relocating vanilla ids too.
static void relocate_drawn_once_bitfield()
{
	// Only needed when an extended magic id (>= EXTENDED_MAGIC_FIRST) exists:
	// ids 64..95 are GF-reserved, so id 96 is the first magic whose drawn-once
	// bit (95) overflows the native 64-bit field. Otherwise leave the vanilla
	// drawn-once bitfield exactly where it is, natively persisted.
	if (ff8_magic_count <= EXTENDED_MAGIC_FIRST)
	{
		ffnx_trace("AddMoreMagic: no magic id >= %d, drawn-once left at vanilla 0x%08X.\n",
			EXTENDED_MAGIC_FIRST, ff8_externals.magic_sg_drawn_once);
		return;
	}

	// Field-script variable 753, inside the already-resolved variable block
	// (field_vars_stack_1CFE9B8, used elsewhere for savemap script vars) -
	// this is not a magic_* field since it is entirely derived, not researched
	// per language.
	uint32_t sg_drawn_once_ext = ff8_externals.field_vars_stack_1CFE9B8 + 753;

	// Exactly 5 genuine sites are known to reference this dword (1 reader +
	// 4 writers); the blind scan finds them with zero false positives (the
	// value is too specific to alias a branch displacement in practice).
	uint32_t patched = relocate_scan(ff8_externals.magic_sg_drawn_once, sg_drawn_once_ext,
		ff8_externals.magic_sg_drawn_once + 1, "drawn-once bitfield");
	if (patched != 5)
		ffnx_warning("AddMoreMagic: expected 5 drawn-once sites, found %u - some drawn-once state may not persist correctly!\n", patched);
}

// Per-character held-magic + junction validation (replaces sub_4BE790, called
// for every character on menu open). The vanilla function builds a "spells
// this character holds" bitfield in a 64-bit STACK buffer indexed by id/32
// (`held[id/32] |= 1 << (id&31)`); for a held id >= 64, id/32 >= 2 writes past
// the 2-dword buffer straight onto the saved registers / return address ->
// stack smash. It ALSO marks a per-character 2-dword global (valid-junction
// bitfield) indexed the same way, which overflows into the next character's
// slot for id >= 64. This replacement uses a 256-bit local bitfield and
// clamps the global write to the vanilla 2-dword range (extended junctions
// stay junctioned but aren't mirrored into the global - purely a
// junction-menu cosmetic detail, never a crash).
static int __cdecl ff8_char_validate_magic(int char_idx)
{
	uint32_t *valid_junction = (uint32_t *)(ff8_externals.magic_valid_junction + 8 * char_idx);
	valid_junction[0] = 0;
	valid_junction[1] = 0;

	uint8_t *chr = (uint8_t *)(ff8_externals.magic_sg_chara_data + CHAR_STRIDE * char_idx);
	uint8_t *magic = chr + CHAR_MAGIC_OFF; // 32 x {id, amount}

	uint32_t held[8] = { 0 }; // 256 bits: any byte id (vanilla stack buffer was only 64 bits)
	for (int i = 0; i < 32; ++i)
	{
		uint8_t id = magic[2 * i], amount = magic[2 * i + 1];
		if (!id || !amount)
		{
			magic[2 * i] = 0; // vanilla clears a slot with a zero id or amount
			magic[2 * i + 1] = 0;
			continue;
		}
		held[id >> 5] |= 1u << (id & 31);
	}

	uint8_t *junction = chr + CHAR_JUNCTION_OFF; // 20 stat slots, each a junctioned magic id
	for (int i = 0; i < 20; ++i)
	{
		uint8_t jid = junction[i];
		if (!jid)
			continue;
		int word = jid >> 5;
		uint32_t bit = 1u << (jid & 31);
		if (held[word] & bit)
		{
			if (word < 2) // vanilla global is only 2 dwords/char; don't overflow it
				valid_junction[word] |= bit;
		}
		else
		{
			junction[i] = 0; // unjunction a spell the character no longer holds
		}
	}

	return 0;
}

// ---- patch application (once, on first grown-kernel load) ---------------
static void ff8_kernel_magic_arm()
{
	if (ff8_magic_armed) return;
	ff8_magic_armed = true;

	uint32_t k_magic_end = ff8_externals.magic_k_magic + VANILLA_MAGIC_COUNT * MAGIC_ENTRY_SIZE;
	uint32_t rewritten = relocate_scan(ff8_externals.magic_k_magic, (uint32_t)&ff8_magic_table[0][0], k_magic_end, "K_MAGIC table");
	// EN 1.2 has 71 genuine K_MAGIC displacement operands in this window; every
	// other supported version matched the same count during verification.
	if (rewritten < 69) ffnx_warning("AddMoreMagic: fewer displacement sites than expected, some magic reads may still use the vanilla table!\n");

	install_id_trampoline(ff8_externals.magic_site_name_getter, REG_EAX, "magic name getter");
	install_id_trampoline(ff8_externals.magic_site_desc_getter, REG_EAX, "magic desc getter");
	install_id_trampoline(ff8_externals.magic_site_spell_visibility, REG_EAX, "draw-list visibility");
	install_id_trampoline(ff8_externals.magic_site_draw_execute, REG_EBX, "draw execution");

	replace_function(ff8_externals.magic_fn_linked_stock, (void *)ff8_linked_stock_field_char_data);
	replace_function(ff8_externals.magic_fn_reorder_magic, (void *)ff8_menu_reorder_magic);
	replace_function(ff8_externals.magic_fn_validate_magic, (void *)ff8_char_validate_magic);

	// Relocate the 64-bit drawn-once savemap bitfield to a persisted 256-bit
	// store in free savemap space, but only when a magic id >= 96 is present
	// (otherwise the vanilla field is left untouched).
	relocate_drawn_once_bitfield();

	ffnx_info("AddMoreMagic: armed with %d magic entries (ids 57-63 free below GFs; extended magic %d-%d; ids 64-95 reserved for GFs; mmagic.bin must cover %d entries / %d bytes).\n",
		ff8_magic_count, EXTENDED_MAGIC_FIRST, ff8_magic_count - 1, ff8_magic_count, ff8_magic_count * 4);
}

// ---- kernel.bin load interception ---------------------------------------
// Replaces the "call LoadFileToBuffer(name, KERNEL_HEADER)" inside
// readFilesKernelNamedicIconSysfnt. Reads the file into our own buffer,
// passes a vanilla-layout image to the game, and keeps the full data.
static int __cdecl ff8_kernel_load_hook(const char *filename, char *dest)
{
	if (ff8_kernel_stash == nullptr)
		ff8_kernel_stash = (char *)driver_malloc(KERNEL_FILE_MAX);

	int size = ((load_file_to_buffer_t)ff8_externals.magic_load_file_to_buf)(filename, ff8_kernel_stash);

	const uint32_t *header = (const uint32_t *)ff8_kernel_stash;
	const uint32_t *offsets = header + 1;
	bool valid = size > (int)sizeof(uint32_t) * (KERNEL_SECTION_COUNT + 1) && header[0] == KERNEL_SECTION_COUNT;
	int entries = valid ? (int)((offsets[KERNEL_MAGIC_SECTION + 1] - offsets[KERNEL_MAGIC_SECTION]) / MAGIC_ENTRY_SIZE) : VANILLA_MAGIC_COUNT;

	if (!valid || entries <= VANILLA_MAGIC_COUNT || entries > MAX_MAGIC_ID)
	{
		// Vanilla (or unexpected) kernel.bin: behave exactly like the
		// original call. Nothing is armed, nothing else is patched.
		if (size > 0) memcpy(dest, ff8_kernel_stash, size);
		if (valid && entries != VANILLA_MAGIC_COUNT)
			ffnx_warning("AddMoreMagic: kernel.bin has %d magic entries (max %d), ignoring extension.\n", entries, MAX_MAGIC_ID);
		return size;
	}

	// Grown kernel.bin: build the vanilla-layout image the exe expects.
	uint32_t data_growth = (entries - VANILLA_MAGIC_COUNT) * MAGIC_ENTRY_SIZE;
	uint32_t *out_header = (uint32_t *)dest;

	out_header[0] = KERNEL_SECTION_COUNT;

	// Data sections (0..30) go to their vanilla offsets. Only the magic
	// section may grow; validate the others still have vanilla sizes.
	for (int i = 0; i < KERNEL_FIRST_TEXT_SEC; ++i)
	{
		uint32_t src = offsets[i];
		uint32_t dst = vanilla_data_offsets[i];
		uint32_t copy_size = vanilla_data_offsets[i + 1] - dst;
		uint32_t src_size = offsets[i + 1] - src;

		out_header[1 + i] = dst;

		if (i != KERNEL_MAGIC_SECTION && src_size != copy_size)
			ffnx_warning("AddMoreMagic: kernel.bin data section %d has non-vanilla size %u (expected %u) - only the magic section may grow; game will likely misbehave!\n", i, src_size, copy_size);

		memcpy(dest + dst, ff8_kernel_stash + src, copy_size);
	}

	// Text sections (31..55): point the header into the stash (the exe
	// resolves these offsets through the header at runtime, and "buffer +
	// offset" arithmetic wraps correctly to the stash). BUT also fill the
	// vanilla-sized text area in `dest` itself with real bytes (shifted back
	// from the stash to account for the magic-section growth) rather than
	// leaving it uninitialized: some code may read kernel text via a
	// hardcoded buffer-relative offset instead of through the header, the
	// same way data sections are hardcoded, and uninitialized memory there
	// caused a crash on first menu open.
	for (int i = KERNEL_FIRST_TEXT_SEC; i < KERNEL_SECTION_COUNT; ++i)
		out_header[1 + i] = (uint32_t)(ff8_kernel_stash + offsets[i]) - (uint32_t)dest;

	uint32_t text_dest_size = VANILLA_KERNEL_SIZE - vanilla_data_offsets[KERNEL_FIRST_TEXT_SEC];
	uint32_t text_src_size = (uint32_t)size - offsets[KERNEL_FIRST_TEXT_SEC];
	memcpy(dest + vanilla_data_offsets[KERNEL_FIRST_TEXT_SEC], ff8_kernel_stash + offsets[KERNEL_FIRST_TEXT_SEC],
		text_src_size < text_dest_size ? text_src_size : text_dest_size);

	// FFNx-side full magic table + text pointer.
	memcpy(ff8_magic_table, ff8_kernel_stash + offsets[KERNEL_MAGIC_SECTION], entries * MAGIC_ENTRY_SIZE);
	ff8_magic_count = entries;
	ff8_magic_text = ff8_kernel_stash + offsets[32]; // magic text section (unused directly; header serves it)

	ffnx_trace("AddMoreMagic: extended kernel.bin detected (%d magic entries, +%u bytes data growth).\n", entries, data_growth);

	ff8_kernel_magic_arm();

	return VANILLA_KERNEL_SIZE;
}

// ---- init ---------------------------------------------------------------
void ff8_kernel_magic_init()
{
	if (!ff8_externals.magic_kernel_read_call)
	{
		ffnx_trace("AddMoreMagic: unsupported game version, extension disabled.\n");
		return;
	}

	// Sanity: the call we are about to replace must be "E8 rel32" to
	// LoadFileToBuffer, and the classification sites must look right
	// (checked again at arm time before patching them).
	const uint8_t *call_site = (const uint8_t *)ff8_externals.magic_kernel_read_call;
	uint32_t call_target = ff8_externals.magic_kernel_read_call + 5 + *(const int32_t *)(call_site + 1);
	if (call_site[0] != 0xE8 || call_target != ff8_externals.magic_load_file_to_buf)
	{
		ffnx_warning("AddMoreMagic: kernel load call site mismatch (0x%02X -> 0x%X), extension disabled.\n", call_site[0], call_target);
		return;
	}

	replace_call(ff8_externals.magic_kernel_read_call, (void *)ff8_kernel_load_hook);
}
