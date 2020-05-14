#pragma once

#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <dsound.h>
#include <process.h>

#include "../log.h"
#include "../music.h"

#include "plugin.h"
#include "out_directsound.h"

void winamp_music_init();
bool winamp_can_play(char* midi);
void winamp_play_music(char* midi, uint id);
void winamp_stop_music();
void winamp_cross_fade_music(char* midi, uint id, int time);
void winamp_pause_music();
void winamp_resume_music();
bool winamp_music_status();
void winamp_set_direct_volume(int volume);
void winamp_set_master_music_volume(int volume);
void winamp_set_music_volume(int volume);
void winamp_set_music_volume_trans(int volume, int frames);
void winamp_set_music_tempo(unsigned char tempo);
void winamp_remember_playing_time();
void winamp_music_cleanup();

