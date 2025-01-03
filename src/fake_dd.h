/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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

#pragma once

#include <stdint.h>

struct ddsurface
{
	void *query_interface;
	void *addref;
	void *release;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	void *blit_fast;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C;
	void *get_palette;
	void *get_pixelformat;
	void *get_surface_desc;
	uint32_t field_5C;
	void *islost;
	void *lock;
	uint32_t field_68;
	void *restore;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t field_7C;
	void *unlock;
	uint32_t field_84;
	uint32_t field_88;
	uint32_t field_8C;
	void *get_dd_interface;
};

struct dddevice
{
	void *query_interface;
	void *addref;
	void *release;
	uint32_t field_C;
	void *create_clipper;
	void *create_palette;
	void *create_surface;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	void *get_caps;
	void *get_display_mode;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C;
	void *set_coop_level;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t field_5C;
};

struct d3d2device
{
	void *query_interface;
	void *addref;
	void *release;
	void *get_caps;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t junk[10];
	void *set_transform;
	uint32_t field_6C;
	uint32_t field_70;
	uint32_t field_74;
	void *draw_indexed_primitive;
};

extern struct ddsurface *_fake_dd_back_surface;
extern struct ddsurface *_fake_dd_temp_surface;
extern struct ddsurface *_fake_dd_front_surface;
extern struct dddevice *_fake_dddevice;
extern struct d3d2device *_fake_d3d2device;
