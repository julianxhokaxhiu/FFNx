/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include "widescreen.h"
#include "battle/effects.h"

#include "../common.h"
#include "../cfg.h"
#include "../ff8.h"
#include "../gl.h"
#include "../globals.h"
#include "../log.h"
#include "../macro.h"
#include "../patch.h"
#include "../renderer.h"
#include "../saveload.h"
#include "../utils.h"
#include "../widescreen.h"
#include "file.h"

#include <cmath>
#include <initializer_list>
#include <unordered_map>
#include <vector>

struct ff8_card_background
{
	uint32_t texture = 0;
	bool attempted = false;
	uint32_t frame = 0;
	bool drawn = false;
	bool traced_draw = false;
};

static ff8_card_background ff8_card_backgrounds[2];
struct ff8_card_background_packet
{
	int background;
	bool first_page;
};

static int ff8_card_background_source = -1;
static int ff8_card_background_sprite = -1;
static std::unordered_map<const void *, ff8_card_background_packet> ff8_card_background_packets;
static std::unordered_map<const void *, int> ff8_card_background_objects;
static uint32_t (*ff8_card_submit_background)(int *, uint32_t *, int, int, int, int);
static void (*ff8_card_draw_sprite)(uint32_t *);
static int (*ff8_card_append_sprite)(int, ff8_graphics_object *);

void ff8_widescreen_reset_card_backgrounds()
{
	for (auto &background : ff8_card_backgrounds)
	{
		if (background.texture) newRenderer.deleteTexture(background.texture);
		background = {};
	}
	ff8_card_background_source = -1;
	ff8_card_background_sprite = -1;
	ff8_card_background_packets.clear();
	ff8_card_background_objects.clear();
}

static uint32_t ff8_load_wide_card_background(ff8_card_background &background, const char *name)
{
	if (background.attempted) return background.texture;
	background.attempted = true;

	char language[16] = {};
	ff8_fs_lang_string(language);
	const char *aspect = aspect_ratio == AR_WIDESCREEN_16X10 ? "16x10" : "16x9";
	const float expected_ratio = aspect_ratio == AR_WIDESCREEN_16X10 ? 1.6f : 16.0f / 9.0f;

	for (const auto &path : {override_mod_path, mod_path})
	{
		if (path.empty()) continue;
		for (const auto &extension : mod_ext)
		{
			for (const char *locale : std::initializer_list<const char *>{language, ""})
			{
				char filename[sizeof(basedir) + 1024] = {};
				const int length = snprintf(filename, sizeof(filename), "%s\\%s\\%s%s%s_%s_00.%s",
					basedir, path.c_str(), locale, *locale ? "\\" : "", name, aspect, extension.c_str());
				if (length < 0 || length >= sizeof(filename))
				{
					ffnx_warning("Card widescreen background path is too long: %s\n", name);
					continue;
				}
				if (!fileExists(filename)) continue;

				uint32_t width = 0, height = 0;
				const uint32_t texture = load_texture_helper(filename, &width, &height, extension == "png", true);
				if (!texture)
				{
					ffnx_warning("Could not load Card widescreen background: %s\n", filename);
					continue;
				}
				if (!width || !height || width > max_texture_size || height > max_texture_size
					|| std::abs(float(width) / height - expected_ratio) > 0.01f)
				{
					ffnx_warning("Card widescreen background must have aspect ratio %s and supported dimensions: %s (%ux%u)\n",
						aspect, filename, width, height);
					newRenderer.deleteTexture(texture);
					continue;
				}
				background.texture = texture;
				return texture;
			}
		}
	}
	if (trace_all || trace_loaders)
		ffnx_trace("No usable Card widescreen background for %s (%s); keeping native background\n", name, aspect);
	return 0;
}

void ff8_widescreen_set_card_background_source(const uint8_t *image)
{
	ff8_card_background_source = image == ff8_externals.cardgame_tim_texture_intro ? 0
		: image == ff8_externals.cardgame_tim_texture_game ? 1 : -1;
}

static uint32_t ff8_card_submit_background_page(int *ordering_table, uint32_t *packet, int z0, int z1, int z2, int z3, bool first_page)
{
	const uint32_t result = ff8_card_submit_background(ordering_table, packet, z0, z1, z2, z3);
	if (getmode_cached()->driver_mode == MODE_CARDGAME && ff8_card_background_source >= 0)
		ff8_card_background_packets[packet] = {ff8_card_background_source, first_page};
	else
		ff8_card_background_packets.erase(packet);
	return result;
}

static uint32_t ff8_card_submit_first_background_page(int *ordering_table, uint32_t *packet, int z0, int z1, int z2, int z3)
{
	return ff8_card_submit_background_page(ordering_table, packet, z0, z1, z2, z3, true);
}

static uint32_t ff8_card_submit_second_background_page(int *ordering_table, uint32_t *packet, int z0, int z1, int z2, int z3)
{
	return ff8_card_submit_background_page(ordering_table, packet, z0, z1, z2, z3, false);
}

static void ff8_card_draw_background_sprite(uint32_t *packet)
{
	const auto it = ff8_card_background_packets.find(packet);
	if (it == ff8_card_background_packets.end())
	{
		ff8_card_draw_sprite(packet);
		return;
	}
	const ff8_card_background_packet background_packet = it->second;
	ff8_card_background_packets.erase(it);
	const int index = background_packet.background;
	const char *name = index == 0 ? "cardgame\\intro" : "cardgame\\game";
	if (!ff8_load_wide_card_background(ff8_card_backgrounds[index], name))
	{
		ff8_card_draw_sprite(packet);
		return;
	}
	if (!background_packet.first_page) return;

	// The callback appends to native batches; retain the background's identity until those batches draw.
	const int previous_sprite = ff8_card_background_sprite;
	ff8_card_background_sprite = index;
	ff8_card_draw_sprite(packet);
	ff8_card_background_sprite = previous_sprite;
}

static int ff8_card_append_background_sprite(int count, ff8_graphics_object *graphics_object)
{
	const int result = ff8_card_append_sprite(count, graphics_object);
	if (ff8_card_background_sprite >= 0 && result)
		ff8_card_background_objects[graphics_object] = ff8_card_background_sprite;
	else
		ff8_card_background_objects.erase(graphics_object);
	return result;
}

bool ff8_widescreen_draw_card_background(uint32_t primitivetype, uint32_t vertextype, const nvertex *vertices, const graphics_object *graphics_object, uint32_t clip)
{
	if (!widescreen_enabled || getmode_cached()->driver_mode != MODE_CARDGAME) return false;
	const auto it = ff8_card_background_objects.find(graphics_object);
	if (it == ff8_card_background_objects.end()) return false;
	auto &background = ff8_card_backgrounds[it->second];
	const uint32_t texture = background.texture;
	if (!texture) return false;
	if (!background.traced_draw && (trace_all || trace_loaders))
	{
		ffnx_trace("Card background draw: %s (native background graphics object %p)\n",
			it->second == 0 ? "cardgame\\intro" : "cardgame\\game", graphics_object);
		background.traced_draw = true;
	}

	// Native clipping can split the first page into multiple graphics objects.
	if (background.drawn && background.frame == frame_counter) return true;
	background.frame = frame_counter;
	background.drawn = true;
	nvertex quad[4] = {vertices[0], vertices[0], vertices[0], vertices[0]};
	for (uint32_t i = 0; i < 4; ++i)
	{
		quad[i]._.x = wide_viewport_x + (i >= 2 ? wide_viewport_width : 0);
		quad[i]._.y = wide_viewport_y + (i % 2 ? wide_viewport_height : 0);
		quad[i].u = i >= 2 ? 1.0f : 0.0f;
		quad[i].v = i % 2 ? 1.0f : 0.0f;
	}
	uint16_t quad_indices[] = {0, 1, 2, 1, 3, 2};
	driver_state saved_state;
	gl_save_state(&saved_state);
	VOBJ(texture_set, texture_set, saved_state.texture_set);
	gl_set_texture(texture, nullptr);
	current_state.texture_filter = true;
	newRenderer.isExternalTexture(true);
	gl_draw_indexed_primitive(primitivetype, TLVERTEX, quad, nullptr, 4, quad_indices, 6, nullptr, nullptr, nullptr, 0, 0);
	gl_load_state(&saved_state);
	newRenderer.isExternalTexture(saved_state.texture_set && VREF(texture_set, ogl.external) != 0);
	newRenderer.doScissorTest(clip != 0);
	return true;
}

static bool ff8_is_widescreen_mode(uint32_t mode)
{
	return mode == MODE_BATTLE || mode == MODE_WORLDMAP || mode == MODE_SWIRL
		|| mode == MODE_MENU || mode == MODE_MAIN_MENU;
}

void ff8_clear(uint32_t clear_color, uint32_t clear_depth, uint32_t unknown, struct game_obj *game_object)
{
	// Native-size scenes must not retain pixels outside their viewport from a preceding widescreen scene.
	const bool clear_native_viewport = widescreen_enabled && !ff8_is_widescreen_mode(getmode_cached()->driver_mode);
	common_clear(clear_color || clear_native_viewport, clear_depth, unknown, game_object);
}

void* ff8_engine_set_wide_viewport(int x, int y, int w, int h)
{
	const bool use_wide_viewport = ff8_is_widescreen_mode(getmode()->driver_mode);
	const int viewport_x = use_wide_viewport ? wide_viewport_x : x;
	const int viewport_y = use_wide_viewport ? wide_viewport_y : y;
	const int viewport_width = use_wide_viewport ? wide_viewport_width : w;
	const int viewport_height = use_wide_viewport ? wide_viewport_height : h;

	*ff8_externals.current_viewport_x_dword_1A7764C = viewport_x;
	*ff8_externals.current_viewport_y_dword_1A77648 = viewport_y;
	*ff8_externals.current_viewport_width_dword_1A77654 = viewport_width;
	*ff8_externals.current_viewport_height_dword_1A77650 = viewport_height;

	*ff8_externals.ssigpu_viewport_x_dword_1CA89D8 = x;
	*ff8_externals.ssigpu_viewport_y_dword_1CA89DC = y;
	*ff8_externals.ssigpu_viewport_width_dword_B7CBF8 = w;
	*ff8_externals.ssigpu_viewport_height_dword_B7CBFC = h;

	if ( w >= 540 || h >= 380 )
	{
		if ( *ff8_externals.dword_B7CE28 != -1 )
			*ff8_externals.flag_d3d_renderer_related_dword_1CCFD94 = *ff8_externals.dword_B7CE28;
	}
	else
	{
		int tmp = *ff8_externals.flag_d3d_renderer_related_dword_1CCFD94;
		*ff8_externals.flag_d3d_renderer_related_dword_1CCFD94 = 0;
		*ff8_externals.dword_B7CE28 = tmp;
	}

	return ff8_externals.engine_setviewport_sub_41E070(viewport_x, viewport_y, viewport_width, viewport_height, common_externals.get_game_object());
}

void* ff8_swirl_set_wide_viewport(int x, int y, int w, int h)
{
	x = 0;
	y = 0;
	w = 640;
	h = 480;

	*ff8_externals.swirl_viewport_x = x;
	*ff8_externals.swirl_viewport_y = y;
	*ff8_externals.swirl_viewport_width = w;
	*ff8_externals.swirl_viewport_height = h;

	return ff8_engine_set_wide_viewport(x, y, w, h);
}

static bool ff8_battle_stage_projection = false;
static void **ff8_battle_active_effect = nullptr;
static void *ff8_the_end_effect = nullptr;

int ff8_battle_stage_draw(uint32_t *model, int descriptor, int depth_table, int depth_shift, int output)
{
	const bool previous_stage_projection = ff8_battle_stage_projection;
	ff8_battle_stage_projection = true;
	const int result = reinterpret_cast<int (*)(uint32_t *, int, int, int, int)>(ff8_externals.sub_5099D0)(model, descriptor, depth_table, depth_shift, output);
	ff8_battle_stage_projection = previous_stage_projection;

	return result;
}

int16_t ff8_battle_world_calc_zoom(int16_t *descriptor, int16_t **vertices, uint32_t **projected_vertices)
{
	const int16_t clip_x = descriptor[10];
	const int16_t clip_width = descriptor[12];
	const int16_t clip_height = descriptor[13];
	int16_t *const source_begin = *vertices;
	uint32_t *const projected_begin = *projected_vertices;
	// The End's custom scenery uses native Y projection; its actors must use the same coordinates.
	const bool native_vertical_projection = ff8_battle_active_effect != nullptr
		&& *ff8_battle_active_effect == ff8_the_end_effect;

	if (getmode_cached()->driver_mode == MODE_BATTLE)
	{
		const int wide_right = wide_viewport_x + wide_viewport_width;
		const int half_bottom = static_cast<int>(ceil((wide_viewport_y + wide_viewport_height) / 2.0));
		const int half_left = static_cast<int>(floor((wide_viewport_x - wide_viewport_width) / 2.0));
		const int half_right = static_cast<int>(ceil((wide_right + wide_viewport_width) / 2.0));

		descriptor[10] = static_cast<int16_t>(half_left);
		descriptor[12] = static_cast<int16_t>(half_right - half_left);
		if (descriptor[11] + descriptor[13] < half_bottom)
			descriptor[13] = static_cast<int16_t>(half_bottom - descriptor[11]);
	}

	const int16_t result = ff8_externals.world_calc_zoom_sub_50F900(descriptor, vertices, projected_vertices);
	if (ff8_battle_stage_projection && clip_height > 0)
	{
		const int vertex_count = *source_begin;
		const int zoom = ff8_externals.camera_zoom_dword_1CA92E4[0];
		const int near_guard = zoom / 8;
		const int clip_left = 8 * descriptor[10];
		const int clip_top = 8 * descriptor[11];
		const int clip_right = clip_left + 8 * descriptor[12];
		const int clip_bottom = clip_top + 8 * descriptor[13];

		for (int vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
		{
			const int16_t *const source = source_begin + 1 + 3 * vertex_index;
			uint32_t *const projected = projected_begin + 2 * vertex_index;
			if ((projected[1] & 0x20000) != 0)
			{
				const double precise_x = ff8_externals.dword_1CA9290[0]
					+ ff8_externals.flt_1CA9234[0] * source[2]
					+ ff8_externals.flt_1CA9234[1] * source[1]
					+ ff8_externals.flt_1CA9234[2] * source[0];
				const double precise_y = ff8_externals.dword_1CA9290[1]
					+ ff8_externals.flt_1CA9234[3] * source[2]
					+ ff8_externals.flt_1CA9234[4] * source[1]
					+ ff8_externals.flt_1CA9234[5] * source[0];
				const double precise_z = ff8_externals.dword_1CA9290[2]
					+ ff8_externals.flt_1CA9234[6] * source[2]
					+ ff8_externals.flt_1CA9234[7] * source[1]
					+ ff8_externals.flt_1CA9234[8] * source[0];
				const int projected_z = static_cast<int>(round(precise_z));

				if (projected_z > near_guard)
				{
					int projected_x = 8 * ff8_externals.word_1CA92DE[0] + static_cast<int>(round(8.0 * precise_x * zoom / projected_z));
					int projected_y = 8 * ff8_externals.word_1CA92DE[2] + static_cast<int>(round(8.0 * precise_y * zoom / projected_z));
					if (projected_x < -8192) projected_x = -8192;
					if (projected_x > 8184) projected_x = 8184;
					if (projected_y < -8192) projected_y = -8192;
					if (projected_y > 8184) projected_y = 8184;

					projected[0] = static_cast<uint16_t>(projected_x) | (static_cast<uint32_t>(static_cast<uint16_t>(projected_y)) << 16);
					projected[1] &= ~(0x20000 | 0x3F00000);
					if (projected_x < clip_left) projected[1] |= 0x1100000;
					else if (projected_x > clip_right) projected[1] |= 0x1200000;
					if (projected_y < clip_top) projected[1] |= 0x2400000;
					else if (projected_y > clip_bottom) projected[1] |= 0x2800000;
				}
			}
		}
	}
	if (getmode_cached()->driver_mode == MODE_BATTLE && clip_height == 216 && !native_vertical_projection)
	{
		const int clip_top = 8 * descriptor[11];
		const int source_height = 8 * clip_height;
		const int target_height = 4 * (wide_viewport_y + wide_viewport_height) - clip_top;

		for (uint32_t *projected = projected_begin; projected < *projected_vertices; projected += 2)
		{
			const int projected_y = static_cast<int16_t>(*projected >> 16);
			int scaled_y = static_cast<int>(round(clip_top + (projected_y - clip_top) * static_cast<double>(target_height) / source_height));
			if (ff8_battle_stage_projection)
			{
				const int target_center = clip_top + target_height / 2;
				scaled_y = static_cast<int>(round(target_center + (scaled_y - target_center) * static_cast<double>(target_height) / source_height));
			}
			if (scaled_y < -32768) scaled_y = -32768;
			if (scaled_y > 32767) scaled_y = 32767;
			*projected = (*projected & 0xFFFF) | (static_cast<uint32_t>(static_cast<uint16_t>(scaled_y)) << 16);
		}
	}
	descriptor[10] = clip_x;
	descriptor[12] = clip_width;
	descriptor[13] = clip_height;

	return result;
}

static uint32_t *ff8_worldmap_projected_vertices;

int ff8_worldmap_project_vertices(int vertex_count)
{
	// World-map packet Y=0 maps to screen Y=16.
	const int clip_left = 4 * wide_viewport_x;
	const int clip_right = clip_left + 4 * wide_viewport_width;
	const int clip_top = 4 * (wide_viewport_y - 16);
	const int clip_bottom = clip_top + 4 * wide_viewport_height;
	static std::vector<uint8_t> visibility;

	if (vertex_count > 0)
	{
		visibility.resize(vertex_count);
		const int zoom = static_cast<uint16_t>(*ff8_externals.camera_zoom_dword_1CA92E4);
		const int center_x = 8 * ff8_externals.word_1CA92DE[0];
		const int center_y = 8 * ff8_externals.word_1CA92DE[2];
		const float *const rotation = ff8_externals.flt_1CA9234;
		const int *const translation = ff8_externals.dword_1CA9290;

		// Test both full coordinates before the native projector overwrites them with 16-bit values.
		for (int vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
		{
			const int16_t *const source = reinterpret_cast<const int16_t *>(ff8_worldmap_projected_vertices + 2 * vertex_index);
			// Match the native single-precision intermediate before integer projection.
			const float camera_x = static_cast<float>(rotation[0] * (8.0 * source[2])
				+ rotation[1] * (8.0 * source[1])
				+ rotation[2] * (8.0 * source[0])
				+ 8.0 * translation[0]);
			const float camera_y = static_cast<float>(rotation[3] * (8.0 * source[2])
				+ rotation[4] * (8.0 * source[1])
				+ rotation[5] * (8.0 * source[0])
				+ 8.0 * translation[1]);
			const double camera_z = rotation[6] * static_cast<double>(source[2])
				+ rotation[7] * static_cast<double>(source[1])
				+ rotation[8] * static_cast<double>(source[0])
				+ translation[2];
			const bool perspective = std::nearbyint(camera_z) > (zoom >> 1);
			const double scale = perspective ? static_cast<double>(zoom) / camera_z : 0.0;
			const double projected_x = center_x + (perspective
				? std::nearbyint(scale * camera_x)
				: 2.0 * std::nearbyint(camera_x));
			const double projected_y = center_y + (perspective
				? std::nearbyint(scale * camera_y)
				: 2.0 * std::nearbyint(camera_y));

			visibility[vertex_index] = (projected_x >= clip_left && projected_x < clip_right ? 1u : 0u)
				| (projected_y >= clip_top && projected_y < clip_bottom ? 2u : 0u);
		}
	}

	const int result = reinterpret_cast<int (*)(int)>(ff8_externals.sub_4023D0)(vertex_count);

	for (int vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
	{
		uint32_t *const projected = ff8_worldmap_projected_vertices + 2 * vertex_index;

		// Only replace X/Y visibility; retain the native depth and near-plane flags.
		projected[1] = (projected[1] & ~0x30000u) | (static_cast<uint32_t>(visibility[vertex_index]) << 16);
	}

	return result;
}

static int (*ff8_worldmap_model_store_depths)(uint32_t *, uint32_t *, uint32_t *);

int ff8_worldmap_model_store_wide_depths(uint32_t *depth0, uint32_t *depth1, uint32_t *depth2)
{
	const int result = ff8_worldmap_model_store_depths(depth0, depth1, depth2);
	const int clip_left = 4 * wide_viewport_x;
	const int clip_right = clip_left + 4 * wide_viewport_width;
	const int clip_top = 4 * (wide_viewport_y - 16);
	const int clip_bottom = clip_top + 4 * wide_viewport_height;

	// This projector clamps packet coordinates, so signed 16-bit X/Y cannot wrap.
	for (uint32_t *depth : {depth0, depth1, depth2})
	{
		const int x = static_cast<int16_t>(depth[-1]);
		const int y = static_cast<int16_t>(depth[-1] >> 16);
		const uint32_t visibility = (x >= clip_left && x < clip_right ? 1u : 0u)
			| (y >= clip_top && y < clip_bottom ? 0x100u : 0u);
		*depth = (*depth & 0xFFFFu) | (visibility << 16);
	}

	return result;
}

static uint32_t (*ff8_worldmap_submit_sky_quad)(int *, uint32_t *, int, int, int, int);

uint32_t ff8_worldmap_submit_wide_sky_quad(int *ordering_table, uint32_t *packet, int z0, int z1, int z2, int z3)
{
	// These two native sky-gradient packets use quarter-pixel X coordinates.
	const uint16_t left = static_cast<uint16_t>(4 * wide_viewport_x);
	const uint16_t right = static_cast<uint16_t>(4 * (wide_viewport_x + wide_viewport_width));
	for (int i = 0; i < 4; ++i)
		packet[2 + 2 * i] = (packet[2 + 2 * i] & 0xFFFF0000u) | (i % 2 == 0 ? left : right);

	return ff8_worldmap_submit_sky_quad(ordering_table, packet, z0, z1, z2, z3);
}

using ff8_shop_draw_panel = int (*)(int, int *, int, int, int);
static ff8_shop_draw_panel ff8_shop_draw_owned;
static ff8_shop_draw_panel ff8_shop_draw_items;
static ff8_shop_draw_panel ff8_shop_draw_gil;

static int ff8_shop_wide_panel_x(int x, int visible_x)
{
	const sprite_viewport &viewport = ff8_externals.menu_viewport[2];
	const int hidden_x = static_cast<int>(ceil((wide_viewport_x + wide_viewport_width - viewport.offset_x) / viewport.scale_x));
	const int extra_distance = hidden_x > 384 ? hidden_x - 384 : 0;

	// Extend the native slide to X=384 without moving its visible endpoint.
	return x + (x - visible_x) * extra_distance / (384 - visible_x);
}

int ff8_shop_draw_wide_owned(int menu, int *graphics, int output, int x, int y)
{
	return ff8_shop_draw_owned(menu, graphics, output, ff8_shop_wide_panel_x(x, 280), y);
}

int ff8_shop_draw_wide_items(int menu, int *graphics, int output, int x, int y)
{
	return ff8_shop_draw_items(menu, graphics, output, ff8_shop_wide_panel_x(x, 24), y);
}

int ff8_shop_draw_wide_gil(int menu, int *graphics, int output, int x, int y)
{
	return ff8_shop_draw_gil(menu, graphics, output, ff8_shop_wide_panel_x(x, 24), y);
}

struct ff8_battle_packet_viewport
{
	int left, top, right, bottom;
};

static ff8_battle_packet_viewport ff8_battle_get_packet_viewport(int units)
{
	const double scale_x = 320.0 * units / *ff8_externals.ssigpu_viewport_width_dword_B7CBF8;
	const double scale_y = 240.0 * units / *ff8_externals.ssigpu_viewport_height_dword_B7CBFC;
	const int x = *ff8_externals.ssigpu_viewport_x_dword_1CA89D8;
	const int y = *ff8_externals.ssigpu_viewport_y_dword_1CA89DC;
	return {
		static_cast<int>(floor((wide_viewport_x - x) * scale_x)),
		static_cast<int>(floor((wide_viewport_y - y) * scale_y)),
		static_cast<int>(ceil((wide_viewport_x + wide_viewport_width - x) * scale_x)),
		static_cast<int>(ceil((wide_viewport_y + wide_viewport_height - y) * scale_y))
	};
}

static int (*ff8_battle_store_triangle)(uint32_t *, uint32_t *, uint32_t *);
static uint32_t *(*ff8_battle_store_fourth)(uint32_t *);
static uint32_t (*ff8_battle_submit_unprojected_quad)(int *, uint32_t *);
// Projecting a quad's fourth vertex shifts the GTE FIFO, so keep the first three packet coordinates.
static uint32_t ff8_battle_projected_vertices[4];

static int ff8_battle_store_wide_triangle(uint32_t *xy0, uint32_t *xy1, uint32_t *xy2)
{
	const int result = ff8_battle_store_triangle(xy0, xy1, xy2);
	ff8_battle_projected_vertices[0] = *xy0;
	ff8_battle_projected_vertices[1] = *xy1;
	ff8_battle_projected_vertices[2] = *xy2;
	return result;
}

static uint32_t *ff8_battle_store_wide_fourth(uint32_t *xy)
{
	uint32_t *const result = ff8_battle_store_fourth(xy);
	ff8_battle_projected_vertices[3] = *xy;
	return result;
}

template<uint32_t vertex_count>
static uint32_t ff8_battle_clip_polygon()
{
	const ff8_battle_packet_viewport viewport = ff8_battle_get_packet_viewport(8);
	uint32_t outside = 3;
	for (uint32_t i = 0; i < vertex_count; ++i)
	{
		const int x = static_cast<int16_t>(ff8_battle_projected_vertices[i]);
		const int y = static_cast<int16_t>(ff8_battle_projected_vertices[i] >> 16);
		const uint32_t flags = (x < viewport.left || x > viewport.right ? 1u : 0u)
			| (y < viewport.top || y > viewport.bottom ? 2u : 0u);
		outside &= flags;
	}

	// Preserve native rejection on each axis: wrapped sky polygons must not bridge opposite screen edges.
	return outside != 0 ? (1u << vertex_count) - 1 : 0;
}

struct ff8_battle_polygon_clip
{
	uint32_t store_triangle, store_fourth, reduction, secondary_test, secondary_size;
	bool patch_compare;
};

static void ff8_battle_patch_polygon_clips(uint32_t renderer, std::initializer_list<ff8_battle_polygon_clip> clips)
{
	for (const auto &clip : clips)
	{
		replace_call(renderer + clip.store_triangle, ff8_battle_store_wide_triangle);
		if (clip.store_fourth != 0)
			replace_call(renderer + clip.store_fourth, ff8_battle_store_wide_fourth);
		// Replace only the screen rejection; retain native projection, depth and winding checks.
		replace_call_function(renderer + clip.reduction, clip.store_fourth != 0 ? ff8_battle_clip_polygon<4> : ff8_battle_clip_polygon<3>);
		if (clip.patch_compare)
			patch_code_byte(renderer + clip.reduction + 6, 0xF8); // cmp al, mask instead of cl/dl
		memset_code(renderer + clip.secondary_test, 0x90, clip.secondary_size);
	}
}

template<bool bottom>
static uint32_t ff8_the_end_submit_wide_cinema(int *ordering_table, uint32_t *packet)
{
	const ff8_battle_packet_viewport viewport = ff8_battle_get_packet_viewport(1);
	// Anchor each bar to its screen edge without changing the native animated thickness.
	const int y_offset = bottom ? viewport.bottom - 216 : viewport.top;
	for (uint32_t i = 0; i < 4; ++i)
	{
		const int x = static_cast<int16_t>(packet[2 + i]);
		const int y = static_cast<int16_t>(packet[2 + i] >> 16) + y_offset;
		const int wide_x = x == 0 ? viewport.left : viewport.right;
		packet[2 + i] = static_cast<uint16_t>(wide_x) | (static_cast<uint32_t>(static_cast<uint16_t>(y)) << 16);
	}
	return ff8_battle_submit_unprojected_quad(ordering_table, packet);
}

static uint32_t ff8_battle_submit_wide_flash(int *ordering_table, uint32_t *packet)
{
	const ff8_battle_packet_viewport viewport = ff8_battle_get_packet_viewport(1);
	for (uint32_t i = 0; i < 4; ++i)
	{
		const int x = static_cast<int16_t>(packet[2 + i]);
		const int y = static_cast<int16_t>(packet[2 + i] >> 16);
		const int wide_x = x == 0 ? viewport.left : (x == 320 ? viewport.right : x);
		const int wide_y = y == 0 ? viewport.top : (y == 240 ? viewport.bottom : y);
		packet[2 + i] = static_cast<uint16_t>(wide_x) | (static_cast<uint32_t>(static_cast<uint16_t>(wide_y)) << 16);
	}
	return ff8_battle_submit_unprojected_quad(ordering_table, packet);
}

static void ff8_the_end_widescreen_init()
{
	const uint32_t effect = ff8_externals.func_off_battle_effects_C81774[FF8BattleEffect::TheEnd];
	ff8_the_end_effect = reinterpret_cast<void *>(get_absolute_value(effect, 0xB));
	ff8_battle_active_effect = reinterpret_cast<void **>(get_absolute_value(ff8_externals.battle_magic_effect_start_call, 0x7));
	ff8_battle_store_triangle = reinterpret_cast<decltype(ff8_battle_store_triangle)>(get_relative_call(effect, 0x1802));
	ff8_battle_store_fourth = reinterpret_cast<decltype(ff8_battle_store_fourth)>(get_relative_call(effect, 0x1AD5));
	ff8_battle_submit_unprojected_quad = reinterpret_cast<decltype(ff8_battle_submit_unprojected_quad)>(get_relative_call(effect, 0x9082));

	// Flat, textured and Gouraud primitives, including the subdivided scenery path.
	ff8_battle_patch_polygon_clips(effect, {
		{0x1802, 0, 0x1882, 0x188B, 8, false},
		{0x1A1D, 0x1AD5, 0x1B10, 0x1B1D, 11, false},
		{0x1CE1, 0, 0x1D68, 0x1D76, 12, true},
		{0x216B, 0x222A, 0x2265, 0x2272, 15, false},
		{0x26AE, 0, 0x272E, 0x273B, 12, false},
		{0x28FE, 0x29B6, 0x29F2, 0x29FF, 15, false},
		{0x2C19, 0, 0x2CA3, 0x2CB1, 12, true},
		{0x312A, 0x31EC, 0x322A, 0x3238, 15, true},
		{0x835C, 0x841E, 0x845C, 0x846A, 15, true}
	});

	replace_call(effect + 0x9082, ff8_the_end_submit_wide_cinema<false>);
	replace_call(effect + 0x90CD, ff8_the_end_submit_wide_cinema<true>);

	// This shared flash callback emits four quarter-screen quads, not one full-screen primitive.
	const uint32_t flash_callback = get_absolute_value(effect, 0xCE5);
	const uint32_t flash_draw = get_absolute_value(flash_callback, 0x19);
	for (uint32_t offset : {0xB7u, 0xFFu, 0x147u, 0x18Eu})
		replace_call(flash_draw + offset, ff8_battle_submit_wide_flash);
}

static void ff8_battle_effects_widescreen_init()
{
	const uint32_t gilgamesh = ff8_externals.func_off_battle_effects_C81774[FF8BattleEffect::GilgameshZantetsuken];
	const uint32_t renderer = get_relative_call(gilgamesh, 0xA79);
	// Shared GTE primitive paths used by Gilgamesh's sky, swords and slash.
	ff8_battle_patch_polygon_clips(renderer, {
		{0x116, 0, 0x166, 0x170, 8, true},
		{0x3D6, 0x43C, 0x45F, 0x46C, 11, false},
		{0x65B, 0, 0x6A9, 0x6B3, 7, true},
		{0x884, 0x906, 0x92A, 0x934, 9, true},
		{0xABD, 0, 0xB0B, 0xB19, 11, true},
		{0xCB6, 0xD1C, 0xD3F, 0xD4C, 15, false},
		{0xF7A, 0, 0xFC8, 0xFD6, 11, true},
		{0x11C4, 0x1246, 0x126A, 0x1278, 13, true}
	});
}

void ff8_widescreen_hook_init() {
	ff8_card_submit_background = reinterpret_cast<decltype(ff8_card_submit_background)>(
		get_relative_call(ff8_externals.sub_5391B0, 0xC7));
	replace_call(ff8_externals.sub_5391B0 + 0xC7, ff8_card_submit_first_background_page);
	replace_call(ff8_externals.sub_5391B0 + 0x177, ff8_card_submit_second_background_page);
	ff8_card_draw_sprite = reinterpret_cast<decltype(ff8_card_draw_sprite)>(ff8_externals.ssigpu_callbacks_1[100]);
	patch_code_dword(reinterpret_cast<uint32_t>(&ff8_externals.ssigpu_callbacks_1[100]),
		reinterpret_cast<uint32_t>(ff8_card_draw_background_sprite));
	ff8_card_append_sprite = reinterpret_cast<decltype(ff8_card_append_sprite)>(
		get_relative_call(ff8_externals.sub_462AD0, 0x9E));
	replace_call(ff8_externals.sub_462AD0 + 0x9E, ff8_card_append_background_sprite);
	replace_call(ff8_externals.sub_462AD0 + 0x25E, ff8_card_append_background_sprite);

	if (FF8_US_VERSION)
	{
		ff8_the_end_widescreen_init();
		ff8_battle_effects_widescreen_init();
		replace_call(ff8_externals.swirl_enter + 0x111, ff8_swirl_set_wide_viewport);
		replace_call(ff8_externals.sub_5099D0 + 0xCD, ff8_battle_world_calc_zoom);
		replace_call(ff8_externals.sub_500870 + 0x760 + 0x15E, ff8_battle_stage_draw);

		const uint32_t battle_stage_init = get_relative_call(ff8_externals.sub_500870, 0x13);
		const uint32_t battle_height = (wide_viewport_y + wide_viewport_height) / 2;
		patch_code_int(battle_stage_init + 0x21, battle_height);
		patch_code_int(battle_stage_init + 0x39, battle_height);
		patch_code_int(battle_stage_init + 0x51, battle_height);
		patch_code_short(battle_stage_init + 0x85, battle_height);
		patch_code_int(battle_stage_init + 0x90, battle_height);
		patch_code_short(battle_stage_init + 0xD3, battle_height);

		const uint32_t worldmap_draw_models = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x267);
		const uint32_t worldmap_draw_model = get_relative_call(worldmap_draw_models, 0x5E5);
		ff8_worldmap_model_store_depths = reinterpret_cast<int (*)(uint32_t *, uint32_t *, uint32_t *)>(get_relative_call(worldmap_draw_model, 0x10D));
		replace_call(worldmap_draw_model + 0x10D, ff8_worldmap_model_store_wide_depths);
		// Keep the projection loop intact, but do not overwrite the C++ visibility flags.
		memset_code(worldmap_draw_model + 0x131, 0x90, 4);
		memset_code(worldmap_draw_model + 0x14D, 0x90, 4);
		memset_code(worldmap_draw_model + 0x172, 0x90, 4);

		const uint32_t worldmap_draw_sky = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0xA78);
		const uint32_t worldmap_draw_sky_gradient = get_relative_call(worldmap_draw_sky, 0x1BA);
		ff8_worldmap_submit_sky_quad = reinterpret_cast<decltype(ff8_worldmap_submit_sky_quad)>(get_relative_call(worldmap_draw_sky_gradient, 0xDE));
		replace_call(worldmap_draw_sky_gradient + 0xDE, ff8_worldmap_submit_wide_sky_quad);
		replace_call(worldmap_draw_sky_gradient + 0x1A7, ff8_worldmap_submit_wide_sky_quad);

		const uint32_t shop_render = get_absolute_value(reinterpret_cast<uint32_t>(ff8_externals.menu_callbacks[11].func), 0x34);
		ff8_shop_draw_owned = reinterpret_cast<ff8_shop_draw_panel>(get_relative_call(shop_render, 0x241));
		ff8_shop_draw_items = reinterpret_cast<ff8_shop_draw_panel>(get_relative_call(shop_render, 0x26B));
		ff8_shop_draw_gil = reinterpret_cast<ff8_shop_draw_panel>(get_relative_call(shop_render, 0x279));
		replace_call(shop_render + 0x241, ff8_shop_draw_wide_owned);
		replace_call(shop_render + 0x26B, ff8_shop_draw_wide_items);
		replace_call(shop_render + 0x279, ff8_shop_draw_wide_gil);
	}

	// Viewport fixes
	replace_function(ff8_externals.engine_setviewport_sub_45B4C0, ff8_engine_set_wide_viewport);

	ff8_worldmap_projected_vertices = reinterpret_cast<uint32_t *>(get_absolute_value(ff8_externals.sub_4023D0, 0xAA) - 4);
	replace_call(ff8_externals.sub_53BB90 + (FF8_US_VERSION ? 0x402 : 0x410), ff8_worldmap_project_vertices);
	replace_call(ff8_externals.sub_53BB90 + (FF8_US_VERSION ? 0xAB1 : 0xAE5), ff8_worldmap_project_vertices);

	const int worldmap_clip_top = 4 * (wide_viewport_y - 16);
	const int worldmap_clip_height = 4 * wide_viewport_height;
	if (FF8_US_VERSION)
	{
		// These fallback offsets match the US release only
		memset_code(ff8_externals.worldmap_fog_filter_polygons_fallback + 0x1D9, 0x90, 2);
		// The fallback's unsigned Y tests also reject negative packet coordinates in the top strip.
		patch_code_int(ff8_externals.worldmap_fog_filter_polygons_fallback + 0x1E0, worldmap_clip_top + worldmap_clip_height);
		patch_code_byte(ff8_externals.worldmap_fog_filter_polygons_fallback + 0x1E8, 0x7E); // jle
		patch_code_byte(ff8_externals.worldmap_fog_filter_polygons_fallback + 0x1EE, 0x7E); // jle
		patch_code_byte(ff8_externals.worldmap_fog_filter_polygons_fallback + 0x1F5, 0x8F); // jg
		memset_code(ff8_externals.worldmap_cull_blocks_sub_53D3C0 + 0x3A2, 0x90, 2);
		memset_code(ff8_externals.worldmap_cull_blocks_sub_53D3C0 + 0x3A8, 0x90, 2);
	}

	// Menu
	ff8_externals.menu_viewport[2].scale_x = 2.0;
	ff8_externals.menu_viewport[2].offset_x = -64.0;
}

static void ff8_expand_screen_overlay(struct nvertex *vertices, uint32_t vertexcount, float minimum_height = 0.0f)
{
	float min_x = vertices[0]._.x;
	float max_x = vertices[0]._.x;
	float min_y = vertices[0]._.y;
	float max_y = vertices[0]._.y;

	for(uint32_t i = 1; i < vertexcount; ++i)
	{
		if(vertices[i]._.x < min_x) min_x = vertices[i]._.x;
		if(vertices[i]._.x > max_x) max_x = vertices[i]._.x;
		if(vertices[i]._.y < min_y) min_y = vertices[i]._.y;
		if(vertices[i]._.y > max_y) max_y = vertices[i]._.y;
	}

	if(vertexcount == 4)
	{
		// Projected scenery also uses TL quads; only expand screen-aligned, constant-depth rectangles.
		for(uint32_t i = 0; i < vertexcount; ++i)
		{
			if((vertices[i]._.x != min_x && vertices[i]._.x != max_x)
				|| (vertices[i]._.y != min_y && vertices[i]._.y != max_y)
				|| vertices[i]._.z != vertices[0]._.z
				|| vertices[i].color.w != vertices[0].color.w)
				return;
		}
	}

	if(max_x - min_x < game_width || floor(max_y - min_y) < floor(minimum_height)) return;

	float wide_min_x = min_x < wide_viewport_x ? min_x : wide_viewport_x;
	float wide_max_x = max_x > wide_viewport_x + wide_viewport_width ? max_x : wide_viewport_x + wide_viewport_width;

	for(uint32_t i = 0; i < vertexcount; ++i)
	{
		if(vertices[i]._.x == min_x) vertices[i]._.x = wide_min_x;
		if(vertices[i]._.x == max_x) vertices[i]._.x = wide_max_x;

		if(vertexcount == 4)
		{
			if(vertices[i]._.y == min_y) vertices[i]._.y = wide_viewport_y;
			if(vertices[i]._.y == max_y) vertices[i]._.y = wide_viewport_y + wide_viewport_height;
		}
	}
}

void ff8_widescreen_adjust_vertices(uint32_t mode, uint32_t vertextype, struct nvertex *vertices, uint32_t vertexcount, uint16_t *indices, uint32_t count)
{
	if(!widescreen_enabled) return;

	if (mode == MODE_CARDGAME && vertextype == TLVERTEX && !current_state.texture_handle
		&& vertexcount == 4 && count == 6)
	{
		for (const auto &background : ff8_card_backgrounds)
		{
			if (background.drawn && background.frame == frame_counter)
			{
				ff8_expand_screen_overlay(vertices, vertexcount, game_height - 48.0f);
				break;
			}
		}
	}

	if(mode == MODE_SWIRL && vertextype == TLVERTEX)
	{
		const float scale_x = wide_viewport_width / static_cast<float>(game_width);
		const float scale_y = wide_viewport_height / static_cast<float>(game_height);

		for(uint32_t i = 0; i < vertexcount; ++i)
		{
			vertices[i]._.x = wide_viewport_x + vertices[i]._.x * scale_x;
			vertices[i]._.y = wide_viewport_y + vertices[i]._.y * scale_y;
		}
	}

	if(vertextype == TLVERTEX && mode == MODE_WORLDMAP
		&& ((vertexcount == 8 && count == 12) || (vertexcount == 4 && count == 6)))
	{
		ff8_expand_screen_overlay(vertices, vertexcount);
	}

	if(vertextype == TLVERTEX && mode == MODE_BATTLE
		&& vertexcount % 4 == 0 && count / 6 == vertexcount / 4 && count % 6 == 0)
	{
		// SSIGPU batches screen overlays with other quads, so classify each primitive, not the draw.
		bool is_quad_batch = true;
		for(uint32_t base = 0, index = 0; base < vertexcount; base += 4, index += 6)
		{
			for(uint32_t i = 0; i < 6; ++i)
			{
				if(indices[index + i] < base || indices[index + i] >= base + 4)
				{
					is_quad_batch = false;
					break;
				}
			}
			if(!is_quad_batch) break;
		}
		if(is_quad_batch)
		{
			// A full-width strip is not a full-screen flash (The End's cinema bars share these batches).
			const float screen_height = *ff8_externals.ssigpu_viewport_height_dword_B7CBFC * (216.0f / 240.0f);
			for(uint32_t base = 0; base < vertexcount; base += 4)
			{
				// Native packets without projection depths use RHW=1; do not widen batched 3D scenery.
				if(vertexcount == 4 || vertices[base].color.w == 1.0f)
					ff8_expand_screen_overlay(vertices + base, 4, screen_height);
			}
		}
	}

}

void ff8_widescreen_framebuffer_capture(uint32_t &x, uint32_t &w)
{
	if (widescreen_enabled)
	{
		if (getmode_cached()->driver_mode == MODE_BATTLE)
		{
			// Battle feedback already uses the wide viewport; its origin is relative to the 4:3 canvas.
			x -= wide_viewport_x;
		}
		else
		{
			// Match the capture rectangle to the widescreen-scaled swirl geometry.
			x = std::lround(double(x) * wide_viewport_width / game_width);
			w = std::lround(double(w) * wide_viewport_width / game_width);
		}
	}
}
