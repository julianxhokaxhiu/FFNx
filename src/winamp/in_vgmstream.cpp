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

/**
Copyright (c) 2008-2019 Adam Gashlin, Fastelbja, Ronny Elfert, bnnm,
                        Christopher Snowhill, NicknineTheEagle, bxaimc,
                        Thealexbarney, CyberBotX, et al

Portions Copyright (c) 2004-2008, Marko Kreen
Portions Copyright 2001-2007  jagarl / Kazunori Ueno <jagarl@creator.club.ne.jp>
Portions Copyright (c) 1998, Justin Frankel/Nullsoft Inc.
Portions Copyright (C) 2006 Nullsoft, Inc.
Portions Copyright (c) 2005-2007 Paul Hsieh
Portions Public Domain originating with Sun Microsystems

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "in_vgmstream.h"

WinampOutModule* outMod;

extern "C" {

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include <windows.h>
#include <minmax.h>
#include <commctrl.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <ctype.h>

#include <vgmstream.h>
#include <plugins.h>


  /* ************************************* */

#define EXT_BUFFER_SIZE 200

DWORD WINAPI __stdcall decode(void* arg);

typedef enum {
    REPLAYGAIN_NONE,
    REPLAYGAIN_ALBUM,
    REPLAYGAIN_TRACK
} replay_gain_type_t;

/* loaded settings */
typedef struct {
    int thread_priority;

    double fade_time;
    double fade_delay;
    double loop_count;
    int ignore_loop;
    int loop_forever;

    int disable_subsongs;
    int downmix_channels;
    int tagfile_disable;
    int exts_unknown_on;
    int exts_common_on;

    replay_gain_type_t gain_type;
    replay_gain_type_t clip_type;
} winamp_settings_t;

/* current song config */
typedef struct {
    int song_play_forever;
    double song_loop_count;
    double song_fade_time;
    double song_fade_delay;
    int song_ignore_loop;
    int song_really_force_loop;
    int song_ignore_fade;
} winamp_song_config;

/* current play state */
typedef struct {
    int paused;
    int decode_abort;
    int seek_needed_samples;
    int decode_pos_ms;
    int decode_pos_samples;
    int stream_length_samples;
    int fade_samples;
    int output_channels;
    double volume;
} winamp_state_t;

#define in_char char

/* Winamp needs at least 576 16-bit samples, stereo, doubled in case DSP effects are active */
#define SAMPLE_BUFFER_SIZE 576
const char* tagfile_name = "!tags.m3u";

/* plugin state */
HANDLE decode_thread_handle = INVALID_HANDLE_VALUE;

VGMSTREAM* vgmstream = NULL, *dup_vgmstream = NULL;

winamp_settings_t settings;
winamp_song_config config, dup_config;
winamp_state_t state, dup_state;
short sample_buffer[SAMPLE_BUFFER_SIZE * 2 * VGMSTREAM_MAX_CHANNELS]; //todo maybe should be dynamic


/* ************************************* */
/* IN_UNICODE                            */
/* ************************************* */
//todo safe ops
//todo there must be a better way to handle unicode...
#define wa_strcmp strcmp
#define wa_strcpy strcpy
#define wa_strncpy strncpy
#define wa_strchr strchr
#define wa_sscanf sscanf
#define wa_L(x) x

/* converts from utf16 to utf8 (if unicode is on) */
static void wa_ichar_to_char(char* dst, size_t dstsize, const in_char* wsrc) {
    strcpy(dst, wsrc);
}

/* converts from utf8 to utf16 (if unicode is on) */
static void wa_char_to_ichar(in_char* wdst, size_t wdstsize, const char* src) {
    strcpy(wdst, src);
}

/* opens a utf16 (unicode) path */
static FILE* wa_fopen(const in_char* wpath) {
    return fopen(wpath, "rb");
}

/* ************************************* */
/* IN_STREAMFILE                         */
/* ************************************* */

/* a STREAMFILE that operates via STDIOSTREAMFILE but handles Winamp's unicode (in_char) paths */
typedef struct {
    STREAMFILE sf;
    STREAMFILE* stdiosf;
    FILE* infile_ref; /* pointer to the infile in stdiosf (partially handled by stdiosf) */
} WINAMP_STREAMFILE;

static STREAMFILE* open_winamp_streamfile_by_file(FILE* infile, const char* path);
static STREAMFILE* open_winamp_streamfile_by_ipath(const in_char* wpath);

static size_t wasf_read(STREAMFILE* streamfile, uint8_t* dest, off_t offset, size_t length) {
    return ((WINAMP_STREAMFILE*)streamfile)->stdiosf->read(((WINAMP_STREAMFILE *)streamfile)->stdiosf, dest, offset, length);
}

static size_t wasf_get_size(STREAMFILE* streamfile) {
    return ((WINAMP_STREAMFILE*)streamfile)->stdiosf->get_size(((WINAMP_STREAMFILE*)streamfile)->stdiosf);
}

static off_t wasf_get_offset(STREAMFILE* streamfile) {
    return ((WINAMP_STREAMFILE*)streamfile)->stdiosf->get_offset(((WINAMP_STREAMFILE*)streamfile)->stdiosf);
}

static void wasf_get_name(STREAMFILE* streamfile, char* buffer, size_t length) {
    ((WINAMP_STREAMFILE*)streamfile)->stdiosf->get_name(((WINAMP_STREAMFILE*)streamfile)->stdiosf, buffer, length);
}

static STREAMFILE* wasf_open(STREAMFILE* streamFile, const char* const filename, size_t buffersize) {
    in_char wpath[PATH_LIMIT];
    char name[PATH_LIMIT];

    if (!filename)
        return NULL;

    /* STREAMFILEs carry char/UTF8 names, convert to wchar for Winamp */
    wa_char_to_ichar(wpath, PATH_LIMIT, filename);
    return open_winamp_streamfile_by_ipath(wpath);
}

static void wasf_close(STREAMFILE* streamfile) {
    /* closes infile_ref + frees in the internal STDIOSTREAMFILE (fclose for wchar is not needed) */
    ((WINAMP_STREAMFILE*)streamfile)->stdiosf->close(((WINAMP_STREAMFILE*)streamfile)->stdiosf);
    free(((WINAMP_STREAMFILE*)streamfile)); /* and the current struct */
}

static STREAMFILE* open_winamp_streamfile_by_file(FILE* infile, const char* path) {
    WINAMP_STREAMFILE* this_sf = NULL;
    STREAMFILE* stdiosf = NULL;

    this_sf = (WINAMP_STREAMFILE * )calloc(1, sizeof(WINAMP_STREAMFILE));
    if (!this_sf) goto fail;

    stdiosf = open_stdio_streamfile_by_file(infile, path);
    if (!stdiosf) goto fail;

    this_sf->sf.read = wasf_read;
    this_sf->sf.get_size = wasf_get_size;
    this_sf->sf.get_offset = wasf_get_offset;
    this_sf->sf.get_name = wasf_get_name;
    this_sf->sf.open = wasf_open;
    this_sf->sf.close = wasf_close;

    this_sf->stdiosf = stdiosf;
    this_sf->infile_ref = infile;

    return &this_sf->sf; /* pointer to STREAMFILE start = rest of the custom data follows */

fail:
    close_streamfile(stdiosf);
    free(this_sf);
    return NULL;
}


static STREAMFILE* open_winamp_streamfile_by_ipath(const in_char* wpath) {
    FILE* infile = NULL;
    STREAMFILE* streamFile;
    char path[PATH_LIMIT];


    /* convert to UTF-8 if needed for internal use */
    wa_ichar_to_char(path, PATH_LIMIT, wpath);

    /* open a FILE from a Winamp (possibly UTF-16) path */
    infile = wa_fopen(wpath);
    if (!infile) {
        /* allow non-existing files in some cases */
        if (!vgmstream_is_virtual_filename(path))
            return NULL;
    }

    streamFile = open_winamp_streamfile_by_file(infile, path);
    if (!streamFile) {
        if (infile) fclose(infile);
    }

    return streamFile;
}

/* opens vgmstream for winamp */
static VGMSTREAM* init_vgmstream_winamp(const in_char* fn, int stream_index) {
    VGMSTREAM* vgmstream = NULL;

    //return init_vgmstream(fn);

    /* manually init streamfile to pass the stream index */
    STREAMFILE* streamFile = open_winamp_streamfile_by_ipath(fn); //open_stdio_streamfile(fn);
    if (streamFile) {
        streamFile->stream_index = stream_index;
        vgmstream = init_vgmstream_from_STREAMFILE(streamFile);
        close_streamfile(streamFile);
    }

    return vgmstream;
}


/* ************************************* */
/* IN_CONFIG                             */
/* ************************************* */

int priority_values[] = {
    THREAD_PRIORITY_IDLE,
    THREAD_PRIORITY_LOWEST,
    THREAD_PRIORITY_BELOW_NORMAL,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_HIGHEST,
    THREAD_PRIORITY_TIME_CRITICAL
};

static void load_defaults(winamp_settings_t* settings) {
    settings->thread_priority = THREAD_PRIORITY_NORMAL;
    settings->fade_time = 10.0;
    settings->fade_delay = 0.0;
    settings->loop_count = 2.0;
    settings->loop_forever = 1;
    settings->ignore_loop = 0;
    settings->disable_subsongs = 0;
    settings->downmix_channels = 0;
    settings->tagfile_disable = 0;
    settings->exts_unknown_on = 1;
    settings->exts_common_on = 1;
    settings->gain_type = REPLAYGAIN_ALBUM;
    settings->clip_type = REPLAYGAIN_TRACK;
}

/* ***************************************** */
/* IN_VGMSTREAM UTILS                        */
/* ***************************************** */

/* parses a modified filename ('fakename') extracting tags parameters (NULL tag for first = filename) */
static int parse_fn_string(const in_char* fn, const in_char* tag, in_char* dst, int dst_size) {
    const in_char* end = wa_strchr(fn, '|');

    if (tag == NULL) {
        wa_strcpy(dst, fn);
        if (end)
            dst[end - fn] = '\0';
        return 1;
    }

    dst[0] = '\0';
    return 0;
}
static int parse_fn_int(const in_char* fn, const in_char* tag, int* num) {
    const in_char* start = wa_strchr(fn, '|');

    if (start > 0) {
        wa_sscanf(start + 1, wa_L("$s=%i "), num);
        return 1;
    }
    else {
        *num = 0;
        return 0;
    }
}

static void set_config_defaults(winamp_song_config* current) {
    current->song_play_forever = settings.loop_forever;
    current->song_loop_count = settings.loop_count;
    current->song_fade_time = settings.fade_time;
    current->song_fade_delay = settings.fade_delay;
    current->song_ignore_loop = settings.ignore_loop;
    current->song_really_force_loop = 0;
    current->song_ignore_fade = 0;
}

static void apply_config(VGMSTREAM* vgmstream, winamp_song_config* current) {

    /* honor suggested config, if any (defined order matters)
     * note that ignore_fade and play_forever should take priority */
    if (vgmstream->config.loop_count) {
        current->song_loop_count = vgmstream->config.loop_count;
    }
    if (vgmstream->config.fade_delay) {
        current->song_fade_delay = vgmstream->config.fade_delay;
    }
    if (vgmstream->config.fade_time) {
        current->song_fade_time = vgmstream->config.fade_time;
    }
    if (vgmstream->config.force_loop) {
        current->song_really_force_loop = 1;
    }
    if (vgmstream->config.ignore_loop) {
        current->song_ignore_loop = 1;
    }
    if (vgmstream->config.ignore_fade) {
        current->song_ignore_fade = 1;
    }

    /* remove non-compatible options */
    if (current->song_play_forever) {
        current->song_ignore_fade = 0;
        current->song_ignore_loop = 0;
    }

    /* change loop stuff, in no particular order */
    if (current->song_really_force_loop) {
        vgmstream_force_loop(vgmstream, 1, 0, vgmstream->num_samples);
    }
    if (current->song_ignore_loop) {
        vgmstream_force_loop(vgmstream, 0, 0, 0);
    }

    /* loop N times, but also play stream end instead of fading out */
    if (current->song_loop_count > 0 && current->song_ignore_fade) {
        vgmstream_set_loop_target(vgmstream, (int)current->song_loop_count);
        current->song_fade_time = 0; /* force no fade */
    }
}

static int winampGetExtendedFileInfo_common(in_char* filename, char* metadata, char* ret, int retlen);

static double get_album_gain_volume(const in_char* fn) {
    char replaygain[64];
    double gain = 0.0;
    int had_replaygain = 0;
    if (settings.gain_type == REPLAYGAIN_NONE)
        return 1.0;

    replaygain[0] = '\0'; /* reset each time to make sure we read actual tags */
    if (settings.gain_type == REPLAYGAIN_ALBUM
        && winampGetExtendedFileInfo_common((in_char*)fn, "replaygain_album_gain", replaygain, sizeof(replaygain))
        && replaygain[0] != '\0') {
        gain = atof(replaygain);
        had_replaygain = 1;
    }

    replaygain[0] = '\0';
    if (!had_replaygain
        && winampGetExtendedFileInfo_common((in_char*)fn, "replaygain_track_gain", replaygain, sizeof(replaygain))
        && replaygain[0] != '\0') {
        gain = atof(replaygain);
        had_replaygain = 1;
    }

    if (had_replaygain) {
        double vol = pow(10.0, gain / 20.0);
        double peak = 1.0;

        replaygain[0] = '\0';
        if (settings.clip_type == REPLAYGAIN_ALBUM
            && winampGetExtendedFileInfo_common((in_char*)fn, "replaygain_album_peak", replaygain, sizeof(replaygain))
            && replaygain[0] != '\0') {
            peak = atof(replaygain);
        }
        else if (settings.clip_type != REPLAYGAIN_NONE
            && winampGetExtendedFileInfo_common((in_char*)fn, "replaygain_track_peak", replaygain, sizeof(replaygain))
            && replaygain[0] != '\0') {
            peak = atof(replaygain);
        }
        return peak != 1.0 ? min(vol, 1.0 / peak) : vol;
    }

    return 1.0;
}


/* ***************************************** */
/* IN_VGMSTREAM                              */
/* ***************************************** */

int winamp_Start_thread() {
    decode_thread_handle = CreateThread(
        NULL,   /* handle cannot be inherited */
        0,      /* stack size, 0=default */
        decode, /* thread start routine */
        NULL,   /* no parameter to start routine */
        0,      /* run thread immediately */
        NULL);  /* don't keep track of the thread id */

    SetThreadPriority(decode_thread_handle, priority_values[settings.thread_priority]);

    return 0; /* success */
}

void winamp_Stop_thread() {
    if (decode_thread_handle != INVALID_HANDLE_VALUE) {
        state.decode_abort = 1;

        /* arbitrary wait milliseconds (error can trigger if the system is *really* busy) */
        if (WaitForSingleObject(decode_thread_handle, 5000) == WAIT_TIMEOUT) {
            MessageBox(nullptr, TEXT("Error stopping decode thread\n"), ("Error"), MB_OK | MB_ICONERROR);
            TerminateThread(decode_thread_handle, 0);
        }
        CloseHandle(decode_thread_handle);
        decode_thread_handle = INVALID_HANDLE_VALUE;
    }
}

/* the decode thread */
DWORD WINAPI __stdcall decode(void* arg) {
    const int max_buffer_samples = SAMPLE_BUFFER_SIZE;
    const int max_samples = state.stream_length_samples;

    while (!state.decode_abort) {
        int samples_to_do;
        int output_bytes;

        if (state.decode_pos_samples + max_buffer_samples > state.stream_length_samples
            && (!settings.loop_forever || !vgmstream->loop_flag))
            samples_to_do = state.stream_length_samples - state.decode_pos_samples;
        else
            samples_to_do = max_buffer_samples;

        /* seek setup (max samples to skip if still seeking, mark done) */
        if (state.seek_needed_samples >= 0) {
            /* reset if we need to seek backwards */
            if (state.seek_needed_samples < state.decode_pos_samples) {
                reset_vgmstream(vgmstream);
                apply_config(vgmstream, &config); /* config is undone by reset */

                state.decode_pos_samples = 0;
                state.decode_pos_ms = 0;
            }

            /* adjust seeking past file, can happen using the right (->) key
             * (should be done here and not in SetOutputTime due to threads/race conditions) */
            if (state.seek_needed_samples > max_samples && !settings.loop_forever) {
                state.seek_needed_samples = max_samples;
            }

            /* adjust max samples to seek */
            if (state.decode_pos_samples < state.seek_needed_samples) {
                samples_to_do = state.seek_needed_samples - state.decode_pos_samples;
                if (samples_to_do > max_buffer_samples)
                    samples_to_do = max_buffer_samples;
            }
            else {
                state.seek_needed_samples = -1;
            }

            /* flush Winamp buffers */
            outMod->Flush((int)state.decode_pos_ms);
        }

        output_bytes = (samples_to_do * state.output_channels * sizeof(short));

        if (samples_to_do == 0) { /* track finished */
            outMod->CanWrite();    /* ? */
            if (!outMod->IsPlaying()) {
                return 0;
            }
            Sleep(10);
        }
        else if (state.seek_needed_samples != -1) { /* seek */
            render_vgmstream(sample_buffer, samples_to_do, vgmstream);

            /* discard decoded samples and keep seeking */
            state.decode_pos_samples += samples_to_do;
            state.decode_pos_ms = state.decode_pos_samples * 1000LL / vgmstream->sample_rate;
        }
        else if (outMod->CanWrite() >= output_bytes) { /* decode */
            render_vgmstream(sample_buffer, samples_to_do, vgmstream);

            /* apply ReplayGain, if needed */
           if (state.volume != 1.0) {
                int j, k;
                for (j = 0; j < samples_to_do; j++) {
                    for (k = 0; k < vgmstream->channels; k++) {
                        sample_buffer[j * vgmstream->channels + k] =
                            (short)(sample_buffer[j * vgmstream->channels + k] * state.volume);
                    }
                }
            }

            outMod->Write((char*)sample_buffer, output_bytes);

            state.decode_pos_samples += samples_to_do;
            state.decode_pos_ms = state.decode_pos_samples * 1000LL / vgmstream->sample_rate;
        }
        else { /* can't write right now */
            Sleep(20);
        }
    }

    return 0;
}

/* ************************************* */
/* IN_TAGS                               */
/* ************************************* */

/* could malloc and stuff but totals aren't much bigger than PATH_LIMITs anyway */
#define WINAMP_TAGS_ENTRY_MAX      30
#define WINAMP_TAGS_ENTRY_SIZE     2048

typedef struct {
    int loaded;
    in_char filename[PATH_LIMIT]; /* tags are loaded for this file */
    int tag_count;

    char keys[WINAMP_TAGS_ENTRY_MAX][WINAMP_TAGS_ENTRY_SIZE + 1];
    char vals[WINAMP_TAGS_ENTRY_MAX][WINAMP_TAGS_ENTRY_SIZE + 1];
} winamp_tags;

winamp_tags last_tags;

/* Loads all tags for a filename in a temp struct to improve performance, as
 * Winamp requests one tag at a time and may reask for the same tag several times */
static void load_tagfile_info(in_char* filename) {
    STREAMFILE* tagFile = NULL;
    in_char filename_clean[PATH_LIMIT];
    char filename_utf8[PATH_LIMIT];
    char tagfile_path_utf8[PATH_LIMIT];
    in_char tagfile_path_i[PATH_LIMIT];
    char* path;


    if (settings.tagfile_disable) { /* reset values if setting changes during play */
        last_tags.loaded = 0;
        last_tags.tag_count = 0;
        return;
    }

    /* clean extra part for subsong tags */
    parse_fn_string(filename, NULL, filename_clean, PATH_LIMIT);

    if (wa_strcmp(last_tags.filename, filename_clean) == 0) {
        return; /* not changed, tags still apply */
    }

    last_tags.loaded = 0;

    /* tags are now for this filename, find tagfile path */
    wa_ichar_to_char(filename_utf8, PATH_LIMIT, filename_clean);
    strcpy(tagfile_path_utf8, filename_utf8);

    path = strrchr(tagfile_path_utf8, '\\');
    if (path != NULL) {
        path[1] = '\0'; /* includes "\", remove after that from tagfile_path */
        strcat(tagfile_path_utf8, tagfile_name);
    }
    else { /* ??? */
        strcpy(tagfile_path_utf8, tagfile_name);
    }
    wa_char_to_ichar(tagfile_path_i, PATH_LIMIT, tagfile_path_utf8);

    wa_strcpy(last_tags.filename, filename_clean);
    last_tags.tag_count = 0;

    /* load all tags from tagfile */
    tagFile = open_winamp_streamfile_by_ipath(tagfile_path_i);
    if (tagFile != NULL) {
        VGMSTREAM_TAGS* tags;
        const char* tag_key, * tag_val;
        int i;

        tags = vgmstream_tags_init(&tag_key, &tag_val);
        vgmstream_tags_reset(tags, filename_utf8);
        while (vgmstream_tags_next_tag(tags, tagFile)) {
            int repeated_tag = 0;
            int current_tag = last_tags.tag_count;
            if (current_tag >= WINAMP_TAGS_ENTRY_MAX)
                continue;

            /* should overwrite repeated tags as global tags may appear multiple times */
            for (i = 0; i < current_tag; i++) {
                if (strcmp(last_tags.keys[i], tag_key) == 0) {
                    current_tag = i;
                    repeated_tag = 1;
                    break;
                }
            }

            last_tags.keys[current_tag][0] = '\0';
            strncat(last_tags.keys[current_tag], tag_key, WINAMP_TAGS_ENTRY_SIZE);
            last_tags.vals[current_tag][0] = '\0';
            strncat(last_tags.vals[current_tag], tag_val, WINAMP_TAGS_ENTRY_SIZE);
            if (!repeated_tag)
                last_tags.tag_count++;
        }

        vgmstream_tags_close(tags);
        close_streamfile(tagFile);
        last_tags.loaded = 1;
    }
}

/* Winamp repeatedly calls this for every known tag currently used in the Advanced Title Formatting (ATF)
 * config, 'metadata' being the requested tag. Returns 0 on failure/tag not found.
 * May be called again after certain actions (adding file to playlist, Play, GetFileInfo, etc), and
 * doesn't seem the plugin can tell Winamp all tags it supports at once or use custom tags. */
 //todo unicode stuff could be improved... probably
static int winampGetExtendedFileInfo_common(in_char* filename, char* metadata, char* ret, int retlen) {
    int i, tag_found;
    int max_len;

    /* load list current tags, if necessary */
    load_tagfile_info(filename);
    if (last_tags.loaded) { /* tagfile not found, fail so default get_title takes over */
        /* find requested tag */
        tag_found = 0;
        max_len = (retlen > 0) ? retlen - 1 : retlen;
        for (i = 0; i < last_tags.tag_count; i++) {
            if (strcasecmp(metadata, last_tags.keys[i]) == 0) {
                ret[0] = '\0';
                strncat(ret, last_tags.vals[i], max_len);
                tag_found = 1;
                break;
            }
        }

        if (tag_found) {
            return 1;
        }
    }

    //TODO: is this always needed for Winamp to use replaygain?
    //strcpy(ret, "1.0"); //should set some default value?
    return strcasecmp(metadata, "replaygain_track_gain") == 0 ? 1 : 0;
}
}

VgmstreamInPlugin::VgmstreamInPlugin(AbstractOutPlugin* outPlugin) :
    AbstractInPlugin(outPlugin)
{
    load_defaults(&settings);
    outMod = outPlugin->getModule();
}

VgmstreamInPlugin::~VgmstreamInPlugin()
{
    cancelDuplicate();
}

bool VgmstreamInPlugin::accept(const char* fn) const
{
    vgmstream_ctx_valid_cfg cfg = { 0 };
    char filename_utf8[PATH_LIMIT];

    wa_ichar_to_char(filename_utf8, PATH_LIMIT, fn);

    cfg.skip_standard = 1; /* validated by Winamp */
    cfg.accept_unknown = settings.exts_unknown_on;
    cfg.accept_common = settings.exts_common_on;

    /* Winamp seem to have bizarre handling of MP3 without standard names (ex song.mp3a),
     * in that it'll try to open normally, rejected if unknown_exts_on is not set, and
     * finally retry with "hi.mp3", accepted if exts_common_on is set. */

     /* returning 0 here means it only accepts the extensions in working_extension_list */
    if (vgmstream_ctx_is_valid(filename_utf8, &cfg) != 0) {
        return true;
    }

    const char* ext = filename_extension(fn);

    if (ext == nullptr || *ext == '\0') {
        return false;
    }

    const char** ext_list;
    size_t ext_list_len;
    int i;

    ext_list = vgmstream_get_formats(&ext_list_len);

    for (i = 0; i < ext_list_len; i++) {
        if (strcmp(ext, ext_list[i]) == 0) {
            return true;
        }
    }

    return false;
}

int VgmstreamInPlugin::play(char* fn)
{
    in_char filename[PATH_LIMIT];
    int stream_index = 0;
    int max_latency;

    /* shouldn't happen */
    if (vgmstream) {
        return 1;
    }

    /* check for info encoded in the filename */
    parse_fn_string(fn, nullptr, filename, PATH_LIMIT);
    parse_fn_int(fn, wa_L("$s"), &stream_index);

    /* open the stream */
    vgmstream = init_vgmstream_winamp(filename, stream_index);
    if (!vgmstream)
        return 1;

    /* config */
    set_config_defaults(&config);
    apply_config(vgmstream, &config);

    /* enable after all config but before outbuf (though ATM outbuf is not dynamic so no need to read input_channels) */
    vgmstream_mixing_autodownmix(vgmstream, settings.downmix_channels);
    vgmstream_mixing_enable(vgmstream, SAMPLE_BUFFER_SIZE, nullptr /*&input_channels*/, &state.output_channels);

    /* reset internals */
    state.paused = 0;
    state.decode_abort = 0;
    state.seek_needed_samples = -1;
    state.decode_pos_ms = 0;
    state.decode_pos_samples = 0;
    state.stream_length_samples = get_vgmstream_play_samples(config.song_loop_count, config.song_fade_time, config.song_fade_delay, vgmstream);
    state.fade_samples = (int)(config.song_fade_time * vgmstream->sample_rate);
    state.volume = get_album_gain_volume(fn);

    /* open the output plugin */
    max_latency = outPlugin->getModule()->Open(vgmstream->sample_rate, state.output_channels, 16, 0, 0);
    if (max_latency < 0) {
        close_vgmstream(vgmstream);
        vgmstream = nullptr;
        return 1;
    }

    /* start */
    return winamp_Start_thread();
}

void VgmstreamInPlugin::pause()
{
    state.paused = 1;
    outPlugin->getModule()->Pause(1);
}

void VgmstreamInPlugin::unPause()
{
    state.paused = 0;
    outPlugin->getModule()->Pause(0);
}

int VgmstreamInPlugin::isPaused()
{
    return state.paused;
}

void VgmstreamInPlugin::stop()
{
    winamp_Stop_thread();

    close_vgmstream(vgmstream);
    vgmstream = nullptr;

    outPlugin->getModule()->Close();
}

int VgmstreamInPlugin::getLength()
{
    if (!vgmstream) {
        return 0;
    }

    return state.stream_length_samples * 1000LL / vgmstream->sample_rate;
}

int VgmstreamInPlugin::getOutputTime()
{
    return state.decode_pos_ms + (outPlugin->getModule()->GetOutputTime() - outPlugin->getModule()->GetWrittenTime());
}

void VgmstreamInPlugin::setOutputTime(int time_in_ms)
{
    if (!vgmstream) {
        return;
    }

    state.seek_needed_samples = (long long)time_in_ms * vgmstream->sample_rate / 1000LL;
}

bool VgmstreamInPlugin::canDuplicate() const
{
    return true;
}

void VgmstreamInPlugin::duplicate()
{
    cancelDuplicate();

    if (!vgmstream) {
        return;
    }

    winamp_Stop_thread();

    // Save current vgmstream
    dup_vgmstream = vgmstream;
    memcpy(&dup_config, &config, sizeof(config));
    memcpy(&dup_state, &state, sizeof(state));
    dup_state.decode_abort = 0;

    vgmstream = nullptr;

    outPlugin->getContext()->Duplicate();
}

int VgmstreamInPlugin::resume(char* fn)
{
    int max_latency;

    if (!dup_vgmstream) {
        if (vgmstream) {
            stop();
        }

        return play(fn);
    }

    if (vgmstream) {
        winamp_Stop_thread();
        close_vgmstream(vgmstream);
    }

    // Restore dup_vgmstream
    vgmstream = dup_vgmstream;
    memcpy(&config, &dup_config, sizeof(config));
    memcpy(&state, &dup_state, sizeof(state));

    dup_vgmstream = nullptr;

    max_latency = outPlugin->getContext()->Resume(vgmstream->sample_rate, state.output_channels, 16, 0, 0);
    if (max_latency < 0) {
        close_vgmstream(vgmstream);
        vgmstream = nullptr;
        return true;
    }

    return winamp_Start_thread();
}

bool VgmstreamInPlugin::cancelDuplicate()
{
    if (!dup_vgmstream) {
        return false;
    }

    close_vgmstream(dup_vgmstream);
    dup_vgmstream = nullptr;

    return outPlugin->getContext()->CancelDuplicate();
}
