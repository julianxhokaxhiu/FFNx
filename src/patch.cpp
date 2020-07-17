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
#include <stdint.h>

uint32_t replace_counter = 0;
uint32_t replaced_functions[512 * 3];

uint32_t replace_function(uint32_t offset, void *func)
{
	DWORD dummy;

	VirtualProtect((void *)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);

	replaced_functions[replace_counter++] = *(unsigned char *)offset;
	replaced_functions[replace_counter++] = *(uint32_t *)(offset + 1);
	replaced_functions[replace_counter++] = offset;

	*(unsigned char *)offset = 0xE9;
	*(uint32_t *)(offset + 1) = ((uint32_t)func - offset) - 5;

	return replace_counter - 3;
}

void unreplace_function(uint32_t func)
{
	uint32_t offset = replaced_functions[func + 2];
	DWORD dummy;

	VirtualProtect((void *)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);
	*(uint32_t *)(offset + 1) = replaced_functions[func + 1];
	*(unsigned char *)offset = replaced_functions[func];
}

void unreplace_functions()
{
	while(replace_counter > 0)
	{
		uint32_t offset = replaced_functions[--replace_counter];
		DWORD dummy;

		VirtualProtect((void *)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);
		*(uint32_t *)(offset + 1) = replaced_functions[--replace_counter];
		*(unsigned char *)offset = replaced_functions[--replace_counter];
	}
}

void replace_call(uint32_t offset, void *func)
{
	DWORD dummy;

	VirtualProtect((void *)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);

	*(uint32_t *)(offset + 1) = ((uint32_t)func - offset) - 5;
}

uint32_t get_relative_call(uint32_t base, uint32_t offset)
{
	return base + *((uint32_t *)(base + offset + 1)) + offset + 5;
}

uint32_t get_absolute_value(uint32_t base, uint32_t offset)
{
	return *((uint32_t *)(base + offset));
}

void patch_code_byte(uint32_t offset, unsigned char r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(unsigned char *)offset = r;
}

void patch_code_word(uint32_t offset, WORD r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(WORD *)offset = r;
}

void patch_code_dword(uint32_t offset, DWORD r)
{
	DWORD dummy;

	VirtualProtect((void*)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(DWORD*)offset = r;
}

void patch_code_int(uint32_t offset, int r)
{
	DWORD dummy;

	VirtualProtect((void*)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(int*)offset = r;
}

void patch_code_uint(uint32_t offset, uint32_t r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(uint32_t *)offset = r;
}

void patch_code_float(uint32_t offset, float r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(float *)offset = r;
}

void patch_code_double(uint32_t offset, double r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(double *)offset = r;
}

void memcpy_code(uint32_t offset, void *data, uint32_t size)
{
	DWORD dummy;

	VirtualProtect((void *)offset, size, PAGE_EXECUTE_READWRITE, &dummy);

	memcpy((void *)offset, data, size);
}

void memset_code(uint32_t offset, uint32_t val, uint32_t size)
{
	DWORD dummy;

	VirtualProtect((void *)offset, size, PAGE_EXECUTE_READWRITE, &dummy);

	memset((void *)offset, val, size);
}
