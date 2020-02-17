/* 
 * FFNx - Complete OpenGL replacement of the Direct3D renderer used in 
 * the original ports of Final Fantasy VII and Final Fantasy VIII for the PC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * fake_dd.h - definitions for the fake DirectDraw interface used to intercept
 * FF8 movie player output
 */

#pragma once

#include "types.h"

struct ddsurface
{
	void *query_interface;
	void *addref;
	void *release;
	uint field_C;
	uint field_10;
	uint field_14;
	uint field_18;
	void *blit_fast;
	uint field_20;
	uint field_24;
	uint field_28;
	uint field_2C;
	uint field_30;
	uint field_34;
	uint field_38;
	uint field_3C;
	uint field_40;
	uint field_44;
	uint field_48;
	uint field_4C;
	void *get_palette;
	void *get_pixelformat;
	void *get_surface_desc;
	uint field_5C;
	void *islost;
	void *lock;
	uint field_68;
	void *restore;
	uint field_70;
	uint field_74;
	uint field_78;
	uint field_7C;
	void *unlock;
	uint field_84;
	uint field_88;
	uint field_8C;
	void *get_dd_interface;
};

struct dddevice
{
	void *query_interface;
	void *addref;
	void *release;
	uint field_C;
	void *create_clipper;
	void *create_palette;
	void *create_surface;
	uint field_1C;
	uint field_20;
	uint field_24;
	uint field_28;
	void *get_caps;
	void *get_display_mode;
	uint field_34;
	uint field_38;
	uint field_3C;
	uint field_40;
	uint field_44;
	uint field_48;
	uint field_4C;
	void *set_coop_level;
	uint field_54;
	uint field_58;
	uint field_5C;
};

struct d3d2device
{
	void *query_interface;
	void *addref;
	void *release;
	void *get_caps;
	uint field_10;
	uint field_14;
	uint field_18;
	uint field_1C;
	uint field_20;
	uint field_24;
	uint field_28;
	uint field_2C;
	uint field_30;
	uint field_34;
	uint field_38;
	uint field_3C;
	uint junk[10];
	void *set_transform;
	uint field_6C;
	uint field_70;
	uint field_74;
	void *draw_indexed_primitive;
};

extern struct ddsurface *_fake_dd_back_surface;
extern struct ddsurface *_fake_dd_temp_surface;
extern struct ddsurface *_fake_dd_front_surface;
extern struct dddevice *_fake_dddevice;
extern struct d3d2device *_fake_d3d2device;
