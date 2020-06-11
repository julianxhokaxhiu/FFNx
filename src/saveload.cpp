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

#include <sys/stat.h>
#include <stdio.h>
#include <direct.h>
#include "renderer.h"

#include "log.h"
#include "gl.h"
#include "macro.h"

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

uint save_texture(void *data, uint width, uint height, uint palette_index, char *name)
{
	char filename[sizeof(basedir) + 1024];
	struct stat dummy;

	_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i.png", basedir, mod_path, name, palette_index);

	make_path(filename);

	if(stat(filename, &dummy)) return newRenderer.saveTexture(filename, width, height, data);
	else return true;
}

uint load_texture_helper(char* name, uint* width, uint* height, bool useLibPng)
{
	uint ret = 0;

	if (useLibPng)
		ret = newRenderer.createTextureLibPng(name, width, height);
	else
		ret = newRenderer.createTexture(name, width, height);

	if (ret > 0)
	{
		if (trace_all || trace_loaders) trace("Using texture: %s\n", name);
	}

	return ret;
}

uint load_texture(char *name, uint palette_index, uint *width, uint *height)
{
	uint ret = 0;
	char filename[sizeof(basedir) + 1024];
	
	const std::vector<std::string> exts =
	{
		"dds",
		"ktx",
		"pvr",
		"exr",
		"tga",
		"jpg",
		"png",
	};
	int extIdxFound = -1;
	struct stat dummy;

	for (int idx = 0; idx < exts.size(); idx++)
	{
		_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i.%s", basedir, mod_path, name, palette_index, exts[idx].c_str());

		if (stat(filename, &dummy) == 0)
		{
			extIdxFound = idx;

			break;
		}
		else
		{
			if (trace_all || show_missing_textures) info("File not found: %s\n", filename);
		}
	}

	
	if (extIdxFound != -1) ret = load_texture_helper(filename, width, height, extIdxFound == (exts.size() - 1)); // PNG files needs a special treatment...

	if(!ret)
	{
		if(palette_index != 0)
		{
			if(trace_all || show_missing_textures) info("No external texture found, falling back to palette 0\n", basedir, mod_path, name, palette_index);
			return load_texture(name, 0, width, height);
		}
		else
		{
			if(trace_all || show_missing_textures) info("No external texture found, switching back to the internal one.\n", basedir, mod_path, name, palette_index);
			return 0;
		}
	}

	if(trace_all) trace("Created texture: %i\n", ret);

	return ret;
}
