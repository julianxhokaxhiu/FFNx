// Copyright (C) 2026 HobbitDur
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "save_data.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <span>

// Native consumers use byte slot indices with 255 as their failure sentinel.
// The last of 24 pages therefore has two usable rows.
inline constexpr int FF8_ITEM_ID_COUNT = 256;
inline constexpr int FF8_ITEM_PAGE_SIZE = 11;
inline constexpr int FF8_ITEM_PAGE_COUNT = 24;
inline constexpr int FF8_ITEM_SLOT_COUNT = 255;
inline constexpr int FF8_ITEM_VANILLA_SLOTS = 198;

using ff8_item_inventory = std::array<savemap_ff8_item, FF8_ITEM_SLOT_COUNT>;

inline void ff8_item_sort(std::span<savemap_ff8_item> inventory)
{
	std::array<uint8_t, FF8_ITEM_ID_COUNT> amounts{};
	for (const auto &item : inventory)
		if (item.item_id && item.item_quantity)
			amounts[item.item_id] = static_cast<uint8_t>(std::min<int>(100, amounts[item.item_id] + item.item_quantity));
	std::fill(inventory.begin(), inventory.end(), savemap_ff8_item{});
	size_t slot = 0;
	for (int id = 1; id < FF8_ITEM_ID_COUNT && slot < inventory.size(); ++id)
		if (amounts[id]) inventory[slot++] = { static_cast<uint8_t>(id), amounts[id] };
}

inline void ff8_item_normalize(std::span<savemap_ff8_item> inventory)
{
	std::array<savemap_ff8_item *, FF8_ITEM_ID_COUNT> first{};
	for (auto &item : inventory)
	{
		if (!item.item_id || !item.item_quantity) { item = {}; continue; }
		item.item_quantity = std::min<uint8_t>(100, item.item_quantity);
		if (auto previous = first[item.item_id])
		{
			previous->item_quantity = static_cast<uint8_t>(std::min<int>(100, previous->item_quantity + item.item_quantity));
			item = {};
		}
		else first[item.item_id] = &item;
	}
}

inline int ff8_item_add(std::span<savemap_ff8_item> inventory, int id, int quantity)
{
	if (id <= 0 || id >= FF8_ITEM_ID_COUNT || !quantity) return 0;
	savemap_ff8_item *empty = nullptr;
	for (auto &item : inventory)
	{
		if (item.item_id == id)
		{
			int amount = std::max(0, item.item_quantity + quantity);
			item.item_quantity = static_cast<uint8_t>(std::min(100, amount));
			if (!amount) item.item_id = 0;
			return amount >= 100;
		}
		if (!item.item_id && !empty) empty = &item;
	}
	if (quantity < 0) return 0;
	if (!empty) return 1;
	*empty = { static_cast<uint8_t>(id), static_cast<uint8_t>(std::min(100, quantity)) };
	return quantity >= 100;
}

// This block belongs in the unused tail of the 8192-byte decompressed save
// image, never in field variables or inside the native savemap. It has its own
// checksum because the vanilla CRC does not cover the tail.
inline constexpr size_t FF8_ITEM_SAVE_OFFSET = 0x1600;
inline constexpr size_t FF8_ITEM_SAVE_IMAGE_SIZE = 0x2000;

#pragma pack(push, 1)
struct ff8_item_save_block
{
	uint32_t magic;
	uint16_t version;
	uint16_t slots;
	uint32_t checksum;
	ff8_item_inventory inventory;
};
#pragma pack(pop)
static_assert(sizeof(savemap_ff8_item) == 2);
static_assert(sizeof(ff8_item_save_block) == 522);
static_assert(FF8_ITEM_SAVE_OFFSET >= 384 + 5028);
static_assert(FF8_ITEM_SAVE_OFFSET + sizeof(ff8_item_save_block) <= FF8_ITEM_SAVE_IMAGE_SIZE);

inline uint32_t ff8_item_save_checksum(const ff8_item_inventory &inventory)
{
	uint32_t hash = 2166136261u;
	for (const auto &item : inventory)
	{
		hash = (hash ^ item.item_id) * 16777619u;
		hash = (hash ^ item.item_quantity) * 16777619u;
	}
	return hash;
}

inline void ff8_item_save(std::span<uint8_t> image, const ff8_item_inventory &inventory)
{
	if (image.size() < FF8_ITEM_SAVE_OFFSET + sizeof(ff8_item_save_block)) return;
	ff8_item_save_block block{ 0x4D495846u, 1, FF8_ITEM_SLOT_COUNT, ff8_item_save_checksum(inventory), inventory };
	std::memcpy(image.data() + FF8_ITEM_SAVE_OFFSET, &block, sizeof(block));
}

inline bool ff8_item_load(std::span<const uint8_t> image, ff8_item_inventory &inventory)
{
	if (image.size() < FF8_ITEM_SAVE_OFFSET + sizeof(ff8_item_save_block)) return false;
	ff8_item_save_block block{};
	std::memcpy(&block, image.data() + FF8_ITEM_SAVE_OFFSET, sizeof(block));
	if (block.magic != 0x4D495846u || block.version != 1 || block.slots != FF8_ITEM_SLOT_COUNT ||
		block.checksum != ff8_item_save_checksum(block.inventory)) return false;
	std::array<bool, FF8_ITEM_ID_COUNT> seen{};
	for (const auto &item : block.inventory)
	{
		if (item.item_quantity > 100 || (!item.item_id && item.item_quantity) ||
			(item.item_id && (!item.item_quantity || seen[item.item_id]))) return false;
		if (item.item_id) seen[item.item_id] = true;
	}
	inventory = block.inventory;
	return true;
}
