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

// -------------------------------------------------------------------------
// Monster-AI "target" opcode (0x04): make the unused target values 228-248
// select a fixed pair of battle slots, so one action hits two targets.
// 228 = slots 0+1, 229 = 0+2, ... 248 = 5+6 (every pair a<b over slots 0-6).
// FF8 has 7 battle slots: 0-2 party, 3-6 enemies (4 max) - slot 7 never
// exists. Vanilla resolves 228-248 to "no target found", so they are free.
//
// MonsterAI decodes values 200-227 with a jump-table switch:
//
//     cmp edi, 1Bh                 ; edi = value - 200
//     ja  <com-file-id scan>       ; anything else, incl. 228-248
//     xor eax, eax
//     mov al, index_table[edi]     ; value    -> case index
//     jmp jump_table[eax*4]        ; case index -> handler
//     ...                          ; case 200 (SELF) computes its mask
//     mov [esp+XX], eax            ; <- every case lands here to store it
//     jmp <opcode loop>
//
// The switch is driven by two data tables, and `eax` still holds the index
// byte when the SELF handler stores it. So we point both tables at our own
// copies and give each pair value *its slot mask* as the case index: the
// game's own `mov [esp+XX], eax` then stores the pair, and its own `jmp`
// resumes the loop. No new code, no register marshalling - three patched
// operands and two tables built from the ones already in the exe.
//
// Vanilla cases are renumbered above the highest pair mask so the two index
// spaces cannot collide; the original tables are never written to.
// -------------------------------------------------------------------------

#include "ai.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"
#include "../../common.h"
#include "../../log.h"

#include <stdint.h>

#define FF8_AI_SLOT_COUNT           7   // battle entity slots: 0-2 party, 3-6 enemies
#define FF8_AI_SWITCH_FIRST_VALUE   200 // first target value handled by the sub-switch
#define FF8_AI_VANILLA_LAST_VALUE   227 // last one vanilla accepts (the `cmp` bound)
#define FF8_AI_PAIR_FIRST_VALUE     228
#define FF8_AI_PAIR_LAST_VALUE      248
#define FF8_AI_PAIR_COUNT           (FF8_AI_PAIR_LAST_VALUE - FF8_AI_PAIR_FIRST_VALUE + 1)
#define FF8_AI_VANILLA_VALUE_COUNT  (FF8_AI_VANILLA_LAST_VALUE - FF8_AI_SWITCH_FIRST_VALUE + 1)
#define FF8_AI_INDEX_TABLE_SIZE     (FF8_AI_PAIR_LAST_VALUE - FF8_AI_SWITCH_FIRST_VALUE + 1)

// Highest mask a pair can produce, and where the renumbered vanilla cases
// start so they never share an index with one.
#define FF8_AI_MAX_PAIR_MASK        ((1 << (FF8_AI_SLOT_COUNT - 2)) | (1 << (FF8_AI_SLOT_COUNT - 1)))
#define FF8_AI_VANILLA_CASE_BASE    (FF8_AI_MAX_PAIR_MASK + 1)
#define FF8_AI_VANILLA_CASE_MAX     32  // retail builds all use 12; the cap keeps the index a byte
#define FF8_AI_JUMP_TABLE_SIZE      (FF8_AI_VANILLA_CASE_BASE + FF8_AI_VANILLA_CASE_MAX)

// Byte offsets inside the sub-switch, measured from the `cmp`.
#define FF8_AI_RANGE_IMM_OFFSET     0x02 // the `cmp` upper bound
#define FF8_AI_JA_NEXT_OFFSET       0x09 // instruction after the `ja`, for its rel32
#define FF8_AI_INDEX_TABLE_OFFSET   0x0D // disp32 of index_table[edi]
#define FF8_AI_JUMP_TABLE_OFFSET    0x14 // disp32 of jump_table[eax*4]
#define FF8_AI_STORE_MASK_OFFSET    0x21 // mov [esp+XX], eax ; jmp <opcode loop>

static_assert(FF8_AI_SLOT_COUNT * (FF8_AI_SLOT_COUNT - 1) / 2 == FF8_AI_PAIR_COUNT,
	"value range must match the number of slot pairs C(7,2)=21");
static_assert(FF8_AI_VANILLA_CASE_BASE + FF8_AI_VANILLA_CASE_MAX <= 256,
	"case indexes are read with a byte load");

// The sub-switch, from the `cmp` onwards. Every retail build emits these
// exact bytes; ANY covers the addresses and the stack displacement only.
// Matching it is what tells us an unexpected exe layout apart, and the fixed
// `cmp` bound also means an already-patched exe is never patched twice.
#define ANY 0x100
static const uint16_t ff8_ai_switch_template[] = {
	0x83, 0xFF, FF8_AI_VANILLA_LAST_VALUE - FF8_AI_SWITCH_FIRST_VALUE, // cmp edi, 1Bh
	0x0F, 0x87, ANY, ANY, ANY, ANY,                                    // ja  <com-file-id scan>
	0x33, 0xC0,                                                        // xor eax, eax
	0x8A, 0x87, ANY, ANY, ANY, ANY,                                    // mov al, index_table[edi]
	0xFF, 0x24, 0x85, ANY, ANY, ANY, ANY,                              // jmp jump_table[eax*4]
	0xB8, 0x01, 0x00, 0x00, 0x00,                                      // mov eax, 1
	0x8B, 0xCB,                                                        // mov ecx, ebx
	0xD3, 0xE0,                                                        // shl eax, cl
	0x89, 0x44, 0x24, ANY,                                             // mov [esp+XX], eax
	0xE9,                                                              // jmp <opcode loop>
};
#undef ANY

#define FF8_AI_TEMPLATE_SIZE (sizeof(ff8_ai_switch_template) / sizeof(ff8_ai_switch_template[0]))

// The tables the patched switch reads. Static so their addresses stay valid
// for as long as the game runs.
static uint8_t ff8_ai_index_table[FF8_AI_INDEX_TABLE_SIZE];
static uint32_t ff8_ai_jump_table[FF8_AI_JUMP_TABLE_SIZE];

// Everything this file needs, resolved from the MonsterAI anchor
// ff8_externals already holds. Nothing outside ai.cpp reads them.
static struct
{
	uint32_t range_check;  // cmp edi, <last vanilla value - 200>
	uint32_t index_table;  // vanilla target value -> case index
	uint32_t jump_table;   // vanilla case index -> handler address
	uint32_t default_scan; // the com-file-id scan, where out-of-range values go
	uint32_t store_mask;   // mov [esp+XX], eax ; jmp <opcode loop>
} ai_ext;

// The sub-switch sits at a different offset in every language build.
static bool ff8_battle_ai_find_externals()
{
	uint32_t range_check_offset;

	switch (version)
	{
	case VERSION_FF8_12_FR:
	case VERSION_FF8_12_FR_NV:
		range_check_offset = 0x1D2E;
		break;
	case VERSION_FF8_12_DE:
	case VERSION_FF8_12_DE_NV:
		range_check_offset = 0x1D29;
		break;
	case VERSION_FF8_12_SP:
	case VERSION_FF8_12_SP_NV:
		range_check_offset = 0x1D58;
		break;
	case VERSION_FF8_12_IT:
	case VERSION_FF8_12_IT_NV:
		range_check_offset = 0x1D44;
		break;
	case VERSION_FF8_12_JP:
	case VERSION_FF8_12_JP_NV:
		range_check_offset = 0x1CF6;
		break;
	case VERSION_FF8_12_US:
	case VERSION_FF8_12_US_NV:
	case VERSION_FF8_12_US_EIDOS:
	case VERSION_FF8_12_US_EIDOS_NV:
		range_check_offset = 0x1D24;
		break;
	default:
		return false;
	}

	ai_ext.range_check = ff8_externals.battle_ai_opcode_sub_487DF0 + range_check_offset;

	const uint8_t *code = (const uint8_t *)ai_ext.range_check;

	for (int i = 0; i < int(FF8_AI_TEMPLATE_SIZE); ++i)
		if (ff8_ai_switch_template[i] <= 0xFF && code[i] != ff8_ai_switch_template[i]) return false;

	ai_ext.index_table = *(uint32_t *)(ai_ext.range_check + FF8_AI_INDEX_TABLE_OFFSET);
	ai_ext.jump_table = *(uint32_t *)(ai_ext.range_check + FF8_AI_JUMP_TABLE_OFFSET);
	ai_ext.store_mask = ai_ext.range_check + FF8_AI_STORE_MASK_OFFSET;

	// The `ja` is the one instruction already pointing at the com-file-id
	// scan, so read its rel32 rather than measuring the scan per build.
	uint32_t ja_next = ai_ext.range_check + FF8_AI_JA_NEXT_OFFSET;
	ai_ext.default_scan = ja_next + *(int32_t *)(ja_next - sizeof(int32_t));

	return true;
}

// Copy the vanilla switch into our tables, then add the pair values.
static bool ff8_battle_ai_build_tables()
{
	const uint8_t *vanilla_index = (const uint8_t *)ai_ext.index_table;
	const uint32_t *vanilla_jump = (const uint32_t *)ai_ext.jump_table;

	// The jump table has no length in the exe: it is as long as the highest
	// index the value table can produce.
	uint32_t vanilla_case_count = 0;

	for (int i = 0; i < FF8_AI_VANILLA_VALUE_COUNT; ++i)
		if (vanilla_index[i] >= vanilla_case_count) vanilla_case_count = vanilla_index[i] + 1;

	if (vanilla_case_count > FF8_AI_VANILLA_CASE_MAX) return false;

	// An index we never assign leads where an out-of-range value already went.
	for (int i = 0; i < FF8_AI_JUMP_TABLE_SIZE; ++i)
		ff8_ai_jump_table[i] = ai_ext.default_scan;

	for (uint32_t i = 0; i < vanilla_case_count; ++i)
		ff8_ai_jump_table[FF8_AI_VANILLA_CASE_BASE + i] = vanilla_jump[i];

	for (int i = 0; i < FF8_AI_VANILLA_VALUE_COUNT; ++i)
		ff8_ai_index_table[i] = (uint8_t)(FF8_AI_VANILLA_CASE_BASE + vanilla_index[i]);

	// Each pair value indexes the jump table with its own slot mask, so the
	// mask is what the SELF handler's store instruction finds in eax.
	int value = FF8_AI_PAIR_FIRST_VALUE;

	for (int a = 0; a < FF8_AI_SLOT_COUNT; ++a)
	{
		for (int b = a + 1; b < FF8_AI_SLOT_COUNT; ++b, ++value)
		{
			uint8_t mask = (uint8_t)((1 << a) | (1 << b));

			ff8_ai_index_table[value - FF8_AI_SWITCH_FIRST_VALUE] = mask;
			ff8_ai_jump_table[mask] = ai_ext.store_mask;
		}
	}

	return true;
}

void ff8_battle_ai_init()
{
	if (!ff8_externals.battle_ai_opcode_sub_487DF0 || !ff8_battle_ai_find_externals() || !ff8_battle_ai_build_tables())
	{
		ffnx_warning("AI target pairs: unexpected MonsterAI layout, skipping patch\n");
		return;
	}

	patch_code_byte(ai_ext.range_check + FF8_AI_RANGE_IMM_OFFSET, FF8_AI_PAIR_LAST_VALUE - FF8_AI_SWITCH_FIRST_VALUE);
	patch_code_dword(ai_ext.range_check + FF8_AI_INDEX_TABLE_OFFSET, (uint32_t)ff8_ai_index_table);
	patch_code_dword(ai_ext.range_check + FF8_AI_JUMP_TABLE_OFFSET, (uint32_t)ff8_ai_jump_table);

	if (trace_all) ffnx_info("AI target pairs: values %d-%d select battle-slot pairs\n", FF8_AI_PAIR_FIRST_VALUE, FF8_AI_PAIR_LAST_VALUE);
}
