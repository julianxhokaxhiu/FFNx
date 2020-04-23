#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <vgmstream.h>

#if defined(__cplusplus)
}
#endif

#include <windows.h>
#include <dsound.h>
#include <math.h>
#include <process.h>

#include "../log.h"
#include "../types.h"

void vgm_music_init();
void vgm_play_music(char* midi, uint id);
void vgm_stop_music();
void vgm_cross_fade_music(char* midi, uint id, int time);
void vgm_pause_music();
void vgm_resume_music();
uint vgm_music_status();
void vgm_set_master_music_volume(int volume);
void vgm_set_music_volume(int volume);
void vgm_set_music_volume_trans(int volume, int step);
void vgm_set_music_tempo(unsigned char tempo);
void vgm_music_cleanup();
