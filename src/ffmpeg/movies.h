#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

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
