/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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
#include "patch.h"

#include <windows.h>
#include <stdint.h>

#include "crashdump.h"

#ifdef PATCH_COLLECT_DUPLICATES
#	include <unordered_set>
std::unordered_set<uint32_t> offsets;
std::unordered_set<uint32_t> addresses;
uint32_t min_addr = 0;
uint32_t max_addr = 0;
#endif

uint32_t replace_counter = 0;
uint32_t replaced_functions[512 * 3];

void check_is_call(const char *name, uint32_t base, uint32_t offset, uint8_t instruction)
{
	if (instruction != 0xE8 && instruction != 0xE9)
	{
		// Warning to diagnose errors faster
		ffnx_warning("%s: Unrecognized call/jmp instruction at 0x%X + 0x%X (0x%X): 0x%X\n", name, base, offset, base + offset, instruction);
	}
}

#ifdef PATCH_COLLECT_DUPLICATES
void check_boundaries(const char *name, uint32_t base, uint32_t offset, uint32_t address)
{
	if (min_addr == 0)
	{
		// Get EXE boundaries
		FFNxStackWalker sw(true);
		if (sw.LoadModules()) {
			min_addr = sw.getBaseAddress();
			max_addr = min_addr + sw.getSize();
		}
	}

	if (min_addr != 0 && (base + offset < min_addr || base + offset > max_addr))
	{
		ffnx_warning("%s: Out of bounds offset at 0x%X + 0x%X (0x%X) (min: 0x%X, max: 0x%X)\n", name, base, offset, base + offset, min_addr, max_addr);
	}

	if (min_addr != 0 && (address < min_addr || address > max_addr))
	{
		ffnx_warning("%s: Out of bounds address at 0x%X + 0x%X (0x%X): 0x%X (min: 0x%X, max: 0x%X)\n", name, base, offset, base + offset, address, min_addr, max_addr);
	}
}

void collect_addresses(const char *name, uint32_t base, uint32_t offset, uint32_t address)
{
	check_boundaries(name, base, offset, address);

	if (offsets.contains(base + offset))
	{
		ffnx_warning("%s: Offset used before 0x%X + 0x%X (0x%X)\n", name, base, offset, base + offset);
	}

	if (addresses.contains(address))
	{
		ffnx_warning("%s: Address used before 0x%X + 0x%X (0x%X): 0x%X\n", name, base, offset, base + offset, address);
	}

	offsets.insert(base + offset);
	addresses.insert(address);
}
#endif

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
	uint32_t offset = replaced_functions[func + 2],
		value = replaced_functions[func + 1],
		instr = replaced_functions[func];
	DWORD dummy;

	VirtualProtect((void *)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);

	// Remember previous state to rereplace later
	replaced_functions[func + 1] = *(uint32_t *)(offset + 1);
	replaced_functions[func] = *(unsigned char *)offset;

	*(uint32_t *)(offset + 1) = value;
	*(unsigned char *)offset = instr;
}

void rereplace_function(uint32_t func)
{
	// In fact this is the same operation than unreplace_function
	unreplace_function(func);
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

	check_is_call(__func__, offset, 0, *((uint8_t *)(offset)));

	VirtualProtect((void *)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);

	*(uint32_t *)(offset + 1) = ((uint32_t)func - offset) - 5;
}

uint32_t replace_call_function(uint32_t offset, void* func)
{
	DWORD dummy;

	VirtualProtect((void*)offset, 5, PAGE_EXECUTE_READWRITE, &dummy);

	replaced_functions[replace_counter++] = *(unsigned char*)offset;
	replaced_functions[replace_counter++] = *(uint32_t*)(offset + 1);
	replaced_functions[replace_counter++] = offset;

	*(unsigned char*)offset = 0xE8;
	*(uint32_t*)(offset + 1) = ((uint32_t)func - offset) - 5;

	return replace_counter - 3;
}

uint32_t get_relative_call(uint32_t base, uint32_t offset)
{
	uint8_t instruction = *((uint8_t *)(base + offset));

	check_is_call(__func__, base, offset, instruction);

	uint32_t ret = base + *((uint32_t *)(base + offset + 1)) + offset + 5;

#ifdef PATCH_COLLECT_DUPLICATES
	collect_addresses(__func__, base, offset, ret);
#endif

	return ret;
}

uint32_t get_absolute_value(uint32_t base, uint32_t offset)
{
#ifdef PATCH_COLLECT_DUPLICATES
	collect_addresses(__func__, base, offset, *((uint32_t *)(base + offset)));
#endif

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

void patch_code_short(uint32_t offset, short r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(short *)offset = r;
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
