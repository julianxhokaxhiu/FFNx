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

#include "../../globals.h"
#include "../../log.h"
#include "../../achievement.h"
#include "../../patch.h"

#include "defs.h"

namespace ff7::battle
{
	namespace
	{
		ff7_graphics_object *(*get_effect_graphics)(int, void *);
		void (*set_primitive_graphics)(int, int, ff7_graphics_object *);
		void (*set_primitive_blend_mode)(int);
		void (*set_primitive_overlay)(int);

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
