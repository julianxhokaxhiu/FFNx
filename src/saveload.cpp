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

uint load_texture_helper(char* name, uint* width, uint* height, uint use_compression)
{
	uint ret = 0;
	struct stat dummy;

	if (stat(name, &dummy) == 0)
	{
		ret = newRenderer.createTexture(name, width, height);

		if (ret > 0)
		{
			if (trace_all || trace_loaders) trace("Using texture: %s\n", name);
		}
	}

	return ret;
}

uint load_texture(char *name, uint palette_index, uint *width, uint *height, uint use_compression)
{
	char filename[sizeof(basedir) + 1024];
	uint ret;

	_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i.dds", basedir, mod_path, name, palette_index);

	// Try loading DDS
	ret = load_texture_helper(filename, width, height, use_compression);
	
	// If not successfull fallback to PNG
	if (!ret)
	{
		_snprintf(filename, sizeof(filename), "%s/%s/%s_%02i.png", basedir, mod_path, name, palette_index);

		ret = load_texture_helper(filename, width, height, use_compression);
	}

	if(!ret)
	{
		if(palette_index != 0)
		{
			if(show_missing_textures) info("tried to load %s/%s/%s_%02i.(dds|png), falling back to palette 0\n", basedir, mod_path, name, palette_index);
			return load_texture(name, 0, width, height, use_compression);
		}
		else
		{
			if(show_missing_textures) info("tried to load %s/%s/%s_%02i.(dds|png), failed\n", basedir, mod_path, name, palette_index);
			return 0;
		}
	}

	if(trace_all) trace("Created texture: %i\n", ret);

	return ret;
}
