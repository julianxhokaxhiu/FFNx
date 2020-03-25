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
#include <zlib.h>
#include <direct.h>

#include "types.h"
#include "log.h"
#include "globals.h"
#include "gl.h"

uint write_png(char *filename, uint width, uint height, char *data)
{
	// TODO: ADAPT TO NEW RENDERER

	return true;
}

uint *read_png(char *filename, uint *_width, uint *_height)
{
    // TODO: ADAPT TO NEW RENDERER

	return NULL;
}
