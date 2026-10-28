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

// Battle anim sets from <direct_mode_path>/exe/effect_NNN.animset, see docs/ff8/mods/battle_anim_sets.md.
// An anim set (body animations, AnimSeq sequences, weapon animations) is bound onto a model for a limit break,
// a Blue Magic cast... The exe passes the set by pointer only, so a set is recognized by the effect running and
// its vanilla contents, which are the same in every release.

#include "anim_set.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"
#include "../../cfg.h"
#include "../../log.h"
#include "../../exe_data.h"

#include <stdint.h>
#include <stdio.h>
#include <map>
#include <vector>

#define FF8_ANIM_SET_MAX_SIZE 0x100000
#define FF8_ANIM_SET_BODY_SECTIONS 0x0C
#define FF8_ANIM_SET_WEAPON_SECTIONS 0x04
#define FF8_BATTLE_ENTITY_EXTERNAL_ANIM_SET 0x08
#define FF8_BATTLE_FIRST_MONSTER_COM_ID 0x10

static_assert(sizeof(ff8_battle_entity_state) == 0x9C);

typedef void *(*ff8_set_com_file_data)(void *anim_header, ff8_battle_com_file_data *com_file_data, int sections);

struct ff8_battle_anim_set_entry {
	uint16_t effect_id;
	uint8_t index; // order in the exe when the effect binds several sets
	uint8_t section_count;
	uint32_t size;
	uint32_t fnv1a;
};

// Every set the exe binds: 72 static blocks, identical in all releases
static const ff8_battle_anim_set_entry vanilla_anim_sets[] = {
	{71, 0, 3, 0x1B74, 0xAB77A114},
	{72, 0, 3, 0x1B74, 0xAB77A114},
	{73, 0, 3, 0x1B74, 0xAB77A114},
	{74, 0, 3, 0x1B74, 0xAB77A114},
	{84, 0, 3, 0x2410, 0x02B88888},
	{86, 0, 3, 0x1D60, 0x76BD0DB7},
	{87, 0, 3, 0x11F4, 0x2DD343B1},
	{88, 0, 3, 0x1BEC, 0x61268848},
	{89, 0, 3, 0x1020, 0xA7C875DF},
	{92, 0, 3, 0x1D0C, 0x21B6F3DB},
	{112, 0, 2, 0x2038, 0x1FC81F5D},
	{141, 0, 3, 0x1D38, 0xA32C1D37},
	{159, 0, 3, 0x2334, 0x2E688265},
	{159, 1, 2, 0x2118, 0xD0B3909C},
	{160, 0, 3, 0x172C, 0xC56319A6},
	{161, 0, 3, 0x2CB4, 0xB6F831A8},
	{161, 1, 2, 0x2FF4, 0x5939BBDF},
	{162, 0, 3, 0x1688, 0xD2E501C2},
	{163, 0, 3, 0xE3C, 0x12510258},
	{164, 0, 3, 0x15A8, 0x18775C74},
	{165, 0, 3, 0x2F18, 0xD9B672F4},
	{184, 0, 3, 0x1B74, 0xAB77A114},
	{188, 0, 3, 0x1570, 0xE7433C69},
	{192, 0, 3, 0x1570, 0xE7433C69},
	{193, 0, 3, 0x1570, 0xE7433C69},
	{194, 0, 3, 0x1BC0, 0xBCEF93E1},
	{195, 0, 3, 0x23CC, 0x421482F2},
	{196, 0, 3, 0xB28, 0x20AE2C87},
	{197, 0, 3, 0x23CC, 0x421482F2},
	{198, 0, 3, 0x23CC, 0x421482F2},
	{200, 0, 3, 0x2628, 0x649E4EE2},
	{212, 0, 2, 0x1350, 0x47DB1C2C},
	{215, 0, 2, 0x1970, 0xDC33CE93},
	{216, 0, 2, 0x13F8, 0x075DCDD9},
	{228, 0, 2, 0x1734, 0x7F4F14F1},
	{229, 0, 3, 0x1B74, 0xAB77A114},
	{230, 0, 3, 0x1B74, 0xAB77A114},
	{259, 0, 3, 0x22C4, 0x87E30369},
	{260, 0, 3, 0x1B74, 0xAB77A114},
	{261, 0, 3, 0x28AC, 0x65452D3C},
	{262, 0, 2, 0x1F68, 0xF82E640D},
	{263, 0, 3, 0x24F8, 0x0ABC31DA},
	{274, 0, 2, 0x4DF4, 0x8CCF7741},
	{275, 0, 3, 0x1B74, 0xAB77A114},
	{297, 0, 3, 0x1B74, 0xAB77A114},
	{299, 0, 3, 0x1B74, 0xAB77A114},
	{300, 0, 3, 0x1B74, 0xAB77A114},
	{305, 0, 3, 0x1B74, 0xAB77A114},
	{306, 0, 3, 0x1B74, 0xAB77A114},
	{307, 0, 3, 0x1B74, 0xAB77A114},
	{317, 0, 3, 0x1B74, 0xAB77A114},
	{332, 0, 3, 0x2568, 0xA43B207E},
	{333, 0, 3, 0x23E0, 0x15946DE1},
	{334, 0, 3, 0x344C, 0xAD73C28E},
	{334, 1, 2, 0x56F8, 0x530CD653},
	{335, 0, 3, 0x203C, 0x96051DCC},
	{335, 1, 2, 0x20D4, 0x89AACE96},
	{336, 0, 3, 0x32E8, 0xEEA0CD91},
	{336, 1, 2, 0x50A8, 0xBBF8EDE0},
	{337, 0, 3, 0x29C0, 0x116BAB93},
	{337, 1, 2, 0x31FC, 0xC86CAC5E},
	{339, 0, 3, 0x2820, 0x25E20240},
	{339, 1, 2, 0x3C84, 0xC97785BB},
	{339, 2, 2, 0xBBC, 0xFF9C1DE1},
	{340, 0, 3, 0x1EF4, 0xAD65B589},
	{340, 1, 2, 0x2D74, 0x6B8D5A8F},
	{341, 0, 3, 0x15A8, 0xCD9FDBB5},
	{341, 1, 2, 0x588, 0x2AEF3537},
	{342, 0, 3, 0x2894, 0xC54D2ACA},
	{342, 1, 2, 0x4A80, 0xBE96F73A},
	{343, 0, 3, 0x1B4C, 0xDE5C84AD},
	{343, 1, 2, 0x2D8C, 0x55E8CCA3},
};

// Effect and index -> file contents for the current battle, empty when there is no valid file
static std::map<uint32_t, std::vector<uint8_t>> anim_set_files;

static uint32_t ff8_battle_anim_set_hash(const uint8_t *data, uint32_t size)
{
	uint32_t hash = 0x811C9DC5;

	for (uint32_t i = 0; i < size; ++i) hash = (hash ^ data[i]) * 0x01000193;

	return hash;
}

static void ff8_battle_anim_set_name(const ff8_battle_anim_set_entry &entry, char *name, size_t size)
{
	if (entry.index == 0) _snprintf(name, size, "effect_%03d", entry.effect_id);
	else _snprintf(name, size, "effect_%03d_%d", entry.effect_id, entry.index);
}

// Animation section: u32 count, u32 offsets[count], then each animation starts with its frame count
static bool ff8_battle_anim_set_animations_valid(const uint8_t *section, uint32_t size)
{
	if (size < 8) return false;

	uint32_t count = *(const uint32_t *)section;

	if (count == 0 || count > (size - 4) / 4) return false;

	const uint32_t *offsets = (const uint32_t *)(section + 4);

	for (uint32_t i = 0; i < count; ++i)
	{
		if (offsets[i] < 4 + 4 * count || offsets[i] >= size || section[offsets[i]] == 0) return false;
	}

	return true;
}

// Sequence section: u16 count, u16 offsets[count] (0: empty sequence), then the byte code
static bool ff8_battle_anim_set_sequences_valid(const uint8_t *section, uint32_t size)
{
	if (size < 4) return false;

	uint32_t count = *(const uint16_t *)section;

	if (count == 0 || count > (size - 2) / 2) return false;

	const uint16_t *offsets = (const uint16_t *)(section + 2);

	for (uint32_t i = 0; i < count; ++i)
	{
		if (offsets[i] != 0 && (offsets[i] < 2 + 2 * count || offsets[i] >= size)) return false;
	}

	return true;
}

static bool ff8_battle_anim_set_is_valid(const std::vector<uint8_t> &file, uint32_t section_count, const char *filename)
{
	const char *error = nullptr;
	const uint32_t *header = (const uint32_t *)file.data();
	uint32_t header_size = 4 * (section_count + 2);

	if (file.size() < header_size) error = "file too small";
	else if (header[0] != section_count) error = "the section count does not match the vanilla set";
	else if (header[1] != header_size) error = "bad first section offset";
	else if (header[section_count + 1] != file.size()) error = "the size in the header does not match the file size";
	else
	{
		for (uint32_t i = 0; i < section_count && error == nullptr; ++i)
		{
			uint32_t start = header[i + 1], end = header[i + 2];

			if (end < start || end > file.size()) error = "section offset out of range";
			else if (i == 1 && !ff8_battle_anim_set_sequences_valid(file.data() + start, end - start)) error = "bad sequence section";
			else if (i != 1 && !ff8_battle_anim_set_animations_valid(file.data() + start, end - start)) error = "bad animation section";
		}
	}

	if (error != nullptr && (trace_all || trace_battle_animation)) ffnx_trace("%s: ignoring %s: %s\n", __func__, filename, error);

	return error == nullptr;
}

// The vanilla set the exe passes: same effect and same contents, or else the only effect with these contents
static const ff8_battle_anim_set_entry *ff8_battle_anim_set_find(const uint32_t *anim_set)
{
	uint32_t section_count = anim_set[0];

	if (section_count != 2 && section_count != 3) return nullptr;

	uint32_t size = anim_set[section_count + 1];
	int effect_id = *ff8_externals.battle_magic_id + 1;
	const ff8_battle_anim_set_entry *found = nullptr;
	uint32_t hash = 0;
	bool hashed = false, ambiguous = false;

	for (const ff8_battle_anim_set_entry &entry : vanilla_anim_sets)
	{
		if (entry.section_count != section_count || entry.size != size) continue;

		if (!hashed)
		{
			hash = ff8_battle_anim_set_hash((const uint8_t *)anim_set, size);
			hashed = true;
		}

		if (entry.fnv1a != hash) continue;

		if (entry.effect_id == effect_id) return &entry;

		if (found != nullptr && found->effect_id != entry.effect_id) ambiguous = true;

		found = &entry;
	}

	if (found != nullptr && (trace_all || trace_battle_animation)) ffnx_trace("%s: set of effect %d bound while effect %d runs%s\n", __func__, found->effect_id, effect_id, ambiguous ? ", several effects share it, keeping the exe set" : "");

	return ambiguous ? nullptr : found;
}

static uint32_t *ff8_battle_anim_set_get_file(const ff8_battle_anim_set_entry &entry)
{
	uint32_t key = (uint32_t(entry.effect_id) << 8) | entry.index;
	auto it = anim_set_files.find(key);

	if (it != anim_set_files.end()) return it->second.empty() ? nullptr : (uint32_t *)it->second.data();

	std::vector<uint8_t> &file_data = anim_set_files[key];
	char name[24], filename[MAX_PATH];

	ff8_battle_anim_set_name(entry, name, sizeof(name));

	if (!ff8_get_exe_path(name, filename, "animset")) return nullptr;

	FILE *file = fopen(filename, "rb");

	if (file == nullptr) return nullptr;

	if (fseek(file, 0, SEEK_END) == 0)
	{
		long size = ftell(file);

		if (size > 0 && size <= FF8_ANIM_SET_MAX_SIZE && fseek(file, 0, SEEK_SET) == 0)
		{
			file_data.resize(size);

			if (fread(file_data.data(), 1, size, file) != size_t(size)) file_data.clear();
		}
	}

	fclose(file);

	if (!ff8_battle_anim_set_is_valid(file_data, entry.section_count, filename))
	{
		file_data.clear();
		file_data.shrink_to_fit();

		return nullptr;
	}

	if (trace_all || trace_battle_animation) ffnx_trace("%s: %s replaces the exe set (%d bytes)\n", __func__, filename, int(file_data.size()));

	return (uint32_t *)file_data.data();
}

// Same as the game: bind the body animations and sequences, and the weapon animations when the model has a weapon
static ff8_battle_com_file_data *ff8_battle_anim_set_bind(ff8_battle_entity_state *entity, ff8_battle_com_file_data *bind_block, uint32_t *anim_set)
{
	const ff8_battle_anim_set_entry *entry = ff8_battle_anim_set_find(anim_set);

	if (entry != nullptr)
	{
		uint32_t *file_set = ff8_battle_anim_set_get_file(*entry);

		if (file_set != nullptr) anim_set = file_set;
	}

	ff8_set_com_file_data set_com_file_data = (ff8_set_com_file_data)ff8_externals.battle_set_com_file_data_sub_502270;
	uint8_t *data = (uint8_t *)anim_set;

	entity->combat_flags |= FF8_BATTLE_ENTITY_EXTERNAL_ANIM_SET;
	bind_block->animations = data + anim_set[1];
	bind_block->sequences = data + anim_set[2];
	set_com_file_data(entity->anim_header, bind_block, FF8_ANIM_SET_BODY_SECTIONS);
	bind_block++;

	if (entity->weapon_anim_header != nullptr)
	{
		bind_block->animations = data + anim_set[3];
		set_com_file_data(entity->weapon_anim_header, bind_block, FF8_ANIM_SET_WEAPON_SECTIONS);
		bind_block++;
	}

	if (entity->com_id < FF8_BATTLE_FIRST_MONSTER_COM_ID)
	{
		entity->sequence_state->saved_base_sequence = entity->sequence_state->base_sequence;
		entity->sequence_state->base_sequence = 1;
	}

	return bind_block;
}

// Files are kept for one battle, so a changed file is read again in the next battle
static void ff8_battle_anim_set_release()
{
	anim_set_files.clear();
}

static int ff8_battle_anim_set_battle_reset()
{
	ff8_battle_anim_set_release();

	return ((int (*)())ff8_externals.sub_500870)();
}

static void ff8_battle_anim_set_battle_exit()
{
	((void (*)())ff8_externals.battle_exit_sub_4868C0)();

	ff8_battle_anim_set_release();
}

void ff8_battle_anim_set_init()
{
	replace_function(ff8_externals.battle_bind_anim_set_sub_5022C0, ff8_battle_anim_set_bind);
	replace_call(ff8_externals.loc_47D490 + 0x85, ff8_battle_anim_set_battle_reset);
	replace_call(ff8_externals.loc_47D490 + 0x1F, ff8_battle_anim_set_battle_exit);
}
