/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
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

#include "../ff7.h"
#include "../log.h"

/*
 * This file contains the changes necessary to support subtractive and 25%
 * blending modes in field backgrounds. Texture pages where these blending
 * modes are used are duplicated and the tile data modified to point to these
 * new pages which have the correct blending mode set.
 */

// helper function initializes page dst, copies texture from src and applies
// blend_mode
void field_load_textures_helper(struct ff7_game_obj *game_object, struct struc_3 *struc_3, uint src, uint dst, uint blend_mode)
{
	struct ff7_tex_header *tex_header;

	ff7_externals.make_struc3(blend_mode, struc_3);

	tex_header = (struct ff7_tex_header *)common_externals.create_tex_header();

	ff7_externals.field_layers[dst]->tex_header = tex_header;

	if(ff7_externals.field_layers[src]->type == 1) ff7_externals.make_field_tex_header_pal(tex_header);
	if(ff7_externals.field_layers[src]->type == 2) ff7_externals.make_field_tex_header(tex_header);

	struc_3->tex_header = tex_header;

	if(src != dst)
	{
		ff7_externals.field_layers[dst]->image_data = external_malloc(256 * 256);
		memcpy(ff7_externals.field_layers[dst]->image_data, ff7_externals.field_layers[src]->image_data, 256 * 256);
	}
	
	tex_header->image_data = (unsigned char*)ff7_externals.field_layers[dst]->image_data;

	tex_header->file.pc_name = (char*)external_malloc(1024);
	sprintf(tex_header->file.pc_name, "field/%s/%s_%02i", strchr(ff7_externals.field_file_name, '\\') + 1, strchr(ff7_externals.field_file_name, '\\') + 1, src);

	ff7_externals.field_layers[dst]->graphics_object = ff7_externals._load_texture(1, PT_S2D, struc_3, 0, game_object->dx_sfx_something);
	ff7_externals.field_layers[dst]->present = true;
}

void field_load_textures(struct ff7_game_obj *game_object, struct struc_3 *struc_3)
{
	uint i;

	ff7_externals.field_convert_type2_layers();

	for(i = 0; i < 29; i++)
	{
		uint blend_mode = 4;

		if(!ff7_externals.field_layers[i]->present) continue;

		if(ff7_externals.field_layers[i]->type == 1)
		{
			if(i >= 24) blend_mode = 0;
			else if(i >= 15) blend_mode = 1;
		}
		else if(ff7_externals.field_layers[i]->type == 2)
		{
			if(i >= 40) blend_mode = 0;
			else if(i >= 33) blend_mode = 1;
		}
		else glitch("unknown field layer type %i\n", ff7_externals.field_layers[i]->type);

		field_load_textures_helper(game_object, struc_3, i, i, blend_mode);

		// these magic numbers have been gleaned from original source data
		// the missing blend modes in question are used in exactly these pages
		// and copying them in this manner does not risk overwriting any other
		// data
		if(i >= 15 && i <= 18 && ff7_externals.field_layers[i]->type == 1) field_load_textures_helper(game_object, struc_3, i, i + 14, 2);
		if(i >= 15 && i <= 20 && ff7_externals.field_layers[i]->type == 1) field_load_textures_helper(game_object, struc_3, i, i + 18, 3);
	}

	*ff7_externals.layer2_end_page += 18;
}

void field_layer2_pick_tiles(short x_offset, short y_offset)
{
	int x_add = (320 - x_offset) * 2;
	int y_add = ((ff7_center_fields ? 232 : 224) - y_offset) * 2;
	uint i;
	struct field_tile *layer2_tiles = *ff7_externals.field_layer2_tiles;

	if(*ff7_externals.field_special_y_offset > 0 && y_offset <= 8) y_offset -= *ff7_externals.field_special_y_offset * 2;

	for(i = 0; i < *ff7_externals.field_layer2_tiles_num; i++)
	{
		uint tile_index = (*ff7_externals.field_layer2_palette_sort)[i] & 0xFFFF;
		uint page;
		int x;
		int y;

		if(layer2_tiles[tile_index].anim_group && !(ff7_externals.field_anim_state[layer2_tiles[tile_index].anim_group] & layer2_tiles[tile_index].anim_bitmask)) continue;

		layer2_tiles[tile_index].field_1040 = 1;

		x = layer2_tiles[tile_index].x * 2 + x_add;
		y = layer2_tiles[tile_index].y * 2 + y_add;

		if(layer2_tiles[tile_index].use_fx_page) page = layer2_tiles[tile_index].fx_page;
		else page = layer2_tiles[tile_index].page;

		if(layer2_tiles[tile_index].use_fx_page && layer2_tiles[tile_index].blend_mode == 2) page += 14;
		if(layer2_tiles[tile_index].use_fx_page && layer2_tiles[tile_index].blend_mode == 3) page += 18;

		ff7_externals.add_page_tile((float)x, (float)y, layer2_tiles[tile_index].z, layer2_tiles[tile_index].u, layer2_tiles[tile_index].v, layer2_tiles[tile_index].palette_index, page);
	}
}
