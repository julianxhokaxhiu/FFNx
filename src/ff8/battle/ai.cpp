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

// Monster-AI "target" opcode (0x04): make the unused target values 228-248
// select a fixed pair of battle slots, so one action hits two targets.
// 228 = slots 0+1, 229 = 0+2, ... 248 = 5+6 (every pair a<b over slots 0-6).
// FF8 has 7 battle slots: 0-2 party, 3-6 enemies (4 max) - slot 7 never exists.
// Vanilla resolves these to "no target found", so the values are free to reuse.
//
// The opcode has no call boundary, so we repoint its range-check `ja` at a tiny
// thunk: it asks ff8_ai_pair_mask_for() for the pair mask, writes it to the
// interpreter's target-mask local and resumes the loop; other values fall
// through to the original code. All game addresses come from ff8_externals.

#include "ai.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../log.h"

#include <stdint.h>

#define FF8_AI_PAIR_FIRST_VALUE 228
#define FF8_AI_PAIR_LAST_VALUE  248
#define FF8_AI_SLOT_COUNT       7  // FF8 battle entity slots: 0-2 party, 3-6 enemies
#define FF8_AI_PAIR_COUNT       (FF8_AI_PAIR_LAST_VALUE - FF8_AI_PAIR_FIRST_VALUE + 1) // 21
#define FF8_AI_TARGET_MASK_LOCAL_OFFSET 0x78 // [esp+0x78] at the range-check ja

static_assert(FF8_AI_SLOT_COUNT * (FF8_AI_SLOT_COUNT - 1) / 2 == FF8_AI_PAIR_COUNT,
	"value range must match the number of slot pairs C(7,2)=21");

// value-228 -> slot bitmask, built from the a<b pair enumeration.
static uint32_t ff8_ai_pair_mask_table[FF8_AI_PAIR_COUNT];

static void ff8_ai_build_pair_mask_table()
{
	int index = 0;
	for (int a = 0; a < FF8_AI_SLOT_COUNT; ++a)
		for (int b = a + 1; b < FF8_AI_SLOT_COUNT; ++b)
			ff8_ai_pair_mask_table[index++] = (1u << a) | (1u << b);
}

// Pair mask for a value in 228-248, or 0 for anything else (handled as vanilla).
extern "C" uint32_t __cdecl ff8_ai_pair_mask_for(uint32_t target_value)
{
	if (target_value < FF8_AI_PAIR_FIRST_VALUE || target_value > FF8_AI_PAIR_LAST_VALUE)
		return 0;

	return ff8_ai_pair_mask_table[target_value - FF8_AI_PAIR_FIRST_VALUE];
}

// Thunk continuations, set from ff8_externals so no game address is hardcoded.
static uint32_t ff8_ai_loop_head_addr = 0;
static uint32_t ff8_ai_default_scan_addr = 0;

// Why asm (no C equivalent): this is a mid-loop intercept - not a call site
// (rules out replace_call) and not a function tail (rules out replace_function
// and FFNx's epilogue-relocation trick, cf. texture_reload_hack in ff8_opengl),
// because both paths must jump back INTO MonsterAI's opcode loop, not return. C
// also can't read dl, address the game's [esp+0x78], or jmp to an absolute
// address. Emitting the bytes via patch_code/memcpy_code would only hide the asm
// as opaque hex, not remove it. So: 8 instructions of pure plumbing, no logic.
//
// Entered from the repointed `ja` with dl = target value byte. On a pair, writes
// the mask and resumes the opcode loop; otherwise restores dl and falls through
// to the original scan.
static __declspec(naked) void ff8_ai_target_pair_thunk()
{
	__asm {
		movzx eax, dl
		push  eax
		call  ff8_ai_pair_mask_for
		test  eax, eax
		jz    do_default
		add   esp, 4
		mov   dword ptr [esp + FF8_AI_TARGET_MASK_LOCAL_OFFSET], eax
		jmp   dword ptr [ff8_ai_loop_head_addr]
	do_default:
		pop   edx                          // restore dl for the original scan
		jmp   dword ptr [ff8_ai_default_scan_addr]
	}
}

void ff8_battle_ai_init()
{
	if (!ff8_externals.battle_ai_target_rangecheck_ja
		|| !ff8_externals.battle_ai_target_default_scan
		|| !ff8_externals.battle_ai_opcode_loop_head)
		return;

	// Patch only if the range-check `ja` (0F 87 rel32) still points at the scan,
	// so an unexpected exe layout is left untouched.
	uint32_t ja = ff8_externals.battle_ai_target_rangecheck_ja;
	uint32_t ja_operand = ja + 2;
	uint32_t ja_next = ja + 6;

	if (*(uint16_t *)ja != 0x870F
		|| ja_next + *(int32_t *)ja_operand != ff8_externals.battle_ai_target_default_scan)
	{
		ffnx_warning("AI target pairs: unexpected MonsterAI layout, skipping patch\n");
		return;
	}

	ff8_ai_build_pair_mask_table();
	ff8_ai_loop_head_addr = ff8_externals.battle_ai_opcode_loop_head;
	ff8_ai_default_scan_addr = ff8_externals.battle_ai_target_default_scan;

	patch_code_dword(ja_operand, (uint32_t)&ff8_ai_target_pair_thunk - ja_next);

	if (trace_all) ffnx_info("AI target pairs: values %d-%d select battle-slot pairs\n", FF8_AI_PAIR_FIRST_VALUE, FF8_AI_PAIR_LAST_VALUE);
}
