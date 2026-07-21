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
// Retail only loads c0m000..c0m143. An encounter picks a monster by com_id
// (one byte per slot in FF8SceneOut.enemy_com_value); the loader adds 150 to
// get a battle-file index and reads BattleFilesArray[index] for the filename.
// c0m000..c0m143 live at indices 166..309, but index 310 is already
// D0C000.DAT, so the c0m block can't grow in place - com_id 160..215 would
// land on the character files.
//
// Two patches fix it:
//   1) Swap BattleFilesArray for a copy with 56 extra entries appended
//      ("C0M144.DAT".."C0M199.DAT") and repoint the loader at it. The original
//      entries keep their indices, so nothing else moves.
//   2) Hook the loader's file-load call and remap com_id 160..215 onto the
//      appended entries instead of D0C.
//
// No encounter format change is needed: enemy_com_value is already a byte, so
// a mod just sets it to c0m# + 16 to use any monster up to c0m199.
//
// Scan needs a fix too. Its "already scanned" bitfield SG_ENEMY_SCANNED_ONCE
// is only 5 DWORDs and gets indexed by com_id/32 with no bounds check, so it
// is only safe up to com_id 159. Scanning a new monster would overflow it into
// the neighbouring Renzokuken/tutorial savemap fields. We relocate the field
// to a bigger block in free, persisted savemap space (field vars 785..816) by
// repointing the two instructions that use it - and only when scene.out
// actually references a new monster, so vanilla saves stay untouched.
// -------------------------------------------------------------------------

#define FF8_BATTLE_FILES_ARRAY_LEN 1117
// c0m file range to add.
#define FF8_FIRST_NEW_C0M 144
#define FF8_LAST_NEW_C0M  199
#define FF8_NEW_C0M_COUNT (FF8_LAST_NEW_C0M - FF8_FIRST_NEW_C0M + 1) // 56
// com_id = c0m# + 16, so the new monsters use com_id 160..215.
#define FF8_FIRST_NEW_COM_ID (FF8_FIRST_NEW_C0M + 16) // 160
#define FF8_LAST_NEW_COM_ID  (FF8_LAST_NEW_C0M + 16)  // 215
// The loader adds 150 to com_id, so com_id 160..215 arrive as file indices
// 310..365 (D0C on retail); the hook remaps those onto the appended entries.
#define FF8_FIRST_NEW_FILE_INDEX (FF8_FIRST_NEW_COM_ID + 150) // 310
#define FF8_LAST_NEW_FILE_INDEX  (FF8_LAST_NEW_COM_ID + 150)  // 365

// SG_ENEMY_SCANNED_ONCE sits 0x54 bytes before field_vars_stack_1CFE9B8 in the
// savemap globals. Only used to sanity-check the address we read from the code.
#define FF8_SG_ENEMY_SCANNED_ONCE_OFFSET (-0x54)
// Where we move the field: field vars 785..816 (8 DWORDs, enough for all 256
// com_ids), in free savemap space. 753..784 are taken by AddMoreMagic.
#define FF8_SCANNED_ONCE_RELOCATE_VAR  785
#define FF8_SCANNED_ONCE_RELOCATE_SIZE 32
static_assert(FF8_SCANNED_ONCE_RELOCATE_SIZE >= 32, "must cover the full 0..255 com_id byte range (8 DWORDs)");

// scene.out record layout: 128 bytes each, enemy_com_value[8] at offset 56.
#define FF8_SCENE_OUT_RECORD_SIZE            128
#define FF8_SCENE_OUT_ENEMY_COM_VALUE_OFFSET 56
#define FF8_SCENE_OUT_ENEMY_SLOTS            8

// Backing store for the appended filenames; the table holds pointers into it
// for the whole session, so it stays static.
static char ff8_extended_c0m_names[FF8_NEW_C0M_COUNT][12]; // "C0M199.DAT" + NUL

// Our replacement for the loader's com_id+150 file lookup. This call is only
// reached for monsters, so an index in 310..365 means one of the new
// c0m144..c0m199 - remap it onto the appended entries and forward the rest.
static int ff8_battle_monster_load_file(int fileIndex, void *dst)
{
	if (fileIndex >= FF8_FIRST_NEW_FILE_INDEX && fileIndex <= FF8_LAST_NEW_FILE_INDEX)
		fileIndex = FF8_BATTLE_FILES_ARRAY_LEN + (fileIndex - FF8_FIRST_NEW_FILE_INDEX);

	return ((int (*)(int, void *))ff8_externals.battle_load_file_sub_508480)(fileIndex, dst);
}

// File size on disk, from the two places FFNx serves loose battle files: the
// "direct" override folder first, then the plain extracted layout.
static bool ff8_get_battle_file_size_on_disk(const char *relative_path, uint32_t *size_out)
{
	char full_path[MAX_PATH];

	snprintf(full_path, sizeof(full_path), "%s/direct/%s", ff8_externals.app_path, relative_path);
	FILE *file = fopen(full_path, "rb");
	if (file == nullptr)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", ff8_externals.app_path, relative_path);
		file = fopen(full_path, "rb");
		if (file == nullptr)
			return false;
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fclose(file);

	if (file_size <= 0)
		return false;

	*size_out = (uint32_t)file_size;
	return true;
}

// True if any encounter in scene.out uses a new monster (com_id >= 160) - the
// Scan relocation below is only needed then. Read through sm_pc_read so file
// overrides count, and sized from the real file so a modded scene.out works. If
// the size can't be found, assume it's needed rather than skip a real fix.
static bool ff8_scene_out_uses_new_monsters()
{
	char scene_out_filename[] = "battle/scene.out";
	uint32_t file_size;
	if (!ff8_get_battle_file_size_on_disk(scene_out_filename, &file_size))
		return true;

	uint8_t *scene_out = new uint8_t[file_size];
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

// Moves SG_ENEMY_SCANNED_ONCE to a bigger block so com_id/32 stays in bounds
// for the whole 0..255 range (see file comment). Only called when scene.out
// uses a new monster.
static void ff8_relocate_enemy_scanned_once()
{
	if (!ff8_externals.field_vars_stack_1CFE9B8
		|| !ff8_externals.battle_enemy_scanned_read_operand
		|| !ff8_externals.battle_enemy_scanned_write_operand)
		return;

	// Read the field's address straight out of the instruction that indexes it.
	// Then guard against a bad resolve before patching: the writer must use the
	// same address, and it must match the savemap layout.
	uint32_t from = *(uint32_t *)ff8_externals.battle_enemy_scanned_read_operand;
	uint32_t expected = ff8_externals.field_vars_stack_1CFE9B8 + FF8_SG_ENEMY_SCANNED_ONCE_OFFSET;
	uint32_t to = ff8_externals.field_vars_stack_1CFE9B8 + FF8_SCANNED_ONCE_RELOCATE_VAR;

	if (from != *(uint32_t *)ff8_externals.battle_enemy_scanned_write_operand || from != expected)
	{
		if (trace_all) ffnx_warning("Extra battle monsters: Scan scanned-once operands do not agree with the savemap layout (read 0x%08X, expected 0x%08X), skipping relocation - scanning c0m144-c0m199 may corrupt the savemap!\n", from, expected);
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

	// 1) Build the extended table (original entries + 56 new ones) and give it
	//    to battle_filenames. One-time allocation, never freed, like the exe's
	//    own static table.
	char **extended_filenames = (char **)driver_malloc((FF8_BATTLE_FILES_ARRAY_LEN + FF8_NEW_C0M_COUNT) * sizeof(char *));
	if (extended_filenames == nullptr)
		return;

	memcpy(extended_filenames, ff8_externals.battle_filenames, FF8_BATTLE_FILES_ARRAY_LEN * sizeof(char *));
	for (uint32_t i = 0; i < FF8_NEW_C0M_COUNT; ++i)
	{
		snprintf(ff8_extended_c0m_names[i], sizeof(ff8_extended_c0m_names[i]), "C0M%03d.DAT", FF8_FIRST_NEW_C0M + i);
		extended_filenames[FF8_BATTLE_FILES_ARRAY_LEN + i] = ff8_extended_c0m_names[i];
	}
	ff8_externals.battle_filenames = extended_filenames;

	//    Repoint the loader's array base at it (the disp32 of
	//    mov ebx, BattleFilesArray[eax*4], at battle_open_file + 0x11). FFNx's
	//    vram.cpp hooks read battle_filenames, so they pick it up too.
	patch_code_dword(ff8_externals.battle_open_file + 0x11, (DWORD)(uintptr_t)ff8_externals.battle_filenames);

	// 2) Redirect the loader's file-load call to our hook.
	replace_call(ff8_externals.battle_monster_file_load_call_site, (void *)&ff8_battle_monster_load_file);

	// 3) Fix the Scan overflow, but only if scene.out actually uses a new
	//    monster - vanilla never reaches com_id 160, so its field is already safe.
	bool uses_new_monsters = ff8_scene_out_uses_new_monsters();

	if (uses_new_monsters)
		ff8_relocate_enemy_scanned_once();

	// Only log when a mod actually uses the new monsters; the table is extended
	// everywhere, so staying quiet on vanilla keeps this out of everyone's log.
	if (uses_new_monsters && trace_all)
		ffnx_trace("Extra battle monsters enabled: c0m%03d-c0m%03d usable via enemy_com_value %d-%d.\n", FF8_FIRST_NEW_C0M, FF8_LAST_NEW_C0M, FF8_FIRST_NEW_COM_ID, FF8_LAST_NEW_COM_ID);
}
