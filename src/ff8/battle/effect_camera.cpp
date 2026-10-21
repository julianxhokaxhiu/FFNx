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

// Battle effect cameras from <direct_mode_path>/exe/effect_NNN.bcam, see docs/ff8/mods/effect_camera.md.
// While a magic cast starts its effect, the file track replaces the camera the effect plays,
// or is played after it when the effect plays none.

#include "effect_camera.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"
#include "../../cfg.h"
#include "../../log.h"
#include "../../exe_data.h"

#include <intrin.h>
#include <stdint.h>
#include <stdio.h>
#include <unordered_map>
#include <vector>

#pragma intrinsic(_ReturnAddress)

#define FF8_CAMERA_TRACK_MAX_SIZE 0xFFFF
#define FF8_MAGIC_CAST_NO_CAMERA 0x01
#define FF8_CALL_INDIRECT_SIZE 6

typedef int (*ff8_magic_effect_init)(ff8_battle_magic_cast_context *cast_context);

// Effect id -> file track, empty when there is no valid file. Kept for the whole game, the engine reads the track in place.
static std::unordered_map<int, std::vector<uint8_t>> effect_camera_tracks;

static ff8_magic_effect_init effect_camera_original_init = nullptr;
static int effect_camera_effect_id = 0;
static uint8_t *effect_camera_track = nullptr;
static bool effect_camera_inside_start = false;
static bool effect_camera_started = false;

static bool ff8_effect_camera_is_valid(const std::vector<uint8_t> &track, const char *filename)
{
	const char *error = nullptr;
	const ff8_battle_camera_track_header *header = (const ff8_battle_camera_track_header *)track.data();

	if (track.size() < sizeof(ff8_battle_camera_track_header)) error = "file too small";
	else if (header->pointer_count != 2 || header->script_offset != sizeof(ff8_battle_camera_track_header)) error = "bad header";
	else if (header->size != track.size()) error = "header size does not match the file size";
	else if (header->keyframes_offset <= header->script_offset || header->keyframes_offset + 2 > header->size) error = "keyframe offset out of range";
	else
	{
		// Keyframe collection: u16 count, u16 offsets[count], u16 collection size
		const uint16_t *collection = (const uint16_t *)(track.data() + header->keyframes_offset);
		uint32_t collection_size = header->size - header->keyframes_offset;
		uint32_t count = collection[0];
		uint32_t table_size = 2 * (count + 2);

		if (count == 0 || table_size > collection_size) error = "bad keyframe set count";
		else if (collection[count + 1] != collection_size) error = "keyframe collection size does not match the file size";
		else
		{
			for (uint32_t i = 1; i <= count; ++i)
			{
				if (collection[i] < table_size || collection[i] >= collection_size)
				{
					error = "keyframe set offset out of range";
					break;
				}
			}
		}
	}

	if (error != nullptr && (trace_all || trace_battle_animation)) ffnx_trace("%s: ignoring %s: %s\n", __func__, filename, error);

	return error == nullptr;
}

static uint8_t *ff8_effect_camera_get_track(int effect_id)
{
	auto it = effect_camera_tracks.find(effect_id);

	if (it != effect_camera_tracks.end()) return it->second.empty() ? nullptr : it->second.data();

	std::vector<uint8_t> &track = effect_camera_tracks[effect_id];
	char name[16], filename[MAX_PATH];

	_snprintf(name, sizeof(name), "effect_%03d", effect_id);

	if (!ff8_get_exe_path(name, filename, "bcam")) return nullptr;

	FILE *file = fopen(filename, "rb");

	if (file == nullptr) return nullptr;

	if (fseek(file, 0, SEEK_END) == 0)
	{
		long size = ftell(file);

		if (size > 0 && size <= FF8_CAMERA_TRACK_MAX_SIZE && fseek(file, 0, SEEK_SET) == 0)
		{
			track.resize(size);

			if (fread(track.data(), 1, size, file) != size_t(size)) track.clear();
		}
	}

	fclose(file);

	if (!ff8_effect_camera_is_valid(track, filename))
	{
		track.clear();
		track.shrink_to_fit();

		return nullptr;
	}

	if (trace_all || trace_battle_animation) ffnx_trace("%s: effect %d uses %s (%d bytes)\n", __func__, effect_id, filename, int(track.size()));

	return track.data();
}

static void *ff8_effect_camera_play(uint8_t *track)
{
	if (effect_camera_inside_start)
	{
		effect_camera_started = true;

		if (effect_camera_track != nullptr)
		{
			if (trace_all || trace_battle_animation) ffnx_trace("%s: effect %d camera replaced by its file\n", __func__, effect_camera_effect_id);

			track = effect_camera_track;
		}
	}

	return ((void *(*)(uint8_t *))ff8_externals.battle_set_camera_track_sub_509970)(track);
}

static int ff8_effect_camera_effect_start(ff8_battle_magic_cast_context *cast_context)
{
	ff8_magic_effect_init init = effect_camera_original_init;

	// Duel and Shot call the same effect again later from elsewhere: leave those calls alone
	if (uint32_t(_ReturnAddress()) != ff8_externals.battle_magic_effect_start_call + FF8_CALL_INDIRECT_SIZE) return init(cast_context);

	effect_camera_track = ff8_effect_camera_get_track(effect_camera_effect_id);
	effect_camera_started = false;
	effect_camera_inside_start = true;

	int ret = init(cast_context);

	effect_camera_inside_start = false;

	if (effect_camera_track != nullptr && !effect_camera_started && !(cast_context->flags & FF8_MAGIC_CAST_NO_CAMERA))
	{
		if (trace_all || trace_battle_animation) ffnx_trace("%s: effect %d camera added from its file\n", __func__, effect_camera_effect_id);

		((void *(*)(uint8_t *))ff8_externals.battle_play_camera_animation_sub_5099A0)(effect_camera_track);
	}

	effect_camera_track = nullptr;

	return ret;
}

// The game calls the loaded effect through a pointer, so we hand it our wrapper instead
static void *ff8_effect_camera_read_effect(int effect_id, ff8_magic_effect_init *init)
{
	void *ret = ((void *(*)(int, ff8_magic_effect_init *))ff8_externals.battle_read_effect_sub_50AF20)(effect_id, init);

	if (*init != nullptr)
	{
		effect_camera_original_init = *init;
		effect_camera_effect_id = effect_id;
		*init = ff8_effect_camera_effect_start;
	}

	return ret;
}

void ff8_battle_effect_camera_init()
{
	replace_call(ff8_externals.sub_50A9A0 + 0xF4, ff8_effect_camera_read_effect);
	replace_function(ff8_externals.battle_play_camera_animation_sub_5099A0, ff8_effect_camera_play);
}
