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

// Battle sounds and field footsteps are panned from the screen X of their source, but the PC GTE
// returns it in eighths of a pixel where the PSX returned pixels:
// - battle: pan = 96 * (x - 160) / 320 + 128, clamped to 80..175, so almost every sound plays on the right
// - field footsteps: pan = x / 4 + 128, clamped to -512..508 first, so steps jump from one side to the other
// The projection calls get the coordinates back in pixels, like the game's own divided-by-8 projection does.

#include "sound_pan.h"

#include "../ff8.h"
#include "../patch.h"
#include "../globals.h"

#include <stdint.h>

// Battle: BdPlayActorSE3D -> screen pan -> projection
#define BATTLE_ACTOR_SOUND_3D_PAN 0xBF
#define BATTLE_PAN_PROJECT 0x1D
// Field: script frame -> footstep sound -> projection
#define FIELD_FOOTSTEP_SOUND 0x32C
#define FIELD_FOOTSTEP_PROJECT 0x6D

typedef int (*project_fn)(void *, int16_t *, int *, int *);

static project_fn project = nullptr;

static int project_pixels(void *position, int16_t *sxy, int *p, int *flag)
{
	const int otz = project(position, sxy, p, flag);

	sxy[0] = int16_t(sxy[0] >> 3);
	sxy[1] = int16_t(sxy[1] >> 3);

	return otz;
}

void ff8_sound_pan_init()
{
	if (!FF8_US_VERSION) return;

	const uint32_t battle_pan = get_relative_call(ff8_externals.battle_actor_sound_3d, BATTLE_ACTOR_SOUND_3D_PAN);
	const uint32_t field_footstep = get_relative_call(common_externals.update_field_entities, FIELD_FOOTSTEP_SOUND);

	project = (project_fn)get_relative_call(battle_pan, BATTLE_PAN_PROJECT);

	replace_call(battle_pan + BATTLE_PAN_PROJECT, project_pixels);
	replace_call(field_footstep + FIELD_FOOTSTEP_PROJECT, project_pixels);
}
