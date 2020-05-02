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
 * ff7/misc.c - replacement routines for miscellaneous FF7 functions
 */

#include <stdint.h>

#include "../types.h"
#include "../ff7.h"
#include "../common.h"
#include "../globals.h"
#include "../cfg.h"
#include "../log.h"

// MDEF fix
uint get_equipment_stats(uint party_index, uint type)
{
	uint character = ff7_externals.party_member_to_char_map[ff7_externals.savemap->party_members[party_index]];

	switch(type)
	{
		case 0:
			return ff7_externals.weapon_data_array[ff7_externals.savemap->chars[character].equipped_weapon].attack_stat;
			break;
		case 1:
			return ff7_externals.armor_data_array[ff7_externals.savemap->chars[character].equipped_armor].defense_stat;
			break;
		case 2:
			return 0;
			break;
		case 3:
			return mdef_fix ? ff7_externals.armor_data_array[ff7_externals.savemap->chars[character].equipped_armor].mdef_stat : 0;
			break;

		default: return 0;
	}
}

char *kernel2_sections[20];
uint kernel2_section_counter;

void kernel2_reset_counters()
{
	uint i;

	if(trace_all) trace("kernel2 reset\n");

	for(i = 0; i < kernel2_section_counter; i++) external_free(kernel2_sections[i]);

	kernel2_section_counter = 0;
}

char *kernel2_add_section(uint size)
{
	char *ret = (char*)external_malloc(size);

	if(trace_all) trace("kernel2 add section %i (%i)\n", kernel2_section_counter, size);

	kernel2_sections[kernel2_section_counter++] = ret;

	return ret;
}

char *kernel2_get_text(uint section_base, uint string_id, uint section_offset)
{
	char *section = kernel2_sections[section_base + section_offset];

	if(trace_all) trace("kernel2 get text (%i+%i:%i)\n", section_base, section_offset, string_id);
	
	return &section[((word *)section)[string_id]];
}

void ff7_wm_activateapp(bool hasFocus)
{

}
