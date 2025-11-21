/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#include "audio.h"
#include "movies.h"
#include "patch.h"
#include "ff7/widescreen.h"
#include "video/movies.h"
#include "redirect.h"
#include "achievement.h"

enum MovieAudioLayers {
	MUSIC = 0,
	VOICE
};

char movie_music_path[512];
char movie_voice_path[512];

uint8_t ff8_movie_cam_buffer[2*1024*1024]; // 2MB

uint32_t ff7_prepare_movie(char *name, uint32_t loop, struct dddevice **dddevice, uint32_t dd2interface)
{
	char drivename[4];
	char dirname[256];
	char filename[128];
	char fmvName[512];
	char newFmvName[512];

	if(trace_all || trace_movies) ffnx_trace("prepare_movie %s\n", name);

	ff7_externals.movie_object->loop = loop;
	ff7_externals.movie_sub_415231(name);

	ff7_externals.movie_object->field_1F8 = 1;
	ff7_externals.movie_object->is_playing = 0;
	ff7_externals.movie_object->movie_end = 0;
	ff7_externals.movie_object->global_movie_flag = 0;
	ff7_externals.movie_object->field_E0 = !((struct ff7_game_obj *)common_externals.get_game_object())->field_968;

	_splitpath(name, drivename, dirname, filename, NULL);
	_snprintf(fmvName, sizeof(fmvName), "%s%s%s.%s", drivename, dirname, filename, ffmpeg_video_ext.c_str());

	redirect_path_with_override(fmvName, newFmvName, sizeof(newFmvName));

	_splitpath(newFmvName, drivename, dirname, filename, NULL);
	// Remove extension
	_snprintf(movie_music_path, sizeof(movie_music_path), "%s%s%s", drivename, dirname, filename);
	_snprintf(movie_voice_path, sizeof(movie_voice_path), "%s%s%s_va", drivename, dirname, filename);

	bool has_ext_audio_file = nxAudioEngine.canPlayMovieAudio(movie_music_path);
	ffmpeg_prepare_movie(newFmvName, !has_ext_audio_file);
	if (!has_ext_audio_file) nxAudioEngine.setStreamMasterVolume(ffmpeg_video_volume / 100.0f);

	ff7_externals.movie_object->global_movie_flag = 1;

	// Required by > 15 FPS movies
	movie_fps_ratio = ffmpeg_get_fps_ratio();
	if (movie_fps_ratio > 1)
	{
		if (strcmp(filename, "opening") == 0)
		{
			*ff7_externals.opening_movie_music_start_frame = *ff7_externals.opening_movie_music_start_frame * movie_fps_ratio;
		}
	}
	// ---------------------------

	if(widescreen_enabled)
		widescreen.initMovieParamsFromConfig(filename);

	if(steam_edition || enable_steam_achievements)
		g_FF7SteamAchievements->initMovieStats(std::string(filename));

	return true;
}

uint32_t ff7_update_movie_sample(LPDIRECTDRAWSURFACE surface)
{
	uint32_t movie_end;

	ff7_externals.movie_object->movie_end = 0;

	if(!ff7_externals.movie_object->is_playing) return false;

retry:
	// Use native movie fps limiter if it's not been playback in fields ( no 3d models drawn on top )
	movie_end = !ffmpeg_update_movie_sample(!(*ff7_externals.field_limit_fps));

	if(movie_end)
	{
		if(trace_all || trace_movies) ffnx_trace("movie end\n");
		if(ff7_externals.movie_object->loop)
		{
			ffmpeg_loop();
			goto retry;
		}

		ff7_externals.movie_object->movie_end = 1;
		is_movie_bgfield = false;

		if(steam_edition || enable_steam_achievements)
			if(g_FF7SteamAchievements->isEndingMovie())
				g_FF7SteamAchievements->unlockGameProgressAchievement();

		return true;
	}

	return true;
}

uint32_t ff7_start_movie()
{
	if(trace_all || trace_movies) ffnx_trace("start_movie\n");

	if(ff7_externals.movie_object->is_playing) return true;

	ff7_externals.movie_object->is_playing = 1;

	if (!is_movie_bgfield) nxAudioEngine.pauseAmbient();
	nxAudioEngine.setMovieMasterVolume(ffmpeg_video_volume / 100.0f);
	nxAudioEngine.playMovieAudio(movie_music_path, MovieAudioLayers::MUSIC);
	nxAudioEngine.playMovieAudio(movie_voice_path, MovieAudioLayers::VOICE, 3.0f);

	return ff7_update_movie_sample(0);
}

uint32_t ff7_stop_movie()
{
	if(trace_all || trace_movies) ffnx_trace("stop_movie\n");

	if(ff7_externals.movie_object->is_playing)
	{
		ff7_externals.movie_object->is_playing = 0;
		ff7_externals.movie_object->movie_end = 0;

		ffmpeg_stop_movie();

		nxAudioEngine.stopMovieAudio(MovieAudioLayers::MUSIC);
		nxAudioEngine.stopMovieAudio(MovieAudioLayers::VOICE);
		if (!is_movie_bgfield) nxAudioEngine.resumeAmbient();
	}

	return true;
}

void ff7_release_movie_objects()
{
	if(trace_all || trace_movies) ffnx_trace("release_movie_objects\n");

	ff7_stop_movie();

	ffmpeg_release_movie_objects();

	ff7_externals.movie_object->global_movie_flag = 0;
}

void draw_current_frame()
{
	if (trace_all || trace_movies) ffnx_trace("draw_current_frame\n");

	ffmpeg_draw_current_frame();

	// FF8 on Steam sometimes forgets to release the movie objects, so we do ensure it's done anyway
	ffmpeg_release_movie_objects();
}

uint32_t ff7_get_movie_frame()
{
	if(!ff7_externals.movie_object->is_playing) return 0;

	return ffmpeg_get_movie_frame();
}

uint32_t ff8_movie_frames;

void ff8_prepare_movie(uint8_t disc, uint32_t movie)
{
	char fmvName[MAX_PATH], camName[MAX_PATH], newFmvName[MAX_PATH], newCamName[MAX_PATH];
	uint32_t camOffset = 0;

	_snprintf(fmvName, sizeof(fmvName), "data/movies/disc%02i_%02ih.%s", disc, movie, ffmpeg_video_ext.c_str());

	if (redirect_path_with_override(fmvName, newFmvName, sizeof(newFmvName)) != 0) {
		_snprintf(newFmvName, sizeof(newFmvName), "%s/%s", ff8_externals.app_path, fmvName);
	}

	if(disc != 4)
	{
		_snprintf(camName, sizeof(camName), "data/movies/disc%02i_%02i.cam", disc, movie);

		if (redirect_path_with_override(camName, newCamName, sizeof(newCamName)) != 0) {
			_snprintf(newCamName, sizeof(newCamName), "%s/%s", ff8_externals.app_path, camName);
		}

		FILE *camFile = fopen(newCamName, "rb");

		if(!camFile)
		{
			if (steam_edition)
			{
				ffnx_error("could not load camera data from %s\n", newCamName);
				return;
			}

			// Try to open the cam files using the FF8 2000 PAK files
			const pak_pointers_entry &pak_pointer = ff8_externals.disc_pak_offsets[disc][movie];

			char filename[MAX_PATH] = {};
			strcpy(filename, ff8_externals.data_drive_path);
			strcat(filename, ff8_externals.disc_pak_filenames[disc]);

			FILE *camFile = fopen(filename, "rb");

			if (!camFile)
			{
				ffnx_error("could not load camera data from %s\n", filename);
				return;
			}

			fseek(camFile, pak_pointer.cam_offset, SEEK_SET);
			fread(&ff8_movie_cam_buffer, 1, pak_pointer.bik_offset - pak_pointer.cam_offset, camFile);
			fclose(camFile);
		}
		else
		{
			fseek(camFile, 0, SEEK_END);
			long camFileSize = ftell(camFile);
			rewind(camFile);
			fread(&ff8_movie_cam_buffer, 1, camFileSize, camFile);
			fclose(camFile);
		}

		ff8_externals.movie_object->movie_intro_pak = false;
	}
	else ff8_externals.movie_object->movie_intro_pak = true;

	ff8_externals.movie_object->camdata_start = (struct ff8_camdata *)(&ff8_movie_cam_buffer[8]);
	ff8_externals.movie_object->camdata_pointer = ff8_externals.movie_object->camdata_start;

	ff8_externals.movie_object->movie_current_frame = 0;

	if(trace_all || trace_movies) ffnx_trace("prepare_movie %s disc=%d movie=%d\n", newFmvName, disc, movie);

	if (steam_edition || enable_steam_achievements) {
		if (disc == 3 && movie == 4) // game ending movie
		{
			int squall_lvl = ff8_externals.get_char_level_4961D0(ff8_externals.savemap->chars[0].exp, 0);
			g_FF8SteamAchievements->unlockEndOfGameAchievement(squall_lvl);
		}
	}

	ff8_movie_frames = ffmpeg_prepare_movie(newFmvName);
}

void ff8_release_movie_objects()
{
	if(trace_all || trace_movies) ffnx_trace("release_movie_objects\n");

	ffmpeg_release_movie_objects();
}

int ff8_update_movie_sample()
{
	if(trace_all || trace_movies) ffnx_trace("update_movie_sample\n");

	ff8_externals.movie_object->movie_current_frame = ffmpeg_get_movie_frame();

	if(ff8_externals.movie_object->camdata_pointer->flag & 0x8) *ff8_externals.byte_1CE4907 = 0;
	if(ff8_externals.movie_object->camdata_pointer->flag & 0x10) *ff8_externals.byte_1CE4907 = 1;
	if(ff8_externals.movie_object->camdata_pointer->flag & 0x20)
	{
		*ff8_externals.byte_1CE4901 = 1;
		ff8_externals.movie_object->field_4C4B0 = 1;
	}
	else
	{
		*ff8_externals.byte_1CE4901 = 0;
		ff8_externals.movie_object->field_4C4B0 = 0;
	}

	if(ff8_externals.movie_object->camdata_pointer->flag & 0x1)
	{
		*ff8_externals.byte_1CE4901 = 1;
		ff8_externals.movie_object->field_4C4B0 = 1;
	}

	*ff8_externals.byte_1CE490D = ff8_externals.movie_object->camdata_pointer->flag & 0x40;

	int ret = 0;

	if (ff8_externals.movie_object->movie_current_frame <= ff8_externals.movie_object->movie_total_frames)
	{
		ffmpeg_update_movie_sample();
		ret = (int)ff8_externals.movie_object->camdata_start;
		ff8_externals.movie_object->camdata_pointer = &ff8_externals.movie_object->camdata_start[ff8_externals.movie_object->movie_current_frame];
	}
	else if(ff8_externals.movie_object->movie_intro_pak)
		ret = ff8_stop_movie();
	else
		ret = ff8_externals.sub_5304B0();

	return ret;
}

int ff8_start_movie()
{
	if(trace_all || trace_movies) ffnx_trace("start_movie\n");

	if(ff8_externals.movie_object->movie_intro_pak) ff8_externals.movie_object->movie_total_frames = ff8_movie_frames;
	else
	{
		ff8_externals.movie_object->movie_total_frames = ((WORD *)ff8_movie_cam_buffer)[3];
		if(trace_all || trace_movies) ffnx_trace("%i frames\n", ff8_externals.movie_object->movie_total_frames);
	}

	ff8_externals.movie_object->field_4C4B0 = 0;

	*ff8_externals.enable_framelimiter = false;

	ff8_externals.movie_object->movie_is_playing = true;

	return ff8_update_movie_sample();
}

int ff8_stop_movie()
{
	if(trace_all || trace_movies) ffnx_trace("stop_movie\n");

	ffmpeg_stop_movie();

	ff8_externals.movie_object->movie_is_playing = false;

	ff8_externals.movie_object->field_4C4AC = 0;
	ff8_externals.movie_object->field_4C4B0 = 0;

	*ff8_externals.enable_framelimiter = true;

	int ret = (int)ff8_externals.movie_object->movie_file_handle;

	if (ff8_externals.movie_object->movie_file_handle)
	{
		ret = CloseHandle(ff8_externals.movie_object->movie_file_handle);
		ff8_externals.movie_object->movie_file_handle = 0;
	}

	return ret;
}

bool is_overlapping_movie_playing()
{
	return ff7_externals.movie_object->is_playing && !is_movie_bgfield;
}

void movie_init()
{
	if(!ff8)
	{
		nxAudioEngine.setMovieAudioMaxSlots(2);

		replace_function(common_externals.prepare_movie, ff7_prepare_movie);
		replace_function(common_externals.release_movie_objects, ff7_release_movie_objects);
		replace_function(common_externals.start_movie, ff7_start_movie);
		replace_function(common_externals.update_movie_sample, ff7_update_movie_sample);
		replace_function(common_externals.stop_movie, ff7_stop_movie);
		replace_function(common_externals.get_movie_frame, ff7_get_movie_frame);
	}
	else
	{
		replace_function(common_externals.prepare_movie, ff8_prepare_movie);
		replace_function(common_externals.release_movie_objects, ff8_release_movie_objects);
		replace_function(common_externals.start_movie, ff8_start_movie);
		replace_function(common_externals.update_movie_sample, ff8_update_movie_sample);
		replace_function(ff8_externals.draw_movie_frame, draw_current_frame);
		replace_function(common_externals.stop_movie, ff8_stop_movie);
	}

	ffmpeg_movie_init();
}
