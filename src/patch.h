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
 * patch.h - helper functions for runtime code patching
 */

#pragma once

#include "globals.h"
#include "log.h"

uint replace_function(uint offset, void *func);
void unreplace_function(uint func);
void unreplace_functions();

void replace_call(uint offset, void *func);

uint get_relative_call(uint base, uint offset);
uint get_absolute_value(uint base, uint offset);
void patch_code_byte(uint offset, unsigned char r);
void patch_code_word(uint offset, word r);
void patch_code_dword(uint offset, DWORD r);
void patch_code_int(uint offset, int r);
void patch_code_uint(uint offset, uint r);
void patch_code_float(uint offset, float r);
void patch_code_double(uint offset, double r);
void memcpy_code(uint offset, void *data, uint size);
void memset_code(uint offset, uint val, uint size);
