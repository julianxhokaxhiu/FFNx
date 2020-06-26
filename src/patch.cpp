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

void patch_code_dword(uint offset, DWORD r)
{
	DWORD dummy;

	VirtualProtect((void*)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(DWORD*)offset = r;
}

void patch_code_int(uint offset, int r)
{
	DWORD dummy;

	VirtualProtect((void*)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(int*)offset = r;
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
