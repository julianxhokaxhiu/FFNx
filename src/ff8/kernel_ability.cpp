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

#include "kernel_ability.h"
#include "kernel_magic.h"

#include "../ff8.h"
#include "../patch.h"
#include "../globals.h"
#include "../common.h"
#include "../log.h"
#include "../utils.h"

#include <initializer_list>
#include <stdint.h>
#include <string.h>

// Ability data stays FFNx-side so growing it does not move the game's other
// kernel sections. See docs/ff8/kernel_abilities.md for the file and save limits.

#define ABILITY_ENTRY_SIZE         8
// The savemap's per-GF learned-ability bitfield is 16 bytes wide.
#define MAX_ABILITY_COUNT          128
#define VANILLA_ABILITY_COUNT      116
#define ABILITY_GROUP_COUNT        7
// Ability block start, relative to the kernel.bin buffer.
#define ABILITY_BLOCK_OFFSET       0x40E0
// Data sections, 0-based as the kernel.bin header lists them.
#define KERNEL_FIRST_ABIL_SEC      11
#define KERNEL_AFTER_ABIL_SEC      18
// Vanilla junction lists sit before the GF summary table, with room for 20
// command and 48 passive entries. Expanded lists need separate storage.
#define VANILLA_COMMAND_CANDIDATES 20
#define CANDIDATE_ENTRY_SIZE       2      // {u8 ability id, u8 group}
// A learned menu ability is one bit of a single dword (RebuildLearnedMenuAbilityMask).
#define MAX_MENU_ABILITIES         32
#define VANILLA_MENU_ABILITIES     24
#define GROUP_COMMAND              1
#define GROUP_STAT_PERCENT         2
#define GROUP_CHARACTER            3
#define GROUP_PARTY                4
#define GROUP_GF                   5
#define GROUP_MENU                 6

static const uint8_t vanilla_group_first[ABILITY_GROUP_COUNT] = { 0, 20, 39, 58, 78, 83, 92 };

// Function offsets are relative to BuildGFAbilityList. US, other Latin builds
// and Japanese builds use different layouts; popup instruction offsets also vary.
enum ability_layout { ABILITY_LAYOUT_US, ABILITY_LAYOUT_LATIN, ABILITY_LAYOUT_JP };

struct ability_layout_offsets
{
	uint32_t menu_mask_call;
	int32_t seal_row;
	int32_t seal_list;
	int32_t group_from_id;
	int32_t learned_popup;
	int32_t draw_learn_status;
	int32_t validate_passives;
	int32_t junction_menu;
	int32_t chara_ability_lists;
	int32_t junction_ability_page;
	int32_t gf_summary;
	int32_t ability_entry_ptr;
	int32_t call_shop_list;
	int32_t draw_list_row;
	uint32_t popup_chain[ABILITY_GROUP_COUNT - 1];
};

static const ability_layout_offsets ability_layouts[] = {
	// US
	{
		.menu_mask_call = 0x3CA6,
		.seal_row = 0x2550,
		.seal_list = 0x26E0,
		.group_from_id = -0x50,
		.learned_popup = -0x6890,
		.draw_learn_status = 0x27A60,
		.validate_passives = 0x2DAF0,
		.junction_menu = 0x2DE40,
		.chara_ability_lists = 0x335A0,
		.junction_ability_page = 0x359D0,
		.gf_summary = 0x360B0,
		.ability_entry_ptr = 0x3AAB0,
		.call_shop_list = 0x3AC30,
		.draw_list_row = 0x3BCF0,
		.popup_chain = { 0x059, 0x068, 0x077, 0x086, 0x095, 0x0A6 },
	},
	// DE, ES, FR, IT
	{
		.menu_mask_call = 0x3CA6,
		.seal_row = 0x2550,
		.seal_list = 0x26E0,
		.group_from_id = -0x50,
		.learned_popup = -0x6890,
		.draw_learn_status = 0x27A60,
		.validate_passives = 0x2DAF0,
		.junction_menu = 0x2DE40,
		.chara_ability_lists = 0x335A0,
		.junction_ability_page = 0x359D0,
		.gf_summary = 0x360B0,
		.ability_entry_ptr = 0x3AAF0,
		.call_shop_list = 0x3AC70,
		.draw_list_row = 0x3BD30,
		.popup_chain = { 0x059, 0x068, 0x077, 0x086, 0x095, 0x0A6 },
	},
	// JP, JP_NV
	{
		.menu_mask_call = 0x3A7D,
		.seal_row = 0x24E0,
		.seal_list = 0x2670,
		.group_from_id = -0x50,
		.learned_popup = -0x6950,
		.draw_learn_status = 0x278E0,
		.validate_passives = 0x2D890,
		.junction_menu = 0x2DBE0,
		.chara_ability_lists = 0x33340,
		.junction_ability_page = 0x35700,
		.gf_summary = 0x35DE0,
		.ability_entry_ptr = 0x3A6F0,
		.call_shop_list = 0x3A870,
		.draw_list_row = 0x3B900,
		.popup_chain = { 0x041, 0x04E, 0x05D, 0x06C, 0x07B, 0x08C },
	},
};

static const ability_layout_offsets &layout_of(int build)
{
	switch (build)
	{
	case VERSION_FF8_12_JP:
	case VERSION_FF8_12_JP_NV:
		return ability_layouts[ABILITY_LAYOUT_JP];
	case VERSION_FF8_12_DE:
	case VERSION_FF8_12_DE_NV:
	case VERSION_FF8_12_SP:
	case VERSION_FF8_12_SP_NV:
	case VERSION_FF8_12_FR:
	case VERSION_FF8_12_FR_NV:
	case VERSION_FF8_12_IT:
	case VERSION_FF8_12_IT_NV:
		return ability_layouts[ABILITY_LAYOUT_LATIN];
	default:
		return ability_layouts[ABILITY_LAYOUT_US];
	}
}

// Every address this file needs, resolved from anchors ff8_externals already
// holds, or from an anchor this file resolves first.
static struct
{
	uint32_t k_ability;             // kernel buffer + 0x40E0, the whole array
	uint32_t kernel_buffer;         // KERNEL_HEADER
	uint32_t *learned_menu_mask;
	uint32_t group_table;           // {u16 section offset, u8 first id, u8 entry size}[7]

	uint32_t fn_get_name;           // getAbilityName(int id)
	uint32_t fn_get_desc;           // getAbilityDescription(int id)
	uint32_t fn_group_from_id;      // getAbilityGroupFromId(int id)
	uint32_t fn_build_list;         // BuildGFAbilityList(gf, out, include_learnable)
	uint32_t fn_menu_mask;          // RebuildLearnedMenuAbilityMask()
	uint32_t fn_learned_popup;      // the "Learned X!" window
	uint32_t fn_draw_learn_status;  // AP progress line in the GF menu
	uint32_t fn_gf_battle_stats;    // computeGFBattleStats(gf)
	uint32_t fn_reset_parse_chara;  // ResetAndParseBattleAndFieldCharacter(slot)
	uint32_t fn_stat_percent_bonus; // GetCharaStatPercentBonus(slot, stat)
	uint32_t fn_add_ap;             // GF_AddApToLearningAbility(gf, ap)
	uint32_t fn_junction_menu;      // StatusJunctionMenuHandler
	uint32_t fn_gf_summary;         // Menu_BuildGFJunctionSummary
	uint32_t fn_draw_list_row;      // Menu_DrawAbilityListRow
	uint32_t fn_validate_passives;  // Menu_ValidateCharaPassiveAbilities
	uint32_t fn_chara_ability_lists; // Menu_BuildCharaAbilityMaskAndLists
	uint32_t fn_junction_ability_page; // the junction menu's ability page
	uint32_t fn_ability_entry_ptr;  // id -> entry pointer, called by the refine menu
	uint32_t fn_call_shop_list;     // the Call Shop menu's ability list
	uint32_t fn_menu_list_refresh;  // rebuilds the field menu ability ids
	uint32_t fn_menu_list_init;     // Call Shop menu initialization
	uint32_t fn_validate_commands;  // drops equipped command abilities outside the command group
	uint32_t fn_draw_seal_row;      // junction menu ability row, greyed out by map seals
	uint32_t fn_draw_seal_list;     // junction window command list, greyed out by map seals

	uint32_t command_candidates;    // {id, group}[20] the junction menu builds
	uint32_t passive_candidates;    // {id, group}[48], right behind it
	uint32_t menu_candidates;       // field menu ability ids, 24 bytes in vanilla
} ability_ext;

static uint8_t ff8_ability_table[MAX_ABILITY_COUNT][ABILITY_ENTRY_SIZE];
static int ff8_ability_count = VANILLA_ABILITY_COUNT;
static uint8_t ff8_group_first[ABILITY_GROUP_COUNT];
// The two candidate lists, FFNx-side and wide enough for the whole id space.
static uint8_t ff8_command_candidates[MAX_ABILITY_COUNT][CANDIDATE_ENTRY_SIZE];
static uint8_t ff8_passive_candidates[MAX_ABILITY_COUNT][CANDIDATE_ENTRY_SIZE];
static uint8_t ff8_menu_candidates[MAX_MENU_ABILITIES];
static bool ff8_ability_armed = false;
static bool ff8_ability_checked = false;
static bool ff8_ability_supported = false;

// Operand offsets inside their owning functions, verified against each retail layout.
struct ability_site { uint32_t *owner; uint32_t offset; const char *what; };

static const ability_site array_sites[] = {
	{ &ability_ext.fn_get_name, 0x00D, "getAbilityName junction" },
	{ &ability_ext.fn_get_name, 0x039, "getAbilityName command" },
	{ &ability_ext.fn_get_name, 0x065, "getAbilityName stat percent" },
	{ &ability_ext.fn_get_name, 0x091, "getAbilityName character" },
	{ &ability_ext.fn_get_name, 0x0BD, "getAbilityName party" },
	{ &ability_ext.fn_get_name, 0x0E7, "getAbilityName GF/menu" },
	{ &ability_ext.fn_get_desc, 0x00D, "getAbilityDescription junction" },
	{ &ability_ext.fn_get_desc, 0x039, "getAbilityDescription command" },
	{ &ability_ext.fn_get_desc, 0x065, "getAbilityDescription stat percent" },
	{ &ability_ext.fn_get_desc, 0x091, "getAbilityDescription character" },
	{ &ability_ext.fn_get_desc, 0x0BD, "getAbilityDescription party" },
	{ &ability_ext.fn_get_desc, 0x0E7, "getAbilityDescription GF/menu" },
	{ &ability_ext.fn_reset_parse_chara, 0x0D3, "junction flags byte 2" },
	{ &ability_ext.fn_reset_parse_chara, 0x0DA, "junction flags byte 0" },
	{ &ability_ext.fn_reset_parse_chara, 0x0E1, "junction flags byte 1" },
	{ &ability_ext.fn_reset_parse_chara, 0x122, "character ability flags" },
	{ &ability_ext.fn_reset_parse_chara, 0x226, "character ability flags" },
	{ &ability_ext.fn_ability_entry_ptr, 0x007, "ability id to entry pointer" },
	{ &ability_ext.fn_call_shop_list,    0x20B, "call shop ability availability" },
	{ &ability_ext.fn_stat_percent_bonus, 0x030, "stat percent stat id" },
	{ &ability_ext.fn_stat_percent_bonus, 0x03D, "stat percent value" },
	{ &ability_ext.fn_add_ap, 0x067, "AP required" },
	{ &ability_ext.fn_junction_menu, 0x4F1, "junction menu flags" },
	{ &ability_ext.fn_junction_menu, 0x22C7, "junction menu flags" },
	{ &ability_ext.fn_junction_menu, 0x2521, "junction menu flags" },
	{ &ability_ext.fn_junction_menu, 0x2712, "junction menu flags" },
	{ &ability_ext.fn_gf_summary, 0x07C, "granted junction flags" },
	{ &ability_ext.fn_draw_list_row, 0x07B, "ability row availability" },
	{ &ability_ext.fn_gf_battle_stats, 0x051, "effect loop start (id 64)" },
};

// The GF group's two range bounds in computeGFBattleStats: pointers at the
// stat_to_increase byte of the first GF and of the first menu ability.
#define SITE_GF_RANGE_LOW   0x076
#define SITE_GF_RANGE_HIGH  0x07D

// The inlined group chain: six "is the id past the start of group N" compares,
// in id order, repeated in five functions. The immediate sits at insn + 2.
struct chain_sites { uint32_t *owner; uint32_t offset[ABILITY_GROUP_COUNT - 1]; const char *what; };

// The popup's row is filled from the layout in find_externals; the other four are the
// same everywhere.
static chain_sites group_chains[] = {
	{ &ability_ext.fn_get_name,      { 0x004, 0x030, 0x05C, 0x088, 0x0B4, 0x0E0 }, "getAbilityName" },
	{ &ability_ext.fn_get_desc,      { 0x004, 0x030, 0x05C, 0x088, 0x0B4, 0x0E0 }, "getAbilityDescription" },
	{ &ability_ext.fn_group_from_id, { 0x004, 0x00C, 0x017, 0x022, 0x02D, 0x03A }, "getAbilityGroupFromId" },
	{ &ability_ext.fn_build_list,    { 0x21F, 0x234, 0x240, 0x24C, 0x258, 0x266 }, "BuildGFAbilityList" },
	{ &ability_ext.fn_learned_popup, { 0, 0, 0, 0, 0, 0 }, "learned popup icon" },
};

// Standalone constants: an id checked on its own, written as a group's first id
// plus a distance into that group (Magic, GF, Draw, Item are the first command
// abilities). The immediate sits at insn + 2. An optional site only greys out a
// menu entry, so a build where it does not match skips it instead of disabling
// the whole extension.
struct group_bound_site { uint32_t *owner; uint32_t offset; int group; int delta; bool optional; const char *what; };

static const group_bound_site group_bound_sites[] = {
	{ &ability_ext.fn_validate_passives,   0x04F, GROUP_STAT_PERCENT, 0, false, "equippable passives start" },
	{ &ability_ext.fn_validate_passives,   0x054, GROUP_GF,           0, false, "equippable passives end" },
	{ &ability_ext.fn_chara_ability_lists, 0x0B8, GROUP_STAT_PERCENT, 0, false, "passive candidate list start" },
	{ &ability_ext.fn_chara_ability_lists, 0x114, GROUP_GF,           0, false, "passive candidate list end" },
	{ &ability_ext.fn_gf_summary,          0x074, GROUP_COMMAND,      0, false, "junction ability check" },
	// The battle character setup: equipped abilities become flags and commands.
	{ &ability_ext.fn_reset_parse_chara,   0x0C2, GROUP_CHARACTER,    0, false, "character abilities start" },
	{ &ability_ext.fn_reset_parse_chara,   0x0C7, GROUP_PARTY,        0, false, "character abilities end" },
	{ &ability_ext.fn_reset_parse_chara,   0x115, GROUP_PARTY,        0, false, "party abilities start" },
	{ &ability_ext.fn_reset_parse_chara,   0x11A, GROUP_GF,           0, false, "party abilities end" },
	{ &ability_ext.fn_reset_parse_chara,   0x20B, GROUP_COMMAND,      0, false, "command abilities start" },
	{ &ability_ext.fn_reset_parse_chara,   0x214, GROUP_STAT_PERCENT, 0, false, "command abilities end" },
	{ &ability_ext.fn_stat_percent_bonus,  0x021, GROUP_STAT_PERCENT, 0, false, "stat percent abilities start" },
	{ &ability_ext.fn_stat_percent_bonus,  0x026, GROUP_CHARACTER,    0, false, "stat percent abilities end" },
	// Unequips any command ability outside the group, so a stale bound erases one.
	{ &ability_ext.fn_validate_commands,   0x040, GROUP_COMMAND,      0, false, "equipped commands start" },
	{ &ability_ext.fn_validate_commands,   0x045, GROUP_STAT_PERCENT, 0, false, "equipped commands end" },
	// Map seals grey out Magic, GF, Draw, Item (two ids) and every other command.
	{ &ability_ext.fn_draw_seal_row,       0x13C, GROUP_COMMAND,      0, true,  "seal row commands start" },
	{ &ability_ext.fn_draw_seal_row,       0x141, GROUP_STAT_PERCENT, 0, true,  "seal row commands end" },
};

struct command_role_site { uint32_t *owner; uint32_t offset; int role; const char *what; };
static const uint8_t command_roles[] = { 2, 3, 6, 4, 0 }; // Magic, GF, Draw, Item, Empty
static const command_role_site command_role_sites[] = {
	{ &ability_ext.fn_draw_seal_row, 0x0D7, 4, "seal row item" },
	{ &ability_ext.fn_draw_seal_row, 0x0DC, 3, "seal row item" },
	{ &ability_ext.fn_draw_seal_row, 0x0F4, 0, "seal row magic" },
	{ &ability_ext.fn_draw_seal_row, 0x10C, 1, "seal row GF" },
	{ &ability_ext.fn_draw_seal_row, 0x124, 2, "seal row draw" },
	{ &ability_ext.fn_draw_seal_row, 0x146, 0, "seal row not magic" },
	{ &ability_ext.fn_draw_seal_row, 0x14B, 1, "seal row not GF" },
	{ &ability_ext.fn_draw_seal_row, 0x150, 2, "seal row not draw" },
	{ &ability_ext.fn_draw_seal_row, 0x155, 4, "seal row not item" },
	{ &ability_ext.fn_draw_seal_row, 0x15A, 3, "seal row not item" },
	{ &ability_ext.fn_draw_seal_list, 0x098, 0, "seal list magic" },
	{ &ability_ext.fn_draw_seal_list, 0x0B0, 1, "seal list GF" },
	{ &ability_ext.fn_draw_seal_list, 0x0C8, 2, "seal list draw" },
	{ &ability_ext.fn_draw_seal_list, 0x0E0, 4, "seal list item" },
	{ &ability_ext.fn_draw_seal_list, 0x0E5, 3, "seal list item" },
	{ &ability_ext.fn_draw_seal_list, 0x0FD, 0, "seal list not magic" },
	{ &ability_ext.fn_draw_seal_list, 0x102, 1, "seal list not GF" },
	{ &ability_ext.fn_draw_seal_list, 0x107, 2, "seal list not draw" },
	{ &ability_ext.fn_draw_seal_list, 0x10C, 4, "seal list not item" },
	{ &ability_ext.fn_draw_seal_list, 0x111, 3, "seal list not item" },
};

// Optional sites whose function did not hold the expected constants.
static bool ff8_seal_row_ok = true;
static bool ff8_seal_list_ok = true;

// Vanilla mask routine signature.
#define SITE_MENU_FIRST_CMP     0x040

// Every operand pointing into one of the two candidate lists. Some address the
// list itself, some its second byte, so each is moved by whatever it pointed at.
struct candidate_site { uint32_t *owner; uint32_t offset; bool passive; const char *what; };

static const candidate_site candidate_sites[] = {
	{ &ability_ext.fn_junction_menu,         0x4E8, false, "junction menu command list" },
	{ &ability_ext.fn_junction_menu,         0x2518, false, "junction menu command list" },
	{ &ability_ext.fn_junction_menu,         0x2709, false, "junction menu command list" },
	{ &ability_ext.fn_junction_menu,         0x28DB, false, "junction menu command list" },
	{ &ability_ext.fn_junction_menu,         0x5186, false, "junction menu command list rebuild" },
	{ &ability_ext.fn_chara_ability_lists,   0x079, false, "command list build" },
	{ &ability_ext.fn_junction_ability_page, 0x0A5, false, "ability page command list" },
	{ &ability_ext.fn_junction_menu,         0x4C2, true,  "junction menu passive list" },
	{ &ability_ext.fn_junction_menu,         0x24F6, true,  "junction menu passive list" },
	{ &ability_ext.fn_junction_menu,         0x26E7, true,  "junction menu passive list" },
	{ &ability_ext.fn_junction_menu,         0x28D0, true,  "junction menu passive list" },
	{ &ability_ext.fn_junction_menu,         0x51E4, true,  "junction menu passive list rebuild" },
	{ &ability_ext.fn_chara_ability_lists,   0x0D5, true,  "passive list build" },
	{ &ability_ext.fn_junction_ability_page, 0x086, true,  "ability page passive list" },
};

// The two reads of an entry's AP field through the group table; their operand
// is the kernel buffer + 4, not a pointer into the array.
static const ability_site ap_base_sites[] = {
	{ &ability_ext.fn_build_list, 0x2A5, "BuildGFAbilityList AP" },
	{ &ability_ext.fn_draw_learn_status, 0x13A, "GF menu AP" },
};

// The field menu stores one byte per learned menu ability. Its vanilla 24-byte
// buffer is immediately followed by the counts, so both writers and all readers
// must move before either loop can grow to 32.
static const ability_site menu_candidate_sites[] = {
	{ &ability_ext.fn_menu_list_refresh, 0x00D, "field menu list refresh" },
	{ &ability_ext.fn_menu_list_init,    0x040, "field menu list initialization" },
	{ &ability_ext.fn_call_shop_list,    0x1B9, "field menu description" },
	{ &ability_ext.fn_call_shop_list,    0x204, "field menu selected ability" },
	{ &ability_ext.fn_call_shop_list,    0x2A4, "field menu refine ability" },
	{ &ability_ext.fn_call_shop_list,    0x3AF, "field menu previous page" },
	{ &ability_ext.fn_call_shop_list,    0x4FC, "field menu next page" },
	{ &ability_ext.fn_draw_list_row,     0x06E, "field menu ability row" },
};

struct menu_list_sites { uint32_t *owner; uint32_t first_id; uint32_t count; };
static const menu_list_sites menu_list_builders[] = {
	{ &ability_ext.fn_menu_list_refresh, 0x025, 0x02E },
	{ &ability_ext.fn_menu_list_init,    0x058, 0x061 },
};

static uint32_t ability_array_end()
{
	return ability_ext.k_ability + VANILLA_ABILITY_COUNT * ABILITY_ENTRY_SIZE;
}

// Where a vanilla candidate list starts, and where it moves to.
static void candidate_range(bool passive, uint32_t *from, uint32_t *to)
{
	*from = passive ? ability_ext.passive_candidates : ability_ext.command_candidates;
	*to = (uint32_t)(passive ? &ff8_passive_candidates[0][0] : &ff8_command_candidates[0][0]);
}

// One past the last byte an operand into that list may hold. The command list
// ends where the passive one starts; the passive one runs to the GF summary
// table, which no operand of either list ever reaches.
static uint32_t candidate_end(bool passive)
{
	return passive
		? ability_ext.passive_candidates + MAX_ABILITY_COUNT * CANDIDATE_ENTRY_SIZE
		: ability_ext.passive_candidates;
}

static uint32_t ability_table_base()
{
	return (uint32_t)&ff8_ability_table[0][0];
}

// Resolve feature-private addresses from the existing FFNx anchors.
static void ff8_kernel_ability_find_externals()
{
	ability_ext.kernel_buffer = uint32_t(ff8_externals.unk_1CF3E48);
	ability_ext.k_ability = ability_ext.kernel_buffer + ABILITY_BLOCK_OFFSET;

	// Call chains.
	ability_ext.fn_menu_mask = get_relative_call(uint32_t(ff8_externals.menu_use_items_sub_4F81F0), layout_of(version).menu_mask_call);
	ability_ext.learned_menu_mask = ability_ext.fn_menu_mask ? (uint32_t *)get_absolute_value(ability_ext.fn_menu_mask, 0x00E) : nullptr;
	ability_ext.fn_build_list = ability_ext.fn_menu_mask ? get_relative_call(ability_ext.fn_menu_mask, 0x24) : 0;
	ability_ext.fn_reset_parse_chara = get_relative_call(ff8_externals.sub_48B7E0, 0x94);
	ability_ext.fn_stat_percent_bonus = ability_ext.fn_reset_parse_chara ? get_relative_call(ability_ext.fn_reset_parse_chara, 0x34C) : 0;

	// The ability text getters sit just before the magic one, in the same
	// family of kernel-text helpers.
	uint32_t magic_name_getter = ff8_kernel_magic_name_getter();
	ability_ext.fn_get_name = magic_name_getter ? magic_name_getter - 0x260 : 0;
	ability_ext.fn_get_desc = magic_name_getter ? magic_name_getter - 0x130 : 0;

	// Deltas from BuildGFAbilityList, per the build's layout.
	uint32_t list = ability_ext.fn_build_list;
	const ability_layout_offsets &layout = layout_of(version);

	ability_ext.fn_group_from_id = list ? list + layout.group_from_id : 0;
	ability_ext.fn_learned_popup = list ? list + layout.learned_popup : 0;
	ability_ext.fn_draw_learn_status = list ? list + layout.draw_learn_status : 0;
	ability_ext.fn_validate_passives = list ? list + layout.validate_passives : 0;
	ability_ext.fn_junction_menu = list ? list + layout.junction_menu : 0;
	ability_ext.fn_chara_ability_lists = list ? list + layout.chara_ability_lists : 0;
	ability_ext.fn_gf_summary = list ? list + layout.gf_summary : 0;
	ability_ext.fn_draw_list_row = list ? list + layout.draw_list_row : 0;

	for (auto &chain : group_chains)
		if (chain.owner == &ability_ext.fn_learned_popup)
			memcpy(chain.offset, layout.popup_chain, sizeof(chain.offset));

	ability_ext.fn_junction_ability_page = list ? list + layout.junction_ability_page : 0;
	// Refine and Call Shop use their own ability lookup helpers.
	ability_ext.fn_ability_entry_ptr = list ? list + layout.ability_entry_ptr : 0;
	ability_ext.fn_call_shop_list = list ? list + layout.call_shop_list : 0;
	ability_ext.fn_menu_list_refresh = ability_ext.fn_call_shop_list ? ability_ext.fn_call_shop_list - 0x110 : 0;
	ability_ext.fn_menu_list_init = ability_ext.fn_call_shop_list ? ability_ext.fn_call_shop_list - 0xD0 : 0;
	ability_ext.menu_candidates = ability_ext.fn_menu_list_refresh
		? get_absolute_value(ability_ext.fn_menu_list_refresh, 0x00D) : 0;

	// Both lists are named by a "mov reg, offset list+1" inside the function that
	// fills them, so read the bases from there rather than hardcoding them.
	if (ability_ext.fn_chara_ability_lists)
	{
		ability_ext.command_candidates = get_absolute_value(ability_ext.fn_chara_ability_lists, 0x079) - 1;
		ability_ext.passive_candidates = get_absolute_value(ability_ext.fn_chara_ability_lists, 0x0D5) - 1;

		// Map-seal callbacks sit at different deltas in the Japanese builds.
		ability_ext.fn_draw_seal_row = ability_ext.fn_chara_ability_lists + layout.seal_row;
		ability_ext.fn_draw_seal_list = ability_ext.fn_chara_ability_lists + layout.seal_list;
	}

	ability_ext.fn_validate_commands = ability_ext.fn_junction_menu ? get_relative_call(ability_ext.fn_junction_menu, 0x17C) : 0;

	// Deltas from the character stat computation.
	ability_ext.fn_gf_battle_stats = ff8_externals.compute_char_stats_sub_495960 + 0x420;
	ability_ext.fn_add_ap = ff8_externals.compute_char_stats_sub_495960 + 0x16B0;

	// The group table's address is the operand of the lea that indexes it,
	// inside BuildGFAbilityList; that operand is the table itself.
	ability_ext.group_table = list ? get_absolute_value(list, 0x28A) : 0;
}

// Validate all required sites before installing any patch.
static bool ff8_kernel_ability_validate()
{
	uint32_t lo = ability_ext.k_ability, hi = ability_array_end();
	bool ok = true;

	if (!ability_ext.fn_build_list || !ability_ext.fn_get_name || !ability_ext.fn_menu_mask || !ability_ext.learned_menu_mask || !ff8_externals.savemap)
	{
		if (trace_all) ffnx_trace("AddMoreAbility: could not resolve the ability functions, extension disabled.\n");
		return false;
	}

	for (const ability_site &site : array_sites)
	{
		uint32_t value = get_absolute_value(*site.owner, site.offset);
		if (value < lo || value >= hi)
		{
			if (trace_all) ffnx_trace("AddMoreAbility: %s at 0x%X does not read the ability array.\n", site.what, *site.owner + site.offset);
			ok = false;
		}
	}

	for (uint32_t offset : { (uint32_t)SITE_GF_RANGE_LOW, (uint32_t)SITE_GF_RANGE_HIGH })
	{
		uint32_t value = get_absolute_value(ability_ext.fn_gf_battle_stats, offset);
		if (value < lo || value >= hi)
		{
			if (trace_all) ffnx_trace("AddMoreAbility: GF ability range bound at 0x%X is not an ability pointer.\n", ability_ext.fn_gf_battle_stats + offset);
			ok = false;
		}
	}

	if (*(uint8_t *)(ability_ext.fn_gf_battle_stats + 0x4C) != 0x1C || get_absolute_value(ability_ext.fn_gf_battle_stats, 0x5D) != 2)
		return false;
	for (const command_role_site &site : command_role_sites)
	{
		if (*site.owner && *(uint8_t *)(*site.owner + site.offset + 2) == vanilla_group_first[GROUP_COMMAND] + site.role) continue;
		bool &owner_ok = site.owner == &ability_ext.fn_draw_seal_row ? ff8_seal_row_ok : ff8_seal_list_ok;
		owner_ok = false;
	}

	// Each chain must still read as the vanilla 20/39/58/78/83/92.
	for (const chain_sites &chain : group_chains)
	{
		for (int group = 1; group < ABILITY_GROUP_COUNT; ++group)
		{
			uint32_t site = *chain.owner + chain.offset[group - 1];

			if (*(uint8_t *)(site + 2) != vanilla_group_first[group])
			{
				if (trace_all) ffnx_trace("AddMoreAbility: %s boundary %d at 0x%X is not %d.\n", chain.what, group, site, vanilla_group_first[group]);
				ok = false;
			}
		}
	}

	for (const group_bound_site &site : group_bound_sites)
	{
		int expected = vanilla_group_first[site.group] + site.delta;

		if (*site.owner && *(uint8_t *)(*site.owner + site.offset + 2) == expected) continue;

		if (site.optional)
		{
			bool &owner_ok = site.owner == &ability_ext.fn_draw_seal_row ? ff8_seal_row_ok : ff8_seal_list_ok;

			if (owner_ok && trace_all) ffnx_trace("AddMoreAbility: %s at 0x%X is not %d - map seals may grey out the wrong command there.\n", site.what, *site.owner + site.offset, expected);
			owner_ok = false;
			continue;
		}

		if (trace_all) ffnx_trace("AddMoreAbility: %s at 0x%X is not %d.\n", site.what, *site.owner + site.offset, expected);
		ok = false;
	}

	// Reject an incompatible mask routine before replacing it.
	const uint8_t menu_mask_code[] = { 0x83, 0xF8, 92, 0x7C, 0x11,
		0x83, 0xF8, 116, 0x7D, 0x0C, 0x8D, 0x48, 0xA4 };
	if (memcmp((const void *)(ability_ext.fn_menu_mask + SITE_MENU_FIRST_CMP), menu_mask_code, sizeof(menu_mask_code)))
	{
		if (trace_all) ffnx_trace("AddMoreAbility: the learned menu ability range checks are not vanilla.\n");
		ok = false;
	}

	for (const ability_site &site : menu_candidate_sites)
	{
		uint32_t value = get_absolute_value(*site.owner, site.offset);
		if (value < ability_ext.menu_candidates || value >= ability_ext.menu_candidates + VANILLA_MENU_ABILITIES)
		{
			if (trace_all) ffnx_trace("AddMoreAbility: %s at 0x%X does not point into the menu ability list.\n", site.what, *site.owner + site.offset);
			ok = false;
		}
	}

	for (const menu_list_sites &sites : menu_list_builders)
	{
		const uint8_t *first = (const uint8_t *)(*sites.owner + sites.first_id);
		const uint8_t *count = (const uint8_t *)(*sites.owner + sites.count);
		if (first[0] != 0x80 || first[1] != 0xC3 || first[2] != vanilla_group_first[GROUP_MENU]
			|| count[0] != 0x83 || count[1] != 0xF9 || count[2] != VANILLA_MENU_ABILITIES)
		{
			if (trace_all) ffnx_trace("AddMoreAbility: menu ability list bounds at 0x%X are not vanilla.\n", *sites.owner);
			ok = false;
		}
	}

	for (const ability_site &site : ap_base_sites)
	{
		if (get_absolute_value(*site.owner, site.offset) != ability_ext.kernel_buffer + 4)
		{
			if (trace_all) ffnx_trace("AddMoreAbility: %s base at 0x%X is not the kernel buffer.\n", site.what, *site.owner + site.offset);
			ok = false;
		}
	}

	// The command list holds 20 entries and the passive one starts right behind it.
	if (ability_ext.passive_candidates - ability_ext.command_candidates
			!= VANILLA_COMMAND_CANDIDATES * CANDIDATE_ENTRY_SIZE)
	{
		if (trace_all) ffnx_trace("AddMoreAbility: the junction menu candidate lists sit at 0x%X and 0x%X, which is not the vanilla layout.\n", ability_ext.command_candidates, ability_ext.passive_candidates);
		ok = false;
	}

	for (const candidate_site &site : candidate_sites)
	{
		uint32_t from, to;

		candidate_range(site.passive, &from, &to);

		uint32_t value = get_absolute_value(*site.owner, site.offset);
		if (value < from || value >= candidate_end(site.passive))
		{
			if (trace_all) ffnx_trace("AddMoreAbility: %s at 0x%X does not point into that list.\n", site.what, *site.owner + site.offset);
			ok = false;
		}
	}

	// The group table must still describe the vanilla layout.
	for (int group = 0; group < ABILITY_GROUP_COUNT; ++group)
	{
		const uint8_t *row = (const uint8_t *)(ability_ext.group_table + 4 * group);
		uint16_t vanilla_offset = (uint16_t)(ABILITY_BLOCK_OFFSET + ABILITY_ENTRY_SIZE * vanilla_group_first[group]);

		if (*(const uint16_t *)row != vanilla_offset || row[2] != vanilla_group_first[group] || row[3] != ABILITY_ENTRY_SIZE)
		{
			if (trace_all) ffnx_trace("AddMoreAbility: ability group table row %d at 0x%X is not vanilla.\n", group, ability_ext.group_table + 4 * group);
			ok = false;
		}
	}

	return ok;
}

static void ff8_rebuild_learned_menu_abilities()
{
	uint32_t mask = 0;
	ff8_gf_ability_entry entries[22];

	for (int gf = 0; gf < G_FORCE_NUM; ++gf)
	{
		if (!ff8_externals.savemap->gfs[gf].exists) continue;

		int count = ((int (*)(int, ff8_gf_ability_entry *, int))ability_ext.fn_build_list)(gf, entries, 0);
		for (int i = 0; i < count; ++i)
		{
			int id = entries[i].id;
			if (id >= ff8_group_first[GROUP_MENU] && id < ff8_ability_count)
				mask |= 1u << (id - ff8_group_first[GROUP_MENU]);
		}
	}

	*ability_ext.learned_menu_mask = mask;
}

static void ff8_patch_ability_data()
{
	uint32_t k_ability = ability_ext.k_ability;
	uint32_t table = ability_table_base();

	// Preserve each field offset when redirecting reads to the FFNx table.
	for (const ability_site &site : array_sites)
	{
		uint32_t operand = *site.owner + site.offset;

		uint32_t offset = *(uint32_t *)operand - k_ability;
		if (site.owner == &ability_ext.fn_gf_battle_stats) offset = 6;
		patch_code_dword(operand, (DWORD)(table + offset));
	}

	patch_code_byte(ability_ext.fn_gf_battle_stats + 0x4C, 0x14); // CompleteAbilities[0]
	patch_code_dword(ability_ext.fn_gf_battle_stats + 0x5D, 4);   // all four learned-mask words

	// AP reads use kernel base + group offset + 8 * (id - first ID).
	// Rebase the kernel pointer so the group table still indexes our records.
	for (const ability_site &site : ap_base_sites)
	{
		uint32_t operand = *site.owner + site.offset;

		patch_code_dword(operand, (DWORD)(table + 4 - ABILITY_BLOCK_OFFSET));
	}
}

static void ff8_patch_ability_groups()
{
	uint32_t table = ability_table_base();

	// GF effect bounds point to the stat_to_increase field (+6).
	uint32_t low = ability_ext.fn_gf_battle_stats + SITE_GF_RANGE_LOW;
	uint32_t high = ability_ext.fn_gf_battle_stats + SITE_GF_RANGE_HIGH;

	patch_code_dword(low, (DWORD)(table + ff8_group_first[GROUP_GF] * ABILITY_ENTRY_SIZE + 6));
	patch_code_dword(high, (DWORD)(table + ff8_group_first[GROUP_MENU] * ABILITY_ENTRY_SIZE + 6));

	for (const chain_sites &chain : group_chains)
	{
		for (int group = 1; group < ABILITY_GROUP_COUNT; ++group)
		{
			patch_code_byte(*chain.owner + chain.offset[group - 1] + 2, ff8_group_first[group]);
		}
	}

	for (const group_bound_site &site : group_bound_sites)
	{
		if (site.owner == &ability_ext.fn_draw_seal_row && !ff8_seal_row_ok) continue;
		if (site.owner == &ability_ext.fn_draw_seal_list && !ff8_seal_list_ok) continue;

		patch_code_byte(*site.owner + site.offset + 2, ff8_group_first[site.group] + site.delta);
	}

	for (int group = 0; group < ABILITY_GROUP_COUNT; ++group)
	{
		patch_code_word(ability_ext.group_table + 4 * group, (WORD)(ABILITY_BLOCK_OFFSET + ABILITY_ENTRY_SIZE * ff8_group_first[group]));
		patch_code_byte(ability_ext.group_table + 4 * group + 2, ff8_group_first[group]);
	}
}

static void ff8_patch_ability_menus()
{
	replace_function(ability_ext.fn_menu_mask, (void *)ff8_rebuild_learned_menu_abilities);

	for (const ability_site &site : menu_candidate_sites)
	{
		uint32_t operand = *site.owner + site.offset;
		patch_code_dword(operand, (DWORD)((uint32_t)ff8_menu_candidates + (*(uint32_t *)operand - ability_ext.menu_candidates)));
	}

	for (const menu_list_sites &sites : menu_list_builders)
	{
		patch_code_byte(*sites.owner + sites.first_id + 2, ff8_group_first[GROUP_MENU]);
		patch_code_byte(*sites.owner + sites.count + 2, (unsigned char)(ff8_ability_count - ff8_group_first[GROUP_MENU]));
	}

	for (const candidate_site &site : candidate_sites)
	{
		uint32_t from, to;

		candidate_range(site.passive, &from, &to);

		uint32_t operand = *site.owner + site.offset;

		patch_code_dword(operand, (DWORD)(to + (*(uint32_t *)operand - from)));
	}
}

static void ff8_patch_command_ability_roles()
{
	uint8_t ids[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	for (int id = ff8_group_first[GROUP_COMMAND]; id < ff8_group_first[GROUP_STAT_PERCENT]; ++id)
		for (int role = 0; role < 5; ++role)
			if (ids[role] == 0xFF && ff8_ability_table[id][5] == command_roles[role]) ids[role] = id;
	for (const command_role_site &site : command_role_sites)
	{
		if (site.owner == &ability_ext.fn_draw_seal_row && !ff8_seal_row_ok) continue;
		if (site.owner == &ability_ext.fn_draw_seal_list && !ff8_seal_list_ok) continue;
		patch_code_byte(*site.owner + site.offset + 2, ids[site.role]);
	}
}

void ff8_kernel_ability_arm()
{
	if (ff8_ability_armed || !ff8_ability_supported) return;
	ff8_ability_armed = true;

	ff8_patch_ability_data();
	ff8_patch_ability_groups();
	ff8_patch_ability_menus();
}

bool ff8_kernel_ability_section_may_grow(int section)
{
	return section >= KERNEL_FIRST_ABIL_SEC && section < KERNEL_AFTER_ABIL_SEC;
}

bool ff8_kernel_ability_read(const char *stash, const uint32_t *offsets, int size)
{
	uint8_t first[ABILITY_GROUP_COUNT];
	int total = 0;

	if (!stash || !offsets || size < 0xE4) return false;
	for (int section = KERNEL_FIRST_ABIL_SEC; section < KERNEL_AFTER_ABIL_SEC; ++section)
	{
		if (offsets[section] < 0xE4 || offsets[section] > offsets[section + 1] || offsets[section + 1] > (uint32_t)size || (offsets[section + 1] - offsets[section]) % ABILITY_ENTRY_SIZE)
		{
			if (trace_all) ffnx_trace("AddMoreAbility: invalid ability section %d.\n", section + 1);
			return false;
		}
	}

	for (int group = 0; group < ABILITY_GROUP_COUNT; ++group)
	{
		int section = KERNEL_FIRST_ABIL_SEC + group;
		int entries = (int)((offsets[section + 1] - offsets[section]) / ABILITY_ENTRY_SIZE);

		if ((group == 0 && entries == 0) || total + entries > MAX_ABILITY_COUNT)
		{
			if (trace_all) ffnx_trace("AddMoreAbility: kernel.bin must retain ability zero and fit within %d ability IDs - ignoring the extension.\n", MAX_ABILITY_COUNT);
			return false;
		}

		first[group] = (uint8_t)total;
		total += entries;
	}

	// Vanilla files do not resolve or patch ability functions.
	if (total == VANILLA_ABILITY_COUNT && !memcmp(first, vanilla_group_first, sizeof(first)))
	{
		const uint8_t *commands = (const uint8_t *)stash + offsets[KERNEL_FIRST_ABIL_SEC + GROUP_COMMAND];
		bool roles_unchanged = true;
		for (int role = 0; role < 5; ++role) roles_unchanged &= commands[role * ABILITY_ENTRY_SIZE + 5] == command_roles[role];
		if (roles_unchanged) return false;
	}

	// Resolve and validate patches only when the ability layout differs.
	if (!ff8_ability_checked)
	{
		ff8_ability_checked = true;
		ff8_kernel_ability_find_externals();
		ff8_ability_supported = ff8_kernel_ability_validate();

		if (!ff8_ability_supported)
			if (trace_all) ffnx_trace("AddMoreAbility: this build is not supported, extension disabled.\n");
	}

	if (!ff8_ability_supported) return false;

	// Group comparisons sign-extend their byte immediates, including an empty menu group.
	if (first[GROUP_MENU] >= MAX_ABILITY_COUNT)
	{
		if (trace_all) ffnx_trace("AddMoreAbility: group start IDs must remain below %d - ignoring the extension.\n", MAX_ABILITY_COUNT);
		return false;
	}

	// A learned menu ability is one bit of a single dword.
	int menu_abilities = total - first[GROUP_MENU];

	if (menu_abilities > MAX_MENU_ABILITIES)
	{
		if (trace_all) ffnx_trace("AddMoreAbility: %d menu abilities, but only %d of them can be remembered as learned - ignoring the extension.\n", menu_abilities, MAX_MENU_ABILITIES);
		return false;
	}

	if (ff8_ability_armed && (total != ff8_ability_count || memcmp(first, ff8_group_first, sizeof(first))))
	{
		if (trace_all) ffnx_trace("AddMoreAbility: kernel.bin ability layout changed after the patches were applied - ignoring the new one.\n");
		return true;
	}

	memcpy(ff8_ability_table, stash + offsets[KERNEL_FIRST_ABIL_SEC], total * ABILITY_ENTRY_SIZE);
	memcpy(ff8_group_first, first, sizeof(first));
	ff8_ability_count = total;

	ff8_kernel_ability_arm();
	ff8_patch_command_ability_roles();

	return true;
}

void ff8_kernel_ability_init()
{
	// The shared kernel loader activates patches only for extended ability sections.
	memcpy(ff8_group_first, vanilla_group_first, sizeof(ff8_group_first));
}
