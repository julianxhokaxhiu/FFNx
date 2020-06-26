/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#include "../renderer.h"

#include "../log.h"
#include "../gl.h"
#include "../macro.h"
#include "../saveload.h"

// check to make sure we can actually load a given texture
bool gl_check_texture_dimensions(uint width, uint height, char *source)
{
	if (width > max_texture_size || height > max_texture_size) {
		error("Texture dimensions exceed max texture size, will not be able to load %s\n", source);
		return false;
	}
	else
		return true;
}

// create a simple texture from pixel data, if the size parameter is used it
// will be treated as a compressed texture
uint gl_create_texture(void *data, uint width, uint height, uint format, uint internalformat, uint size, uint generate_mipmaps)
{
	// TODO: HANDLE COMPRESSION
	uint texture = newRenderer.createTexture(
		(uint8_t*)data,
		width,
		height,
		0,
		RendererTextureType(format),
		generate_mipmaps
	);

	return texture;
}

void *gl_get_pixel_buffer(uint size)
{
	return driver_malloc(size);
}

uint gl_commit_pixel_buffer_generic(void *data, uint width, uint height, uint format, uint internalformat, uint size, uint generate_mipmaps)
{
	return gl_create_texture(data, width, height, format, internalformat, size, generate_mipmaps);;
}

uint gl_commit_pixel_buffer(void *data, uint width, uint height, uint format, uint generate_mipmaps)
{
	return gl_commit_pixel_buffer_generic(data, width, height, format, RendererInternalType::RGBA8, 0, generate_mipmaps);
}

// apply OpenGL texture for a certain palette in a texture set, possibly
// replacing an existing texture which will then be unloaded
void gl_replace_texture(struct texture_set *texture_set, uint palette_index, uint new_texture)
{
	VOBJ(texture_set, texture_set, texture_set);

	if(VREF(texture_set, texturehandle[palette_index]) != 0)
	{
		if(VREF(texture_set, ogl.external)) glitch("oops, may have messed up an external texture\n");
		newRenderer.deleteTexture(VREF(texture_set, texturehandle[palette_index]));
	}

	VRASS(texture_set, texturehandle[palette_index], new_texture);
}

// upload texture for a texture set from raw pixel data
void gl_upload_texture(struct texture_set *texture_set, uint palette_index, void *image_data, uint format)
{
	uint texture;
	uint w, h;
	VOBJ(texture_set, texture_set, texture_set);
	VOBJ(tex_header, tex_header, VREF(texture_set, tex_header));

	if(VREF(texture_set, ogl.external))
	{
		w = VREF(texture_set, ogl.width);
		h = VREF(texture_set, ogl.height);
	}
	else
	{
		w = VREF(tex_header, version) == FB_TEX_VERSION ? VREF(tex_header, fb_tex.w) : VREF(tex_header, tex_format.width);
		h = VREF(tex_header, version) == FB_TEX_VERSION ? VREF(tex_header, fb_tex.h) : VREF(tex_header, tex_format.height);
	}

	gl_check_texture_dimensions(w, h, "unknown");

	texture = gl_commit_pixel_buffer(image_data, w, h, format, false);

	gl_replace_texture(texture_set, palette_index, texture);
}

// prepare texture set for rendering
void gl_bind_texture_set(struct texture_set *_texture_set)
{
	VOBJ(texture_set, texture_set, _texture_set);

	if(VPTR(texture_set))
	{
		VOBJ(tex_header, tex_header, VREF(texture_set, tex_header));

		gl_set_texture(VREF(texture_set, texturehandle[VREF(tex_header, palette_index)]));

		if(VREF(tex_header, version) == FB_TEX_VERSION) current_state.fb_texture = true;
		else current_state.fb_texture = false;
	}
	else gl_set_texture(0);

	current_state.texture_set = VPTRCAST(texture_set, texture_set);
}

// prepare an OpenGL texture for rendering, passing zero to this function will
// disable texturing entirely
void gl_set_texture(uint texture)
{
	if(trace_all) trace("gl_set_texture: set texture %i\n", texture);

	if (texture) {
		newRenderer.useTexture(texture);
		newRenderer.isTexture(true);
	}
	else
	{
		newRenderer.useTexture(0);
		newRenderer.isTexture(false);
	}

	current_state.texture_handle = texture;
	current_state.texture_set = 0;
}
