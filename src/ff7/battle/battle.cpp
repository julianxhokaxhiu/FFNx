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

#include <array>
#include <cmath>

#include "../../globals.h"
#include "../../cfg.h"
#include "../../common.h"
#include "../../gl.h"
#include "../../log.h"
#include "../../achievement.h"
#include "../../patch.h"
#include "../../renderer.h"

#include "defs.h"

namespace ff7::battle
{
	namespace
	{
		struct BattleShadowResource
		{
			matrix_set *matrices;
			ff7_polygon_set *polygons;
		};

		void draw_battle_shadow(const BattleShadowResource *shadow, ff7_game_obj *game_object)
		{
			if (enable_lighting || !game_object->in_scene) return;

			if (!shadow || !shadow->matrices || !shadow->matrices->matrix_world ||
				!shadow->polygons || !shadow->polygons->struc_173)
			{
				ffnx_unexpected_once("Missing FF7 battle shadow draw data\n");
				return;
			}

			const auto strength = shadow->polygons->struc_173->color.a;
			if (!strength) return;

			constexpr size_t sides = 16;
			static const auto rim = [] {
				std::array<vector3<float>, sides> points{};
				for (size_t i = 0; i < sides; ++i)
				{
					const float angle = 6.283185307f * i / sides;
					points[i] = {900.0f * std::cos(angle), 0.0f, 900.0f * std::sin(angle)};
				}
				return points;
			}();

			std::array<nvertex, sides + 1> vertices{};
			std::array<WORD, sides * 3> indices{};
			const auto shade = [strength](uint32_t intensity) {
				return 0xFF000000u | ((intensity * strength / 255u) * 0x010101u);
			};
			vertices[0].color.color = shade(200);
			for (size_t i = 0; i < sides; ++i)
			{
				vertices[i + 1]._ = rim[i];
				vertices[i + 1].color.color = shade(6);
				indices[i * 3 + 1] = static_cast<WORD>(i + 1);
				indices[i * 3 + 2] = static_cast<WORD>((i + 1) % sides + 1);
			}

			// The PSX shadow ordering table sets semi-transparency mode 2 (subtract).
			// Keep the native ground transform and fade, but don't submit the PC mesh.
			driver_state saved;
			gl_save_state(&saved);
			gl_bind_texture_set(nullptr);
			current_state.fb_texture = false;
			gl_set_blend_func(RendererBlendMode::BLEND_SUB);
			internal_set_renderstate(V_DEPTHTEST, true, nullptr);
			internal_set_renderstate(V_DEPTHMASK, false, nullptr);
			internal_set_renderstate(V_NOCULL, true, nullptr);
			internal_set_renderstate(V_ALPHATEST, false, nullptr);
			internal_set_renderstate(V_SHADEMODE, true, nullptr);
			gl_set_worldview_matrix(shadow->matrices->matrix_world);
			if (auto *projection = shadow->polygons->matrix_set ?
				shadow->polygons->matrix_set->matrix_projection : nullptr)
				gl_set_d3dprojection_matrix(projection);
			gl_draw_indexed_primitive(RendererPrimitiveType::PT_TRIANGLES, LVERTEX,
				vertices.data(), nullptr, static_cast<uint32_t>(vertices.size()), indices.data(),
				static_cast<uint32_t>(indices.size()), nullptr, nullptr, nullptr, shadow->polygons->field_4, false);
			gl_load_state(&saved);
		}

		ff7_graphics_object *(*get_effect_graphics)(int, void *);
		void (*set_primitive_graphics)(int, int, ff7_graphics_object *);
		void (*set_primitive_blend_mode)(int);
		void (*set_primitive_overlay)(int);
		void (*draw_effect_resources)(int, int, int *);
		void (*draw_effect_primitives)(int);

		void draw_opaque_effect_resources(int advance, int pass, int *resources)
		{
			// The PC port flushes resource textures before the shared geometry they overlay.
			// Drain shared opaque primitives first; blended and overlay passes keep native order.
			draw_effect_primitives(pass);
			draw_effect_resources(advance, pass, resources);
		}

		byte pandora_box_visibility_flags[10];
		effect100_data *(*pandora_box_native_main_loop)();

		effect100_data *pandora_box_main_loop()
		{
			const auto &effect = ff7_externals.effect100_array_data[*ff7_externals.effect100_array_idx];
			uint16_t phase;
			memcpy(&phase, effect.field_1C, sizeof(phase));
			bool hide_models = !*ff7_externals.g_is_battle_paused && phase == 1 && effect.field_2 == 23;
			bool reveal_models = !*ff7_externals.g_is_battle_paused && phase == 4 && effect.field_2 == 225;

			// Preserve hidden/multipart bits at the native hide/reveal frames,
			// without reverting unrelated animation flag updates.
			if (hide_models)
				for (size_t i = 0; i < ff7_externals.g_battle_model_state.size(); ++i)
					pandora_box_visibility_flags[i] = ff7_externals.g_battle_model_state[i].field_25 & 0x06;

			auto *result = pandora_box_native_main_loop();

			if (reveal_models)
				for (size_t i = 0; i < ff7_externals.g_battle_model_state.size(); ++i)
					ff7_externals.g_battle_model_state[i].field_25 = (ff7_externals.g_battle_model_state[i].field_25 & 0xF9) | pandora_box_visibility_flags[i];

			return result;
		}

		ff7_graphics_object *bind_hyperjump_texture(int index, void *resources)
		{
			auto *graphics = get_effect_graphics(index, resources);
			if (!graphics)
				ffnx_error("Hyper Jump: could not load the debris texture\n");

			set_primitive_blend_mode(1);
			set_primitive_overlay(0);
			set_primitive_graphics(9, 1, graphics);
			return graphics;
		}
	}

	void battle_hook_init()
	{
		// The final call in the actor's shadow block submits only the KAGE resource.
		replace_call_function(ff7_externals.battle_sub_42F3E8 + 0xDC3, draw_battle_shadow);

		const auto draw_effects = reinterpret_cast<uint32_t>(ff7_externals.battle_draw_call_42908C);
		draw_effect_resources = reinterpret_cast<decltype(draw_effect_resources)>(get_relative_call(draw_effects, 0x4F));
		draw_effect_primitives = reinterpret_cast<decltype(draw_effect_primitives)>(get_relative_call(draw_effects, 0x5B));
		replace_call_function(draw_effects + 0x4F, draw_opaque_effect_resources);
		memset_code(draw_effects + 0x5B, 0x90, 5);

		pandora_box_native_main_loop = reinterpret_cast<decltype(pandora_box_native_main_loop)>(ff7_externals.pandora_box_skill_main_loop_566E61);
		patch_code_dword(ff7_externals.pandora_box_skill_init_566871 + 0x1F, reinterpret_cast<uint32_t>(pandora_box_main_loop));

		uint32_t hyperjump = ff7_externals.limit_break_effects_fn_table[30];
		uint32_t load_resources = get_relative_call(hyperjump, 0x4);
		uint32_t init = get_relative_call(hyperjump, 0x9D);
		uint32_t main_loop = get_absolute_value(init, 0x20);
		uint32_t dome = get_absolute_value(main_loop, 0x2F7);
		uint32_t debris = get_absolute_value(dome, 0x245);

		// Replace the unused DOMU5 model with its debris texture: additive, flat-textured triangles.
		uint32_t resource = get_absolute_value(load_resources, 0x28) + 7 * 20;
		patch_code_dword(resource, 0x00192839);
		patch_code_dword(resource + 4, reinterpret_cast<uint32_t>("DOMU1.TIM"));

		get_effect_graphics = reinterpret_cast<decltype(get_effect_graphics)>(get_relative_call(debris, 0x21));
		set_primitive_graphics = reinterpret_cast<decltype(set_primitive_graphics)>(get_relative_call(ff7_externals.roulette_skill_main_loop_566287, 0x1F4));
		set_primitive_blend_mode = reinterpret_cast<decltype(set_primitive_blend_mode)>(get_relative_call(ff7_externals.roulette_skill_main_loop_566287, 0x3EF));
		set_primitive_overlay = reinterpret_cast<decltype(set_primitive_overlay)>(get_relative_call(ff7_externals.battle_draw_quad_5BD473, 0x88));
		replace_call_function(debris + 0x21, bind_hyperjump_texture);

		uint32_t shadow_flare = ff7_externals.enemy_skill_effects_fn_table[22];
		uint32_t shadow_flare_load_resources = get_relative_call(shadow_flare, 0x3);
		uint32_t shadow_flare_resources = get_absolute_value(shadow_flare_load_resources, 0x28);

		// PSX Shadow Flare textures use subtractive TPAGE blending, not the PC port's average blending.
		for (uint32_t i = 0; i < 4; ++i)
		{
			uint32_t texture_resource = shadow_flare_resources + i * 20;
			uint32_t flags = *reinterpret_cast<const uint32_t *>(texture_resource);
			patch_code_dword(texture_resource, (flags & ~7u) | 2u);
		}

		uint32_t ifrit_load_resources = get_relative_call(ff7_externals.run_ifrit_main_5927C1, 0x122);
		uint32_t ifrit_resources = get_absolute_value(ifrit_load_resources, 0x2C);

		// Only the CIRCLE ground-hole model is subtractive; Ifrit's flames remain additive.
		uint32_t ifrit_ground_flags = *reinterpret_cast<const uint32_t *>(ifrit_resources);
		patch_code_dword(ifrit_resources, (ifrit_ground_flags & ~7u) | 2u);
	}

	void magic_thread_start(void (*func)())
	{
		ff7_externals.destroy_magic_effects();

		/*
		* Original function creates a separate thread but the code is not thread
		* safe in any way! Luckily modern PCs are fast enough to load magic
		* effects synchronously.
		*/
		func();
	}

	void load_battle_stage(int param_1, int battle_location_id, int **param_3){
		((void(*)(int, int, int **)) ff7_externals.load_battle_stage)(param_1, battle_location_id, param_3);

		g_FF7SteamAchievements->initCharStatsBeforeBattle(ff7_externals.savemap->chars);
		g_FF7SteamAchievements->unlockBattleSquareAchievement(battle_location_id);
	}

	void battle_sub_5C7F94(int param_1, int param_2){
		((void(*)(int, int)) ff7_externals.battle_sub_5C7F94)(param_1, param_2);

		if (trace_all || trace_achievement)
			ffnx_trace("%s - trying to unlock achievement for gil\n", __func__);
		g_FF7SteamAchievements->unlockGilAchievement(ff7_externals.savemap->gil);
	}

	void display_battle_action_text_sub_6D71FA(short command_id, short action_id){
		ff7_externals.battle_actor_data->formation_entry = 1;
		ff7_externals.battle_actor_data->command_index = command_id;
		ff7_externals.battle_actor_data->action_index = action_id;

		g_FF7SteamAchievements->unlockAchievementByBattleCommandAndAction(command_id, action_id);
	}

	int load_scene_bin_chunk(char *filename, int offset, int size, char **out_buffer, void (*callback)(void))
	{
		int ret = ff7_externals.engine_load_bin_file_sub_419210(filename, offset, size, out_buffer, callback);

		char chunk_file[1024]{0};
		uint32_t chunk_size = 0;
		FILE* fd;

		_snprintf(chunk_file, sizeof(chunk_file), "%s/%s/battle/scene.bin.chunk.%i", basedir, direct_mode_path.c_str(), ff7_externals.modules_global_object->battle_id / 4);

		if ((fd = fopen(chunk_file, "rb")) != NULL)
		{
			fseek(fd, 0L, SEEK_END);
			chunk_size = ftell(fd);
			fseek(fd, 0L, SEEK_SET);
			fread(*out_buffer, sizeof(byte), chunk_size, fd);

			ffnx_trace("%s: scene overridden using %s\n", __func__, chunk_file);
			fclose(fd);
		}
		else if (trace_direct)
			ffnx_trace("%s: could not find %s\n", __func__, chunk_file);

		return ret;
	}
}
