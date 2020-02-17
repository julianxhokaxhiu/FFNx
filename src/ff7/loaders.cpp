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
 * ff7/loaders.c - replacement routines for FF7 file loaders
 */

#include "../types.h"
#include "../common.h"
#include "../ff7.h"
#include "../globals.h"
#include "../log.h"

#include "defs.h"

uint get_frame_data_size(struct anim_header *anim_header)
{
	if(!anim_header) return 0;

	return (anim_header->num_bones * sizeof(struct point3d) + sizeof(struct anim_frame_header)) * anim_header->num_frames;
}

// load .a file, save modpath name somewhere we can retrieve it later (unused)
struct anim_header *load_animation(struct file_context *file_context, char *filename)
{
	struct ff7_file *file = open_file(file_context, filename);
	struct anim_header *ret = NULL;
	uint size;
	uint i;
	uint data_pointer;

	if(trace_all || trace_loaders)
	{
		if(file_context->use_lgp) trace("reading animation file: %s/%s\n", lgp_names[file_context->lgp_num], filename);
		else trace("reading animation file: %s\n", filename);
	}

	if(!file) goto error;

	ret = (anim_header*)common_externals.alloc_read_file(sizeof(*ret), 1, (struct file *)file);

	if(!ret) goto error;
	if(ret->version.version != 1) goto error;

	ret->use_matrix_array = false;
	ret->matrix_array = 0;
	ret->current_matrix_array = 0;

	size = get_frame_data_size(ret);
	if(!size) goto error;

	ret->frame_data = common_externals.alloc_read_file(size, 1, (struct file *)file);
	if(!ret->frame_data) goto error;

	ret->anim_frames = (anim_frame*)external_calloc(sizeof(struct anim_frame), ret->num_frames);

	data_pointer = (uint)ret->frame_data;

	for(i = 0; i < ret->num_frames; i++)
	{
		ret->anim_frames[i].header = (anim_frame_header *)data_pointer;
		data_pointer += sizeof(struct anim_frame_header);
		ret->anim_frames[i].data = (struct point3d *)data_pointer;
		data_pointer += sizeof(struct point3d) * ret->num_bones;
	}

	ret->file.pc_name = make_pc_name(file_context, file, filename, false);

	close_file(file);
	return ret;

error:
	ff7_externals.destroy_animation(ret);
	close_file(file);
	return 0;
};

// load battle HRC file (does not save modpath name)
struct battle_hrc_header *read_battle_hrc(uint use_file_context, struct file_context *file_context, char *filename)
{
	struct battle_hrc_header *ret;
	struct battle_chdir_struc olddir;
	char hrc_filename[200];
	uint size;

	if(use_file_context) ff7_externals.battle_context_chdir(file_context, &olddir);
	else ff7_externals.battle_regular_chdir(&olddir);

	ff7_externals.swap_extension("D", filename, hrc_filename);

	if(trace_all || trace_loaders)
	{
		if(file_context->use_lgp) trace("reading battle hrc file: %s/%s\n", lgp_names[file_context->lgp_num], hrc_filename);
		else trace("reading battle hrc file: %s\n", hrc_filename);
	}

	ret = (battle_hrc_header*)common_externals.alloc_get_file(file_context, &size, hrc_filename);

	if(size < sizeof(*ret))
	{
		ff7_externals.destroy_battle_hrc(false, ret);
		return 0;
	}

	ret->bone_data = 0;

	if(ret->bones > 0) ret->bone_data = (struct battle_hrc_bone *)&ret[1];

	if(use_file_context) ff7_externals.battle_context_olddir(file_context, &olddir);
	else ff7_externals.battle_regular_olddir(&olddir);

	return ret;
}

// load .p file, save modpath name somewhere we can retrieve it later (unused)
struct polygon_data *load_p_file(struct file_context *file_context, uint create_lists, char *filename)
{
	struct polygon_data *ret = ff7_externals.create_polygon_data(false, 0);
	struct ff7_file *file = open_file(file_context, filename);

	if(trace_all || trace_loaders)
	{
		if(file_context->use_lgp) trace("reading p file: %s/%s\n", lgp_names[file_context->lgp_num], filename);
		else trace("reading p file: %s\n", filename);
	}

	if(!file) goto error;
	if(!read_file(sizeof(*ret), ret, file)) goto error;

	ret->vertdata = 0;
	ret->normaldata = 0;
	ret->field_48 = 0;
	ret->texcoorddata = 0;
	ret->vertexcolordata = 0;
	ret->polycolordata = 0;
	ret->edgedata = 0;
	ret->polydata = 0;
	ret->pc_name = make_pc_name(file_context, file, filename, true);
	ret->field_64 = 0;
	ret->hundredsdata = 0;
	ret->groupdata = 0;
	ret->lists = 0;
	ret->boundingboxdata = 0;
	ret->normindextabledata = 0;

	if(ret->version != 1)
	{
		error("invalid version in polygon file %s\n", filename);
		goto error;
	}

	if(ret->field_2C) unexpected("oops, missed some .p data\n");

	ret->vertdata = (struct point3d*)common_externals.alloc_read_file(sizeof(*ret->vertdata), ret->numverts, (struct file *)file);
	ret->normaldata = (struct point3d*)common_externals.alloc_read_file(sizeof(*ret->normaldata), ret->numnormals, (struct file *)file);
	ret->field_48 = (struct point3d*)common_externals.alloc_read_file(sizeof(*ret->field_48), ret->field_14, (struct file *)file);
	ret->texcoorddata = (struct texcoords*)common_externals.alloc_read_file(sizeof(*ret->texcoorddata), ret->numtexcoords, (struct file *)file);
	ret->vertexcolordata = (uint*)common_externals.alloc_read_file(sizeof(*ret->vertexcolordata), ret->numvertcolors, (struct file *)file);
	ret->polycolordata = (uint*)common_externals.alloc_read_file(sizeof(*ret->polycolordata), ret->numpolys, (struct file *)file);
	ret->edgedata = (struct p_edge*)common_externals.alloc_read_file(sizeof(*ret->edgedata), ret->numedges, (struct file *)file);
	ret->polydata = (struct p_polygon*)common_externals.alloc_read_file(sizeof(*ret->polydata), ret->numpolys, (struct file *)file);
	external_free(common_externals.alloc_read_file(sizeof(struct p_polygon), ret->field_28, (struct file *)file));
	ret->field_64 = common_externals.alloc_read_file(3, ret->field_2C, (struct file *)file);
	ret->hundredsdata = (struct p_hundred*)common_externals.alloc_read_file(sizeof(*ret->hundredsdata), ret->numhundreds, (struct file *)file);
	ret->groupdata = (struct p_group*)common_externals.alloc_read_file(sizeof(*ret->groupdata), ret->numgroups, (struct file *)file);
	ret->boundingboxdata = (struct boundingbox*)common_externals.alloc_read_file(sizeof(*ret->boundingboxdata), ret->numboundingboxes, (struct file *)file);
	if(ret->has_normindextable) ret->normindextabledata = (uint*)common_externals.alloc_read_file(sizeof(*ret->normindextabledata), ret->numverts, (struct file *)file);

	if(create_lists) ff7_externals.create_polygon_lists(ret);

	close_file(file);
	return ret;

error:
	ff7_externals.free_polygon_data(ret);
	close_file(file);
	return 0;
}

void destroy_tex_header(struct ff7_tex_header *tex_header)
{
	if(!tex_header) return;

	if((uint)tex_header->file.pc_name > 32) driver_free(tex_header->file.pc_name);

	external_free(tex_header->old_palette_data);
	external_free(tex_header->palette_colorkey);
	external_free(tex_header->tex_format.palette_data);
	external_free(tex_header->image_data);

	external_free(tex_header);
}

// load .tex file, save modpath name somewhere we can retrieve it later
struct ff7_tex_header *load_tex_file(struct file_context *file_context, char *filename)
{
	struct ff7_tex_header *ret = (struct ff7_tex_header *)common_externals.create_tex_header();
	struct ff7_file *file = open_file(file_context, filename);

	if(!file) goto error;
	if(!read_file(sizeof(*ret), ret, file)) goto error;

	ret->image_data = 0;
	ret->old_palette_data = 0;
	ret->palette_colorkey = 0;
	ret->tex_format.palette_data = 0;

	if(ret->version != 1) goto error;
	else
	{
		if(ret->tex_format.use_palette)
		{
			ret->tex_format.palette_data = (uint*)common_externals.alloc_read_file(4, ret->tex_format.palette_size, (struct file *)file);
			if(!ret->tex_format.palette_data) goto error;
		}

		ret->image_data = (unsigned char*)common_externals.alloc_read_file(ret->tex_format.bytesperpixel, ret->tex_format.width * ret->tex_format.height, (struct file *)file);
		if(!ret->image_data) goto error;

		if(ret->use_palette_colorkey)
		{
			ret->palette_colorkey = (char*)common_externals.alloc_read_file(1, ret->palettes, (struct file *)file);
			if(!ret->palette_colorkey) goto error;
		}
	}

	ret->file.pc_name = make_pc_name(file_context, file, filename, false);

	close_file(file);
	return ret;

error:
	destroy_tex_header(ret);
	close_file(file);
	return 0;
}
