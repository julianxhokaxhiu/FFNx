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
 * music.h - music player definitions
 */

#pragma once

#include <DSound.h>
#include "types.h"

void music_init();
uint midi_init(uint unknown);
void music_cleanup();
void play_midi(uint midi);
void cross_fade_midi(uint midi, uint time);
void pause_midi();
void restart_midi();
void stop_midi();
uint midi_status();
void set_master_midi_volume(uint volume);
void set_midi_volume(uint volume);
void set_midi_volume_trans(uint volume, uint step);
void set_midi_tempo(unsigned char tempo);

bool ff7_needs_resume(uint old_mode, uint new_mode, char* old_midi, char* new_midi);
