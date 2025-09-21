/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#include "../ff7.h"
#include "../log.h"

#define FF7_KERNEL_NUM_SECTIONS 27

// KERNEL2
char *kernel2_sections[20];
uint32_t kernel2_section_counter;

void kernel2_reset_counters()
{
	uint32_t i;

	if(trace_all) ffnx_trace("kernel2 reset\n");

	for(i = 0; i < kernel2_section_counter; i++) external_free(kernel2_sections[i]);

	kernel2_section_counter = 0;
}

char *kernel2_add_section(uint32_t size)
{
	char *ret = (char*)external_malloc(size);

	if(trace_all) ffnx_trace("kernel2 add section %i (%i)\n", kernel2_section_counter, size);

	kernel2_sections[kernel2_section_counter++] = ret;

	return ret;
}

char *kernel2_get_text(uint32_t section_base, uint32_t string_id, uint32_t section_offset)
{
	char *section = kernel2_sections[section_base + section_offset];

	if(trace_all) ffnx_trace("kernel2 get text (%i+%i:%i)\n", section_base, section_offset, string_id);

	return &section[((WORD *)section)[string_id]];
}

// ENGINE

void ff7_load_kernel2_wrapper(char *filename)
{
  ff7_externals.kernel_load_kernel2(filename);

	char chunk_file[1024]{0};
	uint32_t chunk_size = 0;
	FILE* fd;

	for (int n = 0; n < FF7_KERNEL_NUM_SECTIONS; n++)
	{
		_snprintf(chunk_file, sizeof(chunk_file), "%s/%s/kernel/kernel.bin.chunk.%i", basedir, direct_mode_path.c_str(), n+1);

		if ((fd = fopen(chunk_file, "rb")) != NULL)
		{
			fseek(fd, 0L, SEEK_END);
			chunk_size = ftell(fd);
			fseek(fd, 0L, SEEK_SET);

			if (0 <= n && n <= 8)
				fread(ff7_externals.kernel_1to9_sections[n], sizeof(byte), chunk_size, fd);
			else
				fread(kernel2_sections[n-9], sizeof(byte), chunk_size, fd);

			ffnx_trace("%s: kernel section %i overridden with %s\n", __func__, n+1, chunk_file);
			fclose(fd);
		}
		else if (trace_direct)
			ffnx_trace("%s: could not find %s\n", __func__, chunk_file);
	}
}
