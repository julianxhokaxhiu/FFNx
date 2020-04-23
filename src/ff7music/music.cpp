/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Lesser General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "music.h"

uint current_id = 0;

// send a message to be intercepted by FF7Music
void send_ff7music(char *fmt, ...)
{
	va_list args;
	char msg[0x100];

	va_start(args, fmt);

	vsnprintf(msg, sizeof(msg), fmt, args);

	OutputDebugStringA(msg);
}

// start playing some music, <midi> is the name of the MIDI file without the .mid extension
void ff7music_play_music(char *midi, uint id)
{
	if(id == current_id) return;

	send_ff7music("reading midi file: %s.mid\n", midi);

	current_id = id;
}

void ff7music_stop_music()
{
	send_ff7music("MIDI stop\n");
}

// cross fade to a new song
void ff7music_cross_fade_music(char *midi, uint id, uint time)
{
	if(id == current_id) return;

	ff7music_stop_music();
	ff7music_play_music(midi, id);
}

void ff7music_pause_music()
{
	
}

void ff7music_resume_music()
{
	
}

// return true if music is playing, false if it isn't
// it's important for some field scripts that this function returns true atleast once when a song has been requested
// 
// even if there's nothing to play because of errors/missing files you cannot return false every time
bool ff7music_music_status()
{
	static bool dummy = true;

	dummy = !dummy;

	return dummy;
}

void ff7music_set_master_music_volume(uint volume)
{
	
}

void ff7music_set_music_volume(uint volume)
{
	
}

// make a volume transition
void ff7music_set_music_volume_trans(uint volume, uint step)
{
	uint from = volume ? 0 : 127;

	send_ff7music("MIDI set volume trans: %d->%d; step=%d\n", from, volume, step);
}

void ff7music_set_music_tempo(unsigned char tempo)
{
	
}

void ff7music_music_cleanup()
{
	ff7music_stop_music();
}