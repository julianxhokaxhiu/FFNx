/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                    //
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
// AddMoreMagic - extended kernel.bin magic section (FF8_EN.exe US 1.2)
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
//     - addMagicToMagicKnown (writes the 64-bit savemap drawn-once
//       bitfield unguarded -> would corrupt savemap with extended ids).
//
// File contract for modders: kernel.bin's section 1 must contain entries
// for ids 0..N-1 *including* 16 dummy 60-byte rows for ids 64..79 when
// N > 64 (id == entry index everywhere). mmagic.bin (standalone menu
// file, 4B/spell, loaded with size-from-file) must be extended to cover
// the highest id; magsort.bin can stay vanilla (new spells sort last).
// Extended drawn-once state is session-only (not persisted in saves).
// -------------------------------------------------------------------------

// ---- FF8_EN.exe US 1.2 addresses ----------------------------------------
#define ADDR_KERNEL_BUFFER      0x1CF3E48u  // KERNEL_HEADER static buffer
#define ADDR_K_MAGIC            0x1CF4064u  // buffer + 540
#define ADDR_K_MAGIC_END        0x1CF4DC0u  // buffer + 540 + 57*60 (= K_GF_JUNCTIONABLE)
#define ADDR_LOAD_FILE_TO_BUF   0x52D400u   // int LoadFileToBuffer(const char*, char*)
#define ADDR_KERNEL_READ_CALL   0x47D336u   // call LoadFileToBuffer(name, KERNEL_HEADER)
#define SITE_NAME_GETTER        0x47E974u   // getMagicText:      cmp eax,40h / jge rel8
#define SITE_DESC_GETTER        0x47E9C4u   // desc getter:       cmp eax,40h / jge rel8
#define SITE_SPELL_VISIBILITY   0x48C7E3u   // draw-list vis:     cmp eax,40h / jge rel8
#define SITE_DRAW_EXECUTE       0x48D53Cu   // draw command:      cmp ebx,40h / jae rel32
#define ADDR_FN_LINKED_STOCK    0x48CAE0u   // linkedStockFieldCharData(int char, int id)
#define ADDR_FN_REORDER_MAGIC   0x4F0030u   // menu_reorder_magic(int char, int preset)
#define ADDR_FN_ADD_MAGIC_KNOWN 0x48B7A0u   // addMagicToMagicKnown(int id)
#define ADDR_FN_VALIDATE_MAGIC  0x4BE790u   // sub_4BE790(int char): per-char held-magic + junction validate
#define ADDR_VALID_JUNCTION     0x1D77154u  // uint32[2] per char (8 chars): valid-junction bitfield
#define CHAR_STRIDE             152         // FF8CharacterData record size
#define CHAR_MAGIC_OFF          16          // 32 x {id:u8, amount:u8}
#define CHAR_JUNCTION_OFF       92          // 20 stat slots, each = a junctioned magic id
#define ADDR_SG_DRAWN_ONCE      0x1CFE95Cu  // savemap 64-bit drawn-once bitfield
#define ADDR_SG_CHARA_DATA      0x1CFE0E8u  // CharacterData[], stride 152, Magic @+16
#define ADDR_F_CHAR_DATA        0x1CFF000u  // FF8FieldCharData[], stride 464
#define ADDR_K_BATTLE_COMMAND   0x1CF3F2Cu  // FF8KernelBattleCommand[], stride 8
#define ADDR_MAGSORT_BUFFER     0x1D2BB5Cu  // magsortData magsortbuffer[N][7], stride 64/preset (direct array, not a pointer)
#define SCAN_CODE_START         0x401000u
#define SCAN_CODE_END           0x500000u

#define VANILLA_KERNEL_SIZE     37992u
#define VANILLA_MAGIC_COUNT     57
#define MAGIC_ENTRY_SIZE        60
#define MAX_MAGIC_ID            256
#define GF_FIRST_ID             64
#define GF_LAST_ID              79
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
static uint32_t ff8_drawn_once_ext[8];       // 256-bit drawn-once store

// Per-site branch targets, filled from the original jcc displacements at
// patch time so they stay correct even if surrounding code shifts slightly.
static uint32_t tramp_gf_target_name;
static uint32_t tramp_gf_target_desc;
static uint32_t tramp_gf_target_vis;
static uint32_t tramp_gf_target_draw;

typedef int(__cdecl *load_file_to_buffer_t)(const char *, char *);

// ---- classification trampolines -----------------------------------------
// Each replaces "cmp reg,40h / jcc gf_path". On entry reg holds the spell
// id; jump to the stored GF target only for 64..79, otherwise return to the
// instruction after the patched site (the magic path). Flags are dead after
// the original jcc, so clobbering them is safe.
#define DEFINE_ID_TRAMPOLINE(fn_name, reg, target_var)  \
	static __declspec(naked) void fn_name()             \
	{                                                   \
		__asm { cmp reg, GF_FIRST_ID }                  \
		__asm { jb magic_path }                         \
		__asm { cmp reg, GF_LAST_ID }                   \
		__asm { ja magic_path }                         \
		__asm { add esp, 4 }                            \
		__asm { jmp [target_var] }                      \
		__asm { magic_path: ret }                       \
	}

DEFINE_ID_TRAMPOLINE(tramp_name_getter, eax, tramp_gf_target_name)
DEFINE_ID_TRAMPOLINE(tramp_desc_getter, eax, tramp_gf_target_desc)
DEFINE_ID_TRAMPOLINE(tramp_spell_visibility, eax, tramp_gf_target_vis)
DEFINE_ID_TRAMPOLINE(tramp_draw_execute, ebx, tramp_gf_target_draw)

// Verify the expected "cmp reg,40h" + jcc encoding, save the jcc target,
// then overwrite the whole compare-and-branch with "call trampoline" (+NOPs).
static bool install_id_trampoline(uint32_t site, void *trampoline, uint32_t *gf_target_out, const char *what)
{
	const uint8_t *p = (const uint8_t *)site;

	if (p[0] != 0x83 || p[2] != 0x40)
	{
		ffnx_warning("AddMoreMagic: unexpected bytes at %s site 0x%X (%02X %02X %02X), skipping patch!\n", what, site, p[0], p[1], p[2]);
		return false;
	}

	uint32_t patch_size;
	if ((p[3] & 0xF0) == 0x70) // jcc rel8
	{
		*gf_target_out = site + 5 + (int8_t)p[4];
		patch_size = 5;
	}
	else if (p[3] == 0x0F && (p[4] & 0xF0) == 0x80) // jcc rel32
	{
		*gf_target_out = site + 9 + *(int32_t *)(p + 5);
		patch_size = 9;
	}
	else
	{
		ffnx_warning("AddMoreMagic: unexpected jcc at %s site 0x%X (%02X), skipping patch!\n", what, site, p[3]);
		return false;
	}

	uint8_t code[9] = { 0xE8, 0, 0, 0, 0, 0x90, 0x90, 0x90, 0x90 };
	*(uint32_t *)&code[1] = (uint32_t)trampoline - (site + 5);
	memcpy_code(site, code, patch_size);

	return true;
}

// ---- K_MAGIC displacement relocation ------------------------------------
// NOTE: reverted the lead-in-byte validation experiment - it rejected all 72
// genuine sites (the real MSVC encoding here doesn't match the assumed
// ModRM/mov-imm32/push-imm32 forms), and disabling relocation entirely did
// NOT stop the menu-open crash, proving this scan was not its cause. Back to
// the plain blind scan-and-patch until the real bug is found.
static void relocate_magic_displacements()
{
	uint32_t rewritten = 0;

	for (uint32_t addr = SCAN_CODE_START; addr < SCAN_CODE_END - 4; ++addr)
	{
		uint32_t value = *(uint32_t *)addr;

		if (value >= ADDR_K_MAGIC && value < ADDR_K_MAGIC_END)
		{
			patch_code_dword(addr, (DWORD)((uint32_t)&ff8_magic_table[0][0] + (value - ADDR_K_MAGIC)));
			++rewritten;
			addr += 3; // skip the rewritten dword
		}
	}

	// EN 1.2 has 72 genuine K_MAGIC displacement operands in this window.
	ffnx_info("AddMoreMagic: relocated %u K_MAGIC displacements (expected ~72).\n", rewritten);
	if (rewritten < 70) ffnx_warning("AddMoreMagic: fewer displacement sites than expected, some magic reads may still use the vanilla table!\n");
}

// ---- replaced functions -------------------------------------------------
// Draw->Stock action setup (replaces linkedStockFieldCharData @ 0x48CAE0).
// Vanilla routed every id >= 64 to the GF branch; only 64..79 belong there.
static void *__cdecl ff8_linked_stock_field_char_data(int char_slot, int spell_id)
{
	uint8_t *chr = (uint8_t *)(ADDR_F_CHAR_DATA + 464 * char_slot);
	const uint8_t *draw_cmd = (const uint8_t *)(ADDR_K_BATTLE_COMMAND + 8 * 10);

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

// Magic menu sort (replaces menu_reorder_magic @ 0x4F0030). The vanilla
// function fills a 64-byte stack array indexed by spell id; extended ids
// would corrupt the stack. Ids absent from the sort preset (all extended
// ones with a vanilla magsort.bin) are appended in ascending id order
// instead of being silently dropped.
static int __cdecl ff8_menu_reorder_magic(int character_id, int sort_preset)
{
	// magsortbuffer is an array of pointers (MenuReadFiles passes it as
	// Menu_GetFile((void**)magsortbuffer, ...) - no '&' - so magsortbuffer[0]
	// itself holds the loaded-file pointer, confirmed by decompiling
	// MenuReadFiles @ 0x4A1C31). Must dereference before indexing by preset.
	const uint8_t *preset = (const uint8_t *)(*(uintptr_t *)ADDR_MAGSORT_BUFFER) + 64 * sort_preset;
	if (!preset[0]) return 0;

	uint8_t amounts[MAX_MAGIC_ID];
	memset(amounts, 0, sizeof(amounts));

	uint8_t *inventory = (uint8_t *)(ADDR_SG_CHARA_DATA + 152 * character_id + 16); // 32 x {id, amount}
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
// >= 2 and it writes past the field into adjacent savemap memory -> corruption
// that manifests as a later NULL-pointer crash in battle stage setup (0x500FB4).
// Rather than patch each site's bounds, relocate the whole field to a 256-bit
// FFNx buffer by rewriting the base displacement (0x1CFE95C) at all 5 sites, so
// every (id-1)/32 access up to id 256 stays in bounds. Drawn-once state becomes
// session-only (not persisted in the save) - a cosmetic "new spell" detail.
static const uint32_t drawn_once_sites[] = {
	0x48B789u, // reader:  and esi, [eax*4 + field]
	0x48B7CAu, // addMagicToMagicKnown writer (fixes the vanilla OOB too)
	0x48B84Eu, // ParseBattleParty writer (battle start)
	0x48B93Bu, // battleToFieldTransition writer
	0x48C819u, // manageMonsterSpellVisibility
};

static void relocate_drawn_once_bitfield()
{
	uint32_t patched = 0;
	for (uint32_t site : drawn_once_sites)
	{
		for (uint32_t off = 0; off < 12; ++off)
		{
			if (*(uint32_t *)(site + off) == ADDR_SG_DRAWN_ONCE)
			{
				patch_code_dword(site + off, (DWORD)(uintptr_t)&ff8_drawn_once_ext[0]);
				++patched;
				break;
			}
		}
	}
	ffnx_info("AddMoreMagic: relocated %u/%u drawn-once bitfield sites to a 256-bit buffer.\n",
		patched, (uint32_t)(sizeof(drawn_once_sites) / sizeof(drawn_once_sites[0])));
}

// Per-character held-magic + junction validation (replaces sub_4BE790 @ 0x4BE790,
// called for every character on menu open). The vanilla function builds a
// "spells this character holds" bitfield in a 64-bit STACK buffer indexed by
// id/32 (`held[id/32] |= 1 << (id&31)`); for a held id >= 64, id/32 >= 2 writes
// past the 2-dword buffer straight onto the saved registers / return address ->
// stack smash -> the 0x4D0C1A crash. It ALSO marks a per-character 2-dword
// global (valid-junction bitfield) indexed the same way, which overflows into
// the next character's slot for id >= 64. This replacement uses a 256-bit local
// bitfield and clamps the global write to the vanilla 2-dword range (extended
// junctions stay junctioned but aren't mirrored into the global - purely a
// junction-menu cosmetic detail, never a crash).
static int __cdecl ff8_char_validate_magic(int char_idx)
{
	uint32_t *valid_junction = (uint32_t *)(ADDR_VALID_JUNCTION + 8 * char_idx);
	valid_junction[0] = 0;
	valid_junction[1] = 0;

	uint8_t *chr = (uint8_t *)(ADDR_SG_CHARA_DATA + CHAR_STRIDE * char_idx);
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

	relocate_magic_displacements();

	install_id_trampoline(SITE_NAME_GETTER, tramp_name_getter, &tramp_gf_target_name, "magic name getter");
	install_id_trampoline(SITE_DESC_GETTER, tramp_desc_getter, &tramp_gf_target_desc, "magic desc getter");
	install_id_trampoline(SITE_SPELL_VISIBILITY, tramp_spell_visibility, &tramp_gf_target_vis, "draw-list visibility");
	install_id_trampoline(SITE_DRAW_EXECUTE, tramp_draw_execute, &tramp_gf_target_draw, "draw execution");

	replace_function(ADDR_FN_LINKED_STOCK, (void *)ff8_linked_stock_field_char_data);
	replace_function(ADDR_FN_REORDER_MAGIC, (void *)ff8_menu_reorder_magic);
	replace_function(ADDR_FN_VALIDATE_MAGIC, (void *)ff8_char_validate_magic);

	// Relocate the 64-bit drawn-once savemap bitfield to a 256-bit buffer so
	// held/junctioned extended ids (>=65) don't overflow it (this replaces the
	// old addMagicToMagicKnown replace_function - patching all 5 access sites
	// fixes the battle-start writers too, which that replacement never covered).
	relocate_drawn_once_bitfield();

	ffnx_info("AddMoreMagic: armed with %d magic entries (ids 57-63 and 80-%d available).\n",
		ff8_magic_count, ff8_magic_count - 1);
	ffnx_info("AddMoreMagic: make sure mmagic.bin covers %d entries (%d bytes).\n",
		ff8_magic_count, ff8_magic_count * 4);
}

// ---- kernel.bin load interception ---------------------------------------
// Replaces the "call LoadFileToBuffer(name, KERNEL_HEADER)" inside
// readFilesKernelNamedicIconSysfnt. Reads the file into our own buffer,
// passes a vanilla-layout image to the game, and keeps the full data.
static int __cdecl ff8_kernel_load_hook(const char *filename, char *dest)
{
	if (ff8_kernel_stash == nullptr)
		ff8_kernel_stash = (char *)driver_malloc(KERNEL_FILE_MAX);

	int size = ((load_file_to_buffer_t)ADDR_LOAD_FILE_TO_BUF)(filename, ff8_kernel_stash);

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

	ffnx_info("AddMoreMagic: extended kernel.bin detected (%d magic entries, +%u bytes data growth).\n", entries, data_growth);

	ff8_kernel_magic_arm();

	return VANILLA_KERNEL_SIZE;
}

// ---- init ---------------------------------------------------------------
void ff8_kernel_magic_init()
{
	// Only the US 1.2 exe is mapped (all addresses above). Other languages
	// need their own table - see the AllMonsterFilesUsable precedent.
	if (!FF8_US_VERSION)
	{
		ffnx_info("AddMoreMagic: unsupported game version, extension disabled.\n");
		return;
	}

	// Sanity: the call we are about to replace must be "E8 rel32" to
	// LoadFileToBuffer, and the classification sites must look right
	// (checked again at arm time before patching them).
	const uint8_t *call_site = (const uint8_t *)ADDR_KERNEL_READ_CALL;
	uint32_t call_target = ADDR_KERNEL_READ_CALL + 5 + *(const int32_t *)(call_site + 1);
	if (call_site[0] != 0xE8 || call_target != ADDR_LOAD_FILE_TO_BUF)
	{
		ffnx_warning("AddMoreMagic: kernel load call site mismatch (0x%02X -> 0x%X), extension disabled.\n", call_site[0], call_target);
		return;
	}

	replace_call(ADDR_KERNEL_READ_CALL, (void *)ff8_kernel_load_hook);
}
