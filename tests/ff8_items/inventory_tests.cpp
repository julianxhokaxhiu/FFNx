#include "../../src/ff8/item_inventory.h"
#include <cassert>
#include <cstdio>

int main()
{
	ff8_item_inventory items{};
	for (int id = 1; id <= 255; ++id) assert(ff8_item_add(items, id, 1) == 0);
	for (int id = 1; id <= 255; ++id) assert(items[id - 1].item_id == id && items[id - 1].item_quantity == 1);
	assert(items[253].item_id == 254 && items[254].item_id == 255);
	assert(ff8_item_add(items, 255, 99) == 1 && items[254].item_quantity == 100);
	assert(ff8_item_add(items, 255, 100) == 1 && items[254].item_quantity == 100);
	auto before = items;
	assert(ff8_item_add(items, 0, 1) == 0 && ff8_item_add(items, 256, 1) == 0);
	assert(std::memcmp(items.data(), before.data(), sizeof(items)) == 0);
	std::reverse(items.begin(), items.end());
	ff8_item_sort(items);
	for (int id = 1; id <= 255; ++id) assert(items[id - 1].item_id == id);
	items[0] = { 199, 100 };
	ff8_item_sort(items);
	assert(items[197].item_id == 199 && items[197].item_quantity == 100);
	std::array<uint8_t, FF8_ITEM_SAVE_IMAGE_SIZE> save;
	save.fill(0xA5);
	ff8_item_save(save, items);
	for (size_t i = 0; i < FF8_ITEM_SAVE_OFFSET; ++i) assert(save[i] == 0xA5);
	for (size_t i = FF8_ITEM_SAVE_OFFSET + sizeof(ff8_item_save_block); i < save.size(); ++i) assert(save[i] == 0xA5);
	ff8_item_inventory loaded{};
	assert(ff8_item_load(save, loaded));
	assert(std::memcmp(items.data(), loaded.data(), sizeof(items)) == 0);
	for (size_t offset : { size_t(0), size_t(4), size_t(6), size_t(8), size_t(12), sizeof(ff8_item_save_block) - 1 })
	{
		auto damaged = save;
		damaged[FF8_ITEM_SAVE_OFFSET + offset] ^= 1;
		auto unchanged = loaded;
		assert(!ff8_item_load(damaged, loaded));
		assert(std::memcmp(loaded.data(), unchanged.data(), sizeof(loaded)) == 0);
	}
	assert(!ff8_item_load(std::span<const uint8_t>(save).first(FF8_ITEM_SAVE_OFFSET + 10), loaded));
	std::array<uint8_t, FF8_ITEM_SAVE_IMAGE_SIZE> vanilla{};
	assert(!ff8_item_load(vanilla, loaded));
	ff8_item_inventory duplicate{};
	duplicate[0] = { 1, 1 }; duplicate[1] = { 1, 1 };
	ff8_item_save(save, duplicate);
	assert(!ff8_item_load(save, loaded));
	ff8_item_inventory invalid{};
	invalid[0] = { 255, 101 };
	ff8_item_save(save, invalid);
	assert(!ff8_item_load(save, loaded));
	std::array<savemap_ff8_item, 2> full{ savemap_ff8_item{ 1, 1 }, savemap_ff8_item{ 2, 1 } };
	assert(ff8_item_add(full, 3, 1) == 1 && full[0].item_id == 1 && full[1].item_id == 2);
	ff8_item_inventory unordered{};
	unordered[0] = { 255, 90 }; unordered[1] = { 0, 10 }; unordered[2] = { 199, 0 }; unordered[3] = { 255, 20 };
	ff8_item_normalize(unordered);
	assert(unordered[0].item_id == 255 && unordered[0].item_quantity == 100);
	assert(!unordered[1].item_id && !unordered[1].item_quantity && !unordered[2].item_id && !unordered[3].item_id);
	assert(ff8_item_add(unordered, 255, -1) == 0 && unordered[0].item_quantity == 99);
	assert(ff8_item_add(unordered, 255, -128) == 0 && !unordered[0].item_id && !unordered[0].item_quantity);
	assert(ff8_item_add(unordered, 199, -1) == 0 && !unordered[0].item_id);
	std::puts("PASS: all IDs, later pages, quantity limits, sorting, save round trips, legacy saves and corrupt blocks");
}
