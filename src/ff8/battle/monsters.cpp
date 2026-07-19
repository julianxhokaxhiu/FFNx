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
//   2) Redirect the single BattleFile_CharacterLoad call inside
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
// (battle_monster_dat_loader + 0x240) and its target (BattleFile_CharacterLoad)
// are likewise resolved relatively in ff8_data.cpp; the +0x240 offset is
// confirmed identical across all seven retail 1.2 exe files
// (EN/FR/DE/IT/SP/JP/JP_NV). The BattleFilesArray table structure itself
// (c0m000..c0m143 at indices 166..309, D0C000.DAT immediately at 310, 1117
// entries total) is confirmed identical across all seven builds, so
// FF8_BATTLE_FILES_ARRAY_LEN below is not version-dependent.
//
// Scan/Libra savemap overflow fix: the "already scanned" bitfield
// SG_ENEMY_SCANNED_ONCE (savemap global @ 0x1cfe964, IDA-verified) is only 5
// DWORDs (20 bytes), immediately followed in the packed savemap struct by
// SG_RENZOKUKEN_AUTO/SG_RENZOKUKEN_INDICATOR/SG_ODIN_ANGEL_GILGA_FLAG/
// SG_TUTORIAL_INFO. Both readers/writers of it - Damage_DispatchByAttackType's
// ATTACK_TYPE_SCAN case (@0x4925a6) and hasEnemyAlreadyScanned (@0x493810) -
// index it as SG_ENEMY_SCANNED_ONCE[(unsigned __int8)com_file_id / 32] with NO
// bounds check, so it's only safe for com_file_id (com_id) 0..159. Every
// com_id this patch adds (160..215, i.e. c0m144..c0m199) is already past
// that: Scanning any of the new monsters (Scan spell / Libra / Enc-None
// chance-scan) would write out of bounds into the Renzokuken/tutorial-flag
// savemap fields above, silently corrupting them.
//
// Fixed the same way AddMoreMagic (a sibling FFNx fork) relocates its
// magic "drawn once" bitfield out of a too-small native field: blind
// value-scan every dword in .text for SG_ENEMY_SCANNED_ONCE's address and
// repoint it at a bigger (32-byte / 8-DWORD, safe for the full 0..255
// com_id byte range) block in verified-free savemap space, keeping the
// existing com_id/32 index math unchanged. SG_ENEMY_SCANNED_ONCE's address
// itself is derived relatively as field_vars_stack_1CFE9B8 - 0x54 (a fixed
// C-struct field offset within the same packed savemap globals, not a
// signature-scanned value, so it needs no separate per-version address
// table); field_vars_stack_1CFE9B8 already resolves per version elsewhere
// in ff8_data.cpp. The relocation target uses savemap free-region bytes
// 785..816 (var 753..784 is already claimed by AddMoreMagic's drawn-once
// relocation - see that fork's kernel_magic.cpp - so this deliberately
// starts right after it to avoid a collision if both forks ever land
// upstream together).
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
// The loader passes BattleFile_CharacterLoad a file index of (com_id + 150), so
// the new com_id 160..215 arrive as indices 310..365 (which retail resolves to
// D0C character files); the hook remaps those onto the appended table entries.
#define FF8_FIRST_NEW_FILE_INDEX (FF8_FIRST_NEW_COM_ID + 150) // 310
#define FF8_LAST_NEW_FILE_INDEX  (FF8_LAST_NEW_COM_ID + 150)  // 365

// SG_ENEMY_SCANNED_ONCE sits 0x54 bytes before field_vars_stack_1CFE9B8 (the
// field-script variable block base, "VARMAP_START") in the same packed
// savemap globals - a fixed C-struct field offset, IDA-verified on US 1.2.
#define FF8_SG_ENEMY_SCANNED_ONCE_OFFSET (-0x54)
// Relocation target: savemap free-region bytes 785..816 (32 bytes / 8 DWORDs,
// safe for the full 0..255 com_id byte range). Bytes 753..784 of this same
// free region are already claimed by AddMoreMagic's drawn-once relocation.
#define FF8_SCANNED_ONCE_RELOCATE_VAR  785
#define FF8_SCANNED_ONCE_RELOCATE_SIZE 32

// scene.out per-record layout (128 bytes/record, enemy_com_value[8] at
// offset 56 - the data format, not the file's total size, which is read
// dynamically below so a modded/enlarged scene.out is handled correctly).
#define FF8_SCENE_OUT_RECORD_SIZE            128
#define FF8_SCENE_OUT_ENEMY_COM_VALUE_OFFSET 56
#define FF8_SCENE_OUT_ENEMY_SLOTS            8

// Must stay static, not stack-local: ff8_battle_monsters_init() hands the exe
// (via patch_code_dword) and ff8_externals.battle_filenames a raw pointer into
// these arrays that's expected to remain valid for the rest of the process.
static char *ff8_extended_battle_filenames[FF8_BATTLE_FILES_ARRAY_LEN + FF8_NEW_C0M_COUNT];
static char ff8_extended_c0m_names[FF8_NEW_C0M_COUNT][12]; // "C0M199.DAT" + NUL = 11

// Original BattleFile_CharacterLoad(fileIndex, dst), saved so the hook below can
// forward to it after remapping.
static int (*ff8_battle_file_character_load)(int fileIndex, void *dst) = nullptr;

// C replacement for the loader's "index = com_id + 150" remap. Hooked onto the
// single BattleFile_CharacterLoad call inside battle_monster_dat_loader, which
// is only ever reached for monsters, so a file index in 310..365 unambiguously
// means one of the newly unlocked c0m144..c0m199; those get remapped onto the
// appended table entries and everything else is forwarded untouched.
static int ff8_battle_monster_load_file(int fileIndex, void *dst)
{
	if (fileIndex >= FF8_FIRST_NEW_FILE_INDEX && fileIndex <= FF8_LAST_NEW_FILE_INDEX)
		fileIndex = FF8_BATTLE_FILES_ARRAY_LEN + (fileIndex - FF8_FIRST_NEW_FILE_INDEX);

	return ff8_battle_file_character_load(fileIndex, dst);
}

// Returns the running exe's actual .text section bounds, read from its own
// PE header (same technique as getProcessEntryPoint() in utils.cpp) rather
// than an assumed/hardcoded address range - adapts automatically to any
// version/build with no per-version literal at all.
static void ff8_get_code_section_bounds(uint32_t *start, uint32_t *end)
{
	HMODULE base = GetModuleHandleA(nullptr);
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((BYTE *)base + dos->e_lfanew);

	*start = (uint32_t)base + nt->OptionalHeader.BaseOfCode;
	*end = *start + nt->OptionalHeader.SizeOfCode;
}

// ---- blind value-scan relocation (same technique as AddMoreMagic's --------
// ---- drawn-once bitfield relocation, see that fork's kernel_magic.cpp) ----
// Scans the exe's .text section for any dword equal to `from` and repoints
// it to `to`. One false-positive class must be excluded: a `call rel32`
// (0xE8) or `jmp rel32` (0xE9) whose opcode + displacement bytes happen to
// numerically equal `from` - such a match has the branch opcode as its low
// byte AND decodes to a target inside real code, which a genuine data
// operand never does, so those are skipped.
static uint32_t ff8_relocate_scan(uint32_t from, uint32_t to, const char *what)
{
	uint32_t scan_start, scan_end;
	ff8_get_code_section_bounds(&scan_start, &scan_end);

	uint32_t rewritten = 0, skipped_branch = 0;

	for (uint32_t addr = scan_start; addr < scan_end - 4; ++addr)
	{
		uint32_t value = *(uint32_t *)addr;

		if (value == from)
		{
			uint8_t op = *(uint8_t *)addr;
			if (op == 0xE8 || op == 0xE9) // call/jmp rel32?
			{
				uint32_t target = addr + 5 + *(int32_t *)(addr + 1);
				if (target >= scan_start && target < scan_end)
				{
					++skipped_branch; // real branch instruction - never touch it
					continue;
				}
			}

			patch_code_dword(addr, (DWORD)to);
			++rewritten;
			addr += 3; // skip the rewritten dword
		}
	}

	if (trace_all) ffnx_trace("Extra battle monsters: %s: relocated %u site(s), skipped %u branch false-positive(s).\n",
		what, rewritten, skipped_branch);
	return rewritten;
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
// native 5-DWORD savemap field to an 8-DWORD block in verified-free savemap
// space, so the existing com_id/32 index math stays in bounds for the full
// 0..255 com_id range instead of only 0..159. Its address is derived
// relatively from field_vars_stack_1CFE9B8, not signature-scanned, so this
// needs no per-version address table. Empirically 3 sites reference it in
// US 1.2 (Damage_DispatchByAttackType's read, hasEnemyAlreadyScanned's
// write, plus one embedded reference in an unreferenced code gap between
// Battle_RollCardCommand and Battle_applyDamage - harmless to relocate
// either way); the actual count is only traced, not asserted, since it may
// differ per language build. Only called when scene.out actually references
// a new monster (see ff8_scene_out_uses_new_monsters above), so a vanilla or
// not-yet-updated scene.out leaves this bitfield untouched.
static_assert(FF8_SCANNED_ONCE_RELOCATE_SIZE >= 32, "must cover the full 0..255 com_id byte range (8 DWORDs)");

static void ff8_relocate_enemy_scanned_once()
{
	if (!ff8_externals.field_vars_stack_1CFE9B8)
		return;

	ff8_relocate_scan(ff8_externals.field_vars_stack_1CFE9B8 + FF8_SG_ENEMY_SCANNED_ONCE_OFFSET,
		ff8_externals.field_vars_stack_1CFE9B8 + FF8_SCANNED_ONCE_RELOCATE_VAR,
		"Scan/Libra scanned-once bitfield");
}

void ff8_battle_monsters_init()
{
	if (!ff8_externals.battle_monster_file_load_call_site || !ff8_externals.battle_file_character_load)
	{
		if (trace_all) ffnx_trace("Extra battle monsters (c0m144-c0m199): unsupported game version, skipping.\n");
		return;
	}

	if (ff8_externals.battle_filenames == nullptr)
	{
		if (trace_all) ffnx_trace("Extra battle monsters: battle_filenames not resolved, skipping.\n");
		return;
	}

	// 1) Copy the original table verbatim, then append the new c0m entries.
	memcpy(ff8_extended_battle_filenames, ff8_externals.battle_filenames, FF8_BATTLE_FILES_ARRAY_LEN * sizeof(char *));
	for (int i = 0; i < FF8_NEW_C0M_COUNT; ++i)
	{
		snprintf(ff8_extended_c0m_names[i], sizeof(ff8_extended_c0m_names[i]), "C0M%03d.DAT", FF8_FIRST_NEW_C0M + i);
		ff8_extended_battle_filenames[FF8_BATTLE_FILES_ARRAY_LEN + i] = ff8_extended_c0m_names[i];
	}

	//    Repoint LoadBattleFile's array base: mov ebx, BattleFilesArray[eax*4].
	//    battle_open_file + 0x11 is the absolute displacement of that instruction.
	patch_code_dword(ff8_externals.battle_open_file + 0x11, (DWORD)(uintptr_t)ff8_extended_battle_filenames);

	//    Also repoint FFNx's own cached base pointer so the hooks that index it
	//    (ff8_battle_open_and_read_file in vram.cpp) resolve the appended
	//    c0m144-199 indices instead of reading past the original array.
	ff8_externals.battle_filenames = ff8_extended_battle_filenames;

	// 2) Redirect the monster loader's BattleFile_CharacterLoad call to the C
	//    hook, keeping the original target to forward to.
	ff8_battle_file_character_load = (int (*)(int, void *))ff8_externals.battle_file_character_load;
	replace_call(ff8_externals.battle_monster_file_load_call_site, (void *)&ff8_battle_monster_load_file);

	// 3) Relocate the Scan/Libra scanned-once bitfield so it stays in bounds
	//    for the new monsters' com_id range too (see file-level comment), but
	//    only when scene.out actually references one - a vanilla or
	//    not-yet-updated scene.out never reaches a com_id past 159, so the
	//    native field is already safe and there's nothing to relocate.
	if (ff8_scene_out_uses_new_monsters())
		ff8_relocate_enemy_scanned_once();

	ffnx_info("Extra battle monsters enabled: c0m%03d-c0m%03d usable via enemy_com_value %d-%d.\n", FF8_FIRST_NEW_C0M, FF8_LAST_NEW_C0M, FF8_FIRST_NEW_COM_ID, FF8_LAST_NEW_COM_ID);
}
