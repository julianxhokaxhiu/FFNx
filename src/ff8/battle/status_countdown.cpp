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

// The status countdown numbers (Doom, and the second counter drawn above it) are placed from the model's
// screen position, raised by 8 between the two counters. The PC GTE gives that position in eighths of a
// pixel and the number drawing turns it into pixels, but the 8 was left in PSX pixels: the numbers sit
// 1 pixel apart instead of 8, so both counters overlap and the first one is 7 pixels lower than on PSX.

#include "status_countdown.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"

#include <stdint.h>

// Animation init -> per-frame entity update -> status visuals -> countdown numbers
#define STATUS_VISUALS_COUNTDOWN 0xBD
// add word [esi + 0xA], -8 before each counter
#define COUNTDOWN_FIRST_RAISE 0x3D
#define COUNTDOWN_SECOND_RAISE 0x99

void ff8_battle_status_countdown_init()
{
	if (!FF8_US_VERSION) return;

	uint32_t animation_init = get_relative_call(ff8_externals.sub_502380, 0x3F);
	uint32_t entity_update = get_absolute_value(animation_init, 0x6);
	uint32_t status_visuals = get_relative_call(entity_update, 0xAD);
	uint32_t countdown = get_relative_call(status_visuals, STATUS_VISUALS_COUNTDOWN);

	// -8 pixels -> -8 * 8 eighths of a pixel
	patch_code_byte(countdown + COUNTDOWN_FIRST_RAISE, uint8_t(-8 * 8));
	patch_code_byte(countdown + COUNTDOWN_SECOND_RAISE, uint8_t(-8 * 8));
}
