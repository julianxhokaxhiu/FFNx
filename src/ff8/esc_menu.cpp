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

#include "esc_menu.h"
#include "../globals.h"
#include "../common.h"
#include "engine.h"

#include <algorithm>
#include <cstring>
#include <string>

// Esc pauses the game like Start, and while that pause is open the pause hooks of vibration.cpp show this menu instead of the
// game's pause screen in the field, world map and battle. Start and the pause key keep the game's pause screen.
// Restart game is the game's own Ctrl+R reset: ff8_get_key_state reports Ctrl+R held for a moment, so each module resets its own way.

enum { ESC_MENU_RETURN, ESC_MENU_RESTART, ESC_MENU_EXIT, ESC_MENU_COUNT };

constexpr uint32_t WINDOW_COLOR = 0x64808080; // Same as the game pause window
constexpr int ROW_HEIGHT = 16;

struct esc_menu_texts
{
	const char *lang;
	const char *rows[ESC_MENU_COUNT];
};

static const esc_menu_texts translations[] = {
	{ "en", { "Return to game", "Restart game", "Exit game" } },
	{ "fr", { "Retour au jeu", "Redémarrer le jeu", "Quitter le jeu" } },
	{ "de", { "Zurück zum Spiel", "Spiel neu starten", "Spiel beenden" } },
	{ "es", { "Volver al juego", "Reiniciar el juego", "Salir del juego" } },
	{ "it", { "Torna al gioco", "Riavvia il gioco", "Esci dal gioco" } },
};

static std::string texts[ESC_MENU_COUNT];
static bool japanese = false;

static ff8_draw_menu_sprite_texture_infos sprites[256]; // The longest translation draws 117 sprites, the game pause screen has room for 85
static uint32_t ot = 0; // PlayStation ordering table, the draw calls read it

static int ok_button = 0x40; // Cross
static int cancel_button = 0x10; // Triangle
static bool esc_pressed = false;
static int opening_frames = 0; // Esc pressed, waiting for the game's pause screen
static bool menu_open = false;
static bool alternative_pause_set = false;
static int cursor = ESC_MENU_RETURN;
static int choice = -1;
static int restart_frames = 0;

// Returns the chosen row, or -1
static int update_menu(int keyon)
{
	// Esc, Start or Cancel
	if (esc_pressed || (keyon & (0x800 | cancel_button)))
	{
		ff8_externals.menu_play_sound_sub_4B92A0(3); // Back sound
		return ESC_MENU_RETURN;
	}

	if (keyon & 0x1000) cursor = (cursor + ESC_MENU_COUNT - 1) % ESC_MENU_COUNT; // Up
	if (keyon & 0x4000) cursor = (cursor + 1) % ESC_MENU_COUNT; // Down
	if (keyon & (0x1000 | 0x4000)) ff8_externals.menu_play_sound_sub_4B92A0(1); // Cursor sound

	if (keyon & ok_button)
	{
		ff8_externals.menu_play_sound_sub_4B92A0(2); // OK sound
		return cursor;
	}

	return -1;
}

static void draw_menu(int a1, const int16_t *pause_window)
{
	std::string rows[ESC_MENU_COUNT];
	int text_width = 0;

	for (int i = 0; i < ESC_MENU_COUNT; ++i)
	{
		rows[i] = texts[i];

		// No Japanese table in ff8_encode_text: the English rows go through the game's converter at each draw, like the title screen's Credits
		if (japanese) ((void(*)(char *))ff8_externals.convert_ascii_to_ff8enc_sub_4A2F20)(rows[i].data());

		text_width = std::max(text_width, ff8_externals.get_text_width_sub_4A0F80(rows[i].c_str()));
	}

	// Centered where the game draws PAUSE, with room for the hand cursor on the left
	int text_height = (ESC_MENU_COUNT - 1) * ROW_HEIGHT + 12; // 12 high text
	int x = pause_window[0] + (pause_window[2] - text_width) / 2 + 8;
	int y = pause_window[1] + (pause_window[3] - text_height) / 2;
	int16_t window[4] = { int16_t(x - 32), int16_t(y - 8), int16_t(text_width + 48), int16_t(ESC_MENU_COUNT * ROW_HEIGHT + 12) };

	auto draw_icon = (ff8_draw_menu_sprite_texture_infos *(*)(int, ff8_draw_menu_sprite_texture_infos *, int, uint16_t, uint16_t, int))ff8_externals.ff8_draw_icon_or_key1;

	// Front to back, like the game pause window. The hand cursor goes where the game menus put it
	ff8_draw_menu_sprite_texture_infos *next = draw_icon(a1, sprites, 0, x - 26, y + cursor * ROW_HEIGHT + 3, WINDOW_COLOR);

	for (int i = 0; i < ESC_MENU_COUNT; ++i) next = ff8_externals.draw_text_sub_4A1020(a1, next, x, y + i * ROW_HEIGHT, rows[i].c_str(), 7); // White

	next = ff8_externals.draw_window_frame_sub_4B6A60(a1, next, window, WINDOW_COLOR);
	ff8_externals.draw_window_background_sub_4B6A80(a1, next, window, WINDOW_COLOR, 8); // Same style as the game pause window
}

static void exit_game()
{
	((ff8_game_obj *)common_externals.get_game_object())->do_quit = 1;
}

static void restart_game()
{
	restart_frames = 30; // Ctrl+R held for a while, the game resets after more than 5 checks in a row
}

void ff8_esc_menu_update(bool esc_down)
{
	static bool esc_was_down = false;

	esc_pressed = esc_down && !esc_was_down;
	esc_was_down = esc_down;

	// Also is_game_paused, the field and world map pause: the game's vibration pause screen clears the other flag each frame
	bool paused = ff8_externals.is_paused_sub_49F510() || *ff8_externals.is_game_paused;

	// Esc waits for the game to pause, then for its pause screen, which the battle draws from its second paused frame.
	// Where Start can't pause, Esc is dropped after 3 frames, so that a later Start shows the game's pause screen
	if (opening_frames > 0 && !paused) opening_frames--;
	if (restart_frames > 0) restart_frames--;

	if (!paused) menu_open = false;

	// The battle clears is_alternative_pause_menu when Start unpauses it, but not on its Ctrl+R reset
	if (alternative_pause_set && !ff8_esc_menu_is_open())
	{
		*ff8_externals.is_alternative_pause_menu = 0;
		alternative_pause_set = false;
	}

	// Esc pauses only a running game, the game's own pause screen stays as it is
	if (!esc_pressed || paused || ff8_esc_menu_is_open() || restart_frames > 0) return;

	uint32_t mode = getmode_cached()->driver_mode;

	if (mode != MODE_FIELD && mode != MODE_WORLDMAP && mode != MODE_BATTLE) return;

	ff8_externals.engine_input_valid_buttons[1] |= 0x800; // Start for one frame on top of the held buttons

	// Same game flag as alternate_pause_battle, kept for the whole pause so the game itself hides the help text
	if (mode == MODE_BATTLE)
	{
		*ff8_externals.is_alternative_pause_menu = 1;
		alternative_pause_set = true;
	}

	opening_frames = 3;
	cursor = ESC_MENU_RETURN;
	choice = -1;
}

bool ff8_esc_menu_is_open()
{
	return menu_open || opening_frames > 0;
}

bool ff8_esc_menu_restarting()
{
	return restart_frames > 0;
}

// Field and world map pause screen, a nonzero return unpauses the game
int ff8_esc_menu_pause_screen(int callback)
{
	menu_open = true;
	opening_frames = 0;

	ff8_externals.input_fill_keystate(); // Like the game pause screen

	if (choice < 0)
	{
		choice = update_menu(ff8_externals.get_keyon(0, 0));

		if (choice == ESC_MENU_EXIT) exit_game();
	}

	draw_menu(int(&ot), ff8_externals.pause_menu_window_rect);

	if (callback) ((void(*)())callback)(); // Like the game pause screen

	// Unpause only after OK and Cancel are released, or the field and world map take them as a new press
	if (choice < 0 || choice == ESC_MENU_EXIT || (ff8_externals.engine_input_valid_buttons[1] & (ok_button | cancel_button))) return 0;

	if (choice == ESC_MENU_RESTART) restart_game(); // The field and world map check Ctrl+R only while they run

	return 1;
}

ff8_draw_menu_sprite_texture_infos *ff8_esc_menu_battle_pause_window(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos)
{
	uint16_t &keyon = *(uint16_t *)(*(int *)ff8_externals.battle_menu_state + 0x14);

	menu_open = true;
	opening_frames = 0;

	if (choice < 0)
	{
		choice = update_menu(keyon);

		if (choice == ESC_MENU_EXIT) exit_game();
		if (choice == ESC_MENU_RESTART) restart_game(); // The battle checks Ctrl+R while paused
	}

	draw_menu(a1, ff8_externals.battle_pause_window_rect);

	// The battle ignores Start for 16 updates after pausing, so keep pressing it until it unpauses
	if (choice == ESC_MENU_RETURN) keyon |= 0x800;

	return draw_infos; // The menu has its own sprites, the PAUSE window ones stay free
}

void ff8_esc_menu_init()
{
	char lang[4] = "";

	concat_lang_str(lang);

	japanese = strcmp(lang, "jp") == 0;

	const esc_menu_texts *t = &translations[0];

	for (const esc_menu_texts &translation : translations)
	{
		if (strcmp(lang, translation.lang) == 0) t = &translation;
	}

	for (int i = 0; i < ESC_MENU_COUNT; ++i) texts[i] = japanese ? t->rows[i] : ff8_encode_text(t->rows[i]);

	// Like the game's menus, the Japanese edition takes Circle for OK and Cross for Cancel
	if (JP_VERSION)
	{
		ok_button = 0x20;
		cancel_button = 0x40;
	}
}
