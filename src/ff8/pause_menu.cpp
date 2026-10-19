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

#include "pause_menu.h"
#include "../globals.h"
#include "../common.h"
#include "../patch.h"
#include "../vibration.h"
#include "engine.h"

#include <algorithm>
#include <cstring>
#include <string>

// The pause screen of the field, world map and battle shows this menu instead of PAUSE, and Esc pauses the game like Start.
// Restart game is the game's own Ctrl+R reset: ff8_get_key_state reports Ctrl+R held for a moment, so each module resets its own way.
// With a pad that can rumble, the Vibration row takes the place of the game's vibration pause screen (the game itself stops the rumble while paused).

enum { PAUSE_MENU_RETURN, PAUSE_MENU_RESTART, PAUSE_MENU_EXIT, PAUSE_MENU_VIBRATION, PAUSE_MENU_COUNT };

constexpr uint32_t WINDOW_COLOR = 0x64808080; // Same as the game pause window
constexpr int ROW_HEIGHT = 16;
constexpr int VALUE_SPACING = 16; // Between Vibration and ON or OFF, with the divider

struct pause_menu_texts
{
	const char *lang;
	const char *rows[PAUSE_MENU_COUNT];
	const char *on;
	const char *off;
};

// Vibration, ON and OFF too: the game's own are pictures in a smaller font, or Config menu texts in a buffer other texts get loaded into
static const pause_menu_texts translations[] = {
	{ "en", { "Return to game", "Restart game", "Exit game", "Vibration" }, "ON", "OFF" },
	{ "fr", { "Retour au jeu", "Redémarrer le jeu", "Quitter le jeu", "Vibration" }, "OUI", "NON" },
	{ "de", { "Zurück zum Spiel", "Spiel neu starten", "Spiel beenden", "Vibration" }, "EIN", "AUS" },
	{ "es", { "Volver al juego", "Reiniciar el juego", "Salir del juego", "Vibración" }, "SÍ", "NO" },
	{ "it", { "Torna al gioco", "Riavvia il gioco", "Esci dal gioco", "Vibrazione" }, "SÌ", "NO" },
};

static std::string texts[PAUSE_MENU_COUNT];
static std::string on_text;
static std::string off_text;
static bool japanese = false;

static ff8_draw_menu_sprite_texture_infos sprites[256]; // The menu draws up to 134 sprites, the game pause screen has room for 85
static uint32_t ot = 0; // PlayStation ordering table, the draw calls read it

static int ok_button = 0x40; // Cross
static int cancel_button = 0x10; // Triangle
static bool esc_pressed = false;
static int row_count = PAUSE_MENU_COUNT - 1;
static int cursor = PAUSE_MENU_RETURN;
static int choice = -1;
static int restart_frames = 0;

static bool vibration_on()
{
	return ((int(*)(int))ff8_externals.vibration_get_is_enabled)(0) == 255; // Port 0, like the game's vibration pause screen
}

// Returns the chosen row, or -1
static int update_menu(int keyon)
{
	// Esc, Start or Cancel
	if (esc_pressed || (keyon & (0x800 | cancel_button)))
	{
		ff8_externals.menu_play_sound_sub_4B92A0(3); // Back sound
		return PAUSE_MENU_RETURN;
	}

	if (keyon & 0x1000) cursor = (cursor + row_count - 1) % row_count; // Up
	if (keyon & 0x4000) cursor = (cursor + 1) % row_count; // Down
	if (keyon & 0x5000) ff8_externals.menu_play_sound_sub_4B92A0(1); // Cursor sound

	if (cursor == PAUSE_MENU_VIBRATION)
	{
		if (keyon & (0xA000 | ok_button)) // Left, Right or OK
		{
			((void(*)(int, int))ff8_externals.vibration_set_is_enabled)(0, vibration_on() ? 0 : 255);
			ff8_externals.menu_play_sound_sub_4B92A0(1); // Cursor sound, like the Config menu switches
		}

		return -1;
	}

	if (keyon & ok_button)
	{
		ff8_externals.menu_play_sound_sub_4B92A0(2); // OK sound
		return cursor;
	}

	return -1;
}

// No Japanese table in ff8_encode_text: the game converts the English texts each time they are drawn, like the title screen's Credits
static std::string game_text(const std::string &text)
{
	std::string ret = text;

	if (japanese) ((void(*)(char *))ff8_externals.convert_ascii_to_ff8enc_sub_4A2F20)(ret.data());

	return ret;
}

static void draw_menu(int a1, const int16_t *pause_window)
{
	bool vibration = row_count == PAUSE_MENU_COUNT;
	std::string rows[PAUSE_MENU_COUNT];
	std::string on = game_text(on_text);
	std::string off = game_text(off_text);
	int value_width = std::max(ff8_externals.get_text_width_sub_4A0F80(on.c_str()), ff8_externals.get_text_width_sub_4A0F80(off.c_str())); // ON and OFF start at the same place
	int text_width = 0;

	for (int i = 0; i < row_count; ++i)
	{
		rows[i] = game_text(texts[i]);

		int width = ff8_externals.get_text_width_sub_4A0F80(rows[i].c_str());

		if (i == PAUSE_MENU_VIBRATION) width += VALUE_SPACING + value_width;

		text_width = std::max(text_width, width);
	}

	// Centered on the pause window, with room for the hand cursor on the left. In battle only the first row is centered,
	// where the game draws PAUSE under the help text, and the others go below it
	int text_height = getmode_cached()->driver_mode == MODE_BATTLE ? 12 : (row_count - 1) * ROW_HEIGHT + 12; // 12 high text
	int x = pause_window[0] + (pause_window[2] - text_width) / 2 + 8;
	int y = pause_window[1] + (pause_window[3] - text_height) / 2;
	int16_t window[4] = { int16_t(x - 32), int16_t(y - 8), int16_t(text_width + 48), int16_t(row_count * ROW_HEIGHT + 12) };
	int value_x = x + text_width - value_width;
	int vibration_y = y + PAUSE_MENU_VIBRATION * ROW_HEIGHT;
	// The game's divider line runs down the rect's left edge from 8 pixels under its top: here 8 before ON or OFF, from the row to the window bottom
	int16_t divider[4] = { int16_t(value_x - 8), int16_t(vibration_y - 8), 8, int16_t(window[1] + window[3] - vibration_y + 8) };

	auto draw_icon = (ff8_draw_menu_sprite_texture_infos *(*)(int, ff8_draw_menu_sprite_texture_infos *, int, uint16_t, uint16_t, int))ff8_externals.ff8_draw_icon_or_key1;

	// Front to back, like the game pause window. The hand cursor goes where the game menus put it
	ff8_draw_menu_sprite_texture_infos *next = draw_icon(a1, sprites, 0, x - 26, y + cursor * ROW_HEIGHT + 3, WINDOW_COLOR);

	for (int i = 0; i < row_count; ++i) next = ff8_externals.draw_text_sub_4A1020(a1, next, x, y + i * ROW_HEIGHT, rows[i].c_str(), 7); // White

	if (vibration)
	{
		next = ff8_externals.draw_text_sub_4A1020(a1, next, value_x, vibration_y, (vibration_on() ? on : off).c_str(), 7);
		next = ff8_externals.draw_window_divider_sub_4B6530(a1, next, divider, WINDOW_COLOR, 1); // Vertical, like the game's vibration pause screen
	}

	next = draw_icon(a1, next, 0x4B, window[0], window[1], WINDOW_COLOR); // PAUSE on the corner, like the game's pause screens with options
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

// Field and world map pause screen, a nonzero return unpauses the game
static int pause_screen(int callback)
{
	ff8_externals.input_fill_keystate(); // Like the game pause screen

	if (choice < 0)
	{
		choice = update_menu(ff8_externals.get_keyon(0, 0));

		if (choice == PAUSE_MENU_EXIT) exit_game();
	}

	draw_menu(int(&ot), ff8_externals.pause_menu_window_rect);

	if (callback) ((void(*)())callback)(); // Like the game pause screen, the world map keeps loading its textures

	// The field and world map compare the buttons with their last update, from before the pause, so they run again only once
	// OK and Cancel are up, or the one that chose here would be a new press there
	if (choice < 0 || choice == PAUSE_MENU_EXIT || (ff8_externals.engine_input_valid_buttons[1] & (ok_button | cancel_button))) return 0;

	if (choice == PAUSE_MENU_RESTART) restart_game(); // They check Ctrl+R only while they run

	return 1;
}

static ff8_draw_menu_sprite_texture_infos *battle_pause_window(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos)
{
	uint16_t &keyon = *(uint16_t *)(*(int *)ff8_externals.battle_menu_state + 0x14);

	if (choice < 0)
	{
		choice = update_menu(keyon);

		if (choice == PAUSE_MENU_EXIT) exit_game();
		if (choice == PAUSE_MENU_RESTART) restart_game(); // The battle checks Ctrl+R while paused
	}

	draw_menu(a1, ff8_externals.battle_pause_window_rect);

	// The battle takes Start again only after 16 of its updates: press it until the battle unpauses
	if (choice == PAUSE_MENU_RETURN) keyon |= 0x800;

	return draw_infos; // The menu has its own sprites, the PAUSE window ones stay free
}

void ff8_pause_menu_update(bool esc_down)
{
	static bool esc_was_down = false;

	esc_pressed = esc_down && !esc_was_down;
	esc_was_down = esc_down;

	bool paused = ff8_externals.is_paused_sub_49F510() != 0; // Field, world map and battle, unlike is_game_paused

	if (restart_frames > 0) restart_frames--;

	row_count = nxVibrationEngine.canRumble() ? PAUSE_MENU_COUNT : PAUSE_MENU_COUNT - 1; // Vibration, the last row, only with a pad that can rumble

	// Each pause starts on Return to game
	if (!paused)
	{
		cursor = PAUSE_MENU_RETURN;
		choice = -1;
	}

	if (cursor >= row_count) cursor = PAUSE_MENU_RETURN; // The pad that can rumble was unplugged

	if (!esc_pressed || paused || restart_frames > 0) return;

	uint32_t mode = getmode_cached()->driver_mode;

	if (mode != MODE_FIELD && mode != MODE_WORLDMAP && mode != MODE_BATTLE) return;

	ff8_externals.engine_input_valid_buttons[1] |= 0x800; // Start for one frame on top of the held buttons
}

bool ff8_pause_menu_restarting()
{
	return restart_frames > 0;
}

void ff8_pause_menu_init()
{
	char lang[4] = "";

	concat_lang_str(lang);

	japanese = strcmp(lang, "jp") == 0;

	const pause_menu_texts *t = &translations[0];

	for (const pause_menu_texts &translation : translations)
	{
		if (strcmp(lang, translation.lang) == 0) t = &translation;
	}

	for (int i = 0; i < PAUSE_MENU_COUNT; ++i) texts[i] = japanese ? t->rows[i] : ff8_encode_text(t->rows[i]);

	on_text = japanese ? t->on : ff8_encode_text(t->on);
	off_text = japanese ? t->off : ff8_encode_text(t->off);

	// Like the game's menus, the Japanese edition takes Circle for OK and Cross for Cancel
	if (JP_VERSION)
	{
		ok_button = 0x20;
		cancel_button = 0x40;
	}

	replace_call(uint32_t(ff8_externals.check_game_is_paused) + 0x88, pause_screen);
	replace_call(ff8_externals.battle_pause_sub_4CD140 + (JP_VERSION ? 0x1F1 : 0x225), battle_pause_window);
}
