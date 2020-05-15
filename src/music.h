#pragma once

#include "types.h"

void music_init();
uint midi_init(uint unknown);
uint ff7_directsound_release();
void music_cleanup();
uint ff7_use_midi(uint midi);
void ff7_play_midi(uint midi);
uint ff8_play_midi(uint midi, uint volume, uint u1, uint u2);
void cross_fade_midi(uint midi, uint time);
void pause_midi();
void restart_midi();
void stop_midi();
uint ff8_stop_midi();
uint midi_status();
uint ff8_set_direct_volume(int volume);
void set_master_midi_volume(uint volume);
void set_midi_volume(uint volume);
void set_midi_volume_trans(uint volume, uint step);
void set_midi_tempo(unsigned char tempo);
uint remember_playing_time();

bool needs_resume(uint old_mode, uint new_mode, char* old_midi, char* new_midi);
