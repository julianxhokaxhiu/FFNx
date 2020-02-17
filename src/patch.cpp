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
 * patch.c - helper functions for runtime code patching
 */

#include <windows.h>

#include "types.h"

uint replace_counter = 0;
uint replaced_functions[512 * 3];

uint replace_function(uint offset, void *func)
{
	DWORD dummy;

	VirtualProtect((void *)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);

	replaced_functions[replace_counter++] = *(unsigned char *)offset;
	replaced_functions[replace_counter++] = *(uint *)(offset + 1);
	replaced_functions[replace_counter++] = offset;

	*(unsigned char *)offset = 0xE9;
	*(uint *)(offset + 1) = ((uint)func - offset) - 5;

	return replace_counter - 3;
}

void unreplace_function(uint func)
{
	uint offset = replaced_functions[func + 2];
	DWORD dummy;

	VirtualProtect((void *)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);
	*(uint *)(offset + 1) = replaced_functions[func + 1];
	*(unsigned char *)offset = replaced_functions[func];
}

void unreplace_functions()
{
	while(replace_counter > 0)
	{
		uint offset = replaced_functions[--replace_counter];
		DWORD dummy;

		VirtualProtect((void *)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);
		*(uint *)(offset + 1) = replaced_functions[--replace_counter];
		*(unsigned char *)offset = replaced_functions[--replace_counter];
	}
}

void replace_call(uint offset, void *func)
{
	DWORD dummy;

	VirtualProtect((void *)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);

	*(uint *)(offset + 1) = ((uint)func - offset) - 5;
}

uint get_relative_call(uint base, uint offset)
{
	return base + *((uint *)(base + offset + 1)) + offset + 5;
}

uint get_absolute_value(uint base, uint offset)
{
	return *((uint *)(base + offset));
}

void patch_code_byte(uint offset, unsigned char r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(unsigned char *)offset = r;
}

void patch_code_word(uint offset, word r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(word *)offset = r;
}

void patch_code_uint(uint offset, uint r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(uint *)offset = r;
}

void patch_code_float(uint offset, float r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(float *)offset = r;
}

void patch_code_double(uint offset, double r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(double *)offset = r;
}

void memcpy_code(uint offset, void *data, uint size)
{
	DWORD dummy;

	VirtualProtect((void *)offset, size, PAGE_EXECUTE_READWRITE, &dummy);

	memcpy((void *)offset, data, size);
}

void memset_code(uint offset, uint val, uint size)
{
	DWORD dummy;

	VirtualProtect((void *)offset, size, PAGE_EXECUTE_READWRITE, &dummy);

	memset((void *)offset, val, size);
}
