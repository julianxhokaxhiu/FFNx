/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#if defined(__cplusplus)
extern "C" {
#endif

extern const GUID IID_ICodecAPI = { 0x901db4c7, 0x31ce, 0x41a2, 0x85,0xdc, 0x8f,0xa0,0xbf,0x41,0xb8,0xda };

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#if defined(__cplusplus)
}
#endif

#include <windows.h>
#include <math.h>
#include <sys/timeb.h>
#include <dsound.h>
#include <dbghelp.h>

#include "../crashdump.h"
#include "../log.h"
#include "../gl.h"

void ffmpeg_movie_init();
void ffmpeg_release_movie_objects();
uint ffmpeg_prepare_movie(char* name);
void ffmpeg_stop_movie();
uint ffmpeg_update_movie_sample();
void ffmpeg_draw_current_frame();
void ffmpeg_loop();
uint ffmpeg_get_movie_frame();
