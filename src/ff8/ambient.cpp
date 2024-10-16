/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include "ambient.h"
#include "audio.h"
#include "common.h"
#include "globals.h"

void ff8_handle_ambient_playback()
{
	struct game_mode *mode = getmode_cached();
	static char filename[64]{0};
	static WORD last_field_id = 0, last_triangle_id = 0, last_battle_id = 0;
	bool playing = false;

	switch (mode->driver_mode)
	{
	case MODE_BATTLE:
		if (last_battle_id != next_battle_scene_id)
		{
			last_battle_id = next_battle_scene_id;

			sprintf(filename, "bat_%d", last_battle_id);
			nxAudioEngine.playAmbient(filename);
		}
		if ((*ff8_externals.is_game_paused != 0) && nxAudioEngine.isAmbientPlaying())
			nxAudioEngine.pauseAmbient();
		else if (!(*ff8_externals.is_game_paused != 0) && !(nxAudioEngine.isAmbientPlaying()))
			nxAudioEngine.resumeAmbient();
		break;
	case MODE_FIELD:
		if (last_field_id != *common_externals.current_field_id)
		{
			last_field_id = *common_externals.current_field_id;

			if (common_externals.current_triangle_id != 0)
			{
				last_triangle_id = *common_externals.current_triangle_id;
				sprintf(filename, "field_%d_%d", last_field_id, *common_externals.current_triangle_id);
				playing = nxAudioEngine.playAmbient(filename);
			}

			if (!playing)
			{
				sprintf(filename, "field_%d", last_field_id);
				playing = nxAudioEngine.playAmbient(filename);
			}
		}
		else if (common_externals.current_triangle_id != 0 && last_field_id == *common_externals.current_field_id && last_triangle_id != *common_externals.current_triangle_id)
		{
			last_triangle_id = *common_externals.current_triangle_id;

			sprintf(filename, "field_%d_%d", last_field_id, *common_externals.current_triangle_id);
			playing = nxAudioEngine.playAmbient(filename);
		}
		break;
	default:
		if (last_field_id != 0 || last_battle_id != 0)
		{
			nxAudioEngine.stopAmbient();
			last_field_id = 0;
			last_battle_id = 0;
			next_battle_scene_id = 0;
		}
		break;
	}
}
