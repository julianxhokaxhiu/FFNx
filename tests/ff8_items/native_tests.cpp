// Run the production module and retail machine code inside this isolated test
// process. No running game is attached, modified or launched.
#define FF8_ITEM_NATIVE_TEST
#include "../../src/ff8/kernel.cpp"
#include "../../src/ff8/kernel_item.cpp"

extern "C" uint8_t retail_image_space[0x2C00000];
item_test_externals ff8_externals{};
int version = 5;

extern "C" void *__cdecl memcpy(void *to, const void *from, size_t n) { auto d = static_cast<uint8_t *>(to); auto s = static_cast<const uint8_t *>(from); for (size_t i = 0; i < n; ++i) d[i] = s[i]; return to; }
extern "C" void *__cdecl memset(void *to, int value, size_t n) { auto d = static_cast<uint8_t *>(to); for (size_t i = 0; i < n; ++i) d[i] = static_cast<uint8_t>(value); return to; }
extern "C" void *__cdecl memmove(void *to, const void *from, size_t n) { auto d = static_cast<uint8_t *>(to); auto s = static_cast<const uint8_t *>(from); if (d < s) return memcpy(to, from, n); while (n) { --n; d[n] = s[n]; } return to; }
extern "C" int __cdecl memcmp(const void *a, const void *b, size_t n) { auto x = static_cast<const uint8_t *>(a), y = static_cast<const uint8_t *>(b); for (size_t i = 0; i < n; ++i) if (x[i] != y[i]) return x[i] - y[i]; return 0; }
extern "C" size_t __cdecl strlen(const char *s) { size_t n = 0; while (s[n]) ++n; return n; }
extern "C" char *__cdecl strcpy(char *to, const char *from) { memcpy(to, from, strlen(from) + 1); return to; }
extern "C" char *__cdecl strcat(char *to, const char *from) { strcpy(to + strlen(to), from); return to; }
extern "C" const void *__cdecl memchr(const void *p, int value, size_t n) { auto s = static_cast<const uint8_t *>(p); for (size_t i = 0; i < n; ++i) if (s[i] == value) return s + i; return nullptr; }

static void print(const char *message) { DWORD written; WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), message, static_cast<DWORD>(strlen(message)), &written, nullptr); }
static void require(bool ok, const char *message) { print(ok ? "PASS: " : "FAIL: "); print(message); print("\n"); if (!ok) ExitProcess(1); }
void ffnx_warning(const char *message, ...) { print(message); }
void ffnx_trace(const char *, ...) {}
void ffnx_info(const char *message, ...) { print(message); }
void *item_test_malloc(unsigned size) { return HeapAlloc(GetProcessHeap(), 0, size); }
void item_test_free(void *p) { HeapFree(GetProcessHeap(), 0, p); }
void memcpy_code(uint32_t address, void *data, uint32_t size) { DWORD old; VirtualProtect(reinterpret_cast<void *>(address), size, PAGE_EXECUTE_READWRITE, &old); memcpy(reinterpret_cast<void *>(address), data, size); }
struct function_backup { uint32_t address; uint8_t bytes[5]; };
static function_backup backups[32];
static unsigned backup_count;
uint32_t replace_function(uint32_t address, void *function)
{
	unsigned index = backup_count++;
	backups[index].address = address;
	memcpy(backups[index].bytes, reinterpret_cast<void *>(address), 5);
	uint8_t bytes[5] = { 0xE9 };
	*reinterpret_cast<uint32_t *>(bytes + 1) = reinterpret_cast<uint32_t>(function) - address - 5;
	memcpy_code(address, bytes, 5);
	return index;
}
void unreplace_function(uint32_t index)
{
	uint8_t bytes[5]; auto &backup = backups[index];
	memcpy(bytes, reinterpret_cast<void *>(backup.address), 5);
	memcpy_code(backup.address, backup.bytes, 5);
	memcpy(backup.bytes, bytes, 5);
}
void rereplace_function(uint32_t index) { unreplace_function(index); }
void replace_call(uint32_t address, void *function) { uint8_t bytes[5] = { 0xE8 }; *reinterpret_cast<uint32_t *>(bytes + 1) = reinterpret_cast<uint32_t>(function) - address - 5; memcpy_code(address, bytes, 5); }

struct bytes { uint8_t *data; DWORD size; };
static bytes read_file(const char *path)
{
	HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	require(file != INVALID_HANDLE_VALUE, path);
	DWORD size = GetFileSize(file, nullptr), read = 0;
	auto data = static_cast<uint8_t *>(VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	require(data && ReadFile(file, data, size, &read, nullptr) && read == size, "read complete fixture");
	CloseHandle(file); return { data, size };
}
static void map_executable(bytes file)
{
	require(reinterpret_cast<uint32_t>(retail_image_space) == 0x401000, "retail image reservation starts at 0x401000");
	DWORD old; require(VirtualProtect(retail_image_space, sizeof(retail_image_space), PAGE_EXECUTE_READWRITE, &old), "map private executable image");
	auto dos = reinterpret_cast<IMAGE_DOS_HEADER *>(file.data);
	auto nt = reinterpret_cast<IMAGE_NT_HEADERS32 *>(file.data + dos->e_lfanew);
	require(nt->OptionalHeader.ImageBase == 0x400000, "retail image base");
	auto section = IMAGE_FIRST_SECTION(nt);
	for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section)
	{
		require(section->VirtualAddress + section->Misc.VirtualSize < sizeof(retail_image_space), "retail section bounds");
		memcpy(reinterpret_cast<void *>(0x400000 + section->VirtualAddress), file.data + section->PointerToRawData, section->SizeOfRawData);
	}
	ff8_externals.savemap = reinterpret_cast<savemap_ff8 *>(0x1CFDC58);
	ff8_externals.kernel = reinterpret_cast<ff8_kernel_vanilla_data *>(0x1CF3E48);
	ff8_externals.main_menu_controller = 0x4E3090;
	ff8_externals.menu_use_items_sub_4F81F0 = reinterpret_cast<char(*)(int)>(0x4F81F0);
	ff8_externals.sm_pc_read = reinterpret_cast<uint32_t(*)(char *, void *)>(0x4C0BCC + 5 + *reinterpret_cast<int32_t *>(0x4C0BCD));
	static char empty[] = ""; ff8_externals.unk_1CFF84C = empty;
}

static unsigned __cdecl empty_string(int, int, int, int) { return reinterpret_cast<unsigned>(ff8_externals.unk_1CFF84C); }
static int __cdecl zero() { return 0; }
static int __cdecl layout_stub(int, void *out, int) { memset(out, 0, 16); return 0; }
static int __cdecl tab_stub(int, int, int current) { return current; }
static int asset_size = 199 * 4;
static bool invalid_asset_type;
static int __cdecl asset_stub(void **destination, char *)
{
	*destination = item_test_malloc(asset_size);
	memset(*destination, 0, asset_size);
	for (int i = 0; i < asset_size / 4; ++i) static_cast<uint8_t *>(*destination)[4 * i] = 7;
	if (invalid_asset_type) static_cast<uint8_t *>(*destination)[4 * 198] = 255;
	return asset_size;
}
static void redirect_original(uint32_t token, void *function)
{
	auto &backup = backups[token];
	backup.bytes[0] = 0xE9;
	*reinterpret_cast<uint32_t *>(backup.bytes + 1) = reinterpret_cast<uint32_t>(function) - backup.address - 5;
}
static LONG WINAPI exception_report(EXCEPTION_POINTERS *exception)
{
	char code[11] = "0x00000000";
	uint32_t address = reinterpret_cast<uint32_t>(exception->ExceptionRecord->ExceptionAddress);
	for (int i = 0; i < 8; ++i) code[9 - i] = "0123456789ABCDEF"[(address >> (i * 4)) & 15];
	print("FAIL: native exception at "); print(code); print("\n");
	ExitProcess(1);
}

extern "C" void audit_start()
{
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
	SetUnhandledExceptionFilter(exception_report);
	char path[MAX_PATH];
	require(GetEnvironmentVariableA("FF8_ITEM_TEST_EXE", path, sizeof(path)), "FF8_ITEM_TEST_EXE configured");
	map_executable(read_file(path));
	if (GetEnvironmentVariableA("FF8_ITEM_TEST_VANILLA_KERNEL", path, sizeof(path)))
	{
		auto vanilla = read_file(path);
		ff8_kernel_file original(reinterpret_cast<char *>(vanilla.data), vanilla.size);
		bool original_changed = true;
		require(original.valid() && ff8_kernel_item_read(original, original_changed) && !original_changed && !active,
			"vanilla kernel leaves all native item code and page counts unchanged");
	}
	require(GetEnvironmentVariableA("FF8_ITEM_TEST_KERNEL", path, sizeof(path)), "FF8_ITEM_TEST_KERNEL configured");
	auto fixture = read_file(path);
	ff8_kernel_file file(reinterpret_cast<char *>(fixture.data), fixture.size);
	require(file.valid(), "extended kernel parses");
	bool changed;
	version = 7;
	require(!ff8_kernel_item_read(file, changed) && !active, "unsupported executable rejects extension before patching");
	version = 5;
	require(supported(), "all retail patch operands and hooks validate");
	auto &first_name = *reinterpret_cast<uint16_t *>(fixture.data + file.header().section_offsets[8]);
	uint16_t original_name = first_name; first_name = 0xFFFE;
	require(!ff8_kernel_item_read(file, changed) && !active, "out-of-range item text rejects before activation");
	first_name = original_name;
	auto offsets = reinterpret_cast<uint32_t *>(fixture.data + 4);
	offsets[8] += 120;
	ff8_kernel_file oversized(reinterpret_cast<char *>(fixture.data), fixture.size);
	require(oversized.valid() && !ff8_kernel_item_read(oversized, changed) && !active, "unsupported item counts reject before activation");
	offsets[8] -= 120;
	auto first = item_patches[0]; uint32_t bad = first.expected ^ 1;
	memcpy_code(first.address, &bad, first.width);
	require(!ff8_kernel_item_read(file, changed) && !active, "modified operand rejects the entire patch set");
	memcpy_code(first.address, &first.expected, first.width);
	require(ff8_kernel_item_read(file, changed) && active && changed, "production item module activates");
	bool operands_ok = true;
	for (const auto &patch : item_patches) operands_ok &= read_operand(patch) == patch_value(patch);
	require(operands_ok, "all patch operands point to the correct expanded buffers");
	// The shared native loader supplies these file-relative text offsets.
	for (int i = 31; i < 56; ++i) ff8_externals.kernel->header.section_offsets[i] = reinterpret_cast<uint32_t>(file.bytes(static_cast<ff8_kernel_section>(i)).data()) - reinterpret_cast<uint32_t>(ff8_externals.kernel);
	require(item_name(199)[0] && item_description(255)[0] && !item_name(256)[0], "new and boundary text lookups");
	bool added = true;
	for (int id = 1; id <= 198; ++id) added &= ff8_kernel_item_add(id, 1) == 0;
	require(reinterpret_cast<int(__cdecl *)(int, char)>(0x47ED00)(199, 1) == 0 && inventory[198].item_id == 199,
		"native acquisition can allocate the first additional inventory slot");
	for (int id = 200; id <= 255; ++id) added &= ff8_kernel_item_add(id, 1) == 0;
	require(added, "populate every item ID");
	require(reinterpret_cast<int(__cdecl *)(int)>(0x56DAF0)(255) != 0, "native inventory lookup reaches ID 255 after slot 198");
	reinterpret_cast<void(__cdecl *)(int)>(0x47ECD0)(254);
	require(inventory[253].item_quantity == 0 && inventory[254].item_quantity == 1, "native removal reaches later pages without corrupting neighbors");
	inventory[253].item_quantity = 1;
	for (int i = 0; i < 32; ++i) ff8_externals.savemap->items.battle_order[i] = static_cast<uint8_t>(i);
	auto all_items = inventory;
	inventory.fill({}); inventory[254] = { 255, 1 };
	// A signed read would treat item 255 as -1 and read before battle_order.
	auto before_order = ff8_externals.savemap->items.battle_order - 2;
	uint8_t saved_before_order = *before_order; *before_order = 0;
	reinterpret_cast<void(__cdecl *)(int)>(0x4AD0C0)(1);
	require(ff8_externals.savemap->items.battle_order[0] == 0, "high item IDs do not reserve a vanilla battle-order entry");
	*before_order = saved_before_order;
	inventory = all_items;
	reinterpret_cast<uint32_t(__cdecl *)()>(0x4EBA80)();
	require(shop_quantities[199] == 1 && shop_quantities[255] == 1, "native shop staging scans all inventory slots and high item IDs");
	shop_quantities[255] = 3;
	reinterpret_cast<void(__cdecl *)(uint32_t)>(0x4EB9F0)(1234);
	require(inventory[254].item_id == 255 && inventory[254].item_quantity == 3, "native shop writeback reaches the last inventory slot");
	inventory[254] = {};
	reinterpret_cast<uint32_t(__cdecl *)()>(0x4EBA80)();
	shop_quantities[255] = 1;
	reinterpret_cast<void(__cdecl *)(uint32_t)>(0x4EB9F0)(1234);
	require(inventory[254].item_id == 255 && inventory[254].item_quantity == 1, "native shop purchase inserts a new high ID after the original capacity");
	build_battle_items();
	require(live_items[32].id == 199 && live_items[87].id == 254 && live_items[88].id == 0, "extended battle list includes 56 new battle items; 255 remains field-only");
	alignas(4) static uint8_t battle_context[128]{};
	*reinterpret_cast<void **>(0x1D6D490) = battle_context;
	std::fill(battle_ui.begin() + 52, battle_ui.end(), uint8_t{ 0xA5 });
	buy_prices[0] = 0x12345678;
	reinterpret_cast<void *(__cdecl *)(int, int, unsigned char, int, char, void *)>(0x4FF0C0)(0, 2, 13, 96, 2, reinterpret_cast<void *>(0x4C8540));
	bool reservations_clear = true;
	for (size_t i = 52; i < battle_ui.size(); ++i) reservations_clear &= battle_ui[i] == 0;
	require(battle_ui[36] == 96 && reservations_clear && buy_prices[0] == 0x12345678,
		"native battle menu initializes 96 reservations without overwriting neighboring state");
	require(reinterpret_cast<int(__cdecl *)(int)>(0x485E60)(254) == 1, "native battle quantity lookup reaches added item");
	require(reinterpret_cast<int(__cdecl *)(int, int)>(0x486B40)(254, 1) == 0 && live_items[87].amount == 0, "native battle consumption reaches added item");
	// Keep the zero-quantity ID until the native writeback finds the saved slot.
	live_items[87].id = 254;
	reinterpret_cast<void(__cdecl *)()>(0x486C10)();
	require(inventory[253].item_quantity == 0, "native battle writeback updates later inventory pages");
	inventory[253].item_quantity = 1;
	// Exercise native page-up/down and Sort through the production interception.
	replace_function(0x4BFFC0, reinterpret_cast<void *>(layout_stub));
	replace_function(0x4BD850, reinterpret_cast<void *>(zero));
	replace_function(0x4B92A0, reinterpret_cast<void *>(zero));
	replace_function(0x4BD630, reinterpret_cast<void *>(empty_string));
	replace_function(0x4C2E00, reinterpret_cast<void *>(tab_stub));
	replace_function(0x49FAD0, reinterpret_cast<void *>(zero));
	replace_function(0x4BD690, reinterpret_cast<void *>(zero));
	replace_function(0x4BE960, reinterpret_cast<void *>(zero));
	alignas(4) uint8_t menu[120]{};
	*reinterpret_cast<uint32_t *>(menu + 0x20) = reinterpret_cast<uint32_t>(inventory.data());
	*reinterpret_cast<uint16_t *>(menu + 0x10) = 6; // page up from page one
	item_menu(reinterpret_cast<int>(menu));
	require(menu[0x4A] == 23 && *reinterpret_cast<uint16_t *>(menu + 0x54) == 253, "native page-up wraps to page 24");
	*reinterpret_cast<uint16_t *>(menu + 0x10) = 8;
	item_menu(reinterpret_cast<int>(menu));
	require(menu[0x4A] == 0 && *reinterpret_cast<uint16_t *>(menu + 0x54) == 0, "native page-down wraps to page one");
	*reinterpret_cast<uint16_t *>(menu + 0x54) = 10;
	*reinterpret_cast<uint16_t *>(menu + 0x10) = 6;
	item_menu(reinterpret_cast<int>(menu));
	require(*reinterpret_cast<uint16_t *>(menu + 0x54) == 254, "page-up from row eleven clamps to the last usable row");
	*reinterpret_cast<uint16_t *>(menu + 0x58) = 10;
	*reinterpret_cast<uint16_t *>(menu + 0x10) = 101;
	item_menu(reinterpret_cast<int>(menu));
	require(*reinterpret_cast<uint16_t *>(menu + 0x58) == 254, "rearrangement target page-up clamps to the last usable row");
	for (int state : { 5, 97, 99 })
	{
		int offset = state == 99 ? 0x58 : 0x54;
		*reinterpret_cast<uint16_t *>(menu + offset) = 254;
		*reinterpret_cast<uint16_t *>(menu + 0x10) = static_cast<uint16_t>(state);
		*reinterpret_cast<uint16_t *>(0x1D76A9A) = 0x4000;
		item_menu(reinterpret_cast<int>(menu));
		require(*reinterpret_cast<uint16_t *>(menu + offset) == 253, "native item/rearrangement down wraps within the two-row final page");
		*reinterpret_cast<uint16_t *>(0x1D76A9A) = 0x1000;
		item_menu(reinterpret_cast<int>(menu));
		require(*reinterpret_cast<uint16_t *>(menu + offset) == 254, "native item/rearrangement up wraps within the two-row final page");
	}
	*reinterpret_cast<uint16_t *>(0x1D76A9A) = 0;
	*reinterpret_cast<uint16_t *>(menu + 0x54) = 0;
	*reinterpret_cast<uint16_t *>(menu + 0x58) = 263;
	*reinterpret_cast<uint16_t *>(menu + 0x10) = 100;
	item_menu(reinterpret_cast<int>(menu));
	bool padding_clear = true;
	for (const auto &item : inventory_buffer.padding) padding_clear &= !item.item_id && !item.item_quantity;
	require(inventory[254].item_id == 1 && inventory[0].item_id == 255 && padding_clear,
		"native rearrangement stays within byte-addressable inventory and leaves display padding empty");
	std::reverse(inventory.begin(), inventory.end());
	*reinterpret_cast<uint16_t *>(menu + 0x10) = 79;
	item_menu(reinterpret_cast<int>(menu));
	require(inventory[198].item_id == 199 && inventory[254].item_id == 255, "Sort state preserves all added IDs");
	static uint8_t menu_vram[0x22000]{};
	*reinterpret_cast<uint8_t **>(0xB86D30) = menu_vram;
	std::array<uint8_t, 8192> image{};
	require(save_image(0, image.data()) == 8192, "native save builder creates the extended 8 KiB image");
	uint16_t crc = static_cast<uint16_t>(reinterpret_cast<int(__cdecl *)(int, const uint8_t *)>(0x500310)(4944, image.data() + 464));
	require(*reinterpret_cast<uint16_t *>(image.data() + 384) == crc && *reinterpret_cast<uint16_t *>(image.data() + 5408) == crc, "native CRC and duplicate checksum remain valid");
	inventory.fill({});
	load_inventory(image.data() + 384, ff8_externals.savemap, 5028);
	require(inventory[254].item_id == 255, "accepted-load hook restores all pages");
	image[FF8_ITEM_SAVE_OFFSET] ^= 1;
	load_inventory(image.data() + 384, ff8_externals.savemap, 5028);
	require(inventory[197].item_id == 198 && !inventory[198].item_id, "corrupt block imports only the legacy inventory and clears stale extra slots");
	ff8_item_inventory saved_items{};
	saved_items[0] = { 1, 1 }; saved_items[200] = { 255, 1 };
	ff8_item_save(image, saved_items);
	image[384 + native_inventory - 0x1CFDC58 + 1] = 50;
	load_inventory(image.data() + 384, ff8_externals.savemap, 5028);
	require(inventory[0].item_quantity == 50 && !inventory[200].item_id, "external native-inventory edits are not overwritten by a stale extension");
	redirect_original(asset_hook, reinterpret_cast<void *>(asset_stub));
	void *asset = nullptr;
	require(load_asset(&asset, const_cast<char *>("menu/mitem.bin")) == 1024 && static_cast<uint8_t *>(asset)[199 * 4] == 10, "menu assets are allocated for all IDs and missing rows are inactive");
	item_test_free(asset);
	asset_size = 1024;
	require(load_asset(&asset, const_cast<char *>("menu/mitem.bin")) == 1024 && static_cast<uint8_t *>(asset)[255 * 4] == 7, "full extended field metadata is retained");
	*reinterpret_cast<void **>(0x1D2BB2C) = asset;
	require(reinterpret_cast<int(__cdecl *)(int)>(0x4F74F0)(255) == 7, "native field metadata lookup reaches item 255");
	auto healing_row = static_cast<uint8_t *>(asset) + 4 * 199;
	healing_row[0] = 0; healing_row[1] = 3; healing_row[2] = 2; healing_row[3] = 0;
	require(reinterpret_cast<int(__cdecl *)(int)>(0x4F7510)(199) == 2 && reinterpret_cast<int(__cdecl *)(int)>(0x4F7530)(199) == 3,
		"new field item retains its native healing parameters and target flags");
	item_test_free(asset);
	invalid_asset_type = true;
	load_asset(&asset, const_cast<char *>("menu/mitem.bin"));
	require(static_cast<uint8_t *>(asset)[198 * 4] == 10, "invalid field effect types cannot index the native dispatch table");
	item_test_free(asset);
	invalid_asset_type = false;
	alignas(4) uint8_t reloaded[1028]{}; reloaded[1024] = 0xA5;
	asset_size = 1028;
	reload_direct(const_cast<char *>("menu/mitem.bin"), reloaded);
	require(reloaded[1024] == 0xA5 && reloaded[255 * 4] == 10, "oversized reloads do not overflow existing menu allocations");
	print("PASS: native item expansion integration\n");
	ExitProcess(0);
}
