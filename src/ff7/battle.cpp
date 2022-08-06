/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#include "../ff7.h"
#include "../log.h"
#include "../achievement.h"
#include "widescreen.h"

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

void ff7_load_battle_stage(int param_1, int battle_location_id, int **param_3){
	((void(*)(int, int, int **)) ff7_externals.load_battle_stage)(param_1, battle_location_id, param_3);

	g_FF7SteamAchievements->initCharStatsBeforeBattle(ff7_externals.savemap->chars);
	g_FF7SteamAchievements->unlockBattleSquareAchievement(battle_location_id);
}

void ff7_battle_sub_5C7F94(int param_1, int param_2){
	((void(*)(int, int)) ff7_externals.battle_sub_5C7F94)(param_1, param_2);

	if (trace_all || trace_achievement)
		ffnx_trace("%s - trying to unlock achievement for gil\n", __func__);
	g_FF7SteamAchievements->unlockGilAchievement(ff7_externals.savemap->gil);
}

void ff7_display_battle_action_text_sub_6D71FA(short command_id, short action_id){
	ff7_externals.battle_actor_data->formation_entry = 1;
	ff7_externals.battle_actor_data->command_index = command_id;
	ff7_externals.battle_actor_data->action_index = action_id;

	g_FF7SteamAchievements->unlockFirstLimitBreakAchievement(command_id, action_id);
}

void ifrit_first_wave_effect_widescreen_fix_sub_66A47E(int wave_data_pointer) {
    if(aspect_ratio == AR_WIDESCREEN) {
        int viewport_width_1_fix = ceil(255.f / game_width * wide_viewport_width) - 255;
        *(short*)(wave_data_pointer + 8) += wide_viewport_x;
        *(short*)(wave_data_pointer + 16) += wide_viewport_x + viewport_width_1_fix * 2;
        *(short*)(wave_data_pointer + 24) += wide_viewport_x;
        *(short*)(wave_data_pointer + 32) += wide_viewport_x + viewport_width_1_fix * 2;
    }

    ff7_externals.engine_draw_sub_66A47E(wave_data_pointer);
}

void ifrit_second_third_wave_effect_widescreen_fix_sub_66A47E(int wave_data_pointer) {
    if(aspect_ratio == AR_WIDESCREEN) {
        int viewport_width_1_fix = ceil(255.f / game_width * wide_viewport_width) - 255;
        int viewport_width_2_fix = ceil(65.f / game_width * wide_viewport_width) - 65;
        *(short*)(wave_data_pointer + 8) += wide_viewport_x + viewport_width_1_fix * 2;
        *(short*)(wave_data_pointer + 16) += wide_viewport_x + (viewport_width_1_fix + viewport_width_2_fix) * 2;
        *(short*)(wave_data_pointer + 24) += wide_viewport_x + viewport_width_1_fix * 2;
        *(short*)(wave_data_pointer + 32) += wide_viewport_x + (viewport_width_1_fix + viewport_width_2_fix) * 2;
    }

    ff7_externals.engine_draw_sub_66A47E(wave_data_pointer);
}