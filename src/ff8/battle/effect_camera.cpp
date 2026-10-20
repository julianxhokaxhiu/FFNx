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
// Battle effect cameras from external files, one file per effect:
//
//     <direct_mode_path>/battle/effect_camera/effect_NNN.cam
//
// NNN is the effect id of the action (kernel.bin magic "effect id", the one
// passed to the effect loader; 1 = Cure). The file is a camera track exactly
// as the exe stores it (see docs/ff8/mods/effect_camera.md).
//
// A magic cast loads its effect init with battle_read_effect_sub_50AF20, then
// calls it once through a function pointer when the effect starts:
//
//     C3_28_GF_data_pointer = MAGIC_EFFECT_LOGIC_CALLBACK(&g_MagicCastContext);
//
// Most effect inits start their camera from there with a static track:
//
//     if (!(cast_context->flags & 1)) Battle_PlayCameraAnimation(&MAGnnn_cameraAnim);
//
// We hook the loader call and hand the game our wrapper instead of the init.
// While the wrapper runs the init, a hook on Battle_PlayCameraAnimation swaps
// in the file track (override). If the init started no camera at all, the
// wrapper starts the file track itself right after it (add). The tick then
// waits for the camera like for a built-in one, and restores the view at the
// end of the effect.
//
// Without a file nothing changes: many effects with no camera track move the
// camera with their own code, so a camera is never forced on them. Tracks
// started outside the effect start (GF cinematics, victory, battle intro)
// are never touched.
// -------------------------------------------------------------------------

#include "effect_camera.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"
#include "../../cfg.h"
#include "../../log.h"

#include <intrin.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <vector>

#pragma intrinsic(_ReturnAddress)

#define FF8_CAMERA_TRACK_POINTER_COUNT 2  // every exe track: script + keyframe collection
#define FF8_CAMERA_TRACK_SCRIPT_OFFSET 8  // the script follows the header
#define FF8_CAMERA_TRACK_MAX_SIZE      0xFFFF // the header size field is a u16
#define FF8_MAGIC_CAST_NO_CAMERA       0x01 // MagicCastContext.flags: the effect must not move the camera
#define FF8_CALL_INDIRECT_SIZE         6    // call dword ptr [disp32]

struct ff8_camera_track_header
{
	uint16_t pointer_count;
	uint16_t script_offset;
	uint16_t keyframes_offset;
	uint16_t size;
};

static_assert(sizeof(ff8_camera_track_header) == 8, "camera track header is 8 bytes");

struct ff8_magic_cast_context
{
	uint8_t attacker_slot;
	uint8_t flags;
	uint16_t target_slot_mask;
	void *action_data;
	uint8_t sequence_mode;
	uint8_t done_flag;
	uint8_t sequence_submode;
	uint8_t unknown_11;
};

typedef int (*ff8_magic_effect_init)(ff8_magic_cast_context *cast_context);
typedef void *(*ff8_battle_read_effect)(int effect_id, ff8_magic_effect_init *init);
typedef void *(*ff8_battle_camera_track)(uint8_t *track);

// Effect id -> file track, read the first time the effect plays. An empty
// track means no usable file. Kept for the whole game: the engine reads the
// track in place for as long as the camera plays.
static std::unordered_map<int, std::vector<uint8_t>> effect_camera_tracks;

static ff8_magic_effect_init effect_camera_original_init = nullptr;
static int effect_camera_effect_id = 0;
static uint8_t *effect_camera_track = nullptr; // file track of the effect starting now
static bool effect_camera_inside_start = false;
static bool effect_camera_started = false;

static bool ff8_effect_camera_is_valid(const std::vector<uint8_t> &track, const char *filename)
{
	const char *error = nullptr;
	const ff8_camera_track_header *header = (const ff8_camera_track_header *)track.data();

	if (track.size() < sizeof(ff8_camera_track_header))
		error = "file too small";
	else if (header->pointer_count != FF8_CAMERA_TRACK_POINTER_COUNT || header->script_offset != FF8_CAMERA_TRACK_SCRIPT_OFFSET)
		error = "bad header";
	else if (header->size != track.size())
		error = "header size does not match the file size";
	else if (header->keyframes_offset <= header->script_offset || header->keyframes_offset + 2 > header->size)
		error = "keyframe offset out of range";
	else
	{
		// Keyframe collection: u16 set count, u16 set offsets[count], u16 collection size,
		// all relative to the collection start.
		const uint8_t *collection = track.data() + header->keyframes_offset;
		uint32_t collection_size = header->size - header->keyframes_offset;
		uint32_t set_count = *(const uint16_t *)collection;
		uint32_t table_size = 2 + 2 * set_count + 2;

		if (set_count == 0 || table_size > collection_size)
			error = "bad keyframe set count";
		else if (*(const uint16_t *)(collection + table_size - 2) != collection_size)
			error = "keyframe collection size does not match the file size";
		else
		{
			for (uint32_t i = 0; i < set_count; ++i)
			{
				uint16_t set_offset = *(const uint16_t *)(collection + 2 + 2 * i);

				if (set_offset < table_size || set_offset >= collection_size)
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
	char filename[MAX_PATH];

	_snprintf(filename, sizeof(filename), "%s/%s/battle/effect_camera/effect_%03d.cam", basedir, direct_mode_path.c_str(), effect_id);

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

// Replaces Battle_PlayCameraAnimation
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

	return ((ff8_battle_camera_track)ff8_externals.battle_set_camera_track_sub_509970)(track);
}

// Stands in for the effect init the game called MAGIC_EFFECT_LOGIC_CALLBACK
static int ff8_effect_camera_effect_start(ff8_magic_cast_context *cast_context)
{
	ff8_magic_effect_init init = effect_camera_original_init;

	// Only the effect start of a magic cast: the Duel and Shot ticks call the
	// same pointer again for the later stages of their effect.
	if (uint32_t(_ReturnAddress()) != ff8_externals.battle_magic_effect_start_call + FF8_CALL_INDIRECT_SIZE) return init(cast_context);

	effect_camera_track = ff8_effect_camera_get_track(effect_camera_effect_id);
	effect_camera_started = false;
	effect_camera_inside_start = true;

	int ret = init(cast_context);

	effect_camera_inside_start = false;

	if (effect_camera_track != nullptr && !effect_camera_started && !(cast_context->flags & FF8_MAGIC_CAST_NO_CAMERA))
	{
		if (trace_all || trace_battle_animation) ffnx_trace("%s: effect %d camera added from its file\n", __func__, effect_camera_effect_id);

		((ff8_battle_camera_track)ff8_externals.battle_play_camera_animation_sub_5099A0)(effect_camera_track);
	}

	effect_camera_track = nullptr;

	return ret;
}

// Replaces the battle_read_effect_sub_50AF20 call of the magic cast tick
static void *ff8_effect_camera_read_effect(int effect_id, ff8_magic_effect_init *init)
{
	void *ret = ((ff8_battle_read_effect)ff8_externals.battle_read_effect_sub_50AF20)(effect_id, init);

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
	// mov eax, [esp+4] ; push eax ; call battle_set_camera_track_sub_509970
	static const uint8_t play_camera_code[] = { 0x8B, 0x44, 0x24, 0x04, 0x50, 0xE8 };
	const uint8_t *start_call = (const uint8_t *)ff8_externals.battle_magic_effect_start_call;

	if (memcmp((const void *)ff8_externals.battle_play_camera_animation_sub_5099A0, play_camera_code, sizeof(play_camera_code)) != 0
		|| start_call[0] != 0xFF || start_call[1] != 0x15 || *(uint32_t *)(start_call + 2) != uint32_t(ff8_externals.battle_magic_effect_callback))
	{
		ffnx_warning("Battle effect camera: unexpected battle code layout, external effect cameras disabled\n");
		return;
	}

	replace_call(ff8_externals.sub_50A9A0 + 0xF4, ff8_effect_camera_read_effect);
	replace_function(ff8_externals.battle_play_camera_animation_sub_5099A0, ff8_effect_camera_play);
}
