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
 * png.c - load/save routines for PNG images
 */

#include <stdio.h>
#include <libpng16/png.h>
#include <zlib.h>
#include <direct.h>

#include "types.h"
#include "log.h"
#include "globals.h"
#include "gl.h"

void _png_error(png_structp png_ptr, const char *error)
{
	error("libpng error: %s\n", error);
}

void _png_warning(png_structp png_ptr, const char *warning)
{
	info("libpng warning: %s\n", warning);
}

uint write_png(char *filename, uint width, uint height, char *data)
{
	uint y;
	png_byte **rowptrs;
	FILE *f;
	png_infop info_ptr;
	png_structp png_ptr;

	if(fopen_s(&f, filename, "wb"))
	{
		error("couldn't open file %s for writing: %s", filename, _strerror(NULL));
		return false;
	}
	
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)0, _png_error, _png_warning);
	
	if(!png_ptr)
	{
		fclose(f);
		return false;
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	
	if(!info_ptr)
	{
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		fclose(f);
		return false;
	}

	if(setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(f);
		return false;
	}
	
	png_init_io(png_ptr, f);
	
	png_set_compression_level(png_ptr, Z_BEST_SPEED);
	
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	
	rowptrs = (png_byte**)driver_malloc(height * 4);
	
	for(y = 0; y < height; y++) rowptrs[y] = (png_byte*)&data[y * width * 4];
	
	png_set_rows(png_ptr, info_ptr, rowptrs);
	
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR, NULL);
	
	png_destroy_write_struct(&png_ptr, &info_ptr);
	
	driver_free(rowptrs);

	fclose(f);

	return true;
}

uint *read_png(char *filename, uint *_width, uint *_height)
{
	png_bytepp rowptrs;
	FILE *f;
	png_infop info_ptr;
	png_structp png_ptr;
	uint width;
	uint height;
	uint *data;
	uint y;
	uint color_type;

	if(fopen_s(&f, filename, "rb")) return 0;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)0, _png_error, _png_warning);

	if(!png_ptr)
	{
		fclose(f);
		return 0;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if(!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(f);
		return 0;
	}

	if(setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(f);
		return 0;
	}

	png_init_io(png_ptr, f);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR, NULL);

	if(png_get_bit_depth(png_ptr, info_ptr) != 8)
	{
		error("invalid bitdepth\n");
		return 0;
	}

	color_type = png_get_color_type(png_ptr, info_ptr);

	if(color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGB_ALPHA)
	{
		error("invalid color type\n");
		return 0;
	}

	rowptrs = png_get_rows(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	*_width = width;
	height = png_get_image_height(png_ptr, info_ptr);
	*_height = height;

	data = (uint*)gl_get_pixel_buffer(width * height * 4);

	if(color_type == PNG_COLOR_TYPE_RGB)
	{
		uint x;

		for(y = 0; y < height; y++)
		{
			uint o = (uint)rowptrs[y];

			for(x = 0; x < width; x++)
			{
				uint b = (*(unsigned char *)o++);
				uint g = (*(unsigned char *)o++);
				uint r = (*(unsigned char *)o++);

				data[y * width + x] = b | g << 8 | r << 16 | 0xFF << 24;
			}
		}
	}

	else for(y = 0; y < height; y++) memcpy(&data[y * width], rowptrs[y], width * 4);

	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	fclose(f);

	return data;
}
