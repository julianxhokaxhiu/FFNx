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
#include "../../patch.h"
#include "../../achievement.h"

#include "defs.h"

namespace ff7::battle
{
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

	static uint32_t destroy_graphics_object_replace_id = 0;

	/*
	* Battle effects bind graphics objects into the texture pages of the PSX
	* GPU emulation and never unbind them. When such an object is destroyed the
	* pages keep pointing at freed memory, and the next effect that draws a
	* textured primitive without binding its own texture reads it. Ruby
	* Weapon's tentacle emerge followed by Cid's Hyper Jump crashes this way
	* (#673). Unbind the object first; an unbound texture slot draws nothing.
	*/
	static void destroy_graphics_object(ff7_graphics_object *object)
	{
		battle_gpu_render_context *context = *ff7_externals.battle_gpu_render_context;

		if (object != nullptr && context != nullptr)
		{
			for (battle_gpu_texture_page *page : context->pages)
			{
				if (page == nullptr) continue;

				for (ff7_graphics_object *&texture : page->textures)
				{
					if (texture == object) texture = nullptr;
				}
			}
		}

		unreplace_function(destroy_graphics_object_replace_id);
		((void(*)(ff7_graphics_object *))ff7_externals.destroy_graphics_object_670FD3)(object);
		rereplace_function(destroy_graphics_object_replace_id);
	}

	void texture_page_hook_init()
	{
		destroy_graphics_object_replace_id = replace_function(ff7_externals.destroy_graphics_object_670FD3, destroy_graphics_object);
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
