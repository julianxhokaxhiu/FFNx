// Copyright (C) 2026 HobbitDur
// SPDX-License-Identifier: GPL-3.0-or-later

#include "kernel_item.h"
#include "kernel.h"
#include "item_inventory.h"
#ifdef FF8_ITEM_NATIVE_TEST
#include "../../tests/ff8_items/native_support.h"
#else
#include "../ff8.h"
#include "../common.h"
#include "../globals.h"
#include "../log.h"
#include "../patch.h"
#endif

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <string_view>

namespace
{
constexpr int first_added_item = 199;
constexpr int vanilla_battle_count = 33;
// 255 is the battle random-item helper's "no item" result. It remains available
// for field items; added battle items use global IDs 199-254.
constexpr int max_battle_records = vanilla_battle_count + 56;
constexpr int battle_slots = 96;
constexpr uint32_t native_inventory = 0x1CFE79C;
constexpr uint32_t native_battle_items = 0x1CF7778;
constexpr uint32_t native_live_items = 0x1D28E78;
constexpr uint32_t native_battle_ui = 0x1D768D0;
constexpr uint32_t native_buy_price = 0x1D8CD18;
constexpr uint32_t native_sell_price = 0x1D8D120;
constexpr uint32_t native_shop_quantity = 0x1D8D058;

enum class item_patch_target { inventory, battle_items, live_items, battle_ui, buy_price, sell_price, shop_quantity, constant };
struct item_patch { uint32_t address, expected, value; uint8_t width; item_patch_target target; };

// Verified instruction operands for the 2013 EN executable. Each expected value
// is checked before any item hook is installed. Other executables fail closed.
#include "kernel_item_patches.h"

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
alignas(4) std::array<uint8_t, 52 + battle_slots> battle_ui{};
std::array<uint32_t, FF8_ITEM_ID_COUNT> buy_prices{}, sell_prices{};
std::array<uint8_t, FF8_ITEM_ID_COUNT> shop_quantities{};
bool active = false;
int item_count = first_added_item;
int battle_record_count = vanilla_battle_count;
uint32_t menu_hook, asset_hook, reload_hook;
uint8_t *current_menu = nullptr;

uint32_t read_operand(const item_patch &patch)
{
	uint32_t value = 0;
	std::memcpy(&value, reinterpret_cast<const void *>(patch.address), patch.width);
	return value;
}

uint32_t patch_value(const item_patch &patch)
{
	uint32_t base = 0;
	switch (patch.target)
	{
	case item_patch_target::inventory: base = reinterpret_cast<uint32_t>(inventory.data()); break;
	case item_patch_target::battle_items: base = reinterpret_cast<uint32_t>(battle_records.data()); break;
	case item_patch_target::live_items: base = reinterpret_cast<uint32_t>(live_items.data()); break;
	case item_patch_target::battle_ui: base = reinterpret_cast<uint32_t>(battle_ui.data()); break;
	case item_patch_target::buy_price: base = reinterpret_cast<uint32_t>(buy_prices.data()); break;
	case item_patch_target::sell_price: base = reinterpret_cast<uint32_t>(sell_prices.data()); break;
	case item_patch_target::shop_quantity: base = reinterpret_cast<uint32_t>(shop_quantities.data()); break;
	case item_patch_target::constant: break;
	}
	return base + patch.value;
}

bool call_is(uint32_t site, uint32_t target)
{
	return *reinterpret_cast<const uint8_t *>(site) == 0xE8 &&
		site + 5 + *reinterpret_cast<const int32_t *>(site + 1) == target;
}

bool supported()
{
	if (!FF8_US_VERSION || reinterpret_cast<uint32_t>(ff8_externals.savemap->items.items) != native_inventory ||
		reinterpret_cast<uint32_t>(ff8_externals.menu_use_items_sub_4F81F0) != 0x4F81F0 ||
		ff8_externals.main_menu_controller != 0x4E3090 ||
		reinterpret_cast<uint32_t>(ff8_externals.kernel->battle_items) != native_battle_items) return false;
	for (const auto &patch : item_patches)
	{
		if (read_operand(patch) != patch.expected)
		{
			ffnx_warning("AddMoreItems: unsupported operand at 0x%X; no item patches applied.\n", patch.address);
			return false;
		}
	}
	for (const auto &hook : item_hook_signatures)
		if (std::memcmp(reinterpret_cast<const void *>(hook.address), hook.bytes, hook.length)) return false;
	return call_is(0x4E3BB0, 0x4E2EF0) && call_is(0x4E3C11, 0x4E2EF0) &&
		call_is(0x4E4F1E, 0x49A7B0) && call_is(0x4E4401, 0x56DA10) &&
		call_is(0x4C0BCC, reinterpret_cast<uint32_t>(ff8_externals.sm_pc_read)) &&
		call_is(0x4F831B, 0x4C0A30) && call_is(0x4F8C50, 0x4C0A30) && call_is(0x4FB8E0, 0x4C0A30);
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
	return reinterpret_cast<int(__cdecl *)(int, int, int)>(0x4C0A30)(pad, rows, row);
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
	char result = reinterpret_cast<char(__cdecl *)(int)>(0x4F81F0)(pointer);
	current_menu = previous_menu;
	rereplace_function(menu_hook);
	clamp_item_cursors(menu);
	return result;
}

int __cdecl save_image(int slot, uint8_t *image)
{
	ff8_item_normalize(inventory);
	// Keep the native projection current before its CRC is computed. The full
	// inventory lives in our block; this projection aids vanilla readers/tools.
	std::memcpy(ff8_externals.savemap->items.items, inventory.data(), FF8_ITEM_VANILLA_SLOTS * sizeof(savemap_ff8_item));
	int result = reinterpret_cast<int(__cdecl *)(int, void *)>(0x4E2EF0)(slot, image);
	if (result == FF8_ITEM_SAVE_IMAGE_SIZE) ff8_item_save({ image, FF8_ITEM_SAVE_IMAGE_SIZE }, inventory);
	return result;
}

void __cdecl load_inventory(const void *source, void *destination, unsigned size)
{
	reinterpret_cast<void(__cdecl *)(const void *, void *, unsigned)>(0x49A7B0)(source, destination, size);
	inventory.fill({});
	std::memcpy(inventory.data(), ff8_externals.savemap->items.items, FF8_ITEM_VANILLA_SLOTS * sizeof(savemap_ff8_item));
	// Only this accepted-load call reaches us, after the vanilla checksum has
	// passed. Preview reads never change the running inventory.
	auto image = static_cast<const uint8_t *>(source) - 384;
	ff8_item_inventory saved{};
	if (ff8_item_load({ image, FF8_ITEM_SAVE_IMAGE_SIZE }, saved) &&
		!std::memcmp(saved.data(), inventory.data(), FF8_ITEM_VANILLA_SLOTS * sizeof(savemap_ff8_item)))
		inventory = saved;
	else
		ffnx_trace("AddMoreItems: imported native inventory (no valid extension block).\n");
	ff8_item_normalize(inventory);
}

int __cdecl new_game()
{
	int result = reinterpret_cast<int(__cdecl *)()>(0x56DA10)();
	inventory.fill({});
	std::memcpy(inventory.data(), ff8_externals.savemap->items.items, FF8_ITEM_VANILLA_SLOTS * sizeof(savemap_ff8_item));
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
	std::memset(destination, 0, FF8_ITEM_ID_COUNT * 4);
	if (kind == menu_asset::mitem)
		for (int id = 0; id < FF8_ITEM_ID_COUNT; ++id) static_cast<uint8_t *>(destination)[id * 4] = 10; // refine only
	if (size < 0 || size > FF8_ITEM_ID_COUNT * 4 || size % 4)
	{
		ffnx_warning("AddMoreItems: invalid %s length %d; disabled item effects/prices.\n", kind == menu_asset::mitem ? "mitem.bin" : "price.bin", size);
		return;
	}
	if (source) std::memcpy(destination, source, size);
	if (kind == menu_asset::mitem)
		for (int id = 0; id < size / 4; ++id)
		{
			auto row = static_cast<uint8_t *>(destination) + 4 * id;
			if (row[0] > 21)
			{
				std::memset(row, 0, 4);
				row[0] = 10;
				ffnx_warning("AddMoreItems: invalid field effect type for item %d; field use disabled.\n", id);
			}
		}
	if (size / 4 < item_count)
		ffnx_warning("AddMoreItems: %s has %d rows for %d item IDs; missing rows use inactive defaults.\n", kind == menu_asset::mitem ? "mitem.bin" : "price.bin", size / 4, item_count);
}

int native_load_asset(void **destination, char *name)
{
	unreplace_function(asset_hook);
	int result = reinterpret_cast<int(__cdecl *)(void **, char *)>(0x4B9530)(destination, name);
	rereplace_function(asset_hook);
	return result;
}

int __cdecl load_asset(void **destination, char *name)
{
	menu_asset kind = asset_kind(name);
	int size = native_load_asset(destination, name);
	if (kind == menu_asset::other || size <= 0) return size;
	void *replacement = external_malloc(FF8_ITEM_ID_COUNT * 4);
	if (!replacement) return 0;
	copy_asset(replacement, *destination, size, kind);
	external_free(*destination);
	*destination = replacement;
	return FF8_ITEM_ID_COUNT * 4;
}

unsigned reload_asset(char *name, void *destination)
{
	void *source = nullptr;
	int size = native_load_asset(&source, name);
	copy_asset(destination, source, size, asset_kind(name));
	if (source) external_free(source);
	return FF8_ITEM_ID_COUNT * 4;
}

unsigned __cdecl reload_direct(char *name, void *destination) { return reload_asset(name, destination); }
unsigned __cdecl reload_menu(char *prefix, const char *name, char *destination)
{
	if (asset_kind(name) != menu_asset::other)
	{
		char path[1024];
		const char *directory = prefix ? prefix : reinterpret_cast<char *(__cdecl *)()>(0x482B80)();
		if (std::strlen(directory) + std::strlen(name) >= sizeof(path)) return 0;
		std::strcpy(path, directory);
		std::strcat(path, name);
		return reload_asset(path, destination);
	}
	unreplace_function(reload_hook);
	unsigned result = reinterpret_cast<unsigned(__cdecl *)(char *, const char *, char *)>(0x4BE8C0)(prefix, name, destination);
	rereplace_function(reload_hook);
	return result;
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
	unsigned random = reinterpret_cast<unsigned(__cdecl *)()>(0x48F020)();
	return eligible[random % count];
}

void arm()
{
	if (active) return;
	inventory.fill({});
	std::memcpy(inventory.data(), ff8_externals.savemap->items.items, FF8_ITEM_VANILLA_SLOTS * sizeof(savemap_ff8_item));
	ff8_item_normalize(inventory);
	std::memcpy(battle_ui.data(), reinterpret_cast<void *>(native_battle_ui), 84);
	for (const auto &patch : item_patches)
	{
		uint32_t value = patch_value(patch);
		memcpy_code(patch.address, &value, patch.width);
	}
	replace_function(0x47EA30, reinterpret_cast<void *>(item_name));
	replace_function(0x47EA90, reinterpret_cast<void *>(item_description));
	menu_hook = replace_function(0x4F81F0, reinterpret_cast<void *>(item_menu));
	asset_hook = replace_function(0x4B9530, reinterpret_cast<void *>(load_asset));
	reload_hook = replace_function(0x4BE8C0, reinterpret_cast<void *>(reload_menu));
	replace_function(0x48C6E0, reinterpret_cast<void *>(build_battle_items));
	replace_function(0x48C670, reinterpret_cast<void *>(update_battle_items));
	replace_function(0x483CA0, reinterpret_cast<void *>(random_battle_item));
	replace_call(0x4E3BB0, reinterpret_cast<void *>(save_image));
	replace_call(0x4E3C11, reinterpret_cast<void *>(save_image));
	replace_call(0x4E4F1E, reinterpret_cast<void *>(load_inventory));
	replace_call(0x4E4401, reinterpret_cast<void *>(new_game));
	replace_call(0x4C0BCC, reinterpret_cast<void *>(reload_direct));
	replace_call(0x4F831B, reinterpret_cast<void *>(item_navigation_source));
	replace_call(0x4F8C50, reinterpret_cast<void *>(item_navigation_target));
	replace_call(0x4FB8E0, reinterpret_cast<void *>(item_navigation_source));
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
		ffnx_warning("AddMoreItems: invalid item section counts; kernel load rejected.\n");
		return false;
	}
	if (!active && !supported())
	{
		ffnx_warning("AddMoreItems: item expansion requires the verified EN executable; kernel load rejected.\n");
		return false;
	}
	// Check text offsets against the file's real section boundaries, including
	// terminators, before native callers can obtain a pointer to a string.
	auto valid_text = [&](ff8_kernel_section section, uint16_t offset) {
		if (offset == 0xFFFF) return true;
		auto bytes = file.bytes(section);
		return offset < bytes.size() && std::memchr(bytes.data() + offset, 0, bytes.size() - offset);
	};
	for (const auto &entry : battle)
		if (!valid_text(ff8_kernel_section::battle_item_text, entry.offset_item_name) || !valid_text(ff8_kernel_section::battle_item_text, entry.offset_item_description))
		{
			ffnx_warning("AddMoreItems: invalid battle-item text offset; kernel load rejected.\n");
			return false;
		}
	for (const auto &entry : other)
		if (!valid_text(ff8_kernel_section::non_battle_item_text, entry.offset_to_item_name) || !valid_text(ff8_kernel_section::non_battle_item_text, entry.offset_to_item_description))
		{
			ffnx_warning("AddMoreItems: invalid item text offset; kernel load rejected.\n");
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
