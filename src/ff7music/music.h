#pragma once

#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <dsound.h>

#include "../log.h"
#include "../types.h"

void ff7music_play_music(char* midi, uint id);
void ff7music_stop_music();
void ff7music_cross_fade_music(char* midi, uint id, uint time);
void ff7music_pause_music();
void ff7music_resume_music();
bool ff7music_music_status();
void ff7music_set_master_music_volume(uint volume);
void ff7music_set_music_volume(uint volume);
void ff7music_set_music_volume_trans(uint volume, uint step);
void ff7music_set_music_tempo(unsigned char tempo);
void ff7music_music_cleanup();