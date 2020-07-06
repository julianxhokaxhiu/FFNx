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

#pragma once

#include "plugin.h"

extern "C" {
#include <vgmstream.h>
}

/* Winamp needs at least 576 16-bit samples, stereo, doubled in case DSP effects are active */
constexpr auto SAMPLE_BUFFER_SIZE = 2048;

 /* current song config */
struct winamp_song_config {
	int song_play_forever;
	double song_loop_count;
	double song_fade_time;
	double song_fade_delay;
	int song_ignore_loop;
	int song_really_force_loop;
	int song_ignore_fade;
};

/* current play state */
struct winamp_state_t {
	int paused;
	int decode_abort;
	int seek_needed_samples;
	int decode_pos_ms;
	int decode_pos_samples;
	int stream_length_samples;
	int fade_samples;
	int output_channels;
	double volume;
};

class VgmstreamInPlugin : public AbstractInPlugin {
private:
	HANDLE decode_thread_handle;
	VGMSTREAM* vgmstream, *dup_vgmstream;
	winamp_song_config config, dup_config;
	winamp_state_t state, dup_state;
	short sample_buffer[SAMPLE_BUFFER_SIZE * 2 * VGMSTREAM_MAX_CHANNELS];

	int startThread();
	void stopThread();
public:
	VgmstreamInPlugin(AbstractOutPlugin* outPlugin);
	virtual ~VgmstreamInPlugin();
	int decodeLoop();

	bool accept(const char* fn) const;

	int play(char* fn);
	void pause();			// pause stream
	void unPause();			// unpause stream
	int isPaused();			// ispaused? return 1 if paused, 0 if not
	void stop();				// stop (unload) stream

	// time stuff
	int getLength();			// get length in ms
	int getOutputTime();		// returns current output time in ms. (usually returns outMod->GetOutputTime()
	void setOutputTime(int time_in_ms);	// seeks to point in stream (in ms). Usually you signal your thread to seek, which seeks and calls outMod->Flush()..

	// Resuming (not part of standard Winamp plugin)
	bool canDuplicate() const;
	void duplicate();
	int resume(char* fn);
	bool cancelDuplicate();

	// Looping (not part of standard Winamp plugin)
	void setLoopingEnabled(bool enabled);
};
