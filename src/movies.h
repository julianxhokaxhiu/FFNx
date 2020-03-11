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
 * movies.h - FMV player definitions
 */

#pragma once

#include <ddraw.h>

#include "types.h"

void movie_init();
uint ff7_prepare_movie(char *, uint, struct dddevice **, uint);
void ff7_release_movie_objects();
uint ff7_start_movie();
uint ff7_update_movie_sample(LPDIRECTDRAWSURFACE);
uint ff7_stop_movie();
uint ff7_get_movie_frame();
void draw_current_frame();
void ff8_prepare_movie(uint disc, uint movie);
void ff8_release_movie_objects();
void ff8_start_movie();
void ff8_update_movie_sample();
void ff8_stop_movie();
uint ff8_get_movie_frame();
