/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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
#include "../utils.h"

#include "defs.h"

#include "external_mesh.h"

uint32_t get_frame_data_size(struct anim_header *anim_header)
{
	if(!anim_header) return 0;

	return (anim_header->num_bones * sizeof(vector3<float>) + sizeof(struct anim_frame_header)) * anim_header->num_frames;
}

// load .a file, save modpath name somewhere we can retrieve it later
struct anim_header *load_animation(struct file_context *file_context, char *filename)
{
	struct ff7_file *file = open_file(file_context, filename);
	struct anim_header *ret = NULL;
	uint32_t size;
	uint32_t i;
	uint32_t data_pointer;

	if(trace_all || trace_loaders)
	{
		if(file_context->use_lgp) ffnx_trace("reading animation file: %s/%s\n", lgp_names[file_context->lgp_num], filename);
		else ffnx_trace("reading animation file: %s\n", filename);
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

	data_pointer = (uint32_t)ret->frame_data;

	for(i = 0; i < ret->num_frames; i++)
	{
		ret->anim_frames[i].header = (anim_frame_header *)data_pointer;
		data_pointer += sizeof(struct anim_frame_header);
		ret->anim_frames[i].data = (vector3<float> *)data_pointer;
		data_pointer += sizeof(vector3<float>) * ret->num_bones;
	}

	ret->file.pc_name = make_pc_name(file_context, file, filename);

	close_file(file);
	return ret;

error:
	ff7_externals.destroy_animation(ret);
	close_file(file);
	return 0;
};

// load battle HRC file (does not save modpath name)
// A model's parts are loaded right after its skeleton: true once the magic.lgp model being loaded has its gltf part
static bool magic_model_has_gltf_part = false;
// The gltf of the battle scene being loaded (on its first part), which has a mesh for each of its pieces
static ExternalMeshInstance* battle_scene_character = nullptr;

struct battle_hrc_header *read_battle_hrc(uint32_t use_file_context, struct file_context *file_context, char *filename)
{
	struct battle_hrc_header *ret;
	struct battle_chdir_struc olddir;
	char hrc_filename[200];
	uint32_t size;

	if(use_file_context) ff7_externals.battle_context_chdir(file_context, &olddir);
	else ff7_externals.battle_regular_chdir(&olddir);

	ff7_externals.swap_extension("D", filename, hrc_filename);

	magic_model_has_gltf_part = false;
	battle_scene_character = nullptr;

	if(trace_all || trace_loaders)
	{
		if(file_context->use_lgp) ffnx_trace("reading battle hrc file: %s/%s\n", lgp_names[file_context->lgp_num], hrc_filename);
		else ffnx_trace("reading battle hrc file: %s\n", hrc_filename);
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

// load .p file, save modpath name somewhere we can retrieve it later
struct polygon_data *load_p_file(struct file_context *file_context, uint32_t create_lists, char *filename)
{
	struct polygon_data *ret = ff7_externals.create_polygon_data(false, 0);
	struct ff7_file *file = open_file(file_context, filename);

	if(trace_all || trace_loaders)
	{
		if(file_context->use_lgp) ffnx_trace("reading p file: %s/%s\n", lgp_names[file_context->lgp_num], filename);
		else ffnx_trace("reading p file: %s\n", filename);
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
	ret->pc_name = make_pc_name(file_context, file, filename);
	ret->field_64 = 0;
	ret->hundredsdata = 0;
	ret->groupdata = 0;
	ret->lists = 0;
	ret->boundingboxdata = 0;
	ret->normindextabledata = 0;

	if(ret->version != 1)
	{
		ffnx_error("invalid version in polygon file %s\n", filename);
		goto error;
	}

	if(ret->field_2C) ffnx_unexpected("oops, missed some .p data\n");

	if (enable_external_mesh)
	{
		// Battle models are asked for by name (CLOUD.P00) and stored in battle.lgp under another (rtam): the
		// file context's name mangler converts them, and gltf exports use the stored name (RTAM.gltf)
		bool is_battle = file_context->use_lgp && !_stricmp(lgp_names[file_context->lgp_num], "battle");
		std::string full_filename = filename;
		if (is_battle && file_context->name_mangler)
		{
			char mangled_name[MAX_PATH] = {};
			file_context->name_mangler(filename, mangled_name);
			if (mangled_name[0]) full_filename = mangled_name;
		}
		size_t lastindex = full_filename.find_last_of(".");
		std::string filename_no_ext = full_filename.substr(0, lastindex);
		std::transform(filename_no_ext.begin(), filename_no_ext.end(), filename_no_ext.begin(), [](unsigned char c) { return std::toupper(c); });

		// Summons (and other animated models in magic.lgp, e.g. LIMIT/CYVADAT for Shiva) are battle-format models whose
		// parts are named <model>.P00, .P01...: one gltf in mesh/magic/, named after the model (CYVADAT.gltf), carries
		// all of them (on the first part loaded; some models have no .P00)
		bool is_magic_model = false;
		if (!is_battle && file_context->use_lgp && !_stricmp(lgp_names[file_context->lgp_num], "magic"))
		{
			std::string ext = lastindex == std::string::npos ? "" : full_filename.substr(lastindex + 1);
			if (ext.size() == 3 && toupper(ext[0]) == 'P' && isdigit(ext[1]) && isdigit(ext[2]))
			{
				is_magic_model = true;
				size_t slash = filename_no_ext.find_last_of("/\\");
				if (slash != std::string::npos) filename_no_ext = filename_no_ext.substr(slash + 1);
			}
		}
		bool is_magic_model_first_part = is_magic_model && !magic_model_has_gltf_part;
		if (is_magic_model) magic_model_has_gltf_part = true;

		// Battle scenes (the arena: STAGEnn.D with parts .P00, .P01...) are exported whole into the gltf of their first
		// part, every piece a mesh named after its part (ONAM, ONAN...). Each piece draws its own mesh, when the game
		// draws that piece.
		bool is_battle_scene = is_battle && !_strnicmp(filename, "STAGE", 5);

		// World map models (Cloud, the chocobo, the Highwind...) come from world_us.lgp and have 3-letter names
		bool is_world = file_context->use_lgp && !_stricmp(lgp_names[file_context->lgp_num], "world");

		// Exports go in a folder named after the lgp the original comes from
		const char* mesh_folder = is_battle ? "battle" : is_magic_model ? "magic" : is_world ? "world" : "field";

		char file_path_gltf[MAX_PATH];
		sprintf(file_path_gltf, "%s/%s/%s/%s.gltf", basedir, external_mesh_path.data(), mesh_folder, filename_no_ext.data());

		char tex_path[MAX_PATH];
		sprintf(tex_path, "%s/%s/%s/textures/", basedir, external_mesh_path.data(), mesh_folder);

		ExternalMeshInstance* battleCharacter = is_battle ? getLastBattleCharacter() : nullptr;

		if (!fileExists(file_path_gltf) && battleCharacter && hasWeaponMesh(*battleCharacter->mesh, filename_no_ext))
		{
			// The weapon of the battle character just loaded: its gltf draws the matching weapon mesh, and the
			// game's own weapon model gets a hidden stand-in
			battleCharacter->equippedWeapon = filename_no_ext;

			auto weaponStandIn = new ExternalMeshInstance(battleCharacter->mesh);
			weaponStandIn->hidden = true;
			ret->field_48 = reinterpret_cast<vector3<float>*>(weaponStandIn);

			if(trace_all || trace_loaders) ffnx_trace("External mesh: weapon %s drawn by the battle model's gltf\n", filename_no_ext.c_str());
		}
		else if (is_magic_model && !is_magic_model_first_part)
		{
			// Drawn by the gltf on the model's first part
			ret->field_48 = nullptr;
		}
		else if (!fileExists(file_path_gltf) && is_battle_scene && battle_scene_character && hasMeshNamed(*battle_scene_character->mesh, filename_no_ext))
		{
			auto piece = new ExternalMeshInstance(battle_scene_character->mesh);
			piece->onlyMesh = filename_no_ext;
			ret->field_48 = reinterpret_cast<vector3<float>*>(piece);

			if(trace_all || trace_loaders) ffnx_trace("External mesh: %s drawn from the battle scene's gltf\n", filename_no_ext.c_str());
		}
		else if (!fileExists(file_path_gltf))
		{
			if(trace_all || trace_loaders) ffnx_trace("External mesh: %s not found, using original model\n", file_path_gltf);

			ret->field_48 = nullptr;
		}
		else
		{
			// Loaded once and shared by every character using it (and kept for later fields)
			auto loadStart = std::chrono::steady_clock::now();
			const char* source = "";
			auto sharedMesh = acquireFieldExternalMesh(file_path_gltf, tex_path, &source);
			float loadMilliseconds = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - loadStart).count();

			if (sharedMesh)
			{
				if(trace_all || trace_loaders) ffnx_trace("External mesh: %s %s in %.0f ms\n", source, file_path_gltf, loadMilliseconds);

				auto character = new ExternalMeshInstance(sharedMesh);
				ret->field_48 = reinterpret_cast<vector3<float>*>(character);
				if (is_battle_scene)
				{
					battle_scene_character = character;
					if (hasMeshNamed(*sharedMesh, filename_no_ext)) character->onlyMesh = filename_no_ext;
				}
				else if (is_battle) setLastBattleCharacter(character);
			}
			else
			{
				ffnx_error("External mesh: failed to load %s, using original model\n", file_path_gltf);

				ret->field_48 = nullptr;
			}
		}
	}
	else
	{
		ret->field_48 = (vector3<float>*)common_externals.alloc_read_file(sizeof(*ret->field_48), ret->field_14, (struct file *)file);
	}

	ret->vertdata = (vector3<float>*)common_externals.alloc_read_file(sizeof(*ret->vertdata), ret->numverts, (struct file *)file);
	// The world map scales a model's parts after loading them: note the original, to measure that scale later
	if(enable_external_mesh && ret->field_48 && file_context->use_lgp && !_stricmp(lgp_names[file_context->lgp_num], "world"))
		reinterpret_cast<ExternalMeshInstance*>(ret->field_48)->noteGameVertices(&ret->vertdata, ret->numverts);
	ret->normaldata = (vector3<float>*)common_externals.alloc_read_file(sizeof(*ret->normaldata), ret->numnormals, (struct file *)file);
	ret->texcoorddata = (struct texcoords*)common_externals.alloc_read_file(sizeof(*ret->texcoorddata), ret->numtexcoords, (struct file *)file);
	ret->vertexcolordata = (uint32_t*)common_externals.alloc_read_file(sizeof(*ret->vertexcolordata), ret->numvertcolors, (struct file *)file);
	ret->polycolordata = (uint32_t*)common_externals.alloc_read_file(sizeof(*ret->polycolordata), ret->numpolys, (struct file *)file);
	ret->edgedata = (struct p_edge*)common_externals.alloc_read_file(sizeof(*ret->edgedata), ret->numedges, (struct file *)file);
	ret->polydata = (struct p_polygon*)common_externals.alloc_read_file(sizeof(*ret->polydata), ret->numpolys, (struct file *)file);
	external_free(common_externals.alloc_read_file(sizeof(struct p_polygon), ret->field_28, (struct file *)file));
	ret->field_64 = common_externals.alloc_read_file(3, ret->field_2C, (struct file *)file);
	ret->hundredsdata = (struct p_hundred*)common_externals.alloc_read_file(sizeof(*ret->hundredsdata), ret->numhundreds, (struct file *)file);
	ret->groupdata = (struct p_group*)common_externals.alloc_read_file(sizeof(*ret->groupdata), ret->numgroups, (struct file *)file);
	ret->boundingboxdata = (struct boundingbox*)common_externals.alloc_read_file(sizeof(*ret->boundingboxdata), ret->numboundingboxes, (struct file *)file);
	if(ret->has_normindextable) ret->normindextabledata = (uint32_t*)common_externals.alloc_read_file(sizeof(*ret->normindextabledata), ret->numverts, (struct file *)file);

	if(create_lists) ff7_externals.create_polygon_lists(ret);

	close_file(file);
	return ret;

error:
	ff7_externals.free_polygon_data(ret);
	close_file(file);
	return 0;
}

void free_polygon_data(struct polygon_data *ret)
{
	if (!ret) return;
	if (enable_external_mesh && ret->field_48)
	{
		// Frees the character; its model stays cached for later fields (up to the cache's budget)
		releaseFieldExternalMesh(reinterpret_cast<ExternalMeshInstance*>(ret->field_48));
		ret->field_48 = nullptr;
	}

	ff7_externals.free_polygon_data_impl(1, ret);
}

void destroy_tex_header(struct ff7_tex_header *tex_header)
{
	if(!tex_header) return;

	if((uint32_t)tex_header->file.pc_name > 32) external_free(tex_header->file.pc_name);

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
			ret->tex_format.palette_data = (uint32_t*)common_externals.alloc_read_file(4, ret->tex_format.palette_size, (struct file *)file);
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

	ret->file.pc_name = make_pc_name(file_context, file, filename);

	close_file(file);
	return ret;

error:
	destroy_tex_header(ret);
	close_file(file);
	return 0;
}

