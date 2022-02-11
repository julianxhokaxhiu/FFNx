/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Cosmos                                             //
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

#include "discohash.h"
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

		// TEMPORARY! WILL BE REMOVED AFTER MIGRATION.
		hash = 0;
		char discohash_filename[sizeof(basedir) + 1024];
		BEBB4185_64(data, dataSize, 0, &hash);
		_snprintf(discohash_filename, sizeof(discohash_filename), "%s/%s/%s_%02i_%llx.png", basedir, mod_path.c_str(), name, palette_index, hash);

		if (use_animated_textures_v2)
		{
			strcpy_s(filename, sizeof(filename), xxhash_filename);

			// This part will help modders in renaming old animated textures to the new hash
			char batch_filename[1024];
			std::ofstream batch_file;
			std::string banner;
			char dirname[1024];

			_splitpath(name, NULL, dirname, NULL, NULL);
			_snprintf(batch_filename, sizeof(batch_filename), "%s/%s/%s/_upgrade_to_animated_textures_v2.bat", basedir, mod_path.c_str(), dirname);

			std::filesystem::path f{ batch_filename };
			if (!std::filesystem::exists(f)) {
				banner =
					"@echo off\n"
					"Rem This file will help you migrating old animated textures files to v2.\n";
			}

			batch_file.open(batch_filename, std::ios_base::app);
			batch_file << banner << R"(rename ")" << discohash_filename << R"(" ")" << xxhash_filename << R"(")" << std::endl;
			// -------------------------------------------
		}
		else
			strcpy_s(filename, sizeof(filename), discohash_filename);
		// -------------------------------------------
	}
	else
		_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i.png", basedir, mod_path.c_str(), name, palette_index);

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

uint32_t load_texture(const void* data, uint32_t dataSize, const char* name, uint32_t palette_index, uint32_t* width, uint32_t* height, struct gl_texture_set* gl_set)
{
	uint32_t ret = 0;
	char filename[sizeof(basedir) + 1024]{ 0 };
	uint64_t hash;
	bool is_animated = gl_set->is_animated;

	struct stat dummy;

	if (is_animated) {
		if (use_animated_textures_v2)
			hash = XXH3_64bits(data, dataSize);
		else
			BEBB4185_64(data, dataSize, 0, &hash);
	}

	for (int idx = 0; idx < mod_ext.size(); idx++)
	{
		if (is_animated)
		{
			_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i_%llx.%s", basedir, mod_path.c_str(), name, palette_index, hash, mod_ext[idx].c_str());

			if (stat(filename, &dummy) != 0)
			{
				if (trace_all || show_missing_textures) ffnx_trace("Could not find animated texture [ %s ].\n", filename);

				_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i.%s", basedir, mod_path.c_str(), name, palette_index, mod_ext[idx].c_str());
			}
		}
		else
			_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i.%s", basedir, mod_path.c_str(), name, palette_index, mod_ext[idx].c_str());

		if (stat(filename, &dummy) == 0)
		{
			if (is_animated && gl_set->animated_textures.count(filename))
			{
				// We already know the texture, return its handler and move on
				ret = gl_set->animated_textures[filename];
				break;
			}

			ret = load_texture_helper(filename, width, height, mod_ext[idx] == "png", true);

			if (trace_all)
			{
				if (ret) ffnx_trace("Created external texture: %u from %s\n", ret, filename);
				else ffnx_warning("External texture [%s] found but not loaded due to memory limitations.\n", filename);
			}

			if (is_animated && ret) gl_set->animated_textures[filename] = ret;

			break;
		}
		else if (trace_all || show_missing_textures)
		{
			ffnx_trace("Could not find [ %s ].\n", filename);
		}
	}

	if(!ret)
	{
		if(palette_index != 0)
		{
			if(trace_all || show_missing_textures) ffnx_info("No external texture found, falling back to palette 0\n", basedir, mod_path.c_str(), name, palette_index);
			return load_texture(data, dataSize, name, 0, width, height, gl_set);
		}
		else
		{
			if(trace_all || show_missing_textures) ffnx_info("No external texture found, switching back to the internal one.\n", basedir, mod_path.c_str(), name, palette_index);
			return 0;
		}
	}
	else
	{
		if (!is_animated)
		{
			// Load additional textures
			for (const auto& it : additional_textures)
			{
				for (int idx = 0; idx < mod_ext.size(); idx++)
				{
					_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i_%s.%s", basedir, mod_path.c_str(), name, palette_index, it.second.c_str(), mod_ext[idx].c_str());

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
	}

	return ret;
}
