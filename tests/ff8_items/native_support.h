#pragma once
#include <windows.h>
#include <cstdint>
#include <span>

#define VERSION_FF8_12_US           5
#define VERSION_FF8_12_US_NV        6
#define VERSION_FF8_12_FR           7
#define VERSION_FF8_12_FR_NV        8
#define VERSION_FF8_12_DE           9
#define VERSION_FF8_12_DE_NV        10
#define VERSION_FF8_12_SP           11
#define VERSION_FF8_12_SP_NV        12
#define VERSION_FF8_12_IT           13
#define VERSION_FF8_12_IT_NV        14
#define VERSION_FF8_12_JP           17
#define VERSION_FF8_12_JP_NV        18

struct item_test_menu_callback { void (*func)(int); uint32_t field_4; };

// The externals the item code reads, as FFNx resolves them (release_externals.h).
struct item_test_externals
{
	savemap_ff8 *savemap;
	ff8_kernel_vanilla_data *kernel;
	char *unk_1CFF84C;
	std::span<uint8_t> menu_shop_staged_items_1D8D058;
	item_test_menu_callback *menu_callbacks;
	uint32_t add_item_to_player_sub_47ED00, battle_ai_opcode_sub_487DF0, battle_sub_4877F0, battle_sub_48D200,
		battle_sub_48FE20, loc_47D490, main_menu_controller, menu_junkshop_sub_4EA890, menu_shop_sub_4EBE40,
		menu_shop_update_gil_and_items_4EB9F0, menu_use_items_sub_4F81F0, open_battle_vibrate_vib, sub_485610,
		sub_4AD8D0, sub_4B3310, sub_4BCBE0, sub_4EA770, sub_50B2A0, sub_54FDA0, worldmap_with_fog_sub_53FAC0;
};
struct item_test_common_externals { uint32_t *execute_opcode_table; };
extern item_test_externals ff8_externals;
extern item_test_common_externals common_externals;
extern int version;
extern bool trace_all;
void ffnx_warning(const char *, ...);
void ffnx_error(const char *, ...);
void ffnx_trace(const char *, ...);
void ffnx_info(const char *, ...);
uint32_t replace_function(uint32_t, void *);
void unreplace_function(uint32_t);
void rereplace_function(uint32_t);
void replace_call(uint32_t, void *);
uint32_t get_relative_call(uint32_t, uint32_t);
uint32_t get_absolute_value(uint32_t, uint32_t);
void patch_code_byte(uint32_t, unsigned char);
void patch_code_word(uint32_t, WORD);
void patch_code_dword(uint32_t, DWORD);
void memcpy_code(uint32_t, void *, uint32_t);
void *item_test_malloc(unsigned);
void item_test_free(void *);
#define external_malloc(size) item_test_malloc(size)
#define external_free(pointer) item_test_free(pointer)
