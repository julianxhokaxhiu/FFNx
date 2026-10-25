/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 HobbitDur                                          //
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

#include "kernel.h"

static constexpr uint32_t record_sizes[] = {
	sizeof(ff8_kernel_battle_command),
	sizeof(ff8_kernel_magic),
	sizeof(ff8_kernel_junctionable_gf),
	sizeof(ff8_kernel_enemy_attack),
	sizeof(ff8_kernel_weapon),
	sizeof(ff8_kernel_renzokuken_finisher),
	sizeof(ff8_kernel_character),
	sizeof(ff8_kernel_battle_item),
	sizeof(ff8_kernel_non_battle_item),
	sizeof(ff8_kernel_non_junctionable_gf),
	sizeof(ff8_kernel_command_ability_data),
	sizeof(ff8_kernel_junction_ability),
	sizeof(ff8_kernel_command_ability),
	sizeof(ff8_kernel_stat_percentage_ability),
	sizeof(ff8_kernel_character_ability),
	sizeof(ff8_kernel_party_ability),
	sizeof(ff8_kernel_gf_ability),
	sizeof(ff8_kernel_menu_ability),
	sizeof(ff8_kernel_temporary_character_limit),
	sizeof(ff8_kernel_blue_magic),
	sizeof(ff8_kernel_blue_magic_parameter),
	sizeof(ff8_kernel_shot),
	sizeof(ff8_kernel_duel),
	sizeof(ff8_kernel_duel_parameters),
	sizeof(ff8_kernel_rinoa_limit_part1),
	sizeof(ff8_kernel_rinoa_limit_part2),
	sizeof(ff8_kernel_slot_array),
	sizeof(ff8_kernel_slot_set),
	sizeof(ff8_kernel_devour),
	sizeof(ff8_kernel_misc),
	sizeof(ff8_kernel_misc_text_offset),
};

ff8_kernel_file::ff8_kernel_file(const char *data, uint32_t size) : data_(data), size_(size), valid_(false)
{
	if (!data || size < sizeof(ff8_kernel_header)) return;
	if (header().section_count != static_cast<uint32_t>(ff8_kernel_section::count)) return;

	uint32_t previous = sizeof(ff8_kernel_header);
	for (uint32_t offset : header().section_offsets)
	{
		if (offset < previous || offset > size) return;
		previous = offset;
	}
	for (uint32_t i = 0; i < 31; ++i)
		if ((header().section_offsets[i + 1] - header().section_offsets[i]) % record_sizes[i]) return;

	valid_ = true;
}

std::span<const char> ff8_kernel_file::bytes(ff8_kernel_section section) const
{
	uint32_t index = static_cast<uint32_t>(section);
	if (!valid_ || index >= static_cast<uint32_t>(ff8_kernel_section::count)) return {};
	uint32_t begin = header().section_offsets[index];
	uint32_t end = index + 1 < static_cast<uint32_t>(ff8_kernel_section::count) ? header().section_offsets[index + 1] : size_;
	return { data_ + begin, end - begin };
}
