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

#include "kernel_item.h"
#include "kernel.h"
#include "item_inventory.h"
#include "../ff8.h"
#include "../common.h"
#include "../globals.h"
#include "../log.h"
#include "../patch.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace
{
constexpr int first_added_item = 199;
constexpr int vanilla_battle_count = 33;
// 255 is the battle random-item helper's "no item" result. It remains available
// for field items; added battle items use global IDs 199-254.
constexpr int max_battle_records = vanilla_battle_count + 56;
constexpr int battle_slots = 96;

enum class item_target { inventory, battle_items, live_items, list_pending, buy_prices, sell_prices, shop_quantities, constant, count };

#include "kernel_item_layouts.h"

item_layout item_layout_of(int game_version)
{
	switch (game_version)
	{
	case VERSION_FF8_12_FR:
	case VERSION_FF8_12_FR_NV: return layout_fr;
	case VERSION_FF8_12_DE:
	case VERSION_FF8_12_DE_NV: return layout_de;
	case VERSION_FF8_12_SP:
	case VERSION_FF8_12_SP_NV: return layout_es;
	case VERSION_FF8_12_IT:
	case VERSION_FF8_12_IT_NV: return layout_it;
	case VERSION_FF8_12_JP:
	case VERSION_FF8_12_JP_NV: return layout_jp;
	default: return layout_en;
	}
}

// Rendering reads all 11 rows even on the final partial page. Keep those reads
// inside zeroed storage, while all inventory operations use only slots 0-254.
struct inventory_storage
{
	ff8_item_inventory items;
	std::array<savemap_ff8_item, FF8_ITEM_PAGE_SIZE * FF8_ITEM_PAGE_COUNT - FF8_ITEM_SLOT_COUNT> padding;
} inventory_buffer{};
static_assert(offsetof(inventory_storage, padding) == FF8_ITEM_SLOT_COUNT * sizeof(savemap_ff8_item));
auto &inventory = inventory_buffer.items;
std::array<ff8_kernel_battle_item, FF8_ITEM_ID_COUNT> battle_records{};
std::array<ff8_kernel_non_battle_item, 223> non_battle_records{};
struct live_item { uint8_t id, amount, battle_flag, target_info, selectable; };
static_assert(sizeof(live_item) == 5);
std::array<live_item, battle_slots> live_items{};
// The battle list window marks rows already picked; it is the only part of the window
// state indexed by row.
std::array<uint8_t, battle_slots> list_pending{};
std::array<uint32_t, FF8_ITEM_ID_COUNT> buy_prices{}, sell_prices{};
std::array<uint8_t, FF8_ITEM_ID_COUNT> shop_quantities{};
bool active = false;
int item_count = first_added_item;
int battle_record_count = vanilla_battle_count;

uint32_t fn[fn_count];
item_layout layout = layout_en;
uint32_t native_base[size_t(item_target::count)];
uint32_t menu_hook;
uint8_t *current_menu = nullptr;

// Original targets of the redirected calls.
uint32_t save_image_original, load_inventory_original, new_game_original, read_file_original,
	navigation_original, navigation_target_original, load_file_original, random_original;

uint32_t site_address(const item_site &site)
{
	const uint16_t offset = site.offset.at[layout];
	return offset && fn[site.fn] ? fn[site.fn] + offset : 0;
}

uint32_t call_address(const item_call &call)
{
	const uint16_t offset = call.offset.at[layout];
	return offset && fn[call.fn] ? fn[call.fn] + offset : 0;
}

uint32_t read_operand(uint32_t address, uint8_t width)
{
	switch (width)
	{
	case 1: return *reinterpret_cast<const uint8_t *>(address);
	case 2: return *reinterpret_cast<const uint16_t *>(address);
	default: return *reinterpret_cast<const uint32_t *>(address);
	}
}

uint32_t expanded_base(item_target target)
{
	switch (target)
	{
	case item_target::inventory: return reinterpret_cast<uint32_t>(inventory.data());
	case item_target::battle_items: return reinterpret_cast<uint32_t>(battle_records.data());
	case item_target::live_items: return reinterpret_cast<uint32_t>(live_items.data());
	case item_target::list_pending: return reinterpret_cast<uint32_t>(list_pending.data());
	case item_target::buy_prices: return reinterpret_cast<uint32_t>(buy_prices.data());
	case item_target::sell_prices: return reinterpret_cast<uint32_t>(sell_prices.data());
	case item_target::shop_quantities: return reinterpret_cast<uint32_t>(shop_quantities.data());
	default: return 0;
	}
}

void find_item_functions()
{
	layout = item_layout_of(version);
	std::fill(std::begin(fn), std::end(fn), 0u);
	item_find_roots(fn);
	for (const auto &ref : item_fn_refs)
	{
		const uint16_t offset = ref.offset.at[layout];
		if (!offset || !fn[ref.from]) continue;
		fn[ref.fn] = ref.call ? get_relative_call(fn[ref.from], offset) : get_absolute_value(fn[ref.from], offset);
	}
}

// Every check names what it found: another mod (a hext, a DLL) patching the same code is the
// usual reason, and the log has to say where to look.
bool unsupported(const char *what, uint32_t address, uint32_t found, uint32_t expected)
{
	ffnx_error("AddMoreItems: %s at 0x%X is 0x%X, expected 0x%X (another mod patching this code?); items stay vanilla.\n", what, address, found, expected);
	return false;
}

// All calls of a group must still call one same function: that function is what the
// redirection forwards to.
bool calls_agree(const item_call *calls, size_t count, uint32_t &target)
{
	target = 0;
	for (size_t i = 0; i < count; ++i)
	{
		const uint32_t address = call_address(calls[i]);
		if (!address) continue;
		const uint8_t opcode = *reinterpret_cast<const uint8_t *>(address);
		const uint32_t called = opcode == 0xE8 ? get_relative_call(address, 0) : 0;
		if (!called) return unsupported("call site", address, opcode, 0xE8);
		if (target && called != target) return unsupported("call site", address, called, target);
		target = called;
	}
	return target != 0;
}

template<size_t N>
bool calls_agree(const item_call (&calls)[N], uint32_t &target) { return calls_agree(calls, N, target); }

bool supported()
{
	find_item_functions();
	for (int i = 0; i < fn_count; ++i)
		if (!fn[i] && (i != fn_jp_shop_row_item || layout == layout_jp))
		{
			ffnx_error("AddMoreItems: native item function %d not found; items stay vanilla.\n", i);
			return false;
		}

	// Native arrays: the ones FFNx knows, then the others from their first operand.
	std::fill(std::begin(native_base), std::end(native_base), 0u);
	native_base[size_t(item_target::inventory)] = reinterpret_cast<uint32_t>(ff8_externals.savemap->items.items);
	native_base[size_t(item_target::battle_items)] = reinterpret_cast<uint32_t>(ff8_externals.kernel->battle_items);
	native_base[size_t(item_target::shop_quantities)] = reinterpret_cast<uint32_t>(ff8_externals.menu_shop_staged_items_1D8D058.data());
	for (const auto &site : item_sites)
	{
		const uint32_t address = site_address(site);
		if (!address || site.target == item_target::constant) continue;
		uint32_t &base = native_base[size_t(site.target)];
		if (!base) base = read_operand(address, site.width) - site.native;
	}
	for (const auto &site : item_sites)
	{
		const uint32_t address = site_address(site);
		if (!address) continue;
		const uint32_t expected = site.target == item_target::constant ? site.native : native_base[size_t(site.target)] + site.native;
		const uint32_t found = read_operand(address, site.width);
		if (found != expected) return unsupported("operand", address, found, expected);
	}

	return calls_agree(save_image_calls, save_image_original) && calls_agree(load_inventory_calls, load_inventory_original) &&
		calls_agree(new_game_calls, new_game_original) && calls_agree(reload_direct_calls, read_file_original) &&
		calls_agree(item_navigation_source_calls, navigation_original) && calls_agree(load_asset_calls, load_file_original) &&
		calls_agree(random_calls, random_original) && calls_agree(item_navigation_target_calls, navigation_target_original) &&
		(navigation_target_original == navigation_original || unsupported("call site", 0, navigation_target_original, navigation_original));
}

// An added battle record whose data (everything after the two text offsets) is all zero is
// a placeholder: that item has no battle use. Records map to IDs by position (record 33 + k
// is item 199 + k), so an editor pads the IDs before a later battle item with these.
bool battle_record_placeholder(int id)
{
	const auto bytes = reinterpret_cast<const uint8_t *>(&battle_records[id]);
	constexpr size_t data = offsetof(ff8_kernel_battle_item, attack_animation_id);
	return id >= first_added_item && std::all_of(bytes + data, bytes + sizeof(ff8_kernel_battle_item), [](uint8_t b) { return b == 0; });
}

char *item_text(int id, bool description)
{
	if (id < 0 || id >= item_count) return ff8_externals.unk_1CFF84C;
	uint16_t offset;
	ff8_kernel_section section;
	if (id < vanilla_battle_count)
	{
		const auto &entry = battle_records[id];
		offset = description ? entry.offset_item_description : entry.offset_item_name;
		section = ff8_kernel_section::battle_item_text;
	}
	else
	{
		const auto &entry = non_battle_records[id - vanilla_battle_count];
		offset = description ? entry.offset_to_item_description : entry.offset_to_item_name;
		section = ff8_kernel_section::non_battle_item_text;
	}
	if (offset == 0xFFFF) return ff8_externals.unk_1CFF84C;
	auto base = reinterpret_cast<char *>(ff8_externals.kernel);
	return base + ff8_externals.kernel->header.section_offsets[static_cast<unsigned>(section)] + offset;
}
char *__cdecl item_name(int id) { return item_text(id, false); }
char *__cdecl item_description(int id) { return item_text(id, true); }

void clamp_item_cursors(uint8_t *menu)
{
	int state = *reinterpret_cast<uint16_t *>(menu + 0x10);
	for (int offset : { 0x54, 0x58 })
	{
		if (offset == 0x58 && (state < 94 || state > 106)) continue;
		auto &cursor = *reinterpret_cast<int16_t *>(menu + offset);
		cursor = std::clamp<int16_t>(cursor, 0, FF8_ITEM_SLOT_COUNT - 1);
	}
}

int item_navigation(int pad, int rows, int row, int cursor_offset)
{
	if (current_menu)
	{
		int cursor = *reinterpret_cast<int16_t *>(current_menu + cursor_offset);
		rows = (std::min)(rows, FF8_ITEM_SLOT_COUNT - cursor / FF8_ITEM_PAGE_SIZE * FF8_ITEM_PAGE_SIZE);
		row = std::clamp(row, 0, rows - 1);
	}
	return reinterpret_cast<int(__cdecl *)(int, int, int)>(navigation_original)(pad, rows, row);
}
int __cdecl item_navigation_source(int pad, int rows, int row) { return item_navigation(pad, rows, row, 0x54); }
int __cdecl item_navigation_target(int pad, int rows, int row) { return item_navigation(pad, rows, row, 0x58); }

// Sort is an inline state-machine arm, not a separately callable native
// function. Intercept that state before its 200-byte local array is touched.
char __cdecl item_menu(int pointer)
{
	auto menu = reinterpret_cast<uint8_t *>(pointer);
	clamp_item_cursors(menu);
	if (*reinterpret_cast<uint16_t *>(menu + 0x10) == 79)
	{
		ff8_item_sort(inventory);
		*reinterpret_cast<uint16_t *>(menu + 0x10) = 3;
		menu[0x61] = 0;
	}
	unreplace_function(menu_hook);
	auto previous_menu = current_menu;
	current_menu = menu;
	char result = reinterpret_cast<char(__cdecl *)(int)>(fn[fn_item_menu])(pointer);
	current_menu = previous_menu;
	rereplace_function(menu_hook);
	clamp_item_cursors(menu);
	return result;
}

void inventory_from_savemap()
{
	inventory.fill({});
	std::copy_n(ff8_externals.savemap->items.items, FF8_ITEM_VANILLA_SLOTS, inventory.begin());
}

int __cdecl save_image(int slot, uint8_t *image)
{
	ff8_item_normalize(inventory);
	// Keep the native projection current before its CRC is computed. The full
	// inventory lives in our block; this projection aids vanilla readers/tools.
	std::copy_n(inventory.begin(), FF8_ITEM_VANILLA_SLOTS, ff8_externals.savemap->items.items);
	int result = reinterpret_cast<int(__cdecl *)(int, void *)>(save_image_original)(slot, image);
	if (result == FF8_ITEM_SAVE_IMAGE_SIZE) ff8_item_save({ image, FF8_ITEM_SAVE_IMAGE_SIZE }, inventory);
	return result;
}

void __cdecl load_inventory(const void *source, void *destination, unsigned size)
{
	reinterpret_cast<void(__cdecl *)(const void *, void *, unsigned)>(load_inventory_original)(source, destination, size);
	inventory_from_savemap();
	// Only this accepted-load call reaches us, after the vanilla checksum has
	// passed. Preview reads never change the running inventory.
	auto image = static_cast<const uint8_t *>(source) - 384;
	ff8_item_inventory saved{};
	if (ff8_item_load({ image, FF8_ITEM_SAVE_IMAGE_SIZE }, saved) &&
		std::equal(saved.begin(), saved.begin() + FF8_ITEM_VANILLA_SLOTS, inventory.begin(),
			[](const savemap_ff8_item &a, const savemap_ff8_item &b) { return a.item_id == b.item_id && a.item_quantity == b.item_quantity; }))
		inventory = saved;
	else if (trace_all)
		ffnx_trace("AddMoreItems: imported native inventory (no valid extension block).\n");
	ff8_item_normalize(inventory);
}

int __cdecl new_game()
{
	int result = reinterpret_cast<int(__cdecl *)()>(new_game_original)();
	inventory_from_savemap();
	ff8_item_normalize(inventory);
	return result;
}

enum class menu_asset { other, mitem, price };
menu_asset asset_kind(const char *name)
{
	std::string_view path(name ? name : "");
	auto split = path.find_last_of("/\\");
	if (split != std::string_view::npos) path.remove_prefix(split + 1);
	if (path == "mitem.bin") return menu_asset::mitem;
	if (path == "price.bin") return menu_asset::price;
	return menu_asset::other;
}

void copy_asset(void *destination, const void *source, int size, menu_asset kind)
{
	auto rows = static_cast<uint8_t *>(destination);
	std::fill_n(rows, FF8_ITEM_ID_COUNT * 4, uint8_t(0));
	if (kind == menu_asset::mitem)
		for (int id = 0; id < FF8_ITEM_ID_COUNT; ++id) rows[id * 4] = 10; // refine only
	if (size < 0 || size > FF8_ITEM_ID_COUNT * 4 || size % 4)
	{
		ffnx_warning("AddMoreItems: invalid %s length %d; disabled item effects/prices.\n", kind == menu_asset::mitem ? "mitem.bin" : "price.bin", size);
		return;
	}
	if (source) std::copy_n(static_cast<const uint8_t *>(source), size, rows);
	if (kind == menu_asset::mitem)
		for (int id = 0; id < size / 4; ++id)
		{
			auto row = rows + 4 * id;
			if (row[0] > 21)
			{
				std::fill_n(row, 4, uint8_t(0));
				row[0] = 10;
				if (trace_all) ffnx_trace("AddMoreItems: invalid field effect type for item %d; field use disabled.\n", id);
			}
		}
	if (size / 4 < item_count && trace_all)
		ffnx_trace("AddMoreItems: %s has %d rows for %d item IDs; missing rows use inactive defaults.\n", kind == menu_asset::mitem ? "mitem.bin" : "price.bin", size / 4, item_count);
}

int __cdecl load_asset(void **destination, char *name)
{
	menu_asset kind = asset_kind(name);
	int size = reinterpret_cast<int(__cdecl *)(void **, char *)>(load_file_original)(destination, name);
	if (kind == menu_asset::other || size <= 0) return size;
	void *replacement = external_malloc(FF8_ITEM_ID_COUNT * 4);
	if (!replacement) return 0;
	copy_asset(replacement, *destination, size, kind);
	external_free(*destination);
	*destination = replacement;
	return FF8_ITEM_ID_COUNT * 4;
}

// Menu reloads read straight into the allocation load_asset made: go through a temporary one
// so an oversized file cannot overflow it.
unsigned __cdecl reload_direct(char *name, void *destination)
{
	if (asset_kind(name) == menu_asset::other)
		return reinterpret_cast<unsigned(__cdecl *)(char *, void *)>(read_file_original)(name, destination);
	void *source = nullptr;
	int size = reinterpret_cast<int(__cdecl *)(void **, char *)>(load_file_original)(&source, name);
	copy_asset(destination, source, size, asset_kind(name));
	if (source) external_free(source);
	return FF8_ITEM_ID_COUNT * 4;
}

void *__cdecl build_battle_items()
{
	live_items.fill({});
	bool used[32]{};
	for (const auto &item : inventory)
	{
		if (!item.item_id || !item.item_quantity) continue;
		int slot = -1;
		if (item.item_id < vanilla_battle_count)
		{
			int order = ff8_externals.savemap->items.battle_order[item.item_id - 1];
			if (order >= 32 || used[order])
				for (order = 0; order < 32 && used[order]; ++order) {}
			if (order == 32) continue;
			used[order] = true;
			slot = order;
		}
		else if (item.item_id >= first_added_item && item.item_id - first_added_item < battle_record_count - vanilla_battle_count &&
			!battle_record_placeholder(item.item_id))
			slot = 32 + item.item_id - first_added_item;
		if (slot < 0) continue;
		const auto &record = battle_records[item.item_id];
		uint8_t flags = static_cast<uint8_t>(record.attack_flags_and_selectability);
		live_items[slot] = { item.item_id, item.item_quantity, record.battle_flag,
			static_cast<uint8_t>(record.target_info), static_cast<uint8_t>(((flags & 0x80) ? 1 : 0) | ((flags & 0x20) ? 0 : 2)) };
	}
	return live_items.data() + live_items.size();
}

void *__cdecl update_battle_items()
{
	for (auto &item : live_items)
	{
		const auto &record = battle_records[item.id];
		uint8_t flags = static_cast<uint8_t>(record.attack_flags_and_selectability);
		item.battle_flag = record.battle_flag;
		item.target_info = static_cast<uint8_t>(record.target_info);
		item.selectable = static_cast<uint8_t>(((flags & 0x80) ? 1 : 0) | ((flags & 0x20) ? 0 : 2));
	}
	return live_items.data() + live_items.size();
}

int __cdecl random_battle_item(int previous)
{
	if (previous) return previous;
	uint8_t eligible[battle_slots];
	int count = 0;
	for (const auto &item : live_items)
		if (item.id && (battle_records[item.id].random_select_flag & 1)) eligible[count++] = item.id;
	if (!count) return 255;
	unsigned random = reinterpret_cast<unsigned(__cdecl *)()>(random_original)();
	return eligible[random % count];
}

// The native version reads item IDs as signed bytes: IDs 128-255 index before the battle
// order. Same rule over the whole inventory: a battle item that is not carried yet takes
// the first battle-order position no carried battle item uses.
void __cdecl reserve_battle_order(int id)
{
	if (id <= 0 || id >= vanilla_battle_count) return;
	auto order = ff8_externals.savemap->items.battle_order;
	uint32_t used = 0;
	for (const auto &item : inventory)
	{
		if (!item.item_quantity || !item.item_id || item.item_id >= vanilla_battle_count) continue;
		if (item.item_id == id) return;
		used |= 1u << (order[item.item_id - 1] & 31);
	}
	int free = 0;
	while (free < 32 && (used >> free & 1)) ++free;
	for (int slot = 0; slot < 32; ++slot)
		if (order[slot] == (free < 32 ? free : 0xFF))
		{
			std::swap(order[id - 1], order[slot]);
			return;
		}
}

template<size_t N>
void redirect(const item_call (&calls)[N], void *function)
{
	for (const auto &call : calls)
		if (uint32_t address = call_address(call)) replace_call(address, function);
}

void arm()
{
	if (active) return;
	inventory_from_savemap();
	ff8_item_normalize(inventory);
	for (const auto &site : item_sites)
	{
		const uint32_t address = site_address(site);
		if (!address) continue;
		const uint32_t value = expanded_base(site.target) + site.value;
		switch (site.width)
		{
		case 1: patch_code_byte(address, uint8_t(value)); break;
		case 2: patch_code_word(address, uint16_t(value)); break;
		default: patch_code_dword(address, value); break;
		}
	}
	replace_function(fn[fn_item_name], reinterpret_cast<void *>(item_name));
	replace_function(fn[fn_item_description], reinterpret_cast<void *>(item_description));
	menu_hook = replace_function(fn[fn_item_menu], reinterpret_cast<void *>(item_menu));
	replace_function(fn[fn_build_battle_items], reinterpret_cast<void *>(build_battle_items));
	replace_function(fn[fn_update_battle_items], reinterpret_cast<void *>(update_battle_items));
	replace_function(fn[fn_random_battle_item], reinterpret_cast<void *>(random_battle_item));
	replace_function(fn[fn_reserve_battle_order], reinterpret_cast<void *>(reserve_battle_order));
	redirect(save_image_calls, reinterpret_cast<void *>(save_image));
	redirect(load_inventory_calls, reinterpret_cast<void *>(load_inventory));
	redirect(new_game_calls, reinterpret_cast<void *>(new_game));
	redirect(reload_direct_calls, reinterpret_cast<void *>(reload_direct));
	redirect(load_asset_calls, reinterpret_cast<void *>(load_asset));
	redirect(item_navigation_source_calls, reinterpret_cast<void *>(item_navigation_source));
	redirect(item_navigation_target_calls, reinterpret_cast<void *>(item_navigation_target));
	ff8_externals.menu_shop_staged_items_1D8D058 = shop_quantities;
	active = true;
	ffnx_info("AddMoreItems: enabled %d item IDs, %d inventory slots (%d pages), %d battle item records.\n", item_count, FF8_ITEM_SLOT_COUNT, FF8_ITEM_PAGE_COUNT, battle_record_count);
}
} // namespace

bool ff8_kernel_item_active() { return active; }
std::span<savemap_ff8_item> ff8_player_items()
{
	if (active) return inventory;
	return ff8_externals.savemap->items.items;
}
int ff8_kernel_item_add(int id, int quantity)
{
	if (id <= 0 || id >= item_count) return 0;
	return ff8_item_add(inventory, id, quantity);
}
bool ff8_kernel_item_section_may_grow(int section) { return section == 7 || section == 8; }

bool ff8_kernel_item_read(const ff8_kernel_file &file, bool &changed)
{
	changed = false;
	auto battle = file.section<ff8_kernel_section::battle_items>();
	auto other = file.section<ff8_kernel_section::non_battle_items>();
	bool grown = battle.size() != vanilla_battle_count || other.size() != 166;
	if (!grown && !active) return true;
	if (battle.size() < vanilla_battle_count || battle.size() > max_battle_records || other.size() < 166 || other.size() > non_battle_records.size() ||
		(battle.size() > vanilla_battle_count && first_added_item + battle.size() - vanilla_battle_count > vanilla_battle_count + other.size()))
	{
		ffnx_error("AddMoreItems: invalid item section counts; items 199-255 are ignored.\n");
		return false;
	}
	if (!active && !supported())
	{
		ffnx_warning("AddMoreItems: the item extension cannot start (see above); items 199-255 are ignored.\n");
		return false;
	}
	// Check text offsets against the file's real section boundaries, including
	// terminators, before native callers can obtain a pointer to a string.
	auto valid_text = [&](ff8_kernel_section section, uint16_t offset) {
		if (offset == 0xFFFF) return true;
		auto bytes = file.bytes(section);
		return offset < bytes.size() && std::find(bytes.begin() + offset, bytes.end(), 0) != bytes.end();
	};
	for (const auto &entry : battle)
		if (!valid_text(ff8_kernel_section::battle_item_text, entry.offset_item_name) || !valid_text(ff8_kernel_section::battle_item_text, entry.offset_item_description))
		{
			ffnx_error("AddMoreItems: invalid battle-item text offset; items 199-255 are ignored.\n");
			return false;
		}
	for (const auto &entry : other)
		if (!valid_text(ff8_kernel_section::non_battle_item_text, entry.offset_to_item_name) || !valid_text(ff8_kernel_section::non_battle_item_text, entry.offset_to_item_description))
		{
			ffnx_error("AddMoreItems: invalid item text offset; items 199-255 are ignored.\n");
			return false;
		}
	battle_records.fill({});
	non_battle_records.fill({ 0xFFFF, 0xFFFF });
	std::copy_n(battle.begin(), vanilla_battle_count, battle_records.begin());
	std::copy(battle.begin() + vanilla_battle_count, battle.end(), battle_records.begin() + first_added_item);
	std::copy(other.begin(), other.end(), non_battle_records.begin());
	item_count = vanilla_battle_count + static_cast<int>(other.size());
	battle_record_count = static_cast<int>(battle.size());
	arm();
	changed = true;
	return true;
}
