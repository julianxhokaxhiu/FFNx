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

// Esc pauses the game like Start, and the pause screen of the field, world map or battle shows this menu instead of PAUSE.
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

static bool enabled = false;
static std::string texts[ESC_MENU_COUNT];

static ff8_draw_menu_sprite_texture_infos sprites[256]; // The menu draws up to 117 sprites, the game pause screen has room for 85
static uint32_t ot = 0; // PlayStation ordering table, the draw calls read it

static bool esc_pressed = false;
static int open_frames = 0;
static bool menu_open = false;
static int cursor = ESC_MENU_RETURN;
static int choice = -1;
static int restart_frames = 0;

// Returns the chosen row, or -1
static int update_menu(int keyon)
{
	// Esc, Start or Cancel
	if (esc_pressed || (keyon & 0x810))
	{
		ff8_externals.menu_play_sound_sub_4B92A0(3); // Back sound
		return ESC_MENU_RETURN;
	}

	if (keyon & 0x1000) cursor = (cursor + ESC_MENU_COUNT - 1) % ESC_MENU_COUNT; // Up
	if (keyon & 0x4000) cursor = (cursor + 1) % ESC_MENU_COUNT; // Down
	if (keyon & 0x5000) ff8_externals.menu_play_sound_sub_4B92A0(1); // Cursor sound

	if (keyon & 0x40) // OK
	{
		ff8_externals.menu_play_sound_sub_4B92A0(2); // OK sound
		return cursor;
	}

	return -1;
}

static void draw_menu(int a1, const int16_t *pause_window)
{
	int text_width = 0;

	for (const std::string &text : texts) text_width = std::max(text_width, ff8_externals.get_text_width_sub_4A0F80(text.c_str()));

	// The first row goes where the game draws PAUSE (in battle, under the help text), the others below it.
	// Horizontally centered on the pause window, with room for the hand cursor on the left
	int x = pause_window[0] + (pause_window[2] - text_width) / 2 + 8;
	int y = pause_window[1] + pause_window[3] / 2 - 6; // 12 high text
	int16_t window[4] = { int16_t(x - 32), int16_t(y - 8), int16_t(text_width + 48), int16_t(ESC_MENU_COUNT * ROW_HEIGHT + 12) };

	auto draw_icon = (ff8_draw_menu_sprite_texture_infos *(*)(int, ff8_draw_menu_sprite_texture_infos *, int, uint16_t, uint16_t, int))ff8_externals.ff8_draw_icon_or_key1;

	// Front to back, like the game pause window. The hand cursor goes where the game menus put it
	ff8_draw_menu_sprite_texture_infos *next = draw_icon(a1, sprites, 0, x - 26, y + cursor * ROW_HEIGHT + 3, WINDOW_COLOR);

	for (int i = 0; i < ESC_MENU_COUNT; ++i) next = ff8_externals.draw_text_sub_4A1020(a1, next, x, y + i * ROW_HEIGHT, texts[i].c_str(), 7); // White

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

	if (!enabled) return;

	esc_pressed = esc_down && !esc_was_down;
	esc_was_down = esc_down;

	bool paused = ff8_externals.is_paused_sub_49F510() != 0; // Field, world map and battle, unlike is_game_paused

	// Esc waits for the game to pause, then for its pause screen, which the battle draws from its second paused frame.
	// Where Start can't pause, Esc is dropped after 3 frames, so that a later Start shows the usual PAUSE
	if (open_frames > 0 && !paused) open_frames--;
	if (restart_frames > 0) restart_frames--;

	if (!paused) menu_open = false;

	if (!esc_pressed || ff8_esc_menu_is_open() || restart_frames > 0) return;

	uint32_t mode = getmode_cached()->driver_mode;

	if (mode != MODE_FIELD && mode != MODE_WORLDMAP && mode != MODE_BATTLE) return;

	// Start for one frame on top of the held buttons, unless the game is paused already
	if (!paused) ff8_externals.engine_input_valid_buttons[1] |= 0x800;

	esc_pressed = false; // This Esc opens the menu, the pause screen must not take it as Return
	open_frames = 3;
	cursor = ESC_MENU_RETURN;
	choice = -1;
}

bool ff8_esc_menu_is_open()
{
	return menu_open || open_frames > 0;
}

bool ff8_esc_menu_restarting()
{
	return restart_frames > 0;
}

// Field and world map pause screen, a nonzero return unpauses the game
int ff8_esc_menu_pause_screen(int callback)
{
	menu_open = true;
	open_frames = 0;

	ff8_externals.input_fill_keystate(); // Like the game pause screen

	if (choice < 0)
	{
		choice = update_menu(ff8_externals.get_keyon(0, 0));

		if (choice == ESC_MENU_EXIT) exit_game();
	}

	draw_menu(int(&ot), ff8_externals.pause_menu_window_rect);

	if (callback) ((void(*)())callback)(); // Like the game pause screen, the world map keeps loading its textures

	// The field and world map compare the buttons with their last update, from before the pause, so they run again only once
	// the buttons are up, or one still held would be a new press
	if (choice < 0 || choice == ESC_MENU_EXIT || (ff8_externals.engine_input_valid_buttons[1] & 0xFFF)) return 0; // Any button but the directions

	if (choice == ESC_MENU_RESTART) restart_game(); // They check Ctrl+R only while they run

	menu_open = false;

	return 1;
}

ff8_draw_menu_sprite_texture_infos *ff8_esc_menu_battle_pause_window(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos)
{
	uint16_t &keyon = *(uint16_t *)(*(int *)ff8_externals.battle_menu_state + 0x14);

	menu_open = true;
	open_frames = 0;

	if (choice < 0)
	{
		choice = update_menu(keyon);

		if (choice == ESC_MENU_EXIT) exit_game();
		if (choice == ESC_MENU_RESTART) restart_game(); // The battle checks Ctrl+R while paused
	}

	draw_menu(a1, ff8_externals.battle_pause_window_rect);

	// The battle takes Start again only after 16 of its updates: press it until the battle unpauses
	if (choice == ESC_MENU_RETURN) keyon |= 0x800;

	return draw_infos; // The menu has its own sprites, the PAUSE window ones stay free
}

void ff8_esc_menu_init()
{
	char lang[4] = "";

	concat_lang_str(lang);

	// No Japanese table in ff8_encode_text, Esc keeps doing nothing
	if (strcmp(lang, "jp") == 0) return;

	const esc_menu_texts *t = &translations[0];

	for (const esc_menu_texts &translation : translations)
	{
		if (strcmp(lang, translation.lang) == 0) t = &translation;
	}

	for (int i = 0; i < ESC_MENU_COUNT; ++i) texts[i] = ff8_encode_text(t->rows[i]);

	enabled = true;
}
