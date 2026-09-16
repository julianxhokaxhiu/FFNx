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
#include "../utils.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

// -------------------------------------------------------------------------
// AddMoreMagic - lets kernel.bin hold more than the vanilla 57 magic spells.
//
// The exe caps magic at 57 entries and reads any id >= 64 as a GF. On a grown
// kernel.bin we hand the game a vanilla-layout image, keep the full magic
// table FFNx-side, repoint the exe's reads to it, and replace the functions
// that would misread an extended id. A stock kernel.bin is left untouched.
//
// Modder contract: kernel.bin section 1 must list ids 0..N-1, including 32
// dummy rows for the GF-reserved ids 64..95, once N > 64. mmagic.bin must
// cover the highest id; magsort.bin can stay vanilla.
// -------------------------------------------------------------------------

#define CHAR_STRIDE             152         // FF8CharacterData record size
#define CHAR_MAGIC_OFF          16          // 32 x {id:u8, amount:u8}
#define CHAR_JUNCTION_OFF       92          // 20 stat slots, each = a junctioned magic id
#define VANILLA_KERNEL_SIZE     37992u
#define VANILLA_MAGIC_COUNT     57
#define MAGIC_ENTRY_SIZE        60
#define MAX_MAGIC_ID            256
// How many code sites each value-scan expects to rewrite. Same on every build.
#define K_MAGIC_SITE_COUNT      71
#define DRAWN_ONCE_SITE_COUNT   5
#define GF_FIRST_ID             64
#define GF_LAST_ID              79          // the exe's 16 real GF ids (64..79)
// Ids 64..95 are reserved for GFs, so mod-added magic starts at 96.
#define GF_RESERVED_COUNT       32
#define EXTENDED_MAGIC_FIRST    (GF_FIRST_ID + GF_RESERVED_COUNT)   // 96
#define KERNEL_SECTION_COUNT    56
#define KERNEL_MAGIC_SECTION    1           // data section that may grow
#define KERNEL_GF_SECTION       2           // GF data section (K_GF)
#define KERNEL_FIRST_TEXT_SEC   31          // sections 31..55 resolved via header
#define KERNEL_FILE_MAX         (1024 * 1024)

// kernel.bin header: uint32 section count, then one uint32 offset per section.
#define KERNEL_SECTION_OFFSET(buffer, i) (((const uint32_t *)(buffer))[1 + (i)])
#define KERNEL_TEXT_MAGIC_SEC   32          // magic names and descriptions
#define KERNEL_TEXT_GF_DESC_SEC 33          // GF descriptions
#define KERNEL_NO_TEXT          0xFFFF      // entry has no string
// Field offsets inside a 60-byte magic entry, and inside a K_GF record.
#define MAGIC_NAME_OFF          0
#define MAGIC_DESC_OFF          2
#define K_GF_STRIDE             132
#define K_GF_DESC_OFF           2
// Savemap GF record stride; the GF's name sits at offset 0 of the record.
#define GF_DATA_STRIDE          68

// Vanilla data-section offsets (sections 0..31; index 31 = first text
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
static uint32_t ff8_drawn_once = 0;          // drawn-once bitfield, vanilla address or the relocated one

// ---- magic-vs-GF classification -----------------------------------------
// Only 64..79 are real GFs. The exe's "id >= 64" shortcut is what an extended
// magic id trips over.
static int __cdecl ff8_is_gf_id(int id)
{
	id &= 0xFFFF; // battle code passes a 16-bit id
	return (id >= GF_FIRST_ID && id <= GF_LAST_ID) ? 1 : 0;
}

// ---- name / description getters (replace_function) ----------------------
// Resolve a kernel string: buffer + text-section header offset + entry offset,
// or a shared empty string when the entry has none.
static char *ff8_kernel_text(int section, uint16_t text_offset)
{
	const uint8_t *buffer = (const uint8_t *)ff8_externals.unk_1CF3E48;

	if (text_offset == KERNEL_NO_TEXT)
		return ff8_externals.unk_1CFF84C;

	return (char *)(buffer + KERNEL_SECTION_OFFSET(buffer, section) + text_offset);
}

// Magic entry text offsets: name at +0, description at +2 (both uint16).
static uint16_t ff8_magic_text_offset(int id, int field_offset)
{
	if (id < 0 || id >= ff8_magic_count)
		return KERNEL_NO_TEXT;

	return *(const uint16_t *)&ff8_magic_table[id][field_offset];
}

// Magic/GF name. Magic reads the FFNx-side table; only 64..79 are GFs.
static char *__cdecl ff8_get_magic_name(int id)
{
	if (ff8_is_gf_id(id))
		return (char *)(ff8_externals.magic_sg_gf_data + GF_DATA_STRIDE * (id - GF_FIRST_ID));

	return ff8_kernel_text(KERNEL_TEXT_MAGIC_SEC, ff8_magic_text_offset(id, MAGIC_NAME_OFF));
}

// Magic/GF description. Same split; GF text lives in kernel.bin's GF section.
static char *__cdecl ff8_get_magic_description(int id)
{
	if (ff8_is_gf_id(id))
	{
		const uint8_t *k_gf = (const uint8_t *)ff8_externals.unk_1CF3E48 + vanilla_data_offsets[KERNEL_GF_SECTION];
		return ff8_kernel_text(KERNEL_TEXT_GF_DESC_SEC, *(const uint16_t *)(k_gf + K_GF_STRIDE * (id - GF_FIRST_ID) + K_GF_DESC_OFF));
	}

	return ff8_kernel_text(KERNEL_TEXT_MAGIC_SEC, ff8_magic_text_offset(id, MAGIC_DESC_OFF));
}

// ---- value-scan relocation (shared by K_MAGIC and drawn-once) -----------
// All this section will scan the code to find a specific value which reference the kernel table, and replace it with the new table address.
// This is needed because the exe has hardcoded addresses for the kernel tables, and we need to redirect them to our new tables in FFNx.

// Repoints every dword in the exe code section [scan_start, scan_end) that
// falls in [from, range_end) by the same delta. A call/jmp rel32 whose bytes
// happen to land in range is skipped - its opcode is the low byte and it
// targets real code, which a data operand never does.
static uint32_t relocate_scan(uint32_t scan_start, uint32_t scan_end, uint32_t from, uint32_t to, uint32_t range_end, const char *what)
{
	uint32_t rewritten = 0, skipped_branch = 0;

	for (uint32_t addr = scan_start; addr < scan_end - 4; ++addr)
	{
		uint32_t value = *(uint32_t *)addr;

		if (value >= from && value < range_end)
		{
			uint8_t op = *(uint8_t *)addr;
			if (op == 0xE8 || op == 0xE9) // call/jmp rel32?
			{
				uint32_t target = addr + 5 + *(int32_t *)(addr + 1);
				if (target >= scan_start && target < scan_end)
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

	if (trace_all) ffnx_trace("AddMoreMagic: %s: relocated %u displacement(s), skipped %u branch false-positive(s).\n", what, rewritten, skipped_branch);
	return rewritten;
}

// ---- replaced functions -------------------------------------------------
// Draw->Stock setup (replaces linkedStockFieldCharData); only 64..79 are GFs.
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

// Magic menu sort (replaces menu_reorder_magic). Vanilla indexes a 64-byte
// stack array by spell id, which extended ids overflow. Ids not in the sort
// preset are appended in id order rather than dropped.
static int __cdecl ff8_menu_reorder_magic(int character_id, int sort_preset)
{
	// magsortbuffer holds the loaded-file pointer directly, so dereference first.
	const uint8_t *preset = (const uint8_t *)(*(uintptr_t *)ff8_externals.magic_magsort_buffer) + 64 * sort_preset;
	if (!preset[0]) return 0;

	uint8_t amounts[MAX_MAGIC_ID];
	memset(amounts, 0, sizeof(amounts));

	uint8_t *inventory = (uint8_t *)(ff8_externals.magic_sg_chara_data + CHAR_STRIDE * character_id + CHAR_MAGIC_OFF); // 32 x {id, amount}
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

// The 64-bit "magic drawn once" savemap bitfield is indexed by (id-1)/32 with
// no bounds check, so a magic id >= 96 writes past it and corrupts the savemap.
// When such an id exists, move the field to free savemap space (field variable
// 753, verified unused and inside the save CRC span) big enough for all 256
// ids. Its DRAWN_ONCE_SITE_COUNT accessors are scattered and one has no anchor,
// so a value-scan repoints them all. A stock game never gets here.
static void relocate_drawn_once_bitfield(uint32_t code_start, uint32_t code_end)
{
	if (ff8_magic_count <= EXTENDED_MAGIC_FIRST)
	{
		if (trace_all) ffnx_trace("AddMoreMagic: no magic id >= %d, drawn-once left at vanilla 0x%08X.\n", EXTENDED_MAGIC_FIRST, ff8_externals.magic_sg_drawn_once);
		return;
	}

	uint32_t sg_drawn_once_ext = ff8_externals.field_vars_stack_1CFE9B8 + 753;
	ff8_drawn_once = sg_drawn_once_ext;

	uint32_t patched = relocate_scan(code_start, code_end, ff8_externals.magic_sg_drawn_once, sg_drawn_once_ext, ff8_externals.magic_sg_drawn_once + 1, "drawn-once bitfield");
	if (patched != DRAWN_ONCE_SITE_COUNT)
		ffnx_warning("AddMoreMagic: expected %d drawn-once sites, found %u - some drawn-once state may not persist correctly!\n", DRAWN_ONCE_SITE_COUNT, patched);
}

// Per-character held-magic + junction validation (replaces sub_4BE790).
// Vanilla builds the "held" bitfield in a 64-bit stack buffer indexed by
// id/32, which a held id >= 64 smashes. Uses a 256-bit local instead, and
// clamps the 2-dword valid-junction global write to its vanilla range.
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

// ---- draw-list spell visibility (replaces manageMonsterSpellVisibility) --
// Copies each monster's 4 draw-list spells into the draw menu and flags the
// ones never drawn before. Replaced whole because vanilla reads any id >= 64
// as a GF.
#define BATTLE_SLOT_STRIDE        208   // FF8BattleSlotData
#define BATTLE_FIRST_MONSTER_SLOT 3     // slots 0..2 are the party
#define BATTLE_MONSTER_COUNT      4     // slots 3..6
#define BATTLE_SLOT_FLAGS_OFF     0x7C
#define BATTLE_SLOT_FLAG_ENABLED  1
#define MONSTER_INFO_DRAW_OFF     0x104 // draw list: 3 level tiers x 4 entries of {id, amount}
#define MONSTER_DRAW_STRIDE       71    // one monster's draw menu record
#define MONSTER_DRAW_LEVEL_OFF    0x46  // level tier (0..2), picks the draw list row
#define MONSTER_DRAW_SLOT_COUNT   4
#define MONSTER_DRAW_SLOT_SIZE    4     // {id, flags, unused, unused}
#define DRAW_SLOT_NEVER_DRAWN     8     // flag bit: spell not in the drawn-once bitfield yet
#define GF_DATA_EXISTS_OFF        0x11  // "GF already obtained" byte of a savemap GF record
static void *__cdecl ff8_manage_monster_spell_visibility()
{
	const uint32_t *drawn_once = (const uint32_t *)ff8_drawn_once;
	uint8_t *slot = (uint8_t *)ff8_externals.magic_battle_first_monster_slot;
	uint8_t *monster = (uint8_t *)ff8_externals.magic_monster_draw_data;
	// Vanilla keeps this across slots and reads it back on an empty draw slot.
	int already_drawn = 0;

	for (int i = 0; i < BATTLE_MONSTER_COUNT; ++i, slot += BATTLE_SLOT_STRIDE, monster += MONSTER_DRAW_STRIDE)
	{
		if (!(slot[BATTLE_SLOT_FLAGS_OFF] & BATTLE_SLOT_FLAG_ENABLED))
			continue;

		const uint8_t *monster_info = *(const uint8_t **)*(const uint8_t **)slot;
		const uint8_t *draw_list = monster_info + MONSTER_INFO_DRAW_OFF + 2 * MONSTER_DRAW_SLOT_COUNT * monster[MONSTER_DRAW_LEVEL_OFF];

		for (int draw_slot = 0; draw_slot < MONSTER_DRAW_SLOT_COUNT; ++draw_slot)
		{
			uint8_t *menu_entry = monster + MONSTER_DRAW_SLOT_SIZE * draw_slot; // {id, flags, ...}
			uint8_t id = draw_list[2 * draw_slot];

			menu_entry[2] = 0;

			if (ff8_is_gf_id(id))
			{
				const uint8_t *gf = (const uint8_t *)(ff8_externals.magic_sg_gf_data + GF_DATA_STRIDE * (id - GF_FIRST_ID));
				menu_entry[0] = gf[GF_DATA_EXISTS_OFF] ? 0 : id; // a GF you already own is not drawable
				continue;
			}

			menu_entry[0] = id;
			if (id)
				already_drawn = (drawn_once[(id - 1) / 32] >> ((id - 1) % 32)) & 1;

			if (already_drawn)
				menu_entry[1] &= ~DRAW_SLOT_NEVER_DRAWN;
			else
				menu_entry[1] |= DRAW_SLOT_NEVER_DRAWN;
		}
	}

	return slot; // vanilla returns the slot it stopped on; no caller uses it
}

// ---- draw command: stocking an extended magic ---------------------------
// The exe only stocks a drawn spell whose id is < 64; above that it takes the
// GF path. Its battle message is built differently in every language, so the
// original function keeps doing the whole draw: the spell is handed to it
// under a free vanilla id whose magic entry points at the real spell, and
// everything the call wrote with that id is renamed back afterwards.
#define COMMAND_DRAW              6
#define DRAW_VARIANT_STOCK        10    // 9 is draw-and-cast, which has no GF check
#define F_CHAR_DATA_STRIDE        464
#define BATTLE_MAGIC_OFF          130   // 32 x {id, amount, ...} in FF8FieldCharData
#define BATTLE_MAGIC_STRIDE       5
#define BATTLE_MAGIC_SLOTS        32
// Battle state globals, as offsets from the battle slot array. The data
// section has the same layout on every retail build, only its base moves.
#define BATTLE_ABILITY_ID         (-0x1C)
#define BATTLE_SEQUENCE_COUNTER   0x5B0
#define BATTLE_TASK_DATA          0x5B4
#define BATTLE_TASK_STRIDE        20
#define BATTLE_TASK_ABILITY_OFF   4

typedef int(__cdecl *compute_command_action_t)(int, int, int, int, int, int, int);

static uint32_t ff8_compute_command_action_replaced = 0;

static uint8_t *ff8_battle_state(int offset)
{
	return (uint8_t *)(ff8_externals.magic_battle_slot_data + offset);
}

// A free id below GF_FIRST_ID - above it the game would read a GF again - that
// the caster does not hold and the monster does not offer. Counts down, so it
// takes the ids above the vanilla 57 spells first.
static int ff8_stand_in_magic_id(const uint8_t *inventory, const uint8_t *monster)
{
	for (int id = GF_FIRST_ID - 1; id > 0; --id)
	{
		bool used = false;

		for (int i = 0; i < BATTLE_MAGIC_SLOTS; ++i)
			if (inventory[BATTLE_MAGIC_STRIDE * i] == id)
				used = true;

		for (int i = 0; i < MONSTER_DRAW_SLOT_COUNT; ++i)
			if (monster[MONSTER_DRAW_SLOT_SIZE * i] == id)
				used = true;

		if (!used)
			return id;
	}

	return 0;
}

static int ff8_call_command_action(int attacker_slot, int command, int id, int variant, int target_slot, int target_mask, int linked)
{
	unreplace_function(ff8_compute_command_action_replaced);
	int ret = ((compute_command_action_t)ff8_externals.battle_sub_48D200)(attacker_slot, command, id, variant, target_slot, target_mask, linked);
	rereplace_function(ff8_compute_command_action_replaced);

	return ret;
}

static int __cdecl ff8_compute_command_action(int attacker_slot, int command, int id, int variant, int target_slot, int target_mask, int linked)
{
	uint16_t spell_id = (uint16_t)id;

	// Everything vanilla still gets right goes straight to the original.
	if (command != COMMAND_DRAW || variant != DRAW_VARIANT_STOCK || spell_id < GF_FIRST_ID || ff8_is_gf_id(spell_id) || spell_id >= MAX_MAGIC_ID)
		return ff8_call_command_action(attacker_slot, command, id, variant, target_slot, target_mask, linked);

	uint8_t *inventory = (uint8_t *)(ff8_externals.magic_f_char_data + F_CHAR_DATA_STRIDE * attacker_slot + BATTLE_MAGIC_OFF);
	uint8_t *monster = (uint8_t *)ff8_externals.magic_monster_draw_data + MONSTER_DRAW_STRIDE * (target_slot - BATTLE_FIRST_MONSTER_SLOT);

	int stand_in = ff8_stand_in_magic_id(inventory, monster);
	if (!stand_in)
	{
		if (trace_all) ffnx_trace("AddMoreMagic: no free magic id to stand in for %d, drawing it as vanilla would.\n", spell_id);
		return ff8_call_command_action(attacker_slot, command, id, variant, target_slot, target_mask, linked);
	}

	// The entry carries the name and the draw resistance, so the call behaves
	// exactly as it would for a vanilla spell.
	uint8_t saved_entry[MAGIC_ENTRY_SIZE];
	memcpy(saved_entry, ff8_magic_table[stand_in], MAGIC_ENTRY_SIZE);
	memcpy(ff8_magic_table[stand_in], ff8_magic_table[spell_id], MAGIC_ENTRY_SIZE);

	// Rename it where the call looks it up, so a spell the caster already owns
	// keeps stacking on its own slot.
	int draw_slot = 0;
	while (draw_slot < MONSTER_DRAW_SLOT_COUNT && monster[MONSTER_DRAW_SLOT_SIZE * draw_slot] != (uint8_t)spell_id)
		++draw_slot;
	if (draw_slot < MONSTER_DRAW_SLOT_COUNT)
		monster[MONSTER_DRAW_SLOT_SIZE * draw_slot] = (uint8_t)stand_in;

	for (int i = 0; i < BATTLE_MAGIC_SLOTS; ++i)
		if (inventory[BATTLE_MAGIC_STRIDE * i] == (uint8_t)spell_id)
			inventory[BATTLE_MAGIC_STRIDE * i] = (uint8_t)stand_in;

	int ret = ff8_call_command_action(attacker_slot, command, stand_in, variant, target_slot, target_mask, linked);

	// Put the real spell back everywhere the call left the stand-in.
	for (int i = 0; i < BATTLE_MAGIC_SLOTS; ++i)
		if (inventory[BATTLE_MAGIC_STRIDE * i] == (uint8_t)stand_in)
			inventory[BATTLE_MAGIC_STRIDE * i] = (uint8_t)spell_id;

	if (draw_slot < MONSTER_DRAW_SLOT_COUNT)
		monster[MONSTER_DRAW_SLOT_SIZE * draw_slot] = (uint8_t)spell_id;

	memcpy(ff8_magic_table[stand_in], saved_entry, MAGIC_ENTRY_SIZE);

	// The queued action carries the ability id the animation will play.
	uint8_t *task = ff8_battle_state(BATTLE_TASK_DATA) + BATTLE_TASK_STRIDE * *ff8_battle_state(BATTLE_SEQUENCE_COUNTER);
	if (*(uint16_t *)(task + BATTLE_TASK_ABILITY_OFF) == stand_in)
		*(uint16_t *)(task + BATTLE_TASK_ABILITY_OFF) = spell_id;
	if (*(uint16_t *)ff8_battle_state(BATTLE_ABILITY_ID) == stand_in)
		*(uint16_t *)ff8_battle_state(BATTLE_ABILITY_ID) = spell_id;

	return ret;
}

// ---- patch application (once, on first grown-kernel load) ---------------
static void ff8_kernel_magic_arm()
{
	if (ff8_magic_armed) return;
	ff8_magic_armed = true;

	uint32_t code_start, code_end;
	getProcessCodeSection(&code_start, &code_end);

	// The exe bakes K_MAGIC_SITE_COUNT operands pointing into the magic table,
	// too many and too scattered to resolve individually; repoint them all to
	// the FFNx-side table with a value-scan.
	uint32_t k_magic_end = ff8_externals.magic_k_magic + VANILLA_MAGIC_COUNT * MAGIC_ENTRY_SIZE;
	uint32_t rewritten = relocate_scan(code_start, code_end, ff8_externals.magic_k_magic, (uint32_t)&ff8_magic_table[0][0], k_magic_end, "K_MAGIC table");
	if (rewritten != K_MAGIC_SITE_COUNT)
		ffnx_warning("AddMoreMagic: expected %d K_MAGIC sites, rewrote %u - some magic reads may still use the vanilla table!\n", K_MAGIC_SITE_COUNT, rewritten);

	ff8_drawn_once = ff8_externals.magic_sg_drawn_once;

	replace_function(ff8_externals.manage_monster_spell_visibility_sub_48C7A0, (void *)ff8_manage_monster_spell_visibility);
	ff8_compute_command_action_replaced = replace_function(ff8_externals.battle_sub_48D200, (void *)ff8_compute_command_action);
	replace_function(ff8_externals.magic_fn_name_getter, (void *)ff8_get_magic_name);
	replace_function(ff8_externals.magic_fn_desc_getter, (void *)ff8_get_magic_description);
	replace_function(ff8_externals.magic_fn_linked_stock, (void *)ff8_linked_stock_field_char_data);
	replace_function(ff8_externals.magic_fn_reorder_magic, (void *)ff8_menu_reorder_magic);
	replace_function(ff8_externals.magic_fn_validate_magic, (void *)ff8_char_validate_magic);

	relocate_drawn_once_bitfield(code_start, code_end);

	ffnx_info("AddMoreMagic: armed with %d magic entries (ids 57-63 free below GFs; extended magic %d-%d; ids 64-95 reserved for GFs; mmagic.bin must cover %d entries / %d bytes).\n", ff8_magic_count, EXTENDED_MAGIC_FIRST, ff8_magic_count - 1, ff8_magic_count, ff8_magic_count * 4);
}

// ---- kernel.bin load interception ---------------------------------------
// Replaces the kernel.bin load: reads the file into our stash, hands the game
// a vanilla-layout image, and arms the extension if the file is grown.
static int __cdecl ff8_kernel_load_hook(const char *filename, char *dest)
{
	if (ff8_kernel_stash == nullptr)
		ff8_kernel_stash = (char *)driver_malloc(KERNEL_FILE_MAX);

	int size = int(ff8_externals.sm_pc_read((char *)filename, ff8_kernel_stash));

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
	int non_vanilla_sections = 0;

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
		{
			++non_vanilla_sections;
			if (trace_all) ffnx_trace("AddMoreMagic: kernel.bin data section %d has size %u, expected %u.\n", i, src_size, copy_size);
		}

		memcpy(dest + dst, ff8_kernel_stash + src, copy_size);
	}

	if (non_vanilla_sections > 0)
		ffnx_warning("AddMoreMagic: kernel.bin has %d data section(s) with a non-vanilla size - only the magic section may grow; game will likely misbehave!\n", non_vanilla_sections);

	// Text sections (31..55): point the header at the stash so they can grow
	// freely. Also fill dest's vanilla-sized text area with real bytes - some
	// code reads it by hardcoded offset, and leaving it uninitialised crashed
	// the first menu open.
	for (int i = KERNEL_FIRST_TEXT_SEC; i < KERNEL_SECTION_COUNT; ++i)
		out_header[1 + i] = (uint32_t)(ff8_kernel_stash + offsets[i]) - (uint32_t)dest;

	uint32_t text_dest_size = VANILLA_KERNEL_SIZE - vanilla_data_offsets[KERNEL_FIRST_TEXT_SEC];
	uint32_t text_src_size = (uint32_t)size - offsets[KERNEL_FIRST_TEXT_SEC];
	memcpy(dest + vanilla_data_offsets[KERNEL_FIRST_TEXT_SEC], ff8_kernel_stash + offsets[KERNEL_FIRST_TEXT_SEC], text_src_size < text_dest_size ? text_src_size : text_dest_size);

	// FFNx-side full magic table.
	memcpy(ff8_magic_table, ff8_kernel_stash + offsets[KERNEL_MAGIC_SECTION], entries * MAGIC_ENTRY_SIZE);
	ff8_magic_count = entries;

	if (trace_all) ffnx_trace("AddMoreMagic: extended kernel.bin detected (%d magic entries, +%u bytes data growth).\n", entries, data_growth);

	ff8_kernel_magic_arm();

	return VANILLA_KERNEL_SIZE;
}

// ---- init ---------------------------------------------------------------
void ff8_kernel_magic_init()
{
	if (!ff8_externals.magic_kernel_read_call)
	{
		if (trace_all) ffnx_trace("AddMoreMagic: unsupported game version, extension disabled.\n");
		return;
	}

	// Sanity: the call we replace must be the kernel.bin read through sm_pc_read.
	uint32_t call_target = get_relative_call(ff8_externals.magic_kernel_read_call, 0);
	if (call_target != uint32_t(ff8_externals.sm_pc_read))
	{
		ffnx_warning("AddMoreMagic: kernel load call site mismatch (0x%X), extension disabled.\n", call_target);
		return;
	}

	replace_call(ff8_externals.magic_kernel_read_call, (void *)ff8_kernel_load_hook);
}
