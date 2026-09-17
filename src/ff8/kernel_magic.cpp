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
// Instruction operands that read the magic table; listed in ff8_data.cpp.
#define K_MAGIC_SITE_COUNT      71
// Instruction operands that read the drawn-once bitfield; listed in ff8_data.cpp.
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


// Every address this file needs, resolved from anchors ff8_externals
// already holds. Nothing outside kernel_magic.cpp reads them, so they
// live here rather than in the shared externals struct.
static struct
{
	uint32_t read_kernel_files_sub_47D2A0;
	uint32_t set_all_monster_info_sub_48BA10;
	uint32_t manage_monster_spell_visibility_sub_48C7A0;
	uint32_t linked_menu_magic_sub_4F02F0;
	uint32_t k_magic;               // buffer + 0x21C (K_MAGIC data label)
	uint32_t kernel_read_call;      // call sm_pc_read(name, KERNEL_HEADER)
	uint32_t fn_name_getter;        // getMagicText(int id), replaced wholesale in C
	uint32_t fn_desc_getter;        // magic description getter(int id), replaced wholesale in C
	uint32_t battle_first_monster_slot; // FF8BattleSlotData[3], stride 208: the 4 monster slots
	uint32_t monster_draw_data;     // monster draw menu records, stride 71: {id, flags, 0, 0}[4], level tier at +0x46
	uint32_t battle_slot_data;      // FF8BattleSlotData[7], stride 208: 3 party slots then 4 monster slots
	uint32_t fn_linked_stock;       // linkedStockFieldCharData(int char, int id)
	uint32_t fn_reorder_magic;      // menu_reorder_magic(int char, int preset)
	uint32_t fn_validate_magic;     // sub_4BE790(int char): per-char held-magic + junction validate
	uint32_t f_char_data;           // FF8FieldCharData[], stride 464
	uint32_t k_battle_command;      // FF8KernelBattleCommand[], stride 8
	uint32_t valid_junction;        // uint32[2] per char (8 chars): valid-junction bitfield
	uint32_t sg_chara_data;         // CharacterData[], stride 152, Magic @+16
	uint32_t magsort_buffer;        // magsortData magsortbuffer[N][7], stride 64/preset (direct array, not a pointer)
	uint32_t sg_drawn_once;         // savemap 64-bit drawn-once bitfield (ids 1-64)
	uint32_t sg_gf_data;            // savemap GF records, stride 68, name at +0
	uint32_t fn_target_mask;          // getMagicTargetMask(int id)
	uint32_t fn_pick_random_action;   // confused/berserk action roll
	uint32_t fn_confused_action;      // confused target pick
	uint32_t fn_queue_command;        // queuePlayerBattleCommand
	uint32_t fn_stat_compute;         // Stat_ComputeCharaStat
	uint32_t fn_stat_hit;             // Stat_ComputeCharaHit
	uint32_t fn_stat_eva;             // Stat_ComputeCharaEva
	uint32_t fn_elem_attack;          // get_elem_attack
	uint32_t fn_elem_attack_value;    // get_elem_attack_value
	uint32_t fn_elem_def_value;       // getMagicElemDefValue
	uint32_t fn_jstatus_attack;       // getJStatusAttack
	uint32_t fn_status2_from_jstatus; // getStatus2FromJstatusAttack
	uint32_t fn_status_attack_value;  // computeStatusAttackValue
	uint32_t fn_mental_defense;       // get_mental_defense
	uint32_t fn_junction_swap;        // junction menu magic swap
	uint32_t fn_junction_value;       // linkedMagicJunctionValue
	uint32_t fn_auto_junction_spell;  // Junction_AutoPickBestSpellForStat
	uint32_t fn_menu_magic_hp;        // magic menu HP preview
	uint32_t fn_unused_magic_read;    // never called; repointed anyway
	uint32_t k_magic_reads[71];       // every operand pointing into the magic table
	uint32_t drawn_once_reads[5];     // every operand pointing at the drawn-once bitfield
} magic_ext;

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
		return (char *)(magic_ext.sg_gf_data + GF_DATA_STRIDE * (id - GF_FIRST_ID));

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

// ---- replaced functions -------------------------------------------------
// Draw->Stock setup (replaces linkedStockFieldCharData); only 64..79 are GFs.
static void *__cdecl ff8_linked_stock_field_char_data(int char_slot, int spell_id)
{
	uint8_t *chr = (uint8_t *)(magic_ext.f_char_data + 464 * char_slot);
	const uint8_t *draw_cmd = (const uint8_t *)(magic_ext.k_battle_command + 8 * 10);

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
		if (magic[11] & 0x80) // attackFlags: can target KO'd units
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
	const uint8_t *preset = (const uint8_t *)(*(uintptr_t *)magic_ext.magsort_buffer) + 64 * sort_preset;
	if (!preset[0]) return 0;

	uint8_t amounts[MAX_MAGIC_ID];
	memset(amounts, 0, sizeof(amounts));

	uint8_t *inventory = (uint8_t *)(magic_ext.sg_chara_data + CHAR_STRIDE * character_id + CHAR_MAGIC_OFF); // 32 x {id, amount}
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
// ids, and repoint its DRAWN_ONCE_SITE_COUNT accessors at it. A stock game
// never gets here.
static void relocate_drawn_once_bitfield()
{
	// The replaced draw-list function reads this, so it must point somewhere real
	// even when the field stays where the game put it.
	ff8_drawn_once = magic_ext.sg_drawn_once;

	if (ff8_magic_count <= EXTENDED_MAGIC_FIRST)
	{
		if (trace_all) ffnx_trace("AddMoreMagic: no magic id >= %d, drawn-once left at vanilla 0x%08X.\n", EXTENDED_MAGIC_FIRST, magic_ext.sg_drawn_once);
		return;
	}

	ff8_drawn_once = ff8_externals.field_vars_stack_1CFE9B8 + 753;

	uint32_t patched = 0;
	for (int i = 0; i < DRAWN_ONCE_SITE_COUNT; ++i)
	{
		uint32_t operand = magic_ext.drawn_once_reads[i];
		uint32_t points_at = *(uint32_t *)operand;

		if (points_at != magic_ext.sg_drawn_once)
		{
			ffnx_warning("AddMoreMagic: drawn-once read %d at 0x%X points at 0x%X, not at the bitfield - skipping it!\n", i, operand, points_at);
			continue;
		}

		patch_code_dword(operand, (DWORD)ff8_drawn_once);
		++patched;
	}

	if (patched != DRAWN_ONCE_SITE_COUNT)
		ffnx_warning("AddMoreMagic: moved %u of %d drawn-once reads - some drawn-once state may not persist correctly!\n", patched, DRAWN_ONCE_SITE_COUNT);
}

// Per-character held-magic + junction validation (replaces sub_4BE790).
// Vanilla builds the "held" bitfield in a 64-bit stack buffer indexed by
// id/32, which a held id >= 64 smashes. Uses a 256-bit local instead, and
// clamps the 2-dword valid-junction global write to its vanilla range.
static int __cdecl ff8_char_validate_magic(int char_idx)
{
	uint32_t *valid_junction = (uint32_t *)(magic_ext.valid_junction + 8 * char_idx);
	valid_junction[0] = 0;
	valid_junction[1] = 0;

	uint8_t *chr = (uint8_t *)(magic_ext.sg_chara_data + CHAR_STRIDE * char_idx);
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
	uint8_t *slot = (uint8_t *)magic_ext.battle_first_monster_slot;
	uint8_t *monster = (uint8_t *)magic_ext.monster_draw_data;
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
				const uint8_t *gf = (const uint8_t *)(magic_ext.sg_gf_data + GF_DATA_STRIDE * (id - GF_FIRST_ID));
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

// Handle from replace_function, so the original can be called back below.
static uint32_t ff8_compute_command_action_replaced = 0;

static uint8_t *ff8_battle_state(int offset)
{
	return (uint8_t *)(magic_ext.battle_slot_data + offset);
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

// The dispatcher itself is replaced, so its first bytes are a jump back here.
// Lift the replacement for the length of the call and put it straight back, the
// way ff8/vibration.cpp does - the alternative, hooking call sites, has to know
// all twelve of them and misses whichever one a future build adds.
static int ff8_call_command_action(int attacker_slot, int command, int id, int variant, int target_slot, int target_mask, int linked)
{
	unreplace_function(ff8_compute_command_action_replaced);
	int ret = ((compute_command_action_t)ff8_externals.battle_sub_48D200)(attacker_slot, command, id, variant, target_slot, target_mask, linked);
	rereplace_function(ff8_compute_command_action_replaced);

	return ret;
}

static int __cdecl ff8_compute_command_action(int attacker_slot, int command, int id, int variant, int target_slot, int target_mask, int linked)
{
	// The dispatcher takes these as a word and as bytes, so its callers write only
	// the low part of the register they push ("mov dl, [esi+2]") and leave whatever
	// was in the rest. Read them at their real width or a stocked draw shows up as
	// 0x4000000A rather than 10.
	uint16_t spell_id = (uint16_t)id;
	uint8_t command_id = (uint8_t)command;
	uint8_t draw_variant = (uint8_t)variant;
	uint8_t target = (uint8_t)target_slot;

	// Everything vanilla still gets right goes straight to the dispatcher, with the
	// arguments exactly as they arrived.
	if (command_id != COMMAND_DRAW || draw_variant != DRAW_VARIANT_STOCK || spell_id < GF_FIRST_ID || ff8_is_gf_id(spell_id) || spell_id >= MAX_MAGIC_ID)
		return ff8_call_command_action(attacker_slot, command, id, variant, target_slot, target_mask, linked);

	uint8_t *inventory = (uint8_t *)(magic_ext.f_char_data + F_CHAR_DATA_STRIDE * attacker_slot + BATTLE_MAGIC_OFF);
	uint8_t *monster = (uint8_t *)magic_ext.monster_draw_data + MONSTER_DRAW_STRIDE * (target - BATTLE_FIRST_MONSTER_SLOT);

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

	// Repoint every instruction that reads the vanilla magic table at the FFNx
	// side one. The operands are listed in ff8_data.cpp; each is checked to
	// still point into the table before it is touched.
	uint32_t k_magic_end = magic_ext.k_magic + VANILLA_MAGIC_COUNT * MAGIC_ENTRY_SIZE;
	uint32_t table = (uint32_t)&ff8_magic_table[0][0];
	uint32_t rewritten = 0;

	for (int i = 0; i < K_MAGIC_SITE_COUNT; ++i)
	{
		uint32_t operand = magic_ext.k_magic_reads[i];
		uint32_t points_at = *(uint32_t *)operand;

		if (points_at < magic_ext.k_magic || points_at >= k_magic_end)
		{
			ffnx_warning("AddMoreMagic: magic table read %d at 0x%X points at 0x%X, not at the magic table - skipping it!\n", i, operand, points_at);
			continue;
		}

		patch_code_dword(operand, (DWORD)(table + (points_at - magic_ext.k_magic)));
		++rewritten;
	}

	if (rewritten != K_MAGIC_SITE_COUNT)
		ffnx_warning("AddMoreMagic: repointed %u of %d magic table reads - some magic reads may still use the vanilla table!\n", rewritten, K_MAGIC_SITE_COUNT);

	// Take over the functions that would misread an extended id.
	ff8_compute_command_action_replaced = replace_function(ff8_externals.battle_sub_48D200, (void *)ff8_compute_command_action);
	replace_function(magic_ext.manage_monster_spell_visibility_sub_48C7A0, (void *)ff8_manage_monster_spell_visibility);
	replace_function(magic_ext.fn_name_getter, (void *)ff8_get_magic_name);
	replace_function(magic_ext.fn_desc_getter, (void *)ff8_get_magic_description);
	replace_function(magic_ext.fn_linked_stock, (void *)ff8_linked_stock_field_char_data);
	replace_function(magic_ext.fn_reorder_magic, (void *)ff8_menu_reorder_magic);
	replace_function(magic_ext.fn_validate_magic, (void *)ff8_char_validate_magic);

	relocate_drawn_once_bitfield();

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
// Resolve them all. Runs before anything else in this file is armed.
// Offsets that differ per build are grouped by the exe they come from,
// not hardcoded per address.
static void ff8_kernel_magic_find_externals()
{
	magic_ext.read_kernel_files_sub_47D2A0 = get_relative_call(get_relative_call(ff8_externals.sub_470440, 0x22), 0);
	magic_ext.kernel_read_call = magic_ext.read_kernel_files_sub_47D2A0 + 0x96; // call sm_pc_read(name, KERNEL_HEADER)

	magic_ext.set_all_monster_info_sub_48BA10 = get_relative_call(ff8_externals.sub_47CCB0, 0x996);
	magic_ext.manage_monster_spell_visibility_sub_48C7A0 = get_relative_call(magic_ext.set_all_monster_info_sub_48BA10, 0x1A7);
	magic_ext.battle_first_monster_slot = get_absolute_value(magic_ext.manage_monster_spell_visibility_sub_48C7A0, 0x6); // mov eax, offset BATTLE_SLOT_DATA[3]
	magic_ext.monster_draw_data = get_absolute_value(magic_ext.manage_monster_spell_visibility_sub_48C7A0, 0xD) - 0x46; // mov ebp, offset MONSTER_DATA_INVENTORY[0].levelTier
	magic_ext.battle_slot_data = magic_ext.battle_first_monster_slot - 3 * 208; // the 3 party slots sit before the monsters
	magic_ext.fn_linked_stock = magic_ext.manage_monster_spell_visibility_sub_48C7A0 + 0x340;

	// computeCommandAction: the getMagicText call it makes, at a build
	// specific offset.
	uint32_t name_getter_offset;
	switch (version)
	{
	case VERSION_FF8_12_JP:
	case VERSION_FF8_12_JP_NV:
		name_getter_offset = 0x3EB;
		break;
	case VERSION_FF8_12_DE:
	case VERSION_FF8_12_DE_NV:
	case VERSION_FF8_12_IT:
	case VERSION_FF8_12_IT_NV:
		name_getter_offset = 0x3F0;
		break;
	case VERSION_FF8_12_FR:
	case VERSION_FF8_12_FR_NV:
	case VERSION_FF8_12_SP:
	case VERSION_FF8_12_SP_NV:
		name_getter_offset = 0x3BC;
		break;
	default: // US (incl. Eidos)
		name_getter_offset = 0x3C3;
		break;
	}
	magic_ext.fn_name_getter = get_relative_call(ff8_externals.battle_sub_48D200, name_getter_offset);
	magic_ext.fn_desc_getter = magic_ext.fn_name_getter + 0x50;

	magic_ext.fn_validate_magic = get_relative_call(uint32_t(ff8_externals.menu_callbacks[1].func), 0xE5);
	magic_ext.linked_menu_magic_sub_4F02F0 = get_absolute_value(uint32_t(ff8_externals.menu_callbacks[3].func), 0x8);
	magic_ext.fn_reorder_magic = get_relative_call(magic_ext.linked_menu_magic_sub_4F02F0, 0x47BC);

	magic_ext.k_battle_command = uint32_t(ff8_externals.unk_1CF3E48) + 0xE4;  // kernel.bin data section 0
	magic_ext.k_magic          = uint32_t(ff8_externals.unk_1CF3E48) + 0x21C; // kernel.bin data section 1

	magic_ext.sg_gf_data      = get_absolute_value(magic_ext.fn_name_getter, 0x43);     // lea eax, SG_GF_DATA[edx*4]
	magic_ext.f_char_data     = get_absolute_value(magic_ext.fn_linked_stock, 0x28);    // lea eax, F_CHAR_DATA[edx]
	magic_ext.valid_junction  = get_absolute_value(magic_ext.fn_validate_magic, 0x14);  // mov VALID_JUNCTION[ebp*8], ebx
	magic_ext.magsort_buffer  = get_absolute_value(magic_ext.fn_reorder_magic, 0x2);    // mov ecx, MAGSORT_BUFFER
	magic_ext.sg_chara_data   = get_absolute_value(magic_ext.fn_validate_magic, 0x38) - 16; // lea esi, SG_CHARA_DATA[edi]
	magic_ext.sg_drawn_once   = get_absolute_value(ff8_externals.sub_48B7E0, 0x71);               // or DRAWN_ONCE[eax*4], edx

	// The functions whose code reads the magic table. Each is resolved from an
	// address FFNx already knows.
	uint32_t battle_tick_atb = get_relative_call(ff8_externals.sub_4A84E0, 0x2F6);
	uint32_t player_random_attack = get_relative_call(battle_tick_atb, 0x18D);
	magic_ext.fn_queue_command = get_relative_call(player_random_attack, 0x35);
	magic_ext.fn_pick_random_action = get_relative_call(ff8_externals.sub_485610, 0x2E7);
	magic_ext.fn_confused_action = get_relative_call(ff8_externals.sub_485610, 0x20B);
	// getMagicTargetMask is only reached from MonsterAI, whose call sits at a
	// different offset on every build; it always precedes the action roll.
	magic_ext.fn_target_mask = magic_ext.fn_pick_random_action - 0x80;

	magic_ext.fn_stat_compute = get_relative_call(ff8_externals.compute_char_stats_sub_495960, 0xF2);
	magic_ext.fn_stat_hit = get_relative_call(ff8_externals.compute_char_stats_sub_495960, 0x285);
	magic_ext.fn_stat_eva = get_relative_call(ff8_externals.compute_char_stats_sub_495960, 0x2C8);
	magic_ext.fn_elem_attack = get_relative_call(ff8_externals.compute_char_stats_sub_495960, 0x302);
	magic_ext.fn_elem_attack_value = get_relative_call(ff8_externals.compute_char_stats_sub_495960, 0x30E);
	magic_ext.fn_elem_def_value = get_relative_call(ff8_externals.compute_char_stats_sub_495960, 0x326);
	magic_ext.fn_status2_from_jstatus = get_relative_call(ff8_externals.compute_char_stats_sub_495960, 0x33C);
	magic_ext.fn_jstatus_attack = get_relative_call(ff8_externals.compute_char_stats_sub_495960, 0x348);
	magic_ext.fn_status_attack_value = get_relative_call(ff8_externals.compute_char_stats_sub_495960, 0x355);
	magic_ext.fn_mental_defense = get_relative_call(ff8_externals.compute_char_stats_sub_495960, 0x368);

	uint32_t junction_menu_init = get_relative_call(uint32_t(ff8_externals.menu_callbacks[18].func), 0x31);
	uint32_t junction_menu = get_absolute_value(junction_menu_init, 0xB);
	magic_ext.fn_junction_swap = get_relative_call(junction_menu, 0xB78);
	magic_ext.fn_auto_junction_spell = get_relative_call(get_relative_call(junction_menu, 0x1B6C), 0x35);
	magic_ext.fn_junction_value = get_relative_call(get_relative_call(magic_ext.linked_menu_magic_sub_4F02F0, 0xA37), 0x27F);
	magic_ext.fn_menu_magic_hp = get_absolute_value(get_absolute_value(uint32_t(ff8_externals.menu_callbacks[3].func), 0x3), 0x8A8);
	// Dead code on every build, but it still reads the table.
	magic_ext.fn_unused_magic_read = magic_ext.fn_name_getter + 0x3FE0;

	// Offsets that differ per build: these sit in functions that assemble
	// battle or menu text, whose code layout is language specific.
	// computeCommandAction
	static const uint16_t dispatcher_reads_us[10] = { 0x533, 0x53A, 0x576, 0x5BB, 0x5C6, 0x6A0, 0x6A6, 0x6B1, 0x9DC, 0x9E3 };
	static const uint16_t dispatcher_reads_fr[10] = { 0x507, 0x50E, 0x549, 0x58E, 0x599, 0x673, 0x679, 0x684, 0x9B5, 0x9BC };
	static const uint16_t dispatcher_reads_de[10] = { 0x4FD, 0x508, 0x53D, 0x585, 0x58C, 0x66A, 0x670, 0x67B, 0x9B7, 0x9BE };
	static const uint16_t dispatcher_reads_sp[10] = { 0x521, 0x528, 0x563, 0x5A8, 0x5B3, 0x68D, 0x693, 0x69E, 0x9DA, 0x9E1 };
	static const uint16_t dispatcher_reads_it[10] = { 0x54A, 0x551, 0x58A, 0x5D2, 0x5D9, 0x6B7, 0x6BD, 0x6C8, 0xA04, 0xA0B };
	static const uint16_t dispatcher_reads_jp[10] = { 0x4EF, 0x4FA, 0x52F, 0x577, 0x57E, 0x65C, 0x662, 0x66D, 0x99E, 0x9A5 };
	// Battle_applyDamage (text part)
	static const uint16_t damage_reads_us[8] = { 0xB93, 0xBAB, 0xBB9, 0xBC1, 0xC68, 0xC80, 0xC8E, 0xC96 };
	static const uint16_t damage_reads_fr[8] = { 0xAF8, 0xB10, 0xB1E, 0xB26, 0xBCD, 0xBE5, 0xBF3, 0xBFB };
	static const uint16_t damage_reads_de[8] = { 0xB2F, 0xB47, 0xB55, 0xB5D, 0xC04, 0xC1C, 0xC2A, 0xC32 };
	static const uint16_t damage_reads_sp[8] = { 0xB02, 0xB1A, 0xB28, 0xB30, 0xBD7, 0xBEF, 0xBFD, 0xC05 };
	static const uint16_t damage_reads_it[8] = { 0xB18, 0xB30, 0xB3E, 0xB46, 0xBED, 0xC05, 0xC13, 0xC1B };
	static const uint16_t damage_reads_jp[8] = { 0xAF7, 0xB0F, 0xB1D, 0xB25, 0xBCC, 0xBE4, 0xBF2, 0xBFA };
	// MonsterAI
	static const uint16_t monster_ai_reads_us[2] = { 0x1A86, 0x222A };
	static const uint16_t monster_ai_reads_fr[2] = { 0x1A90, 0x2234 };
	static const uint16_t monster_ai_reads_de[2] = { 0x1A8B, 0x222F };
	static const uint16_t monster_ai_reads_sp[2] = { 0x1ABA, 0x225E };
	static const uint16_t monster_ai_reads_it[2] = { 0x1AA6, 0x224A };
	static const uint16_t monster_ai_reads_jp[2] = { 0x1A58, 0x2206 };
	// magic menu (text part)
	static const uint16_t menu_magic_reads_us[2] = { 0x2C04, 0x2C0B };
	static const uint16_t menu_magic_reads_fr[2] = { 0x2C04, 0x2C0B };
	static const uint16_t menu_magic_reads_de[2] = { 0x2C04, 0x2C0B };
	static const uint16_t menu_magic_reads_sp[2] = { 0x2C04, 0x2C0B };
	static const uint16_t menu_magic_reads_it[2] = { 0x2C04, 0x2C0B };
	static const uint16_t menu_magic_reads_jp[2] = { 0x2C10, 0x2C17 };

	const uint16_t *dispatcher_reads = dispatcher_reads_us;
	const uint16_t *damage_reads = damage_reads_us;
	const uint16_t *monster_ai_reads = monster_ai_reads_us;
	const uint16_t *menu_magic_reads = menu_magic_reads_us;
	switch (version)
	{
	case VERSION_FF8_12_FR:
	case VERSION_FF8_12_FR_NV:
		dispatcher_reads = dispatcher_reads_fr;
		damage_reads = damage_reads_fr;
		monster_ai_reads = monster_ai_reads_fr;
		menu_magic_reads = menu_magic_reads_fr;
		break;
	case VERSION_FF8_12_DE:
	case VERSION_FF8_12_DE_NV:
		dispatcher_reads = dispatcher_reads_de;
		damage_reads = damage_reads_de;
		monster_ai_reads = monster_ai_reads_de;
		menu_magic_reads = menu_magic_reads_de;
		break;
	case VERSION_FF8_12_SP:
	case VERSION_FF8_12_SP_NV:
		dispatcher_reads = dispatcher_reads_sp;
		damage_reads = damage_reads_sp;
		monster_ai_reads = monster_ai_reads_sp;
		menu_magic_reads = menu_magic_reads_sp;
		break;
	case VERSION_FF8_12_IT:
	case VERSION_FF8_12_IT_NV:
		dispatcher_reads = dispatcher_reads_it;
		damage_reads = damage_reads_it;
		monster_ai_reads = monster_ai_reads_it;
		menu_magic_reads = menu_magic_reads_it;
		break;
	case VERSION_FF8_12_JP:
	case VERSION_FF8_12_JP_NV:
		dispatcher_reads = dispatcher_reads_jp;
		damage_reads = damage_reads_jp;
		monster_ai_reads = monster_ai_reads_jp;
		menu_magic_reads = menu_magic_reads_jp;
		break;
	default: // US (incl. Eidos)
		break;
	}

	uint32_t magic_reads[] = {
		magic_ext.fn_name_getter + 0x13, // getMagicText
		magic_ext.fn_desc_getter + 0x13, // magic description getter
		magic_ext.fn_unused_magic_read + 0xAC, // never called, kept for parity
		magic_ext.fn_target_mask + 0x10, // getMagicTargetMask
		magic_ext.fn_target_mask + 0x65, // getMagicTargetMask
		magic_ext.fn_pick_random_action + 0xF2, // confused/berserk action roll
		magic_ext.fn_pick_random_action + 0x12F, // confused/berserk action roll
		magic_ext.fn_pick_random_action + 0x17E, // confused/berserk action roll
		magic_ext.fn_confused_action + 0x3B, // confused target pick
		magic_ext.fn_confused_action + 0x82, // confused target pick
		magic_ext.fn_confused_action + 0xB8, // confused target pick
		magic_ext.fn_confused_action + 0x105, // confused target pick
		magic_ext.fn_queue_command + 0x1CE, // queuePlayerBattleCommand
		magic_ext.fn_queue_command + 0x1ED, // queuePlayerBattleCommand
		ff8_externals.sub_485610 + 0x671, // BattleAction_ExecuteCommand
		magic_ext.fn_linked_stock + 0x4B, // linkedStockFieldCharData
		magic_ext.fn_linked_stock + 0x54, // linkedStockFieldCharData
		magic_ext.fn_linked_stock + 0x5A, // linkedStockFieldCharData
		uint32_t(ff8_externals.battle_get_draw_magic_amount_48FD20) + 0x99, // draw quantity
		ff8_externals.battle_sub_48FE20 + 0x29B, // Battle_applyDamage
		ff8_externals.battle_sub_48FE20 + 0x2A6, // Battle_applyDamage
		ff8_externals.battle_sub_48FE20 + 0x2B2, // Battle_applyDamage
		ff8_externals.battle_sub_48FE20 + 0x2BE, // Battle_applyDamage
		uint32_t(ff8_externals.sub_4954B0) + 0x33, // setMenuFlagMagicOnCharaData
		uint32_t(ff8_externals.sub_4954B0) + 0x62, // setMenuFlagMagicOnCharaData
		uint32_t(ff8_externals.sub_4954B0) + 0x72, // setMenuFlagMagicOnCharaData
		uint32_t(ff8_externals.compute_char_max_hp_496310) + 0x7A, // Stat_ComputeCharaMaxHP
		magic_ext.fn_stat_compute + 0x64, // Stat_ComputeCharaStat
		magic_ext.fn_stat_compute + 0xEC, // Stat_ComputeCharaStat
		magic_ext.fn_stat_compute + 0x120, // Stat_ComputeCharaStat
		magic_ext.fn_stat_compute + 0x154, // Stat_ComputeCharaStat
		magic_ext.fn_stat_compute + 0x185, // Stat_ComputeCharaStat
		magic_ext.fn_stat_compute + 0x1B6, // Stat_ComputeCharaStat
		magic_ext.fn_stat_hit + 0x26, // Stat_ComputeCharaHit
		magic_ext.fn_stat_eva + 0x26, // Stat_ComputeCharaEva
		magic_ext.fn_elem_attack + 0x1E, // get_elem_attack
		magic_ext.fn_elem_attack_value + 0x26, // get_elem_attack_value
		magic_ext.fn_elem_def_value + 0x45, // getMagicElemDefValue
		magic_ext.fn_elem_def_value + 0x4F, // getMagicElemDefValue
		magic_ext.fn_jstatus_attack + 0x1C, // getJStatusAttack
		magic_ext.fn_status2_from_jstatus + 0x21, // getStatus2FromJstatusAttack
		magic_ext.fn_status_attack_value + 0x26, // computeStatusAttackValue
		magic_ext.fn_mental_defense + 0x44, // get_mental_defense
		magic_ext.fn_mental_defense + 0x50, // get_mental_defense
		magic_ext.fn_junction_swap + 0xD, // junction menu magic swap
		magic_ext.fn_junction_value + 0x1A, // linkedMagicJunctionValue
		magic_ext.fn_auto_junction_spell + 0x4D, // Junction_AutoPickBestSpellForStat
		magic_ext.linked_menu_magic_sub_4F02F0 + 0x35A, // magic menu
		magic_ext.fn_menu_magic_hp + 0xC0, // magic menu HP preview
		ff8_externals.battle_sub_48D200 + dispatcher_reads[0], // computeCommandAction
		ff8_externals.battle_sub_48D200 + dispatcher_reads[1], // computeCommandAction
		ff8_externals.battle_sub_48D200 + dispatcher_reads[2], // computeCommandAction
		ff8_externals.battle_sub_48D200 + dispatcher_reads[3], // computeCommandAction
		ff8_externals.battle_sub_48D200 + dispatcher_reads[4], // computeCommandAction
		ff8_externals.battle_sub_48D200 + dispatcher_reads[5], // computeCommandAction
		ff8_externals.battle_sub_48D200 + dispatcher_reads[6], // computeCommandAction
		ff8_externals.battle_sub_48D200 + dispatcher_reads[7], // computeCommandAction
		ff8_externals.battle_sub_48D200 + dispatcher_reads[8], // computeCommandAction
		ff8_externals.battle_sub_48D200 + dispatcher_reads[9], // computeCommandAction
		ff8_externals.battle_sub_48FE20 + damage_reads[0], // Battle_applyDamage (text part)
		ff8_externals.battle_sub_48FE20 + damage_reads[1], // Battle_applyDamage (text part)
		ff8_externals.battle_sub_48FE20 + damage_reads[2], // Battle_applyDamage (text part)
		ff8_externals.battle_sub_48FE20 + damage_reads[3], // Battle_applyDamage (text part)
		ff8_externals.battle_sub_48FE20 + damage_reads[4], // Battle_applyDamage (text part)
		ff8_externals.battle_sub_48FE20 + damage_reads[5], // Battle_applyDamage (text part)
		ff8_externals.battle_sub_48FE20 + damage_reads[6], // Battle_applyDamage (text part)
		ff8_externals.battle_sub_48FE20 + damage_reads[7], // Battle_applyDamage (text part)
		ff8_externals.battle_ai_opcode_sub_487DF0 + monster_ai_reads[0], // MonsterAI
		ff8_externals.battle_ai_opcode_sub_487DF0 + monster_ai_reads[1], // MonsterAI
		magic_ext.linked_menu_magic_sub_4F02F0 + menu_magic_reads[0], // magic menu (text part)
		magic_ext.linked_menu_magic_sub_4F02F0 + menu_magic_reads[1], // magic menu (text part)
	};
	memcpy(magic_ext.k_magic_reads, magic_reads, sizeof(magic_reads));

	// The five instructions that index the drawn-once bitfield. Four sit around
	// ParseBattleParty, the fifth in the draw list visibility function.
	magic_ext.drawn_once_reads[0] = ff8_externals.sub_48B7E0 - 0x54;
	magic_ext.drawn_once_reads[1] = ff8_externals.sub_48B7E0 - 0x13;
	magic_ext.drawn_once_reads[2] = ff8_externals.sub_48B7E0 + 0x71;
	magic_ext.drawn_once_reads[3] = ff8_externals.sub_48B7E0 + 0x15E;
	magic_ext.drawn_once_reads[4] = magic_ext.manage_monster_spell_visibility_sub_48C7A0 + 0x7C;
}

void ff8_kernel_magic_init()
{
	ff8_kernel_magic_find_externals();

	if (!magic_ext.kernel_read_call)
	{
		if (trace_all) ffnx_trace("AddMoreMagic: unsupported game version, extension disabled.\n");
		return;
	}

	// Sanity: the call we replace must be the kernel.bin read through sm_pc_read.
	uint32_t call_target = get_relative_call(magic_ext.kernel_read_call, 0);
	if (call_target != uint32_t(ff8_externals.sm_pc_read))
	{
		ffnx_warning("AddMoreMagic: kernel load call site mismatch (0x%X), extension disabled.\n", call_target);
		return;
	}

	replace_call(magic_ext.kernel_read_call, (void *)ff8_kernel_load_hook);
}
