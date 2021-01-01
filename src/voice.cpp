/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
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

#include "voice.h"

#include "audio.h"
#include "field.h"
#include "patch.h"

int (*opcode_old_message)();
int (*opcode_old_ask)(int);

DWORD previous_master_music_volume = 0x64; // Assume maximum by default
void (*set_master_music_volume)(uint32_t);
float voice_volume = -1.0f;

//=============================================================================

void begin_voice()
{
	voice_volume = 2.0f + (100 - external_voice_music_fade_volume) / 100.0f;

	if (enable_voice_music_fade)
	{
		if (use_external_music)
		{
			float new_master_volume = external_voice_music_fade_volume / 100.0f;

			if (new_master_volume < nxAudioEngine.getMusicMasterVolume())
				nxAudioEngine.setMusicMasterVolume(new_master_volume, 1);
		}
		else
		{
			if (external_voice_music_fade_volume < *common_externals.master_midi_volume)
			{
				previous_master_music_volume = *common_externals.master_midi_volume;
				set_master_music_volume(external_voice_music_fade_volume);
			}
		}
	}
}

bool play_voice(char* field_name, byte dialog_id, byte page_count)
{
	char name[MAX_PATH];

	char page = 'a' + page_count;
	if (page > 'z') page = 'z';
	sprintf(name, "%s/%u%c", field_name, dialog_id, page);

	if (!nxAudioEngine.canPlayVoice(name) && page_count == 0)
		sprintf(name, "%s/%u", field_name, dialog_id);

	return nxAudioEngine.playVoice(name, voice_volume);
}

void play_option(char* field_name, byte dialog_id, byte option_count)
{
	char name[MAX_PATH];

	sprintf(name, "%s/%u_%u", field_name, dialog_id, option_count);

	nxAudioEngine.playVoice(name, voice_volume);
}

void end_voice(uint32_t time = 0)
{
	nxAudioEngine.stopVoice(time);

	if (enable_voice_music_fade)
	{
		if (use_external_music)
			nxAudioEngine.restoreMusicMasterVolume(time > 0 ? time : 1);
		else
			set_master_music_volume(previous_master_music_volume);
	}
}

//=============================================================================

bool is_dialog_opening(byte code)
{
	return (code == 0);
}

bool is_dialog_starting(byte old_code, byte new_code)
{
	return (
		old_code == 0 && old_code != new_code
	);
}

bool is_dialog_paging(byte old_code, byte new_code)
{
	return (
		(old_code == 14 && new_code == 2) ||
		(old_code == 4 && new_code == 8)
	);
}

bool is_dialog_closing(byte old_code, byte new_code)
{
	return (
		old_code != new_code && new_code == 7
	);
}

bool is_dialog_closed(byte old_code, byte new_code)
{
	return (
		old_code == 7 && new_code == 0
	);
}

byte get_dialog_opcode(byte window_id)
{
	return ff7_externals.opcode_message_loop_code[24 * window_id];
}

//=============================================================================

int opcode_voice_message()
{
	static byte message_page_count = 0;
	static WORD message_last_opcode = 0;
	static bool is_voice_acting = false;

	byte window_id = get_field_parameter(0);
	byte dialog_id = get_field_parameter(1);
	byte message_current_opcode = get_dialog_opcode(window_id);
	char* field_name = get_current_field_name();

	bool _is_dialog_opening = is_dialog_opening(message_current_opcode);
	bool _is_dialog_starting = is_dialog_starting(message_last_opcode, message_current_opcode);
	bool _is_dialog_paging = is_dialog_paging(message_last_opcode, message_current_opcode);
	bool _is_dialog_closing = is_dialog_closing(message_last_opcode, message_current_opcode);
	bool _is_dialog_closed = is_dialog_closed(message_last_opcode, message_current_opcode);

	if (_is_dialog_paging) message_page_count++;

	if (_is_dialog_opening)
	{
		message_page_count = 0;
		begin_voice();
	}
	if (_is_dialog_starting || _is_dialog_paging)
	{
		is_voice_acting = play_voice(field_name, dialog_id, message_page_count);
		if (trace_all || trace_opcodes) trace("opcode[MESSAGE]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u\n", field_name, window_id, dialog_id, message_page_count);
	}
	else if (_is_dialog_closing)
	{
		end_voice();
	}

	// Auto close the message if it was voice acted and the audio file has finished playing
	if (is_voice_acting && !nxAudioEngine.isVoicePlaying())
	{
		is_voice_acting = false;
		simulate_OK_button = true;
	}

	message_last_opcode = message_current_opcode;

	return opcode_old_message();
}

int opcode_voice_ask(int unk)
{
	static byte message_page_count = 0;
	static WORD message_last_opcode = 0;
	static WORD message_last_option = 0;

	byte window_id = get_field_parameter(1);
	byte dialog_id = get_field_parameter(2);
	byte message_current_opcode = get_dialog_opcode(window_id);
	byte message_current_option = (ff7_externals.opcode_ask_question_code[24 * window_id] - 6) / 16;
	char* field_name = get_current_field_name();

	bool _is_dialog_opening = is_dialog_opening(message_current_opcode);
	bool _is_dialog_starting = is_dialog_starting(message_last_opcode, message_current_opcode);
	bool _is_dialog_paging = is_dialog_paging(message_last_opcode, message_current_opcode);
	bool _is_dialog_closing = is_dialog_closing(message_last_opcode, message_current_opcode);
	bool _is_dialog_closed = is_dialog_closed(message_last_opcode, message_current_opcode);
	bool _is_dialog_option_changed = (message_last_option != message_current_option);

	if (_is_dialog_paging) message_page_count++;

	if (_is_dialog_opening)
	{
		message_page_count = 0;
		begin_voice();
	}
	if (_is_dialog_starting || _is_dialog_paging)
	{
		play_voice(field_name, dialog_id, message_page_count);
		if (trace_all || trace_opcodes) trace("opcode[ASK]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u\n", field_name, window_id, dialog_id, message_page_count);
	}
	else if (_is_dialog_option_changed)
	{
		play_option(field_name, dialog_id, message_current_option);
		if (trace_all || trace_opcodes) trace("opcode[ASK]: field=%s,window_id=%u,dialog_id=%u,option_id=%u\n", field_name, window_id, dialog_id, message_current_option);
	}
	else if (_is_dialog_closing)
	{
		end_voice();
	}

	message_last_option = message_current_option;
	message_last_opcode = message_current_opcode;

	return opcode_old_ask(unk);
}

void voice_init()
{
	if (!ff8)
	{
		set_master_music_volume = (void (*)(uint32_t))common_externals.set_master_midi_volume;

		opcode_old_message = (int (*)())ff7_externals.opcode_message;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x40], (DWORD)&opcode_voice_message);

		opcode_old_ask = (int (*)(int))ff7_externals.opcode_ask;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x48], (DWORD)&opcode_voice_ask);
	}
}
