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

static_assert(offsetof(ff8_battle_ui_context, window_border_flags) == 0x23);
static_assert(offsetof(ff8_battle_ui_context, window_rect) == 0x34);
static_assert(offsetof(ff8_battle_ui_context, scaled_window_rect) == 0x3C);
static_assert(offsetof(ff8_battle_ui_context, icon_id) == 0x44);

static uint32_t *ff8_battle_draw_window_box(int *ot, uint32_t *prim, int open_scale, int icon_id)
{
	ff8_battle_ui_context *context = *ff8_externals.battle_ui_context_dword_1D6D490;
	int16_t *window_rect = context->window_rect;
	int16_t *scaled_window_rect = context->scaled_window_rect;
	uint32_t border_flags = context->window_border_flags;

	ff8_externals.battle_compute_window_scale_rect_sub_4A7440(open_scale, window_rect, scaled_window_rect);

	if (open_scale < FF8_BATTLE_WINDOW_FULLY_OPEN)
	{
		int16_t width = scaled_window_rect[2], height = scaled_window_rect[3];

		scaled_window_rect[2] -= 2;
		scaled_window_rect[3] -= 2;
		prim = ff8_externals.battle_emit_relative_draw_area_sub_4A73A0(ot, prim, scaled_window_rect);
		scaled_window_rect[2] = width;
		scaled_window_rect[3] = height;
	}

	if (icon_id)
	{
		context = *ff8_externals.battle_ui_context_dword_1D6D490;
		context->icon_palette = 0;
		context->icon_id = uint16_t(icon_id);
		prim = ff8_externals.battle_draw_icon_sub_4B77C0(ot, prim, uint16_t(icon_id), window_rect[0], window_rect[1], context->color, context->icon_palette);
	}

	prim = ff8_externals.battle_draw_window_background_sub_4B6A60(ot, prim, scaled_window_rect, (*ff8_externals.battle_ui_context_dword_1D6D490)->color);
	prim = ff8_externals.battle_draw_window_border_sub_4B6A80(ot, prim, window_rect, (*ff8_externals.battle_ui_context_dword_1D6D490)->color, border_flags);

	if (open_scale < FF8_BATTLE_WINDOW_FULLY_OPEN) prim = ff8_externals.battle_emit_relative_draw_area_sub_4A73A0(ot, prim, scaled_window_rect);
	else prim = ff8_externals.battle_emit_full_draw_area_sub_4A7630(ot, prim);

	ff8_externals.battle_register_window_overlap_sub_4B1660((*ff8_externals.battle_ui_context_dword_1D6D490)->scaled_window_rect);

	return prim;
}

void ff8_battle_ui_window_init()
{
	replace_function(ff8_externals.battle_draw_window_box_sub_4A7510, ff8_battle_draw_window_box);
}
