/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#pragma once

#include <windows.h>
#include <math.h>
#include <sys/timeb.h>
#include <dbghelp.h>

#include "../crashdump.h"
#include "../log.h"
#include "../gl.h"

void ffmpeg_movie_init();
void ffmpeg_release_movie_objects();
uint32_t ffmpeg_prepare_movie(char* name, bool with_audio = true);
void ffmpeg_stop_movie();
uint32_t ffmpeg_update_movie_sample(bool use_movie_fps = true);
void ffmpeg_draw_current_frame();
void ffmpeg_loop();
uint32_t ffmpeg_get_movie_frame();

short ffmpeg_get_fps_ratio();

void draw_yuv_frame(uint32_t buffer_index);
