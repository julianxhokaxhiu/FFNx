/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#include <sys/stat.h>
#include <stdio.h>
#include <direct.h>
#include "renderer.h"

#include "log.h"
#include "gl.h"
#include "macro.h"

#include <xxhash.h>

// TEMPORARY! WILL BE REMOVED AFTER MIGRATION.
#include <iostream>
#include <fstream>
#include <filesystem>
// -------------------------------------------

std::map<uint16_t, std::string> additional_textures = {
	{RendererTextureSlot::TEX_NML, "nml"},
	{RendererTextureSlot::TEX_PBR, "pbr"}
};

void make_path(char *name)
{
	char *next = name;

	while((next = strchr(next, '/')))
	{
		char tmp[128];

		while(next[0] == '/') next++;

		strncpy(tmp, name, next - name);
		tmp[next - name] = 0;

		_mkdir(tmp);
	}
}

void normalize_path(char *name)
{
	if (ff8)
	{
		int idx = 0;
		while (name[idx] != 0)
		{
			if (name[idx] == '\\') name[idx] = '/';
			idx++;
		}
	}
}

void save_texture(const void *data, uint32_t dataSize, uint32_t width, uint32_t height, uint32_t palette_index, const char *name, bool is_animated)
{
	char filename[sizeof(basedir) + 1024];
	struct stat dummy;
	uint64_t hash;

	if (is_animated)
	{
		char xxhash_filename[sizeof(basedir) + 1024];
		hash = XXH3_64bits(data, dataSize);
		_snprintf(xxhash_filename, sizeof(xxhash_filename), "%s/%s/%s_%02i_%llx.png", basedir, mod_path.c_str(), name, palette_index, hash);
	}
	else if (palette_index != uint32_t(-1))
	{
		_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i.png", basedir, mod_path.c_str(), name, palette_index);
	}
	else
	{
		_snprintf(filename, sizeof(filename), "%s/%s/%s.png", basedir, mod_path.c_str(), name);
	}

	normalize_path(filename);

	make_path(filename);

	if (stat(filename, &dummy) != 0)
	{
		if (!newRenderer.saveTexture(filename, width, height, data)) ffnx_error("Save texture failed for the file [ %s ].\n", filename);
	}
	else
		ffnx_warning("Save texture skipped because the file [ %s ] already exists.\n", filename);
}

uint32_t load_texture_helper(char* name, uint32_t* width, uint32_t* height, bool useLibPng, bool isSrgb)
{
	uint32_t ret = 0;

	normalize_path(name);

	if (useLibPng)
		ret = newRenderer.createTextureLibPng(name, width, height, isSrgb);
	else
	{
		uint32_t mipCount = 0;
		ret = newRenderer.createTexture(name, width, height, &mipCount, isSrgb);
	}

	if (ret)
	{
		if (trace_all || trace_loaders) ffnx_trace("Using texture: %s\n", name);
	}

	return ret;
}

uint32_t load_normal_texture(const void* data, uint32_t dataSize, const char* name, uint32_t palette_index, uint32_t* width, uint32_t* height, struct gl_texture_set* gl_set, std::string tex_path)
{
	uint32_t ret = 0;
	char filename[sizeof(basedir) + 1024]{ 0 };
	struct stat dummy;

	for (int idx = 0; idx < mod_ext.size(); idx++)
	{
		_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i.%s", basedir, tex_path.c_str(), name, palette_index, mod_ext[idx].c_str());

		ret = load_texture_helper(filename, width, height, mod_ext[idx] == "png", true);

		if(ret)
		{
			if (trace_all) ffnx_trace("Created external texture: %u from %s\n", ret, filename);
			break;
		}
	}

	if(!ret)
	{
		if(palette_index != 0)
		{
			if(trace_all || show_missing_textures) ffnx_info("No external texture found, falling back to palette 0\n");
			return load_normal_texture(data, dataSize, name, 0, width, height, gl_set, tex_path);
		}
		else
		{
			if(trace_all || show_missing_textures) ffnx_info("No external texture found, switching back to the internal one.\n");
			return 0;
		}
	}
	else
	{
		// Load additional textures
		for (const auto& it : additional_textures)
		{
			for (int idx = 0; idx < mod_ext.size(); idx++)
			{
				_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i_%s.%s", basedir, tex_path.c_str(), name, palette_index, it.second.c_str(), mod_ext[idx].c_str());

				if (stat(filename, &dummy) == 0)
				{
					if (gl_set->additional_textures.count(it.first)) newRenderer.deleteTexture(gl_set->additional_textures[it.first]);
					gl_set->additional_textures[it.first] = load_texture_helper(filename, width, height, mod_ext[idx] == "png", false);
					break;
				}
				else if (trace_all || show_missing_textures)
				{
					ffnx_trace("Could not find [ %s ].\n", filename);
				}
			}
		}
	}

	return ret;
}

uint32_t load_animated_texture(const void* data, uint32_t dataSize, const char* name, uint32_t palette_index, uint32_t* width, uint32_t* height, struct gl_texture_set* gl_set, std::string tex_path)
{
	uint32_t ret = 0;
	char filename[sizeof(basedir) + 1024]{ 0 };
	uint64_t hash = XXH3_64bits(data, dataSize);
	char texture_key[1024] { 0 };
	_snprintf(texture_key, sizeof(texture_key), "%s_%02i_%llx", name, palette_index, hash);

	// If texture has been cached, return immediately its handler
	if (gl_set->animated_textures.contains(texture_key))
	{
		return gl_set->animated_textures[texture_key];
	}

	// Check for animated texture with hash
	for (int idx = 0; idx < mod_ext.size(); idx++)
	{
		_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i_%llx.%s", basedir, tex_path.c_str(), name, palette_index, hash, mod_ext[idx].c_str());

		ret = load_texture_helper(filename, width, height, mod_ext[idx] == "png", true);

		if(ret)
		{
			if(trace_all) ffnx_trace("Created animated external texture: %u from %s\n", ret, filename);
			gl_set->animated_textures[texture_key] = ret;
			return ret;
		}

		if (trace_all || show_missing_textures) ffnx_trace("Could not find animated texture [ %s ].\n", filename);
	}

	// If animated texture not found, check for base texture
	for (int idx = 0; idx < mod_ext.size(); idx++)
	{
		_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i.%s", basedir, tex_path.c_str(), name, palette_index, mod_ext[idx].c_str());

		ret = load_texture_helper(filename, width, height, mod_ext[idx] == "png", true);

		if(ret)
		{
			if(trace_all) ffnx_trace("Created external texture: %u from %s\n", ret, filename);
			gl_set->animated_textures[texture_key] = ret;
			return ret;
		}

		if (trace_all || show_missing_textures) ffnx_trace("Could not find base texture [ %s ].\n", filename);
	}

	// Finally, if everything fails, return the one with palette index 0 or no texture
	if(palette_index != 0)
	{
		if(trace_all || show_missing_textures) ffnx_info("No external texture found, falling back to palette 0\n");
		ret = load_animated_texture(data, dataSize, name, 0, width, height, gl_set, tex_path);
		if(ret) gl_set->animated_textures[texture_key] = ret;
		return ret;
	}
	else
	{
		if(tex_path == mod_path) // Are we on the last lookup layer?
		{
			if(trace_all || show_missing_textures) ffnx_info("No external texture found, switching back to the internal one.\n");
			gl_set->animated_textures[texture_key] = 0; // short circuit and prevent further lookups
		}
		else if(trace_all || show_missing_textures) ffnx_info("No external texture found.\n");

		return 0;
	}

}

uint32_t load_texture(const void* data, uint32_t dataSize, const char* name, uint32_t palette_index, uint32_t* width, uint32_t* height, struct gl_texture_set* gl_set)
{
	uint32_t ret = 0;

	if(gl_set->is_animated)
	{
		if (!override_mod_path.empty())
			ret = load_animated_texture(data, dataSize, name, palette_index, width, height, gl_set, override_mod_path);

		if (ret == 0)
			ret = load_animated_texture(data, dataSize, name, palette_index, width, height, gl_set, mod_path);
	}
	else
	{
		if (!override_mod_path.empty())
			ret = load_normal_texture(data, dataSize, name, palette_index, width, height, gl_set, override_mod_path);

		if (ret == 0)
			ret = load_normal_texture(data, dataSize, name, palette_index, width, height, gl_set, mod_path);
	}

	return ret;
}
