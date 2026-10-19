/****************************************************************************/
//	Copyright (C) 2026 Julian Xhokaxhiu								   //
//	Copyright (C) 2026 HobbitDur										  //
//																		  //
//	This file is part of FFNx											 //
//																		  //
//	FFNx is free software: you can redistribute it and/or modify		  //
//	it under the terms of the GNU General Public License as published by  //
//	the Free Software Foundation, either version 3 of the License		 //
//																		  //
//	FFNx is distributed in the hope that it will be useful,			   //
//	but WITHOUT ANY WARRANTY; without even the implied warranty of		//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		 //
//	GNU General Public License for more details.						  //
/****************************************************************************/

#include "kernel_command.h"
#include "kernel_magic.h"
#include "kernel.h"

#include "../ff8.h"
#include "../patch.h"
#include "../globals.h"
#include "../common.h"
#include "../log.h"

#include <stdint.h>

// Added commands reuse a data-driven vanilla command, with their own name and effect.
#define KERNEL_COMMAND_SECTION		0
#define KERNEL_COMMAND_DATA_SECTION   10
#define COMMAND_ENTRY_SIZE sizeof(ff8_kernel_battle_command)
#define COMMAND_DATA_ENTRY_SIZE sizeof(ff8_kernel_command_ability_data)
#define VANILLA_COMMAND_COUNT		 39
#define VANILLA_COMMAND_DATA_COUNT	12
// Command ids from 0xB0 up are the engine's own (Ultimecia, Renzokuken finisher,
// G-Force, Doom countdown...).
#define MAX_COMMAND_COUNT			 0xB0
// abilityDataID is a byte and 0xFF means "no data".
#define NO_COMMAND_DATA			   0xFF
#define MAX_COMMAND_DATA_COUNT		0xFF
#define REMOVED_COMMAND              0xFE
#define REMOVED_COMMAND_DATA         0xFE

#define COMMAND_MAD_RUSH			  0x18

// The data-driven family, the only case a new command can run through.
static const uint8_t data_driven_commands[] = { 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1E, 0x1F, 0x20, 0x21, 0x22 };

// ---- state --------------------------------------------------------------
static ff8_kernel_battle_command ff8_command_table[MAX_COMMAND_COUNT];
static ff8_kernel_command_ability_data ff8_command_data_table[MAX_COMMAND_DATA_COUNT];
static uint8_t ff8_command_behaves_like[MAX_COMMAND_COUNT];
static int ff8_command_count = VANILLA_COMMAND_COUNT;
static int ff8_command_data_count = VANILLA_COMMAND_DATA_COUNT;
static bool ff8_command_armed = false;
static bool ff8_command_filter_hooked = false;
static bool ff8_command_disabled[MAX_COMMAND_COUNT];
static bool ff8_command_has_disabled = false;
static uint32_t command_parse_calls[3];
static bool ff8_command_checked = false;
static bool ff8_command_supported = false;

struct command_externals
{
	uint32_t name, description, several_hit, linked_stock, limit_menu, parse;
};
static command_externals command_ext;

struct command_site
{
	uint32_t *owner;
	uint32_t offset;
	bool data;
};

static const command_site common_sites[] = {
	{ &command_ext.name, 0x8, false },
	{ &command_ext.description, 0x8, false },
	{ &command_ext.several_hit, 0x1A, false },
	{ &command_ext.linked_stock, 0x1C, false },
	{ &command_ext.linked_stock, 0x2E, false },
	{ &command_ext.limit_menu, 0xC1, false },
	{ &ff8_externals.battle_sub_48FE20, 0x258, true },
	{ &ff8_externals.battle_sub_48FE20, 0x263, true },
	{ &ff8_externals.battle_sub_48FE20, 0x26F, true },
	{ &ff8_externals.battle_sub_48FE20, 0x27B, true },
	{ &command_ext.parse, 0x236, false },
	{ &command_ext.parse, 0x248, false },
	{ &command_ext.parse, 0x264, false },
	{ &command_ext.parse, 0x27E, true },
	{ &command_ext.parse, 0x2F7, false },
	{ &command_ext.parse, 0x30A, false },
	{ &command_ext.parse, 0x332, false },
	{ &command_ext.parse, 0x33F, false },
};

struct command_layout
{
	uint32_t dispatcher[9];
	uint32_t damage[4];
};

static const command_layout layout_en = {
	.dispatcher = { 0x23E, 0x250, 0x257, 0x108F, 0x10A1, 0x10A8, 0x10F1, 0x1104, 0x110B },
	.damage = { 0xB07, 0xB1A, 0xB31, 0xB39 },
};

static const command_layout layout_de = {
	.dispatcher = { 0x23F, 0x251, 0x258, 0x106F, 0x1081, 0x1088, 0x10D1, 0x10E4, 0x10EB },
	.damage = { 0xAA3, 0xAB6, 0xACD, 0xAD5 },
};

static const command_layout layout_es = {
	.dispatcher = { 0x23F, 0x251, 0x258, 0x1092, 0x10A4, 0x10AB, 0x10F4, 0x1107, 0x110E },
	.damage = { 0xA76, 0xA89, 0xAA0, 0xAA8 },
};

static const command_layout layout_fr = {
	.dispatcher = { 0x23F, 0x251, 0x258, 0x1068, 0x107A, 0x1081, 0x10CA, 0x10DD, 0x10E4 },
	.damage = { 0xA6C, 0xA7F, 0xA96, 0xA9E },
};

static const command_layout layout_it = {
	.dispatcher = { 0x23F, 0x251, 0x258, 0x10BC, 0x10CE, 0x10D5, 0x111E, 0x1131, 0x1138 },
	.damage = { 0xA8C, 0xA9F, 0xAB6, 0xABE },
};

static const command_layout layout_jp = {
	.dispatcher = { 0x259, 0x26B, 0x272, 0x1051, 0x1063, 0x106A, 0x10B3, 0x10C6, 0x10CD },
	.damage = { 0xA6B, 0xA7E, 0xA95, 0xA9D },
};

static const command_layout &command_layout_of(int game_version)
{
	switch (game_version)
	{
	case VERSION_FF8_12_JP:
	case VERSION_FF8_12_JP_NV: return layout_jp;
	case VERSION_FF8_12_DE:
	case VERSION_FF8_12_DE_NV: return layout_de;
	case VERSION_FF8_12_SP:
	case VERSION_FF8_12_SP_NV: return layout_es;
	case VERSION_FF8_12_FR:
	case VERSION_FF8_12_FR_NV: return layout_fr;
	case VERSION_FF8_12_IT:
	case VERSION_FF8_12_IT_NV: return layout_it;
	default: return layout_en;
	}
}

static command_site command_sites[31];
static uint32_t k_command, k_command_end, k_command_data, k_command_data_end;

static void ff8_kernel_command_find_externals()
{
	k_command = (uint32_t)ff8_externals.kernel->battle_commands;
	k_command_end = k_command + VANILLA_COMMAND_COUNT * COMMAND_ENTRY_SIZE;
	k_command_data = (uint32_t)ff8_externals.kernel->command_ability_data;
	k_command_data_end = k_command_data + VANILLA_COMMAND_DATA_COUNT * COMMAND_DATA_ENTRY_SIZE;

	uint32_t magic_name = ff8_kernel_magic_name_getter();
	uint32_t visibility = ff8_kernel_magic_spell_visibility_fn();
	command_ext.name = magic_name + 0x260;
	command_ext.description = magic_name + 0x290;
	command_ext.several_hit = visibility + 0x180;
	command_ext.linked_stock = visibility + 0x340;
	command_ext.limit_menu = visibility + 0x540;
	command_parse_calls[0] = ff8_externals.sub_48B7E0 + 0x94;
	command_parse_calls[1] = ff8_externals.compute_char_stats_sub_495960 + 0x5A5;
	command_parse_calls[2] = ff8_externals.compute_char_stats_sub_495960 + 0x18AC;
	command_ext.parse = get_relative_call(ff8_externals.sub_48B7E0, 0x94);

	int count = 0;
	for (const command_site &site : common_sites)
		command_sites[count++] = site;

	const command_layout &layout = command_layout_of(version);
	for (int i = 0; i < 9; ++i)
		command_sites[count++] = { &ff8_externals.battle_sub_48D200, layout.dispatcher[i], i % 3 != 0 };
	for (uint32_t offset : layout.damage)
		command_sites[count++] = { &ff8_externals.battle_sub_48FE20, offset, true };
}

static bool ff8_kernel_command_validate()
{
	for (uint32_t call : command_parse_calls)
		if (get_relative_call(call, 0) != command_ext.parse) return false;

	for (const command_site &site : command_sites)
	{
		if (!*site.owner) return false;
		uint32_t value = get_absolute_value(*site.owner, site.offset);
		uint32_t lo = site.data ? k_command_data : k_command;
		uint32_t hi = site.data ? k_command_data_end : k_command_end;
		if (value < lo || value >= hi)
		{
			if (trace_all) ffnx_trace("AddMoreCommand: table operand at 0x%X is unsupported.\n", *site.owner + site.offset);
			return false;
		}
	}
	return true;
}

static int ff8_kernel_command_parse_character(int character, int slot)
{
	int result = ((int (*)(int, int))command_ext.parse)(character, slot);
	if (character == 0xFF || slot < 0 || slot >= (int)ff8_externals.char_comp_stats_1CFF000.size()) return result;
	auto &stats = ff8_externals.char_comp_stats_1CFF000[slot];
	for (auto &command : stats.commands)
		if (ff8_kernel_command_removed(command.command_id)) memset_code((uint32_t)&command, 0, sizeof(command));
	if (ff8_kernel_command_removed(stats.limit_command.command_id)) memset_code((uint32_t)&stats.limit_command, 0, sizeof(stats.limit_command));
	return result;
}

static void ff8_kernel_command_arm()
{
	if (!ff8_command_supported) return;
	if (!ff8_command_armed)
	{
		for (const command_site &site : command_sites)
		{
			uint32_t old_base = site.data ? k_command_data : k_command;
			uint32_t new_base = site.data ? (uint32_t)ff8_command_data_table : (uint32_t)ff8_command_table;
			patch_code_dword(*site.owner + site.offset, new_base + get_absolute_value(*site.owner, site.offset) - old_base);
		}
		ff8_command_armed = true;
	}
	if (ff8_command_has_disabled && !ff8_command_filter_hooked)
	{
		for (uint32_t call : command_parse_calls) replace_call(call, (void *)ff8_kernel_command_parse_character);
		ff8_command_filter_hooked = true;
	}
}

bool ff8_kernel_command_section_may_grow(int section)
{
	return section == KERNEL_COMMAND_SECTION || section == KERNEL_COMMAND_DATA_SECTION;
}

static bool is_data_driven(uint8_t command)
{
	for (uint8_t family : data_driven_commands)
		if (family == command) return true;

	return false;
}

bool ff8_kernel_command_read(const ff8_kernel_file &file)
{
	if (!file.valid()) return false;
	auto command_section = file.section<ff8_kernel_section::battle_commands>();
	auto data_section = file.section<ff8_kernel_section::command_ability_data>();
	int commands = (int)command_section.size();
	int data = (int)data_section.size();

	// A stock kernel.bin leaves here unless an earlier load needs its tables reset.
	bool removed = false;
	if (commands == VANILLA_COMMAND_COUNT && data == VANILLA_COMMAND_DATA_COUNT)
	{
		for (int id = 0; id < commands; ++id) removed |= command_section[id].execution_type == REMOVED_COMMAND;
		for (int id = 0; id < data; ++id) removed |= data_section[id].padding == REMOVED_COMMAND_DATA;
		if (!removed && !ff8_command_armed) return false;
	}

	if (commands < VANILLA_COMMAND_COUNT || commands > MAX_COMMAND_COUNT
		|| data < VANILLA_COMMAND_DATA_COUNT || data > MAX_COMMAND_DATA_COUNT)
	{
		if (trace_all) ffnx_trace("AddMoreCommand: kernel.bin has %d battle commands (%d-%d allowed) and %d command ability data entries (%d-%d allowed) - ignoring the extension.\n",
			commands, VANILLA_COMMAND_COUNT, MAX_COMMAND_COUNT, data, VANILLA_COMMAND_DATA_COUNT, MAX_COMMAND_DATA_COUNT);
		return false;
	}

	uint8_t behaves_like[MAX_COMMAND_COUNT] = {};
	bool disabled[MAX_COMMAND_COUNT] = {};

	for (int id = 0; id < commands; ++id)
	{
		const auto &entry = command_section[id];
		uint8_t data_id = entry.ability_data_id;

		bool fixed_effect = id == 7 || id == 29 || is_data_driven((uint8_t)id) || id >= VANILLA_COMMAND_COUNT;
		bool missing_data = data_id == NO_COMMAND_DATA || (data_id < data && data_section[data_id].padding == REMOVED_COMMAND_DATA);
		disabled[id] = entry.execution_type == REMOVED_COMMAND || (fixed_effect && missing_data);
		removed |= disabled[id];
		if (entry.execution_type == REMOVED_COMMAND) continue;
		if (data_id != NO_COMMAND_DATA && data_id >= data)
		{
			if (trace_all) ffnx_trace("AddMoreCommand: battle command %d uses command ability data %d, but there are only %d - ignoring the extension.\n", id, data_id, data);
			return false;
		}

		if (id < VANILLA_COMMAND_COUNT) continue;

		if (missing_data)
		{
			if (trace_all) ffnx_trace("AddMoreCommand: battle command %d has no command ability data, which a new command needs - ignoring the extension.\n", id);
			return false;
		}

		uint8_t family = entry.execution_type ? entry.execution_type : COMMAND_MAD_RUSH;

		if (!is_data_driven(family))
		{
			if (trace_all) ffnx_trace("AddMoreCommand: battle command %d behaves like command %d, which does not take its effect from command ability data - ignoring the extension.\n", id, family);
			return false;
		}

		behaves_like[id] = family;
	}

	// First extended file seen: only now is it worth resolving and checking.
	if (!ff8_command_checked)
	{
		ff8_command_checked = true;
		ff8_kernel_command_find_externals();
		ff8_command_supported = ff8_kernel_command_validate();

		if (!ff8_command_supported)
			if (trace_all) ffnx_trace("AddMoreCommand: this build is not supported, extension disabled.\n");
	}

	if (!ff8_command_supported) return false;

	if (ff8_command_armed && (commands != ff8_command_count || data != ff8_command_data_count))
	{
		if (trace_all) ffnx_trace("AddMoreCommand: kernel.bin command layout changed after the patches were applied - ignoring the new one.\n");
		return true;
	}

	memcpy_code((uint32_t)(ff8_command_table), (void *)(command_section.data()), commands * COMMAND_ENTRY_SIZE);
	memcpy_code((uint32_t)(ff8_command_data_table), (void *)(data_section.data()), data * COMMAND_DATA_ENTRY_SIZE);
	memcpy_code((uint32_t)(ff8_command_behaves_like), (void *)(behaves_like), sizeof(behaves_like));
	memcpy_code((uint32_t)(ff8_command_disabled), (void *)(disabled), sizeof(disabled));
	ff8_command_has_disabled = removed;
	ff8_command_count = commands;
	ff8_command_data_count = data;

	ff8_kernel_command_arm();

	return true;
}

const ff8_kernel_battle_command *ff8_kernel_command_get(int command)
{
	if (command < 0 || command >= ff8_command_count) return nullptr;
	if (ff8_command_armed) return &ff8_command_table[command];
	return ff8_externals.kernel ? &ff8_externals.kernel->battle_commands[command] : nullptr;
}

// ---- battle -------------------------------------------------------------
bool ff8_kernel_command_removed(uint8_t command)
{
	return ff8_command_armed && command < MAX_COMMAND_COUNT && ff8_command_disabled[command];
}

bool ff8_kernel_command_owns(uint8_t command)
{
	return ff8_command_armed && command >= VANILLA_COMMAND_COUNT && command < ff8_command_count && !ff8_command_disabled[command];
}

typedef int(__cdecl *compute_command_action_t)(int, int, int, int, int, int, int);

int ff8_kernel_command_execute(int attacker_slot, int command, int id, int variant, int target_slot, int target_mask, int linked)
{
	// The dispatcher reads the command as a byte, and its callers leave junk in
	// the rest of the register; keep that junk, change only the byte.
	uint8_t command_id = (uint8_t)command;
	uint8_t family = ff8_command_behaves_like[command_id];

	// The family's case reads the battle command entry for the data id and the
	// name, so the new command's entry stands in for the family's own.
	ff8_kernel_battle_command saved;
	memcpy_code((uint32_t)(&saved), (void *)(&ff8_command_table[family]), COMMAND_ENTRY_SIZE);
	memcpy_code((uint32_t)(&ff8_command_table[family]), (void *)(&ff8_command_table[command_id]), COMMAND_ENTRY_SIZE);

	int ret = ((compute_command_action_t)ff8_externals.battle_sub_48D200)(attacker_slot, (command & ~0xFF) | family, id, variant, target_slot, target_mask, linked);

	memcpy_code((uint32_t)(&ff8_command_table[family]), (void *)(&saved), COMMAND_ENTRY_SIZE);

	return ret;
}
