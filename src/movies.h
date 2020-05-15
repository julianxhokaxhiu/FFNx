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
