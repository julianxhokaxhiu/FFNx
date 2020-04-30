#pragma once

#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <dsound.h>
#include <process.h>

#include "../log.h"
#include "../types.h"
#include "../music.h"
#include "../cfg.h"

#include "plugin.h"

void winamp_music_init();
void winamp_play_music(char* midi, uint id);
void winamp_stop_music();
void winamp_cross_fade_music(char* midi, uint id, int time);
void winamp_pause_music();
void winamp_resume_music();
bool winamp_music_status();
void winamp_set_master_music_volume(int volume);
void winamp_set_music_volume(int volume);
void winamp_set_music_volume_trans(int volume, int step);
void winamp_set_music_tempo(unsigned char tempo);
void winamp_music_cleanup();