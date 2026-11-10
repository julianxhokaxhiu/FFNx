/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 HobbitDur                                          //
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

// The PC port draws the queued menu sprites each time a window sets a draw area, clipped to that new area.
// A window only sets one while it opens, so the content of a fully open window stays queued and the next
// opening window clips it away: the Magic or Item list disappears for a few frames when the HP window opens
// on an ally target. A fully open window now ends with a full screen draw area, so its content is drawn first.

#include "ui_window.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"

#include <stddef.h>
#include <stdint.h>

#define FF8_BATTLE_WINDOW_FULLY_OPEN 0x1000

struct ff8_battle_ui_context {
	uint32_t color;
	uint8_t unknown_04[0x1F];
	uint8_t window_border_flags;
	uint8_t unknown_24[0x10];
	int16_t window_rect[4];
	int16_t scaled_window_rect[4];
	uint16_t icon_id;
	uint16_t icon_palette;
};

static_assert(offsetof(ff8_battle_ui_context, window_border_flags) == 0x23);
static_assert(offsetof(ff8_battle_ui_context, window_rect) == 0x34);
static_assert(offsetof(ff8_battle_ui_context, scaled_window_rect) == 0x3C);
static_assert(offsetof(ff8_battle_ui_context, icon_id) == 0x44);

typedef void (*ff8_battle_compute_window_scale_rect)(int open_scale, int16_t *window_rect, int16_t *scaled_window_rect);
typedef uint32_t *(*ff8_battle_emit_relative_draw_area)(int *ot, uint32_t *prim, int16_t *rect);
typedef uint32_t *(*ff8_battle_emit_full_draw_area)(int *ot, uint32_t *prim);
typedef uint32_t *(*ff8_battle_draw_icon)(int *ot, uint32_t *prim, int icon_id, int x, int y, uint32_t color, int palette);
typedef uint32_t *(*ff8_battle_draw_window_background)(int *ot, uint32_t *prim, int16_t *rect, uint32_t color);
typedef uint32_t *(*ff8_battle_draw_window_border)(int *ot, uint32_t *prim, int16_t *rect, uint32_t color, uint32_t border_flags);
typedef void (*ff8_battle_register_window_overlap)(int16_t *rect);

static struct
{
	ff8_battle_ui_context **context;
	ff8_battle_compute_window_scale_rect compute_window_scale_rect;
	ff8_battle_emit_relative_draw_area emit_relative_draw_area;
	ff8_battle_emit_full_draw_area emit_full_draw_area;
	ff8_battle_draw_icon draw_icon;
	ff8_battle_draw_window_background draw_window_background;
	ff8_battle_draw_window_border draw_window_border;
	ff8_battle_register_window_overlap register_window_overlap;
} battle_ui;

static uint32_t *ff8_battle_draw_window_box(int *ot, uint32_t *prim, int open_scale, int icon_id)
{
	ff8_battle_ui_context *context = *battle_ui.context;
	int16_t *window_rect = context->window_rect;
	int16_t *scaled_window_rect = context->scaled_window_rect;
	uint32_t border_flags = context->window_border_flags;

	battle_ui.compute_window_scale_rect(open_scale, window_rect, scaled_window_rect);

	if (open_scale < FF8_BATTLE_WINDOW_FULLY_OPEN)
	{
		int16_t width = scaled_window_rect[2], height = scaled_window_rect[3];

		scaled_window_rect[2] -= 2;
		scaled_window_rect[3] -= 2;
		prim = battle_ui.emit_relative_draw_area(ot, prim, scaled_window_rect);
		scaled_window_rect[2] = width;
		scaled_window_rect[3] = height;
	}

	if (icon_id)
	{
		context = *battle_ui.context;
		context->icon_palette = 0;
		context->icon_id = uint16_t(icon_id);
		prim = battle_ui.draw_icon(ot, prim, uint16_t(icon_id), window_rect[0], window_rect[1], context->color, context->icon_palette);
	}

	prim = battle_ui.draw_window_background(ot, prim, scaled_window_rect, (*battle_ui.context)->color);
	prim = battle_ui.draw_window_border(ot, prim, window_rect, (*battle_ui.context)->color, border_flags);

	if (open_scale < FF8_BATTLE_WINDOW_FULLY_OPEN) prim = battle_ui.emit_relative_draw_area(ot, prim, scaled_window_rect);
	else prim = battle_ui.emit_full_draw_area(ot, prim);

	battle_ui.register_window_overlap((*battle_ui.context)->scaled_window_rect);

	return prim;
}

void ff8_battle_ui_window_init()
{
	uint32_t draw_window_box = ff8_externals.battle_draw_window_box_sub_4A7510;

	battle_ui.context = (ff8_battle_ui_context **)get_absolute_value(draw_window_box, 0x2);
	battle_ui.compute_window_scale_rect = (ff8_battle_compute_window_scale_rect)get_relative_call(draw_window_box, 0x20);
	battle_ui.emit_relative_draw_area = (ff8_battle_emit_relative_draw_area)get_relative_call(draw_window_box, 0x4F);
	battle_ui.draw_icon = (ff8_battle_draw_icon)get_relative_call(draw_window_box, 0xB0);
	battle_ui.draw_window_background = (ff8_battle_draw_window_background)get_relative_call(draw_window_box, 0xC8);
	battle_ui.draw_window_border = (ff8_battle_draw_window_border)get_relative_call(draw_window_box, 0xDE);
	battle_ui.register_window_overlap = (ff8_battle_register_window_overlap)get_relative_call(draw_window_box, 0x107);
	battle_ui.emit_full_draw_area = (ff8_battle_emit_full_draw_area)get_relative_call(ff8_externals.battle_menu_draw_inline_command_list_sub_4BCCD0, 0x7E);

	replace_function(draw_window_box, ff8_battle_draw_window_box);
}
