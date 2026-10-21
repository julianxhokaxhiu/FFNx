#pragma once
#include <windows.h>
#include <cstdint>
#include <span>

struct item_test_externals
{
	savemap_ff8 *savemap;
	char (*menu_use_items_sub_4F81F0)(int);
	uint32_t main_menu_controller;
	ff8_kernel_vanilla_data *kernel;
	uint32_t (*sm_pc_read)(char *, void *);
	char *unk_1CFF84C;
	std::span<uint8_t> menu_shop_staged_items_1D8D058;
};
extern item_test_externals ff8_externals;
extern int version;
#define FF8_US_VERSION (version == 5)
void ffnx_warning(const char *, ...);
void ffnx_trace(const char *, ...);
void ffnx_info(const char *, ...);
uint32_t replace_function(uint32_t, void *);
void unreplace_function(uint32_t);
void rereplace_function(uint32_t);
void replace_call(uint32_t, void *);
void memcpy_code(uint32_t, void *, uint32_t);
void *item_test_malloc(unsigned);
void item_test_free(void *);
#define external_malloc(size) item_test_malloc(size)
#define external_free(pointer) item_test_free(pointer)
