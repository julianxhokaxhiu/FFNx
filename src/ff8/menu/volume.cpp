/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 AxlRose                                            //
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

#include "volume.h"
#include "../../globals.h"
#include "../../audio.h"
#include "../../cfg.h"
#include "../../common.h"
#include "../../patch.h"
#include "../engine.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>

// The Sound screen is the game Config list with other rows, swapped in around the game update and draw calls.
// Game sliders only edit savemap config bytes, so the FFNx volumes use the bytes of options not on the screen.

enum { SOUND_SFX, SOUND_MUSIC, SOUND_VOICE, SOUND_AMBIENT, SOUND_MOVIE, SOUND_COUNT };

constexpr int CONFIG_SOUND_ROW = 8; // Game sound effects slider
constexpr int16_t SFX_TEXT_ID = 0x35; // The game checks this id to apply the sound effects volume
constexpr int16_t FIRST_TEXT_ID = 0x100; // FFNx texts, past the game ones: the screen rows, then the Sound row

// Config bytes for the music, voice, ambient and movie sliders: battle speed, battle message, field message and camera
constexpr uint16_t FFNX_CONFIG_BYTES[] = { 0, 1, 2, 6 };

struct sound_texts
{
	const char *lang;
	const char *names[SOUND_COUNT + 1];
	const char *helps[SOUND_COUNT + 1];
};

static const sound_texts translations[] = {
	{ "en", { "Sound effects", "Music", "Voice", "Ambient", "Movie", "Sound" },
		{ "Volume of the sound effects", "Volume of the music", "Volume of the voice acting", "Volume of the ambient sounds", "Volume of the movies", "Change the sound volumes" } },
	{ "fr", { "Effets sonores", "Musique", "Voix", "Ambiance", "Vidéo", "Son" },
		{ "Volume des effets sonores", "Volume de la musique", "Volume des voix", "Volume des sons d'ambiance", "Volume des vidéos", "Régler les volumes sonores" } },
	{ "de", { "Soundeffekte", "Musik", "Sprache", "Umgebung", "Video", "Ton" },
		{ "Lautstärke der Soundeffekte", "Lautstärke der Musik", "Lautstärke der Sprachausgabe", "Lautstärke der Umgebungsgeräusche", "Lautstärke der Videos", "Lautstärken einstellen" } },
	{ "es", { "Efectos de sonido", "Música", "Voz", "Ambiente", "Vídeo", "Sonido" },
		{ "Volumen de los efectos de sonido", "Volumen de la música", "Volumen de las voces", "Volumen de los sonidos ambientales", "Volumen de los vídeos", "Ajustar los volúmenes del sonido" } },
	{ "it", { "Effetti sonori", "Musica", "Voci", "Ambiente", "Filmati", "Audio" },
		{ "Volume degli effetti sonori", "Volume della musica", "Volume delle voci", "Volume dei suoni ambientali", "Volume dei filmati", "Regolare i volumi dell'audio" } },
};

static long *volumes[SOUND_COUNT] = { nullptr, &external_music_volume, &external_voice_volume, &external_ambient_volume, &ffmpeg_video_volume };

// Without external music the game plays MIDI music, its volume is not in FFNx.toml and starts at 100% like in the game
static long midi_volume = 100;
static int32_t game_midi_volume = 0; // DirectMusic master volume set by the game, in hundredths of dB

static std::string texts[SOUND_COUNT + 1][2]; // Name and help
static ff8_menu_config_input screen_rows[SOUND_COUNT + 1]; // And the end row
static uint16_t screen_row_count = 0;
static uint8_t screen_volumes[SOUND_COUNT] = {};
static bool screen_open = false;

static void swap_screen_rows()
{
	uint8_t *config = &ff8_externals.savemap->config.battle_speed;

	std::swap_ranges(screen_rows, screen_rows + SOUND_COUNT + 1, ff8_externals.menu_config_input_desc);
	std::swap(screen_row_count, *ff8_externals.menu_config_input_count);

	for (int i = SOUND_MUSIC; i < SOUND_COUNT; ++i) std::swap(screen_volumes[i], config[FFNX_CONFIG_BYTES[i - SOUND_MUSIC]]);
}

// The Music volume goes on top of the game one, like the external music: 50% is -6 dB, 0% is silence
static BOOL ff8_menu_volume_set_midi_volume(int32_t volume)
{
	game_midi_volume = volume;

	if (midi_volume <= 0) volume = -10000;
	else if (midi_volume < 100) volume = std::max<int32_t>(volume + int32_t(2000 * log10(midi_volume / 100.0)), -10000);

	return ff8_externals.dmusicperf_set_master_volume_sub_46F530(volume);
}

static void set_volume(int i, long volume)
{
	*volumes[i] = volume;

	switch (i)
	{
	case SOUND_MUSIC:
		if (use_external_music) nxAudioEngine.setMusicMasterVolume(volume / 100.0f);
		else ff8_menu_volume_set_midi_volume(game_midi_volume);
		break;
	case SOUND_VOICE: nxAudioEngine.setVoiceMasterVolume(volume / 100.0f); break;
	case SOUND_AMBIENT: nxAudioEngine.setAmbientMasterVolume(volume / 100.0f); break;
	// Movies read ffmpeg_video_volume when they start
	}
}

static void ff8_menu_volume_controller(ff8_menu_config_task *task)
{
	uint32_t &keys = *ff8_externals.menu_data_1D76A9C;

	// Config is opening, closing or in Customize
	if (task->state != 3) screen_open = false;
	// OK on the Sound row
	else if (!screen_open && task->row == CONFIG_SOUND_ROW && (keys & 0x40))
	{
		ff8_externals.menu_play_sound_sub_4B92A0(2); // OK sound
		task->row = SOUND_SFX;
		screen_open = true;

		for (int i = SOUND_MUSIC; i < SOUND_COUNT; ++i) screen_volumes[i] = uint8_t(*volumes[i]);
	}

	// Back returns to the Config list, clear it so the game does not leave Config
	if (screen_open && (keys & 0x10))
	{
		keys &= ~0x10;
		ff8_externals.menu_play_sound_sub_4B92A0(3); // Back sound
		task->row = CONFIG_SOUND_ROW;
		screen_open = false;
	}

	if (screen_open) swap_screen_rows();

	((void(*)(ff8_menu_config_task *))ff8_externals.menu_config_controller)(task);

	if (screen_open)
	{
		swap_screen_rows();

		for (int i = SOUND_MUSIC; i < SOUND_COUNT; ++i)
		{
			if (screen_volumes[i] != *volumes[i]) set_volume(i, screen_volumes[i]);
		}
	}
}

static uint32_t ff8_menu_volume_render_list(ff8_menu_config_task *task, uint32_t a2, uint32_t a3, int x, int y)
{
	if (screen_open) swap_screen_rows();

	uint32_t ret = ((uint32_t(*)(ff8_menu_config_task *, uint32_t, uint32_t, int, int))ff8_externals.menu_config_render_list_sub_4EEB70)(task, a2, a3, x, y);

	if (screen_open) swap_screen_rows();

	return ret;
}

static char *ff8_menu_volume_get_text(int pool_id, int cat_id, int text_id, int help)
{
	// Show "Sound effects" for the game slider, next to the other volumes
	if (text_id == SFX_TEXT_ID) text_id = FIRST_TEXT_ID + SOUND_SFX;

	int i = text_id - FIRST_TEXT_ID;

	if (i >= 0 && i <= SOUND_COUNT) return texts[i][help != 0].data();

	return ((char *(*)(int, int, int, int))ff8_externals.get_text_data)(pool_id, cat_id, text_id, help);
}

void ff8_menu_volume_init()
{
	char lang[4] = "";

	concat_lang_str(lang);

	// No Japanese table in ff8_encode_text, keep the game slider
	if (strcmp(lang, "jp") == 0) return;

	const sound_texts *t = &translations[0];

	for (const sound_texts &translation : translations)
	{
		if (strcmp(lang, translation.lang) == 0) t = &translation;
	}

	for (int i = 0; i <= SOUND_COUNT; ++i)
	{
		texts[i][0] = ff8_encode_text(t->names[i]);
		texts[i][1] = ff8_encode_text(t->helps[i]);
	}

	ff8_menu_config_input &sound_row = ff8_externals.menu_config_input_desc[CONFIG_SOUND_ROW];

	screen_rows[screen_row_count++] = sound_row; // Game slider

	for (int i = SOUND_MUSIC; i < SOUND_COUNT; ++i)
	{
		// Without FFmpeg the game plays the movies itself
		if (i == SOUND_MOVIE && !enable_ffmpeg_videos) continue;

		ff8_menu_config_input &row = screen_rows[screen_row_count++];

		row.text_id_name = int16_t(FIRST_TEXT_ID + i);
		row.type = 0x21; // Slider 0..100
		row.mask = FFNX_CONFIG_BYTES[i - SOUND_MUSIC]; // Savemap config byte
	}

	screen_rows[screen_row_count].text_id_name = -1; // End of configuration lines

	sound_row.text_id_name = int16_t(FIRST_TEXT_ID + SOUND_COUNT); // "Sound"
	sound_row.type = 6; // Label, OK opens the Sound screen

	uint32_t menu_config_start = uint32_t(ff8_externals.menu_callbacks[8].func);

	// The Config start gives the controller to its menu task, then calls it once
	patch_code_dword(menu_config_start + 0x8, DWORD(ff8_menu_volume_controller));
	replace_call(menu_config_start + 0x148, ff8_menu_volume_controller);
	replace_call(ff8_externals.menu_config_render + 0x125, ff8_menu_volume_render_list);
	replace_call(ff8_externals.menu_config_controller + 0x255, ff8_menu_volume_get_text); // Help text
	replace_call(ff8_externals.menu_config_render_list_sub_4EEB70 + 0x3D3, ff8_menu_volume_get_text); // Row names

	// Without external music, Music sets the volume of the game MIDI music
	if (!use_external_music)
	{
		volumes[SOUND_MUSIC] = &midi_volume;
		replace_call(uint32_t(ff8_externals.dmusicperf_set_volume_sub_46C6F0) + 0x48, ff8_menu_volume_set_midi_volume); // MIDI volume set right away
		replace_call(ff8_externals.volume_music_update + 0x98, ff8_menu_volume_set_midi_volume); // MIDI volume fade step
	}
}
