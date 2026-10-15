#pragma once
#define NOMINMAX
#include <windows.h>
#include <cstdint>
#include <cstdarg>
#include <cstring>
#include "src/ff8/save_data.h"
#include "production_types.h"
int audit_printf(const char*, ...);

enum { VERSION_FF8_12_JP = 1, VERSION_FF8_12_JP_NV, VERSION_FF8_12_DE,
 VERSION_FF8_12_DE_NV, VERSION_FF8_12_SP, VERSION_FF8_12_SP_NV,
 VERSION_FF8_12_FR, VERSION_FF8_12_FR_NV, VERSION_FF8_12_IT, VERSION_FF8_12_IT_NV };
static int version = 0;
static bool trace_all = true;
static unsigned trace_count = 0;
static uint32_t test_magic_name_getter = 0x47E970;
struct HarnessExternals {
 char* unk_1CF3E48;
 uint32_t menu_use_items_sub_4F81F0;
 uint32_t sub_48B7E0;
 uint32_t compute_char_stats_sub_495960;
 savemap_ff8* savemap;
 uint32_t (*sm_pc_read)(char*,char*);
};
static HarnessExternals ff8_externals = { (char*)0x1CF3E48, 0x4F81F0, 0x48B7E0, 0x495960, nullptr, nullptr };
static uint32_t ff8_kernel_magic_name_getter() { return test_magic_name_getter; }
static uint32_t get_relative_call(uint32_t fn, uint32_t off) {
 auto p = fn + off;
 if (*(uint8_t*)p != 0xE8) { audit_printf("Unexpected call opcode at %08X\n", p); return 0; }
 return p + 5 + *(int32_t*)(p + 1);
}
static uint32_t get_absolute_value(uint32_t base, uint32_t off) { return *(uint32_t*)(base+off); }
static uint32_t replace_function(uint32_t base, void* fn) { *(uint8_t*)base=0xE9; *(uint32_t*)(base+1)=(uint32_t)fn-base-5; return 0; }
static void patch_code_byte(uint32_t p, uint8_t v) { *(uint8_t*)p = v; }
static void patch_code_word(uint32_t p, uint16_t v) { *(uint16_t*)p = v; }
static void patch_code_dword(uint32_t p, uint32_t v) { *(uint32_t*)p = v; }
void ffnx_trace(const char* fmt, ...);

void ffnx_warning(const char* fmt, ...);
