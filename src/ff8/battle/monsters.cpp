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
#include "../../cfg.h"
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
// Fix (two memory patches):
//   1) Build an enlarged copy of BattleFilesArray = the original entries
//      verbatim + 56 appended pointers to new "C0M144.DAT".."C0M199.DAT",
//      and repoint LoadBattleFile's array base to it. Every existing index
//      resolves identically (the prefix is copied verbatim); nothing shifts.
//   2) Redirect the single battle-file load call inside
//      battle_monster_dat_loader (the monster load path) to a C hook that
//      remaps the com_id 160..215 file indices into the appended range
//      instead of letting them collide with D0C.
//
// Encounters need no exe change: enemy_com_value is already a byte, so setting
// it to (c0m# + 16) selects any monster up to c0m199.
//
// ff8_externals.battle_open_file / battle_filenames already resolve
// dynamically per version via the existing get_relative_call chain in
// ff8_data.cpp (sub_47CCB0 -> ... -> battle_open_file), so this file uses
// them as-is with no version branching of its own. The hooked call site
// (battle_monster_dat_loader + 0x240) and its target
// (battle_load_file_sub_508480) are likewise resolved relatively in
// ff8_data.cpp; the +0x240 offset is confirmed identical across all seven
// retail 1.2 exe files (EN/FR/DE/IT/SP/JP/JP_NV). The BattleFilesArray table
// (c0m000..c0m143 at indices 166..309, D0C000.DAT immediately at 310) is
// confirmed identical across all seven builds. Its length is not hardcoded
// either: it is counted at init by walking the table while its entries still
// look like pointers into the module image (see ff8_count_battle_filenames).
//
// Scan savemap overflow fix: the "already scanned" bitfield
// SG_ENEMY_SCANNED_ONCE (savemap global @ 0x1cfe964, IDA-verified, modelled
// as savemap_ff8_battle::ennemy_scanned_once in ff8/save_data.h) is only 5
// DWORDs (20 bytes), immediately followed in the packed savemap struct by
// SG_RENZOKUKEN_AUTO/SG_RENZOKUKEN_INDICATOR/SG_ODIN_ANGEL_GILGA_FLAG/
// SG_TUTORIAL_INFO. It has exactly one reader and one writer:
//   - reader @0x4925a6, in Damage_DispatchByAttackType's ATTACK_TYPE_SCAN
//     case: tests the bit for the target's com_id (read out of the battle
//     entity array, battle_entities_1D27BCB) and, when set, flags the enemy's
//     info as already known.
//   - writer @0x493810, which is a pure setter - "mark this com_id scanned",
//     `field[id / 32] |= 1 << (id % 32)` and nothing else.
// Both index it as SG_ENEMY_SCANNED_ONCE[(unsigned __int8)com_file_id / 32]
// with NO bounds check, so it's only safe for com_file_id (com_id) 0..159.
// Every com_id this patch adds (160..215, i.e. c0m144..c0m199) is already
// past that: casting Scan on any of the new monsters would write out of
// bounds into the Renzokuken/tutorial-flag savemap fields above, silently
// corrupting them.
//
// Fixed by repointing both instructions at a bigger (32-byte / 8-DWORD, safe
// for the full 0..255 com_id byte range) block in free savemap space, keeping
// the existing com_id/32 index math unchanged. The two disp32 operands are
// resolved relatively in ff8_data.cpp off battle_sub_48FE20 (see there), and
// re-checked below before anything is written, so a build whose layout does
// not match is left untouched rather than corrupted.
//
// SG_ENEMY_SCANNED_ONCE's own address is derived as
// field_vars_stack_1CFE9B8 - 0x54, a fixed C-struct field offset within the
// same packed savemap globals, so it needs no per-version address table
// either; field_vars_stack_1CFE9B8 already resolves per version elsewhere in
// ff8_data.cpp.
//
// The relocation target is field-script variables 785..816, inside the free
// block that runs from var 753 to var 1023 (271 bytes). That block is unused
// on three independent axes: no field script touches it (all 882 *.jsm
// scanned), no exe code references it (embedded-address scan), and it is zero
// in 28 real save files. Var 752 is the last script-used one. The block also
// sits inside the save's CRC span (image bytes 4561..4831), so anything
// written there survives a normal save, and unlike the temporary variables at
// 1024+ it is never cleared on field entry - both of which the scanned-once
// bitfield needs, since it is meant to persist. Runtime address is simply
// field_vars_stack_1CFE9B8 + var.
//
// Vars 753..784 are already claimed by AddMoreMagic's drawn-once relocation
// (a sibling FFNx fork, see its kernel_magic.cpp), so this deliberately
// starts right after them to avoid a collision if both ever land upstream.
//
// Note the third .text reference to this field, in an unreferenced code gap
// near Battle_RollCardCommand, is deliberately left alone: it is dead code on
// every retail 1.2 build (EN, ES, FR, IT, DE, JP), so relocating it would
// serve no purpose.
// -------------------------------------------------------------------------

// Sanity bound while counting the original BattleFilesArray; it holds 1117
// entries on every retail 1.2 build, so this only exists to stop a runaway
// walk if the table ever looks unfamiliar.
#define FF8_BATTLE_FILES_ARRAY_MAX 4096
// c0m file range to add.
#define FF8_FIRST_NEW_C0M 144
#define FF8_LAST_NEW_C0M  199
#define FF8_NEW_C0M_COUNT (FF8_LAST_NEW_C0M - FF8_FIRST_NEW_C0M + 1) // 56
// com_id = c0m# + 16, so the added monsters use com_id 160..215.
#define FF8_FIRST_NEW_COM_ID (FF8_FIRST_NEW_C0M + 16) // 160
#define FF8_LAST_NEW_COM_ID  (FF8_LAST_NEW_C0M + 16)  // 215
// The loader passes the battle-file loader an index of (com_id + 150), so
// the new com_id 160..215 arrive as indices 310..365 (which retail resolves to
// D0C character files); the hook remaps those onto the appended table entries.
#define FF8_FIRST_NEW_FILE_INDEX (FF8_FIRST_NEW_COM_ID + 150) // 310
#define FF8_LAST_NEW_FILE_INDEX  (FF8_LAST_NEW_COM_ID + 150)  // 365

// Where SG_ENEMY_SCANNED_ONCE sits relative to field_vars_stack_1CFE9B8 (the
// field-script variable block base, "VARMAP_START"): 0x54 bytes before it, in
// the same packed savemap globals. This is only used to sanity-check the
// address read out of the instructions that index the field - it is not how
// the address is obtained.
#define FF8_SG_ENEMY_SCANNED_ONCE_OFFSET (-0x54)
// Relocation target: field-script variables 785..816 (32 bytes / 8 DWORDs,
// safe for the full 0..255 com_id byte range), inside the verified-free
// 753..1023 block described in the file comment above. Vars 753..784 are
// already claimed by AddMoreMagic's drawn-once relocation.
#define FF8_SCANNED_ONCE_RELOCATE_VAR  785
#define FF8_SCANNED_ONCE_RELOCATE_SIZE 32
static_assert(FF8_SCANNED_ONCE_RELOCATE_SIZE >= 32, "must cover the full 0..255 com_id byte range (8 DWORDs)");

// scene.out per-record layout (128 bytes/record, enemy_com_value[8] at
// offset 56 - the data format, not the file's total size, which is read
// dynamically below so a modded/enlarged scene.out is handled correctly).
#define FF8_SCENE_OUT_RECORD_SIZE            128
#define FF8_SCENE_OUT_ENEMY_COM_VALUE_OFFSET 56
#define FF8_SCENE_OUT_ENEMY_SLOTS            8

// Both of these are handed to the exe (via patch_code_dword) and to
// ff8_externals.battle_filenames as raw pointers, so they must stay valid for
// the rest of the process: heap-allocated once, never freed, and static.
static char **ff8_extended_battle_filenames = nullptr;
static char ff8_extended_c0m_names[FF8_NEW_C0M_COUNT][12]; // "C0M199.DAT" + NUL = 11
// Length of the original BattleFilesArray, counted at init (see below).
static uint32_t ff8_battle_files_count = 0;

// Counts the original BattleFilesArray instead of hardcoding its length. Every
// entry is a pointer to a filename string inside the loaded module image, and
// the dword immediately after the last entry is not (it is 0x101 on US 1.2),
// so the run of in-image pointers ends exactly at the end of the array. Bounds
// come from the module's own PE header, so this needs no per-version literal.
static uint32_t ff8_count_battle_filenames(const char *const *filenames)
{
	HMODULE module = GetModuleHandleA(nullptr);
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((BYTE *)module + dos->e_lfanew);

	uintptr_t image_start = (uintptr_t)module;
	uintptr_t image_end = image_start + nt->OptionalHeader.SizeOfImage;

	uint32_t count = 0;
	while (count < FF8_BATTLE_FILES_ARRAY_MAX)
	{
		uintptr_t entry = (uintptr_t)filenames[count];

		if (entry < image_start || entry >= image_end)
			break;

		++count;
	}

	return count;
}

// Original battle_load_file_sub_508480(fileIndex, dst), saved so the hook below
// can forward to it after remapping.
static int (*ff8_battle_load_file)(int fileIndex, void *dst) = nullptr;

// C replacement for the loader's "index = com_id + 150" remap. Hooked onto the
// single battle-file load call inside battle_monster_dat_loader, which is only
// ever reached for monsters, so a file index in 310..365 unambiguously means
// one of the newly unlocked c0m144..c0m199; those get remapped onto the
// appended table entries and everything else is forwarded untouched.
static int ff8_battle_monster_load_file(int fileIndex, void *dst)
{
	if (fileIndex >= FF8_FIRST_NEW_FILE_INDEX && fileIndex <= FF8_LAST_NEW_FILE_INDEX)
		fileIndex = ff8_battle_files_count + (fileIndex - FF8_FIRST_NEW_FILE_INDEX);

	return ff8_battle_load_file(fileIndex, dst);
}

// Looks up a battle-relative file's real size on disk without reading it,
// checking the same two places FFNx actually stores loose files: the
// "direct" override folder first, then the plain extracted layout. Doesn't
// need to know anything about the packed-archive format (fl/fs/fi tables) -
// modern FFNx installs serve battle files as loose files in one of these two
// places, not from the original PSX archives.
static bool ff8_get_battle_file_size_on_disk(const char *relative_path, uint32_t *size_out)
{
	char full_path[MAX_PATH];
	WIN32_FILE_ATTRIBUTE_DATA attr;

	snprintf(full_path, sizeof(full_path), "%s\\direct\\%s", ff8_externals.app_path, relative_path);
	if (!GetFileAttributesExA(full_path, GetFileExInfoStandard, &attr))
	{
		snprintf(full_path, sizeof(full_path), "%s\\%s", ff8_externals.app_path, relative_path);
		if (!GetFileAttributesExA(full_path, GetFileExInfoStandard, &attr))
			return false;
	}

	*size_out = attr.nFileSizeLow;
	return true;
}

// Reads scene.out (through FFNx's own sm_pc_read, so file overrides are
// respected the same way the game itself would load it) and checks every
// encounter's enemy_com_value[8] bytes for one referencing a newly unlocked
// monster (com_id >= FF8_FIRST_NEW_COM_ID). Only encounter data determines
// whether the relocation below is ever needed - the exe/kernel side has no
// equivalent "count" to gate on the way AddMoreMagic gates on kernel magic
// count. The read buffer is sized from the file's real size on disk (see
// ff8_get_battle_file_size_on_disk above), not an assumed vanilla constant,
// so a modded/enlarged scene.out is scanned correctly. If the size can't be
// determined at all (e.g. a packed-archive-only install with no loose
// scene.out anywhere), be conservative and report "needed" rather than
// silently skip a real fix.
static bool ff8_scene_out_uses_new_monsters()
{
	uint32_t file_size;
	if (!ff8_get_battle_file_size_on_disk("battle\\scene.out", &file_size))
		return true;

	uint8_t *scene_out = new uint8_t[file_size];
	char scene_out_filename[] = "battle/scene.out";
	uint32_t read_size = ff8_externals.sm_pc_read(scene_out_filename, scene_out);
	if (read_size > file_size)
		read_size = file_size;

	bool uses_new_monsters = false;
	for (uint32_t offset = FF8_SCENE_OUT_ENEMY_COM_VALUE_OFFSET;
		!uses_new_monsters && offset + FF8_SCENE_OUT_ENEMY_SLOTS <= read_size;
		offset += FF8_SCENE_OUT_RECORD_SIZE)
	{
		for (int slot = 0; slot < FF8_SCENE_OUT_ENEMY_SLOTS; ++slot)
		{
			if (scene_out[offset + slot] >= FF8_FIRST_NEW_COM_ID)
			{
				uses_new_monsters = true;
				break;
			}
		}
	}

	delete[] scene_out;
	return uses_new_monsters;
}

// Relocates SG_ENEMY_SCANNED_ONCE (see the file-level comment above) from its
// native 5-DWORD savemap field to an 8-DWORD block in free savemap space, so
// the existing com_id/32 index math stays in bounds for the full 0..255
// com_id range instead of only 0..159. Only called when scene.out actually
// references a new monster (see ff8_scene_out_uses_new_monsters above), so a
// vanilla or not-yet-updated scene.out leaves this bitfield untouched.
static void ff8_relocate_enemy_scanned_once()
{
	if (!ff8_externals.field_vars_stack_1CFE9B8
		|| !ff8_externals.battle_enemy_scanned_read_operand
		|| !ff8_externals.battle_enemy_scanned_write_operand)
		return;

	// Take the field's address from the instruction that indexes it rather than
	// computing it: whatever the reader dereferences is the field, by
	// definition. Two independent checks then guard against a mis-resolved
	// operand, since writing to one would corrupt unrelated code: the writer
	// must reference the same address, and it must sit where the savemap
	// layout says it does.
	uint32_t from = *(uint32_t *)ff8_externals.battle_enemy_scanned_read_operand;
	uint32_t expected = ff8_externals.field_vars_stack_1CFE9B8 + FF8_SG_ENEMY_SCANNED_ONCE_OFFSET;
	uint32_t to = ff8_externals.field_vars_stack_1CFE9B8 + FF8_SCANNED_ONCE_RELOCATE_VAR;

	if (from != *(uint32_t *)ff8_externals.battle_enemy_scanned_write_operand || from != expected)
	{
		ffnx_warning("Extra battle monsters: Scan scanned-once operands do not agree with the savemap layout (read 0x%08X, expected 0x%08X), skipping relocation - scanning c0m144-c0m199 may corrupt the savemap!\n", from, expected);
		return;
	}

	patch_code_dword(ff8_externals.battle_enemy_scanned_read_operand, to);
	patch_code_dword(ff8_externals.battle_enemy_scanned_write_operand, to);

	if (trace_all) ffnx_trace("Extra battle monsters: Scan scanned-once bitfield relocated to var %d.\n", FF8_SCANNED_ONCE_RELOCATE_VAR);
}

void ff8_battle_monsters_init()
{
	if (!ff8_externals.battle_monster_file_load_call_site || !ff8_externals.battle_load_file_sub_508480)
	{
		if (trace_all) ffnx_trace("Extra battle monsters (c0m144-c0m199): unsupported game version, skipping.\n");
		return;
	}

	if (ff8_externals.battle_filenames == nullptr)
	{
		if (trace_all) ffnx_trace("Extra battle monsters: battle_filenames not resolved, skipping.\n");
		return;
	}

	ff8_battle_files_count = ff8_count_battle_filenames(ff8_externals.battle_filenames);
	if (ff8_battle_files_count == 0 || ff8_battle_files_count >= FF8_BATTLE_FILES_ARRAY_MAX)
	{
		ffnx_warning("Extra battle monsters: unexpected battle file table length (%u), skipping.\n", ff8_battle_files_count);
		return;
	}

	// 1) Copy the original table verbatim, then append the new c0m entries.
	ff8_extended_battle_filenames = (char **)driver_malloc((ff8_battle_files_count + FF8_NEW_C0M_COUNT) * sizeof(char *));
	if (ff8_extended_battle_filenames == nullptr)
		return;

	memcpy(ff8_extended_battle_filenames, ff8_externals.battle_filenames, ff8_battle_files_count * sizeof(char *));
	for (uint32_t i = 0; i < FF8_NEW_C0M_COUNT; ++i)
	{
		snprintf(ff8_extended_c0m_names[i], sizeof(ff8_extended_c0m_names[i]), "C0M%03d.DAT", FF8_FIRST_NEW_C0M + i);
		ff8_extended_battle_filenames[ff8_battle_files_count + i] = ff8_extended_c0m_names[i];
	}

	//    Repoint LoadBattleFile's array base: mov ebx, BattleFilesArray[eax*4].
	//    battle_open_file + 0x11 is the absolute displacement of that instruction.
	patch_code_dword(ff8_externals.battle_open_file + 0x11, (DWORD)(uintptr_t)ff8_extended_battle_filenames);

	//    Also repoint FFNx's own cached base pointer so the hooks that index it
	//    (ff8_battle_open_and_read_file in vram.cpp) resolve the appended
	//    c0m144-199 indices instead of reading past the original array.
	ff8_externals.battle_filenames = ff8_extended_battle_filenames;

	// 2) Redirect the monster loader's battle-file load call to the C hook,
	//    keeping the original target to forward to.
	ff8_battle_load_file = (int (*)(int, void *))ff8_externals.battle_load_file_sub_508480;
	replace_call(ff8_externals.battle_monster_file_load_call_site, (void *)&ff8_battle_monster_load_file);

	// 3) Relocate the Scan scanned-once bitfield so it stays in bounds
	//    for the new monsters' com_id range too (see file-level comment), but
	//    only when scene.out actually references one - a vanilla or
	//    not-yet-updated scene.out never reaches a com_id past 159, so the
	//    native field is already safe and there's nothing to relocate.
	bool uses_new_monsters = ff8_scene_out_uses_new_monsters();

	if (uses_new_monsters)
		ff8_relocate_enemy_scanned_once();

	// Only worth reporting when a mod actually references one of the new
	// monsters: the table is extended on every supported build, but on a
	// vanilla install nothing ever indexes into the appended range, so staying
	// quiet there keeps this out of everyone else's log.
	if (uses_new_monsters && trace_all)
		ffnx_trace("Extra battle monsters enabled: c0m%03d-c0m%03d usable via enemy_com_value %d-%d (%u original battle files).\n", FF8_FIRST_NEW_C0M, FF8_LAST_NEW_C0M, FF8_FIRST_NEW_COM_ID, FF8_LAST_NEW_COM_ID, ff8_battle_files_count);
}
