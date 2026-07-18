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

#include "monsters.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"
#include "../../log.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// -------------------------------------------------------------------------
// Unlock the unused battle monster models c0m144..c0m199.
//
// Retail FF8 can only load c0m000..c0m143 (144 of the 200 c0m*.dat that ship
// in battle.fs). The chain, verified in FF8_EN.exe (US 1.2):
//   - Encounter def: FF8SceneOut.enemy_com_value[8] (1 byte/slot) = com_id.
//   - sub_507080 dispatches com_id >= 16 (a monster) to battle_monster_dat_loader.
//   - That loader computes the battle-file index as (com_id + 150) and calls
//     LoadBattleFile, which reads BattleFilesArray[index] for the filename.
//   - c0m000 is BattleFilesArray[166] (com_id 16), c0m143 is [309] (com_id 159).
//     Index 310 is immediately D0C000.DAT (character files), so the c0m block
//     cannot grow in place, and com_id 160..215 currently resolve to D0C/garbage.
//
// Fix (three memory patches):
//   1) Build an enlarged copy of BattleFilesArray = the original entries
//      verbatim + 56 appended pointers to new "C0M144.DAT".."C0M199.DAT".
//   2) Repoint LoadBattleFile's array base to the enlarged copy. Every existing
//      index resolves identically (the prefix is copied verbatim); nothing shifts.
//   3) Hook the loader's "index = com_id + 150" so com_id 160..215 map to the
//      appended range instead of colliding with D0C.
//
// Encounters need no exe change: enemy_com_value is already a byte, so setting
// it to (c0m# + 16) selects any monster up to c0m199.
//
// ff8_externals.battle_open_file / battle_filenames already resolve
// dynamically per version via the existing get_relative_call chain in
// ff8_data.cpp (sub_47CCB0 -> ... -> battle_open_file), so this file uses
// them as-is with no version branching of its own. battle_monster_dat_loader
// itself is only ever reached through a function-pointer task dispatch
// (never a direct call/jmp instruction anywhere in the exe), so its
// "add eax, 150" com_id remap site can't be resolved through a relative-call
// chain either; ff8_externals.battle_monster_dat_loader_com_id_add_site is
// derived in ff8_data.cpp from a per-version absolute address for its caller
// BattleTask_DispatchComEntityLoad (same fallback as sub_54A0D0 there, since
// that caller also has no static xref anywhere in the exe), plus a fixed
// +0x23A byte offset confirmed identical across all seven retail 1.2 exe
// files (EN/FR/DE/IT/SP/JP/JP_NV) - see ff8_data.cpp for the address table.
// The BattleFilesArray table structure itself (c0m000..c0m143
// at indices 166..309, D0C000.DAT immediately at 310, 1117 entries total) is
// confirmed identical across all seven builds, so FF8_BATTLE_FILES_ARRAY_LEN
// below is not version-dependent.
// -------------------------------------------------------------------------

// Length of the original BattleFilesArray (US 1.2), indices 0..1116.
#define FF8_BATTLE_FILES_ARRAY_LEN 1117
// c0m file range to add.
#define FF8_FIRST_NEW_C0M 144
#define FF8_LAST_NEW_C0M  199
#define FF8_NEW_C0M_COUNT (FF8_LAST_NEW_C0M - FF8_FIRST_NEW_C0M + 1) // 56
// com_id = c0m# + 16, so the added monsters use com_id 160..215.
#define FF8_FIRST_NEW_COM_ID (FF8_FIRST_NEW_C0M + 16) // 160
#define FF8_LAST_NEW_COM_ID  (FF8_LAST_NEW_C0M + 16)  // 215
// New entries are appended at index FF8_BATTLE_FILES_ARRAY_LEN, so:
//   index(com_id) = FF8_BATTLE_FILES_ARRAY_LEN + (com_id - FF8_FIRST_NEW_COM_ID)
//                 = com_id + (FF8_BATTLE_FILES_ARRAY_LEN - FF8_FIRST_NEW_COM_ID)
#define FF8_NEW_C0M_INDEX_BIAS (FF8_BATTLE_FILES_ARRAY_LEN - FF8_FIRST_NEW_COM_ID) // 957

static char *ff8_extended_battle_filenames[FF8_BATTLE_FILES_ARRAY_LEN + FF8_NEW_C0M_COUNT];
static char ff8_extended_c0m_names[FF8_NEW_C0M_COUNT][12]; // "C0M199.DAT" + NUL = 11

// Custom "eax in / eax out" trampoline that replaces the loader's com_id+150.
// eax = com_id on entry, returns the BattleFilesArray index in eax. Only eax
// and the flags are touched; the surrounding code does not rely on the flags
// after this point (it just pushes eax and calls), so this is transparent.
static __declspec(naked) void ff8_battle_monster_index_remap()
{
	__asm {
		cmp eax, FF8_FIRST_NEW_COM_ID   // 160
		jb  normal
		cmp eax, FF8_LAST_NEW_COM_ID    // 215
		ja  normal
		add eax, FF8_NEW_C0M_INDEX_BIAS // 957 -> appended c0m144..c0m199
		ret
	normal:
		add eax, 150                    // original c0m000..c0m143 (and unused >= 216)
		ret
	}
}

void ff8_battle_monsters_init()
{
	uint32_t add_site = ff8_externals.battle_monster_dat_loader_com_id_add_site;
	if (!add_site)
	{
		ffnx_trace("Extra battle monsters (c0m144-c0m199): unsupported game version, skipping.\n");
		return;
	}

	char **orig = (char **)ff8_externals.battle_filenames;
	if (orig == nullptr)
	{
		ffnx_trace("Extra battle monsters: battle_filenames not resolved, skipping.\n");
		return;
	}

	// 1) Copy the original table verbatim, then append the new c0m entries.
	memcpy(ff8_extended_battle_filenames, orig, FF8_BATTLE_FILES_ARRAY_LEN * sizeof(char *));
	for (int i = 0; i < FF8_NEW_C0M_COUNT; ++i)
	{
		snprintf(ff8_extended_c0m_names[i], sizeof(ff8_extended_c0m_names[i]), "C0M%03d.DAT", FF8_FIRST_NEW_C0M + i);
		ff8_extended_battle_filenames[FF8_BATTLE_FILES_ARRAY_LEN + i] = ff8_extended_c0m_names[i];
	}

	// 2) Repoint LoadBattleFile's array base: mov ebx, BattleFilesArray[eax*4].
	//    battle_open_file + 0x11 is the absolute displacement of that instruction.
	patch_code_dword(ff8_externals.battle_open_file + 0x11, (DWORD)(uintptr_t)ff8_extended_battle_filenames);

	//    Also repoint FFNx's own cached base pointer so the hooks that index it
	//    (ff8_battle_open_and_read_file in vram.cpp) resolve the appended
	//    c0m144-199 indices instead of reading past the original array.
	ff8_externals.battle_filenames = ff8_extended_battle_filenames;

	// 3) Replace "add eax, 96h" (5 bytes) with "call ff8_battle_monster_index_remap".
	uint8_t call_patch[5];
	call_patch[0] = 0xE8;
	*(uint32_t *)&call_patch[1] = (uint32_t)&ff8_battle_monster_index_remap - (add_site + 5);
	memcpy_code(add_site, call_patch, sizeof(call_patch));

	ffnx_info("Extra battle monsters enabled: c0m%03d-c0m%03d usable via enemy_com_value %d-%d.\n", FF8_FIRST_NEW_C0M, FF8_LAST_NEW_C0M, FF8_FIRST_NEW_COM_ID, FF8_LAST_NEW_COM_ID);
}
