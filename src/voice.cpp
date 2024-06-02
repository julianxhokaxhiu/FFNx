/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include "ff8/engine.h"

#include <iomanip>
#include <sstream>
#include <queue>

enum class display_type
{
	NONE = 0,
	DIALOGUE,
	CHAR_CMD
};

enum class message_kind
{
	NONE = 0,
	ASK,
	MESSAGE,
	DRAWPOINT
};

struct battle_text_aux_data{
	bool has_started {false};
	bool follow_voice {false};
	display_type text_type {display_type::NONE};
	int page_count {0};
	uint16_t enemy_id;
	cmd_id command_id;
	byte char_id;
};

struct opcode_message_status
{
	opcode_message_status() :
		// Common
		message_page_count(0),
		message_last_opcode(0),
		is_voice_acting(false),
		message_last_transition(0),
		message_last_option(UCHAR_MAX),
		message_dialog_id(0),
		char_id(0),
		message_kind(message_kind::NONE),
		field_name("")
	{}
	byte message_page_count = 0;
	WORD message_last_opcode = 0;
	bool is_voice_acting = false;
	WORD message_last_transition = 0;
	byte message_last_option = UCHAR_MAX;
	int message_dialog_id = 0;
	byte char_id = 0;
	message_kind message_kind = message_kind::NONE;
	std::string field_name = "";
};

// FF7
int (*opcode_old_message)();
int (*opcode_old_ask)(int);
int (*opcode_old_wmode)();
int (*opcode_wm_old_message)(uint8_t,uint8_t);
int (*opcode_wm_old_ask)(uint8_t,uint8_t,uint8_t,uint8_t,WORD*);
int (*opcode_old_tutor)();
void (*ff7_set_master_music_volume)(uint32_t);

// FF8
int (*ff8_opcode_old_mes)(int);
int (*ff8_opcode_old_ames)(int);
int (*ff8_opcode_old_amesw)(int);
int (*ff8_opcode_old_ramesw)(int);
int (*ff8_opcode_old_ask)(int);
int (*ff8_opcode_old_aask)(int);
int (*ff8_opcode_old_drawpoint)(int);

int ff8_current_window_dialog_id = -1;
std::map<int, int> ff8_field_window_stack_count;
std::map<int, char*> ff8_battle_actor_name;

// COMMON
byte opcode_ask_current_option = UCHAR_MAX;

std::map<int,bool> simulate_OK_disabled;

DWORD previous_master_music_volume = 0x64; // Assume maximum by default
float voice_volume = -1.0f;

std::array<battle_text_aux_data, 64> other_battle_display_text_queue;
std::queue<short> display_string_actor_queue;
std::map<int, opcode_message_status> current_opcode_message_status;

//=============================================================================

void set_voice_volume()
{
	voice_volume = 2.0f + (100 - external_voice_music_fade_volume) / 100.0f;
}

void begin_voice(byte window_id = 0)
{
	current_opcode_message_status[window_id] = opcode_message_status();

	set_voice_volume();

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

				if (ff8)
					ff8_externals.dmusicperf_set_volume_sub_46C6F0(floor(127 * (external_voice_music_fade_volume / 100.0f)), 0);
				else
					ff7_set_master_music_volume(external_voice_music_fade_volume);
			}
		}
	}
}

bool play_voice(char* field_name, byte window_id, byte dialog_id, byte page_count)
{
	char name[MAX_PATH];

	char page = 'a' + page_count;
	if (page > 'z') page = 'z';

	sprintf(name, "%s/w%u_%u%c", field_name, window_id, dialog_id, page);

	if (!nxAudioEngine.canPlayVoice(name))
		sprintf(name, "%s/%u%c", field_name, dialog_id, page);

	if (!nxAudioEngine.canPlayVoice(name) && page_count == 0)
	{
		sprintf(name, "%s/w%u_%u", field_name, window_id, dialog_id);

		if (!nxAudioEngine.canPlayVoice(name))
			sprintf(name, "%s/%u", field_name, dialog_id);
	}

	return nxAudioEngine.playVoice(name, window_id, voice_volume);
}

bool play_battle_dialogue_voice(short enemy_id, std::string tokenized_dialogue)
{
	char name[MAX_PATH];

	sprintf(name, "_battle/enemy_%04X/%s", enemy_id, tokenized_dialogue.c_str());

	return nxAudioEngine.playVoice(name, 0, voice_volume);
}

bool play_battle_cmd_voice(byte char_id, cmd_id command_id, std::string tokenized_dialogue, int page_count)
{
	char name[MAX_PATH];
	bool playing;

	char page = 'a' + page_count;
	if (page > 'z') page = 'z';
	sprintf(name, "_battle/char_%02X/cmd_%02X_", char_id, command_id);

	switch(command_id)
	{
	case cmd_id::CMD_MANIPULATE:
		// 2 cases: manipulated, couldnt
	case cmd_id::CMD_STEAL:
	case cmd_id::CMD_MUG:
		// 3 cases: nothing, couldnt, stole
		sprintf(name + strlen(name), "%s", split(tokenized_dialogue, "_")[0].c_str());
		break;
	default:
		sprintf(name + strlen(name), "%c", page);
		break;
	}

	playing = nxAudioEngine.playVoice(name, 0, voice_volume);

	if(!playing && page_count == 0)
	{
		sprintf(name, "_battle/char_%02X/cmd_%02X", char_id, command_id);
		playing = nxAudioEngine.playVoice(name, 0, voice_volume);
	}

	return playing;
}

bool play_battle_char_action_voice(byte char_id, byte command_id, short action_id)
{
	char name[MAX_PATH];
	bool playing;

	sprintf(name, "_battle/char_%02X/cmd_%02X_%04X", char_id, command_id, action_id);
	playing = nxAudioEngine.playVoice(name, 0, voice_volume);

	if(!playing)
	{
		sprintf(name, "_battle/char_%02X/cmd_%02X", char_id, command_id);
		playing = nxAudioEngine.playVoice(name, 0, voice_volume);
	}

	return playing;
}

bool play_battle_enemy_action_voice(uint16_t enemy_id, byte command_id, short action_id)
{
	char name[MAX_PATH];
	bool playing;

	sprintf(name, "_battle/enemy_%04X/cmd_%02X_%04X", enemy_id, command_id, action_id);
	playing = nxAudioEngine.playVoice(name, 0, voice_volume);

	if(!playing)
	{
		sprintf(name, "_battle/enemy_%04X/cmd_%02X", enemy_id, command_id);
		playing = nxAudioEngine.playVoice(name, 0, voice_volume);
	}

	return playing;
}

void play_option(char* field_name, byte window_id, byte dialog_id, byte option_count)
{
	char name[MAX_PATH];

	sprintf(name, "%s/%u_%u", field_name, dialog_id, option_count);

	nxAudioEngine.playVoice(name, window_id, voice_volume);
}

void end_voice(byte window_id = 0, uint32_t time = 0)
{
	nxAudioEngine.stopVoice(window_id, time);

	if (enable_voice_music_fade)
	{
		if (use_external_music)
			nxAudioEngine.restoreMusicMasterVolume(time > 0 ? time : 1);
		else if (ff8)
			ff8_externals.dmusicperf_set_volume_sub_46C6F0(previous_master_music_volume, 0);
		else
			ff7_set_master_music_volume(previous_master_music_volume);
	}
}

//=============================================================================

bool is_dialog_opening(WORD code)
{
	return (code == 0);
}

bool is_dialog_starting(WORD old_code, WORD new_code)
{
	return (
		old_code == 0 && old_code != new_code
	);
}

bool is_dialog_paging(WORD old_code, WORD new_code)
{
	if (ff8)
		return (
			(old_code == 10 && new_code == 1)
		);
	else
		return (
			(old_code == 14 && new_code == 2) ||
			(old_code == 4 && new_code == 8)
		);
}

bool is_dialog_closing(WORD old_code, WORD new_code)
{
	if (ff8)
		return (
			new_code < old_code
		);
	else
		return (
			old_code != new_code && new_code == 7
		);
}

bool is_dialog_closed(WORD old_code, WORD new_code)
{
	if (ff8)
		return (
			new_code < old_code && new_code == 0
		);
	else
		return (
			old_code == 7 && new_code == 0
		);
}

byte get_dialog_opcode(byte window_id)
{
	return ff7_externals.opcode_message_loop_code[24 * window_id];
}

//=============================================================================

// -- FIELD --

byte get_field_dialog_char_id(byte window_id) {
	if (ff7_externals.field_entity_id_list[window_id] != *ff7_externals.current_entity_id) return 0;

	if (ff7_externals.field_entity_id_list[window_id] == 0x0) return 0;

	byte id = *(byte*)ff7_externals.current_dialog_string_pointer[window_id];

	switch(id) {
		case 0xEA: // CLOUD
		case 0xEB: // BARRET
		case 0xEC: // TIFA
		case 0xED: // AERITH
		case 0xEE: // RED XIII
		case 0xEF: // YUFFIE
		case 0xF0: // CAIT SITH
		case 0xF1: // VINCENT
		case 0xF2: // CID
			return id;
		default:
			return 0;
	}
}

void ff7_handle_wmode_reset()
{
	struct game_mode *mode = getmode_cached();
	static WORD last_field_id = 0;
	uint32_t last_driver_mode = 0;

	if (last_field_id != *ff7_externals.field_id)
	{
		last_field_id = *ff7_externals.field_id;

		simulate_OK_disabled.clear();
	}
	else if (last_driver_mode != mode->driver_mode)
	{
		last_driver_mode = mode->driver_mode;

		simulate_OK_disabled.clear();
	}
}

int opcode_wmode()
{
	byte window_id = get_field_parameter<byte>(0);
	byte window_permanent = get_field_parameter<byte>(2);

	simulate_OK_disabled[window_id] = window_permanent;

	return opcode_old_wmode();
}

int opcode_voice_message()
{
	byte window_id = get_field_parameter<byte>(0);
	byte dialog_id = get_field_parameter<byte>(1);
	byte message_current_opcode = get_dialog_opcode(window_id);
	char* field_name = get_current_field_name();

	bool _is_dialog_opening = is_dialog_opening(message_current_opcode);
	bool _is_dialog_starting = is_dialog_starting(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_paging = is_dialog_paging(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_closing = is_dialog_closing(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_closed = is_dialog_closed(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);

	if (_is_dialog_paging) current_opcode_message_status[window_id].message_page_count++;

	if (_is_dialog_opening)
	{
		begin_voice(window_id);
	}
	else if (_is_dialog_starting || _is_dialog_paging)
	{
		if (_is_dialog_starting) current_opcode_message_status[window_id].char_id = get_field_dialog_char_id(window_id);
		if (trace_all || trace_opcodes) ffnx_trace("opcode[MESSAGE]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u,char=%X\n", field_name, window_id, dialog_id, current_opcode_message_status[window_id].message_page_count, current_opcode_message_status[window_id].char_id);
		current_opcode_message_status[window_id].is_voice_acting = play_voice(field_name, window_id, dialog_id, current_opcode_message_status[window_id].message_page_count);
	}
	else if (_is_dialog_closing)
	{
		end_voice(window_id);
		simulate_OK_disabled[window_id] = false;
		current_opcode_message_status[window_id].is_voice_acting = false;
	}

	// Auto close the message if it was voice acted and the audio file has finished playing
	if (current_opcode_message_status[window_id].is_voice_acting && !nxAudioEngine.isVoicePlaying(window_id))
	{
		current_opcode_message_status[window_id].is_voice_acting = false;
		if (!simulate_OK_disabled[window_id] && enable_voice_auto_text) simulate_OK_button = true;
	}

	current_opcode_message_status[window_id].message_last_opcode = message_current_opcode;

	return opcode_old_message();
}

int opcode_voice_tutor()
{
	static const int window_id = 0;

	current_opcode_message_status[window_id].message_dialog_id = get_field_parameter<byte>(0);

	return opcode_old_tutor();
}

int opcode_voice_parse_options(uint8_t window_id, uint8_t dialog_id, uint8_t first_question_id, uint8_t last_question_id, WORD *current_question_id)
{
	opcode_ask_current_option = *current_question_id;

	return ff7_externals.field_opcode_ask_update_loop_6310A1(window_id, dialog_id, first_question_id, last_question_id, current_question_id);
}

int opcode_voice_ask(int unk)
{
	byte window_id = get_field_parameter<byte>(1);
	byte dialog_id = get_field_parameter<byte>(2);
	byte message_current_opcode = get_dialog_opcode(window_id);
	char* field_name = get_current_field_name();

	bool _is_dialog_opening = is_dialog_opening(message_current_opcode);
	bool _is_dialog_starting = is_dialog_starting(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_paging = is_dialog_paging(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_closing = is_dialog_closing(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_closed = is_dialog_closed(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_option_changed = (current_opcode_message_status[window_id].message_last_option != opcode_ask_current_option);

	if (_is_dialog_paging) current_opcode_message_status[window_id].message_page_count++;

	if (_is_dialog_opening)
	{
		opcode_ask_current_option = UCHAR_MAX;
		begin_voice(window_id);
	}
	else if (_is_dialog_starting || _is_dialog_paging)
	{
		if (_is_dialog_starting) current_opcode_message_status[window_id].char_id = get_field_dialog_char_id(window_id);
		if (trace_all || trace_opcodes) ffnx_trace("opcode[ASK]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u,char=%X\n", field_name, window_id, dialog_id, current_opcode_message_status[window_id].message_page_count, current_opcode_message_status[window_id].char_id);
		current_opcode_message_status[window_id].is_voice_acting = play_voice(field_name, window_id, dialog_id, current_opcode_message_status[window_id].message_page_count);
	}
	else if (_is_dialog_option_changed)
	{
		if (trace_all || trace_opcodes) ffnx_trace("opcode[ASK]: field=%s,window_id=%u,dialog_id=%u,option_id=%u,char=%X\n", field_name, window_id, dialog_id, opcode_ask_current_option,current_opcode_message_status[window_id].char_id);
		play_option(field_name, window_id, dialog_id, opcode_ask_current_option);
	}
	else if (_is_dialog_closing)
	{
		end_voice(window_id);
	}

	current_opcode_message_status[window_id].message_last_opcode = message_current_opcode;
	current_opcode_message_status[window_id].message_last_option = opcode_ask_current_option;

	return opcode_old_ask(unk);
}

// -- WORLDMAP --

int opcode_wm_message(uint8_t window_id, uint8_t dialog_id) {
	byte message_current_opcode = get_dialog_opcode(window_id);

	bool _is_dialog_opening = is_dialog_opening(message_current_opcode);
	bool _is_dialog_starting = is_dialog_starting(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_paging = is_dialog_paging(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_closing = is_dialog_closing(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_closed = is_dialog_closed(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);

	if (_is_dialog_paging) current_opcode_message_status[window_id].message_page_count++;

	if (_is_dialog_opening)
	{
		begin_voice(window_id);
		current_opcode_message_status[window_id].message_dialog_id = dialog_id;
	}
	else if (_is_dialog_starting || _is_dialog_paging)
	{
		if (trace_all || trace_opcodes) ffnx_trace("wm_opcode[MESSAGE]: window_id=%u,dialog_id=%u,paging_id=%u\n", window_id, current_opcode_message_status[window_id].message_dialog_id, current_opcode_message_status[window_id].message_page_count);
		current_opcode_message_status[window_id].is_voice_acting = play_voice("_world", window_id, current_opcode_message_status[window_id].message_dialog_id, current_opcode_message_status[window_id].message_page_count);
	}
	else if (_is_dialog_closing)
	{
		end_voice(window_id);
		simulate_OK_disabled[window_id] = false;
		current_opcode_message_status[window_id].is_voice_acting = false;
	}

	// Auto close the message if it was voice acted and the audio file has finished playing
	if (current_opcode_message_status[window_id].is_voice_acting && !nxAudioEngine.isVoicePlaying(window_id))
	{
		current_opcode_message_status[window_id].is_voice_acting = false;
		if (!simulate_OK_disabled[window_id] && enable_voice_auto_text) (*ff7_externals.field_global_object_ptr)->field_80 |= 0x20;
	}

	current_opcode_message_status[window_id].message_last_opcode = message_current_opcode;

	return opcode_wm_old_message(window_id, dialog_id);
}

int opcode_wm_ask(uint8_t window_id, uint8_t dialog_id, uint8_t first_question_id, uint8_t last_question_id, WORD *current_question_id) {
	byte message_current_opcode = get_dialog_opcode(window_id);

	bool _is_dialog_opening = is_dialog_opening(message_current_opcode);
	bool _is_dialog_starting = is_dialog_starting(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_paging = is_dialog_paging(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_closing = is_dialog_closing(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_closed = is_dialog_closed(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_option_changed = (current_opcode_message_status[window_id].message_last_option != *current_question_id);

	if (_is_dialog_paging) current_opcode_message_status[window_id].message_page_count++;

	if (_is_dialog_opening)
	{
		begin_voice(window_id);
	}
	else if (_is_dialog_starting || _is_dialog_paging)
	{
		if (trace_all || trace_opcodes) ffnx_trace("wm_opcode[ASK]: window_id=%u,dialog_id=%u,paging_id=%u\n", window_id, dialog_id, current_opcode_message_status[window_id].message_page_count);
		current_opcode_message_status[window_id].message_dialog_id = dialog_id;
		current_opcode_message_status[window_id].is_voice_acting = play_voice("_world", window_id, dialog_id, current_opcode_message_status[window_id].message_page_count);
	}
	else if (_is_dialog_option_changed)
	{
		if (trace_all || trace_opcodes) ffnx_trace("wm_opcode[ASK]: window_id=%u,dialog_id=%u,option_id=%u\n", window_id, current_opcode_message_status[window_id].message_dialog_id, *current_question_id);
		play_option("_world", window_id, current_opcode_message_status[window_id].message_dialog_id, *current_question_id);
	}
	else if (_is_dialog_closing)
	{
		end_voice(window_id);
	}

	current_opcode_message_status[window_id].message_last_opcode = message_current_opcode;
	current_opcode_message_status[window_id].message_last_option = *current_question_id;

	return opcode_wm_old_ask(window_id, dialog_id, first_question_id, last_question_id, current_question_id);
}

// -- BATTLE --

std::string decode_ff7_text(const char *encoded_text)
{
	std::string decoded_text{};
	int index = 0;
	char current_char;
	while (current_char = encoded_text[index++], current_char != char(0xFF))
	{
		switch (current_char)
		{
		case 0xEB:
			decoded_text.append("{item_name}");
			index += 3;
			break;
		case 0xEC:
			decoded_text.append("{number}");
			index += 3;
			break;
		case 0xED:
			decoded_text.append("{target_name}");
			index += 3;
			break;
		case 0xEE:
			decoded_text.append("{attack_name}");
			index += 3;
			break;
		case 0xEF:
			decoded_text.append("{special_number}");
			index += 3;
			break;
		case 0xF0:
			decoded_text.append("{target_letter}");
			index += 3;
			break;
		case 0xF8:
			index += 2;
			break;
		default:
			decoded_text.push_back(current_char + 0x20);
			break;
		}
	}
	return decoded_text;
}

std::string tokenize_text(std::string decoded_text)
{
	transform(decoded_text.begin(), decoded_text.end(), decoded_text.begin(), tolower);

	std::string filename{};
	for(auto current_char : decoded_text)
	{
		if(current_char >= 'a' && current_char <= 'z')
			filename += current_char;
		else if(current_char >= '0' && current_char <= '9')
			filename += current_char;
		else if(current_char == ' ')
			filename += '_';
		else if(current_char == '{' || current_char == '}')
			filename += current_char;
	}
	return filename;
}

void ff7_enqueue_script_display_string(short actor_id, byte command_index, uint16_t rel_attack_index)
{
	display_string_actor_queue.push(actor_id);

	if (trace_all || trace_battle_text) ffnx_trace("Push display string for actorId: %d\n", actor_id);

	((void (*)(short, byte, uint16_t))ff7_externals.enqueue_script_action)(actor_id, command_index, rel_attack_index);
}

void ff7_add_text_to_display_queue(WORD buffer_idx, byte wait_frames, byte n_frames, WORD param_4)
{
	auto text_data = std::find_if(ff7_externals.battle_display_text_queue.begin(), ff7_externals.battle_display_text_queue.end(),
								  [](const battle_text_data &data)
								  { return data.buffer_idx == -1; });

	if (text_data != ff7_externals.battle_display_text_queue.end())
	{
		text_data->buffer_idx = buffer_idx;
		text_data->field_2 = param_4;
		text_data->wait_frames = wait_frames * battle_frame_multiplier - 1;
		int n_frames_int = (n_frames == 0) ? ((int (*)())ff7_externals.get_n_frames_display_action_string)() : n_frames * battle_frame_multiplier;
		text_data->n_frames = std::min(n_frames_int, UCHAR_MAX); // safely cast int to unsigned char

		if (trace_all || trace_battle_text)
			ffnx_trace("Add text string to be displayed: (text_id: %d, field_2: %d, wait_frames: %d, n_frames: %d)\n", text_data->buffer_idx, text_data->field_2, text_data->wait_frames, text_data->n_frames);

		int index = std::distance(ff7_externals.battle_display_text_queue.begin(), text_data);
		byte attacker_id = ff7_externals.anim_event_queue[*ff7_externals.anim_event_index].attackerID;
		if (attacker_id == 10 && n_frames == 0 && buffer_idx >= 256)
		{
			other_battle_display_text_queue[index].has_started = false;
			other_battle_display_text_queue[index].follow_voice = true;
			other_battle_display_text_queue[index].text_type = display_type::DIALOGUE;
			if (!display_string_actor_queue.empty())
			{
				short actor_id = display_string_actor_queue.front();
				display_string_actor_queue.pop();
				other_battle_display_text_queue[index].enemy_id = ff7_externals.battle_context->actor_vars[actor_id].formationID;
			}
		}
		else
		{
			if (attacker_id == 10)
				attacker_id = ff7_externals.anim_event_queue[0].attackerID;

			other_battle_display_text_queue[index].has_started = false;
			other_battle_display_text_queue[index].follow_voice = false;
			other_battle_display_text_queue[index].page_count = index;
			other_battle_display_text_queue[index].command_id = static_cast<cmd_id>(ff7_externals.battle_context->lastCommandIdx);

			if (attacker_id >= 0 && attacker_id <= 2)
			{
				other_battle_display_text_queue[index].text_type = display_type::CHAR_CMD;
				other_battle_display_text_queue[index].char_id = ff7_externals.battle_context->actor_vars[attacker_id].index;
			}
			else
			{
				other_battle_display_text_queue[index].text_type = display_type::NONE;
			}
		}
	}
}

void ff7_update_display_text_queue()
{
	*ff7_externals.field_battle_word_BF2E08 = *ff7_externals.field_battle_word_BF2E08 | 2;
	auto &text_data_first = ff7_externals.battle_display_text_queue.front();
	auto &other_text_data_first = other_battle_display_text_queue.front();

	if (text_data_first.buffer_idx != -1)
	{
		if (text_data_first.wait_frames == 0)
		{
			// Begin voice
			if(!other_text_data_first.has_started)
			{
				const char *encoded_text = ff7_externals.get_kernel_text(8, text_data_first.buffer_idx, 8);
				std::string decoded_text = decode_ff7_text(encoded_text);
				std::string tokenized_dialogue;

				begin_voice();
				switch (other_text_data_first.text_type)
				{
				case display_type::DIALOGUE:
					tokenized_dialogue = tokenize_text(decoded_text);
					other_text_data_first.has_started = play_battle_dialogue_voice(other_text_data_first.enemy_id, tokenized_dialogue);

					if (trace_all || trace_battle_text)
						ffnx_trace("Begin voice of EnemyID: %04X for text: %s (filename: %s)\n", other_text_data_first.enemy_id, decoded_text.c_str(), tokenized_dialogue.c_str());

					break;
				case display_type::CHAR_CMD:
					tokenized_dialogue = tokenize_text(decoded_text);
					other_text_data_first.has_started = play_battle_cmd_voice(other_text_data_first.char_id, other_text_data_first.command_id,
																			  tokenized_dialogue, other_text_data_first.page_count);

					if (trace_all || trace_battle_text)
						ffnx_trace("Begin voice for (character ID: %d; command ID: %X) [filename: %s]\n",
								   other_text_data_first.char_id, other_text_data_first.command_id, tokenized_dialogue.c_str());

					break;
				default:
					break;
				}

				other_text_data_first.text_type = (!other_text_data_first.has_started) ? display_type::NONE : other_text_data_first.text_type;
				if(!other_text_data_first.has_started)
					end_voice();
			}

			if (text_data_first.field_2 != 0)
			{
				((void (*)(uint16_t))ff7_externals.battle_sfx_play_effect_430D14)(0x2D7);
				text_data_first.field_2 = 0;
			}

			if (text_data_first.n_frames == 0)
			{
				text_data_first.buffer_idx = -1;

				// End voice
				if (other_text_data_first.has_started)
				{
					other_text_data_first.has_started = false;
					end_voice();
				}

				for (int i = 0; i < ff7_externals.battle_display_text_queue.size(); i++)
				{
					if (text_data_first.buffer_idx == -1)
					{
						for (int j = 0; j < ff7_externals.battle_display_text_queue.size() - 1; j++)
						{
							ff7_externals.battle_display_text_queue[j] = ff7_externals.battle_display_text_queue[j + 1];
							other_battle_display_text_queue[j] = other_battle_display_text_queue[j + 1];
						}
						*ff7_externals.field_battle_word_BF2032 = 0xFFFF;
					}
				}

				return;
			}

			int show_text = (ff7_externals.battle_sub_66C3BF)();
			if (show_text)
				((void (*)(short))ff7_externals.set_battle_text_active)(text_data_first.buffer_idx);

			if (*ff7_externals.g_is_battle_paused || !*ff7_externals.g_is_battle_running_9AD1AC)
				return;

			// Ending voice
			if (other_text_data_first.has_started && other_text_data_first.follow_voice)
			{
				if (!nxAudioEngine.isVoicePlaying())
				{
					text_data_first.n_frames = 0;
				}
			}
			else
			{
				text_data_first.n_frames--;
			}
			return;
		}
		if (!*ff7_externals.g_is_battle_paused && *ff7_externals.g_is_battle_running_9AD1AC)
			text_data_first.wait_frames--;
	}

	((void (*)(short))ff7_externals.set_battle_text_active)(-1);
	*ff7_externals.field_battle_word_BF2E08 = *ff7_externals.field_battle_word_BF2E08 & 0xFFFD;
}

void ff7_display_battle_action_text()
{
	auto &effect_data = ff7_externals.effect100_array_data[*ff7_externals.effect100_array_idx];
	constexpr short VOICE_STARTED = -2;
	constexpr short VOICE_NOT_STARTED = -1;

	if(effect_data.field_6 <= 0)
	{
		// Play voice
		if(effect_data.field_6 == 0)
		{
			byte actor_id = *ff7_externals.g_active_actor_id;
			bool hasStarted = false;
			if(actor_id >= 0 && actor_id <= 2)
			{
				byte char_id = ff7_externals.battle_context->actor_vars[actor_id].index;
				byte command_id = ff7_externals.g_battle_model_state[actor_id].commandID;
				uint16_t action_id = ff7_externals.g_small_battle_model_state[actor_id].actionIdx;

				set_voice_volume();
				hasStarted = play_battle_char_action_voice(char_id, command_id, action_id);
			}
			else if(actor_id >= 4 && actor_id < 10)
			{
				uint16_t enemy_id = ff7_externals.battle_context->actor_vars[actor_id].formationID;
				byte command_id = ff7_externals.g_battle_model_state[actor_id].commandID;
				uint16_t action_id = ff7_externals.g_small_battle_model_state[actor_id].actionIdx;

				set_voice_volume();
				hasStarted = play_battle_enemy_action_voice(enemy_id, command_id, action_id);
			}

			if(hasStarted)
				effect_data.field_6 = VOICE_STARTED;
			else
				effect_data.field_6 = VOICE_NOT_STARTED;
		}

		if(effect_data.n_frames == 0)
		{
			effect_data.field_0 = 0xFFFF;
		}
		else
		{
			int show_text = (ff7_externals.battle_sub_66C3BF)();
			if(show_text)
			{
				byte command_id = ff7_externals.g_battle_model_state[*ff7_externals.g_active_actor_id].commandID;
				uint16_t action_id = ff7_externals.g_small_battle_model_state[*ff7_externals.g_active_actor_id].actionIdx;
				((void(*)(short, short))ff7_externals.display_battle_action_text_sub_6D71FA)(command_id, action_id);
				if(*ff7_externals.g_is_battle_running_9AD1AC != 0)
					effect_data.n_frames--;
			}
		}
	}
	else
	{
		if(*ff7_externals.g_is_battle_running_9AD1AC != 0)
			effect_data.field_6--;
	}
}

// -- MENU --

int ff7_menu_tutorial_render()
{
	static const int window_id = 0;
	int dialog_id = current_opcode_message_status[window_id].message_dialog_id;

	byte message_current_opcode = *ff7_externals.menu_tutorial_window_state;

	bool _is_dialog_opening = (current_opcode_message_status[window_id].message_last_opcode == 0 && message_current_opcode == 1);
	bool _is_dialog_starting = (current_opcode_message_status[window_id].message_last_opcode == 1 && message_current_opcode == 2);
	bool _is_dialog_closing = (current_opcode_message_status[window_id].message_last_opcode == 3 && message_current_opcode == 0);

	if (_is_dialog_opening)
	{
		begin_voice(window_id);
		current_opcode_message_status[window_id].message_dialog_id = dialog_id;
	}
	else if (_is_dialog_starting)
	{
		std::string decoded_text = decode_ff7_text((char*)*ff7_externals.menu_tutorial_window_text_ptr);
		std::string tokenized_dialogue = tokenize_text(decoded_text);

		if (trace_all || trace_opcodes) ffnx_trace("[TUTOR]: id=%d,text=%s\n", dialog_id, decoded_text.c_str());

		char voice_file[MAX_PATH];
		sprintf(voice_file, "_tutor/%04u/%s", dialog_id, tokenized_dialogue.c_str());
		current_opcode_message_status[window_id].is_voice_acting = nxAudioEngine.playVoice(voice_file, 0, voice_volume);
	}
	else if (_is_dialog_closing)
	{
		end_voice(window_id);
		simulate_OK_disabled[window_id] = false;
		current_opcode_message_status[window_id].is_voice_acting = false;
	}

	// Auto close the message if it was voice acted and the audio file has finished playing
	if (current_opcode_message_status[window_id].is_voice_acting && !nxAudioEngine.isVoicePlaying(window_id))
	{
		current_opcode_message_status[window_id].is_voice_acting = false;
		if (!simulate_OK_disabled[window_id] && enable_voice_auto_text) (*ff7_externals.field_global_object_ptr)->field_80 |= 0x20;
	}

	current_opcode_message_status[window_id].message_last_opcode = message_current_opcode;

	return ff7_externals.menu_tutorial_sub_6C49FD();
}

//=============================================================================

char* ff8_battle_get_monster_name(int idx)
{
	char* ret = *((char**)*(ff8_externals.battle_char_struct_dword_1D27B10 + 0x34 * idx));

	ff8_battle_actor_name[idx] = ret;

	return ret;
}

char* ff8_battle_get_actor_name(int idx)
{
	char* ret;
  BYTE actor_id = *(ff8_externals.byte_1CFF1C3 + 0x1D0 * idx);

  if ( !actor_id )
    ret = ff8_externals.unk_1CFDC70;
  else if ( actor_id == 4 )
    ret = ff8_externals.unk_1CFDC7C;
  else if ( *(ff8_externals.word_1CF75EC + 0x12 * *(ff8_externals.byte_1CFF1C3 + 0x1D0 * idx)) == -1 )
    ret = ff8_externals.unk_1CFF84C;
	else
		ret = ff8_externals.unk_1CF3E48 + *(ff8_externals.word_1CF75EC + 0x12 * *(ff8_externals.byte_1CFF1C3 + 0x1D0 * idx)) + *ff8_externals.dword_1CF3EE0;

	ff8_battle_actor_name[idx] = ret;

	return ret;
}

char *ff8_field_get_dialog_string(char *msg, int dialog_id)
{
	ff8_current_window_dialog_id = dialog_id;

	return msg + *(uint32_t *)(msg + 4 * dialog_id);
}

int ff8_world_dialog_assign_text(int idx, int dialog_id, char *text)
{
	int window_id = *(ff8_externals.worldmap_windows_idx_map + 0x10 * idx);

	current_opcode_message_status[window_id].message_dialog_id = dialog_id;

	return ff8_externals.world_dialog_assign_text_sub_543790(idx, dialog_id, text);
}

int ff8_opcode_voice_drawpoint(int unk)
{
	byte idx = *(byte *)(unk + 388);

	int magic_id = *(DWORD *)(unk + 4 * idx--);
	static int window_id = 6;
	static int default_option = 2;
	ff8_win_obj *win = ff8_externals.windows + window_id;

	int ret = ff8_opcode_old_drawpoint(unk);

	current_opcode_message_status[window_id].message_page_count = magic_id;
	current_opcode_message_status[window_id].message_dialog_id = ff8_current_window_dialog_id;
	current_opcode_message_status[window_id].message_kind = message_kind::DRAWPOINT;
	current_opcode_message_status[window_id].field_name = "_drawpoint";

	if (ff8_current_window_dialog_id == 4)
	{
		if (opcode_ask_current_option == UCHAR_MAX)
			current_opcode_message_status[window_id].message_last_option = opcode_ask_current_option = default_option;
		else
			opcode_ask_current_option = win->current_choice_question;
	}

	return ret;
}

int ff8_opcode_voice_mes(int unk)
{
	byte idx = *(byte *)(unk + 388);

	int dialog_id = *(DWORD *)(unk + 4 * idx--);
	int window_id = *(DWORD *)(unk + 4 * idx);
	ff8_win_obj *win = ff8_externals.windows + window_id;
	int message_current_opcode = win->state;
	char* field_name = get_current_field_name();

	current_opcode_message_status[window_id].message_dialog_id = dialog_id;
	current_opcode_message_status[window_id].message_kind = message_kind::MESSAGE;
	current_opcode_message_status[window_id].field_name = get_current_field_name();

	return ff8_opcode_old_mes(unk);
}

int ff8_opcode_voice_ames(int unk)
{
	byte idx = *(byte *)(unk + 388);

	idx--; // Y Position of window
	idx--; // X position of window
	int dialog_id = *(DWORD *)(unk + 4 * idx--);
	int window_id = *(DWORD *)(unk + 4 * idx);
	ff8_win_obj *win = ff8_externals.windows + window_id;
	int message_current_opcode = win->state;
	char* field_name = get_current_field_name();

	current_opcode_message_status[window_id].message_dialog_id = dialog_id;
	current_opcode_message_status[window_id].message_kind = message_kind::MESSAGE;
	current_opcode_message_status[window_id].field_name = get_current_field_name();

	return ff8_opcode_old_ames(unk);
}

int ff8_opcode_voice_amesw(int unk)
{
	byte idx = *(byte *)(unk + 388);

	idx--; // Y Position of window
	idx--; // X position of window
	int dialog_id = *(DWORD *)(unk + 4 * idx--);
	int window_id = *(DWORD *)(unk + 4 * idx);
	ff8_win_obj *win = ff8_externals.windows + window_id;
	int message_current_opcode = win->state;
	char* field_name = get_current_field_name();

	current_opcode_message_status[window_id].message_dialog_id = dialog_id;
	current_opcode_message_status[window_id].message_kind = message_kind::MESSAGE;
	current_opcode_message_status[window_id].field_name = get_current_field_name();

	return ff8_opcode_old_amesw(unk);
}

int ff8_opcode_voice_ramesw(int unk)
{
	byte idx = *(byte *)(unk + 388);

	idx--; // Y Position of window
	idx--; // X position of window
	int dialog_id = *(DWORD *)(unk + 4 * idx--);
	int window_id = *(DWORD *)(unk + 4 * idx);
	ff8_win_obj *win = ff8_externals.windows + window_id;
	int message_current_opcode = win->state;
	char* field_name = get_current_field_name();

	current_opcode_message_status[window_id].message_dialog_id = dialog_id;
	current_opcode_message_status[window_id].message_kind = message_kind::MESSAGE;
	current_opcode_message_status[window_id].field_name = get_current_field_name();

	return ff8_opcode_old_ramesw(unk);
}

int ff8_opcode_voice_ask(int unk)
{
	byte idx = *(byte *)(unk + 388);

	idx--; // Line of cancel option
	int default_option = *(DWORD *)(unk + 4 * idx--); // Line of default option
	idx--; // Line of last option
	idx--; // Line of first option
	int dialog_id = *(DWORD *)(unk + 4 * idx--);
	int window_id = *(DWORD *)(unk + 4 * idx);
	ff8_win_obj *win = ff8_externals.windows + window_id;
	int message_current_opcode = win->state;
	char* field_name = get_current_field_name();

	current_opcode_message_status[window_id].message_dialog_id = dialog_id;
	current_opcode_message_status[window_id].message_kind = message_kind::ASK;
	current_opcode_message_status[window_id].field_name = get_current_field_name();

	if (opcode_ask_current_option == UCHAR_MAX)
		current_opcode_message_status[window_id].message_last_option = opcode_ask_current_option = default_option;
	else
		opcode_ask_current_option = win->current_choice_question;

	return ff8_opcode_old_ask(unk);
}

int ff8_opcode_voice_aask(int unk)
{
	byte idx = *(byte *)(unk + 388);

	idx--; // Y Position of window
	idx--; // X position of window
	idx--; // Line of cancel option
	int default_option = *(DWORD *)(unk + 4 * idx--); // Line of default option
	idx--; // Line of last option
	idx--; // Line of first option
	int dialog_id = *(DWORD *)(unk + 4 * idx--);
	int window_id = *(DWORD *)(unk + 4 * idx);
	ff8_win_obj *win = ff8_externals.windows + window_id;
	int message_current_opcode = win->state;
	char* field_name = get_current_field_name();

	current_opcode_message_status[window_id].message_dialog_id = dialog_id;
	current_opcode_message_status[window_id].message_kind = message_kind::ASK;
	current_opcode_message_status[window_id].field_name = get_current_field_name();

	if (opcode_ask_current_option == UCHAR_MAX)
		current_opcode_message_status[window_id].message_last_option = opcode_ask_current_option = default_option;
	else
		opcode_ask_current_option = win->current_choice_question;

	return ff8_opcode_old_aask(unk);
}

int ff8_show_dialog(int window_id, int state, int a3)
{
	struct game_mode *mode = getmode_cached();

	int dialog_id = current_opcode_message_status[window_id].message_dialog_id;
	ff8_win_obj *win = ff8_externals.windows + window_id;
	int message_current_opcode = win->state;
	message_kind message_kind = current_opcode_message_status[window_id].message_kind;
	std::string field_name = current_opcode_message_status[window_id].field_name;
	byte message_page_count = current_opcode_message_status[window_id].message_page_count;

	bool _is_dialog_opening = is_dialog_opening(win->open_close_transition);
	bool _is_dialog_starting = is_dialog_starting(current_opcode_message_status[window_id].message_last_transition, win->open_close_transition);
	bool _is_dialog_paging = is_dialog_paging(current_opcode_message_status[window_id].message_last_opcode, message_current_opcode);
	bool _is_dialog_option_changed = (current_opcode_message_status[window_id].message_last_option != opcode_ask_current_option);
	bool _is_dialog_closing = is_dialog_closing(current_opcode_message_status[window_id].message_last_transition, win->open_close_transition);
	bool _is_dialog_closed = is_dialog_closed(current_opcode_message_status[window_id].message_last_transition, win->open_close_transition);
	bool _is_dialog_ask = (message_kind == message_kind::ASK) || (message_kind == message_kind::DRAWPOINT);

	if (_is_dialog_paging) current_opcode_message_status[window_id].message_page_count++;

	// Skip voice over on Tutorials
	if (mode->driver_mode == MODE_FIELD)
	{
		if (_is_dialog_opening)
		{
			opcode_ask_current_option = UCHAR_MAX;
			begin_voice(window_id);
			current_opcode_message_status[window_id].message_dialog_id = dialog_id;
			current_opcode_message_status[window_id].message_last_option = opcode_ask_current_option;
			current_opcode_message_status[window_id].message_kind = message_kind;
			current_opcode_message_status[window_id].field_name = field_name;
			if (message_kind == message_kind::DRAWPOINT) current_opcode_message_status[window_id].message_page_count = message_page_count;
			if (message_kind == message_kind::MESSAGE)
			{
				if (ff8_field_window_stack_count.find(*common_externals.current_field_id) == ff8_field_window_stack_count.end())
					ff8_field_window_stack_count[*common_externals.current_field_id] = 0;

				ff8_field_window_stack_count[*common_externals.current_field_id]++;

				if (ff8_field_window_stack_count[*common_externals.current_field_id] > 1) simulate_OK_disabled[window_id] = true;
			}
		}
		else if (_is_dialog_starting || _is_dialog_paging)
		{
			if (_is_dialog_starting) current_opcode_message_status[window_id].char_id = 0; // TODO
			if (trace_all || trace_opcodes) ffnx_trace("[MESSAGE]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u,char=%X\n", field_name.c_str(), window_id, dialog_id, current_opcode_message_status[window_id].message_page_count, current_opcode_message_status[window_id].char_id);
			current_opcode_message_status[window_id].is_voice_acting = play_voice((char*)field_name.c_str(), window_id, current_opcode_message_status[window_id].message_dialog_id, current_opcode_message_status[window_id].message_page_count);
		}
		else if (_is_dialog_option_changed && _is_dialog_ask)
		{
			if (trace_all || trace_opcodes) ffnx_trace("[ASK]: field=%s,window_id=%u,dialog_id=%u,option_id=%u,char=%X\n", field_name.c_str(), window_id, dialog_id, opcode_ask_current_option,current_opcode_message_status[window_id].char_id);
			play_option((char*)field_name.c_str(), window_id, dialog_id, opcode_ask_current_option);
		}
		else if (_is_dialog_closing)
		{
			end_voice(window_id);
			simulate_OK_disabled[window_id] = false;
			current_opcode_message_status[window_id].is_voice_acting = false;
			opcode_ask_current_option = UCHAR_MAX;
			ff8_current_window_dialog_id = -1;
			if (message_kind == message_kind::MESSAGE)
			{
				if (ff8_field_window_stack_count[*common_externals.current_field_id] > 0)
					ff8_field_window_stack_count[*common_externals.current_field_id]--;
			}
		}

		// Auto close the message if it was voice acted and the audio file has finished playing
		if (message_kind == message_kind::MESSAGE)
		{
			if (current_opcode_message_status[window_id].is_voice_acting && !nxAudioEngine.isVoicePlaying(window_id))
			{
				current_opcode_message_status[window_id].is_voice_acting = false;
				if (!simulate_OK_disabled[window_id] && enable_voice_auto_text) simulate_OK_button = true;
			}
		}
	}
	else if (mode->driver_mode == MODE_BATTLE)
	{
		bool _has_dialog_text_changed = win->field_30 != current_opcode_message_status[window_id].message_dialog_id;
		bool _is_dialog_closing = is_dialog_closing(current_opcode_message_status[window_id].message_last_transition, message_current_opcode);

		if (_is_dialog_opening)
		{
			begin_voice(window_id);
		}
		else if (_is_dialog_starting || _has_dialog_text_changed)
		{
			std::string decoded_text = ff8_decode_text(win->text_data1);
			std::string tokenized_dialogue = tokenize_text(decoded_text);
			std::string actor_name = ff8_decode_text(ff8_battle_actor_name[LOBYTE(*ff8_externals.battle_current_actor_talking)]);
			std::string tokenized_actor = tokenize_text(actor_name);

			if (trace_all || trace_opcodes || trace_battle_text) ffnx_trace("[BATTLE]: scene_id=%u,actor=%s,text=%s\n", *ff8_externals.battle_encounter_id, actor_name.c_str(), decoded_text.c_str());

			char voice_file[MAX_PATH];
			sprintf(voice_file, "_battle/%s/%s", tokenized_actor.c_str(), tokenized_dialogue.c_str());
			current_opcode_message_status[window_id].is_voice_acting = nxAudioEngine.playVoice(voice_file, 0, voice_volume);
		}
		else if (_is_dialog_closing)
		{
			end_voice(window_id);
			current_opcode_message_status[window_id].is_voice_acting = false;
		}

		current_opcode_message_status[window_id].message_dialog_id = win->field_30;
	}
	else if (mode->driver_mode == MODE_WORLDMAP)
	{
		if (_is_dialog_opening)
		{
			begin_voice(window_id);
			current_opcode_message_status[window_id].message_dialog_id = dialog_id;
		}
		else if (_is_dialog_starting)
		{
			if (dialog_id < 0)
			{
				std::string decoded_text = ff8_decode_text(win->text_data1);
				std::string tokenized_dialogue = tokenize_text(decoded_text);
				if (trace_all || trace_opcodes) ffnx_trace("[WORLD]: window_id=%u,text=%s\n", window_id, decoded_text.c_str());

				char voice_file[MAX_PATH];
				sprintf(voice_file, "_world/text/%s", tokenized_dialogue.c_str());
				current_opcode_message_status[window_id].is_voice_acting = nxAudioEngine.playVoice(voice_file, 0, voice_volume);
			}
			else
			{
				if (trace_all || trace_opcodes) ffnx_trace("[WORLD]: window_id=%u,dialog_id=%d\n", window_id, current_opcode_message_status[window_id].message_dialog_id);
				current_opcode_message_status[window_id].is_voice_acting = play_voice("_world", window_id, current_opcode_message_status[window_id].message_dialog_id, current_opcode_message_status[window_id].message_page_count);
			}
		}
		else if (_is_dialog_closing)
		{
			end_voice(window_id);
			current_opcode_message_status[window_id].is_voice_acting = false;
		}
	}
	else if (mode->mode == FF8_MODE_TUTO)
	{
		bool _has_dialog_text_changed = win->field_30 != current_opcode_message_status[window_id].message_dialog_id;
		bool _is_dialog_closing = is_dialog_closing(current_opcode_message_status[window_id].message_last_transition, message_current_opcode);

		if (_is_dialog_opening)
		{
			begin_voice(window_id);
		}
		else if (_is_dialog_starting || _has_dialog_text_changed)
		{
			std::string decoded_text = ff8_decode_text(win->text_data1);
			std::string tokenized_dialogue = tokenize_text(decoded_text);

			if (trace_all || trace_opcodes || trace_battle_text) ffnx_trace("[TUTO]: id=%u,text=%s\n", *ff8_externals.current_tutorial_id, decoded_text.c_str());

			char voice_file[MAX_PATH];
			sprintf(voice_file, "_tuto/%04u/%s", *ff8_externals.current_tutorial_id, tokenized_dialogue.c_str());
			current_opcode_message_status[window_id].is_voice_acting = nxAudioEngine.playVoice(voice_file, 0, voice_volume);
		}
		else if (_is_dialog_closing)
		{
			end_voice(window_id);
			simulate_OK_disabled[window_id] = false;
			current_opcode_message_status[window_id].is_voice_acting = false;
		}

		current_opcode_message_status[window_id].message_dialog_id = win->field_30;

		if (current_opcode_message_status[window_id].is_voice_acting && !nxAudioEngine.isVoicePlaying(window_id))
		{
			current_opcode_message_status[window_id].is_voice_acting = false;
			if (!simulate_OK_disabled[window_id] && enable_voice_auto_text) simulate_OK_button = true;
		}
	}

	current_opcode_message_status[window_id].message_last_opcode = message_current_opcode;
	current_opcode_message_status[window_id].message_last_transition = win->open_close_transition;
	current_opcode_message_status[window_id].message_last_option = opcode_ask_current_option;

	return ff8_externals.show_dialog(window_id, state, a3);
}

//=============================================================================

void voice_init()
{
	// Prepare up to 10 voice slots
	nxAudioEngine.setVoiceMaxSlots(10);

	if (!ff8)
	{
		ff7_set_master_music_volume = (void (*)(uint32_t))common_externals.set_master_midi_volume;

		opcode_old_message = (int (*)())ff7_externals.opcode_message;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x40], (DWORD)&opcode_voice_message);

		opcode_old_ask = (int (*)(int))ff7_externals.opcode_ask;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x48], (DWORD)&opcode_voice_ask);
		replace_call_function((uint32_t)ff7_externals.opcode_ask + 0x8E, opcode_voice_parse_options);

		opcode_old_wmode = (int (*)())ff7_externals.opcode_wmode;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x52], (DWORD)&opcode_wmode);

		opcode_wm_old_message = (int (*)(uint8_t,uint8_t))ff7_externals.world_opcode_message;
		replace_call_function(ff7_externals.world_opcode_message_sub_75EE86 + 0x2B, opcode_wm_message);
		replace_call_function(ff7_externals.world_sub_75EF46 + 0x8C, opcode_wm_message);

		opcode_wm_old_ask = (int (*)(uint8_t,uint8_t,uint8_t,uint8_t,WORD*))ff7_externals.world_opcode_ask;
		replace_call_function(ff7_externals.world_opcode_ask_sub_75EEBB + 0x3C, opcode_wm_ask);
		replace_call_function(ff7_externals.world_sub_75EF46 + 0xAF, opcode_wm_ask);

		opcode_old_tutor = (int (*)())ff7_externals.opcode_tutor;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x21], (DWORD)&opcode_voice_tutor);

		replace_function(ff7_externals.add_text_to_display_queue, ff7_add_text_to_display_queue);
		replace_function(ff7_externals.update_display_text_queue, ff7_update_display_text_queue);
		replace_function(ff7_externals.display_battle_action_text_42782A, ff7_display_battle_action_text);
		replace_call_function(ff7_externals.run_enemy_ai_script + 0xB7F, ff7_enqueue_script_display_string);
		replace_call_function(ff7_externals.menu_sub_6CB56A + 0x2B7, ff7_menu_tutorial_render);
	}
	else
	{
		// == Field ==
		// All possible message and ask windows
		ff8_opcode_old_mes = (int (*)(int))ff8_externals.opcode_mes;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x47], (DWORD)&ff8_opcode_voice_mes);

		ff8_opcode_old_ames = (int (*)(int))ff8_externals.opcode_ames;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x65], (DWORD)&ff8_opcode_voice_ames);

		ff8_opcode_old_amesw = (int (*)(int))ff8_externals.opcode_amesw;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x64], (DWORD)&ff8_opcode_voice_amesw);

		ff8_opcode_old_ramesw = (int (*)(int))ff8_externals.opcode_ramesw;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x116], (DWORD)&ff8_opcode_voice_ramesw);

		ff8_opcode_old_ask = (int (*)(int))ff8_externals.opcode_ask;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x4A], (DWORD)&ff8_opcode_voice_ask);

		ff8_opcode_old_aask = (int (*)(int))ff8_externals.opcode_aask;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x6F], (DWORD)&ff8_opcode_voice_aask);

		ff8_opcode_old_drawpoint = (int (*)(int))ff8_externals.opcode_drawpoint;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x137], (DWORD)&ff8_opcode_voice_drawpoint);

		replace_function(ff8_externals.field_get_dialog_string, ff8_field_get_dialog_string);
		replace_call(ff8_externals.sub_4A0C00 + 0x5F, ff8_show_dialog);

		// == Battle ==
		replace_function(ff8_externals.battle_get_monster_name_sub_495100, ff8_battle_get_monster_name);
		replace_function(ff8_externals.battle_get_actor_name_sub_47EAF0, ff8_battle_get_actor_name);

		// == World Map ==
		replace_call_function(ff8_externals.sub_543CB0 + (FF8_US_VERSION ? 0x638 : (FF8_SP_VERSION || FF8_IT_VERSION) ? 0x61C : 0x605), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_5484B0 + (FF8_US_VERSION ? 0x524 : 0x4FD), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_54A230 + (FF8_US_VERSION ? 0xF : 0xD), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_54D7E0 + (FF8_US_VERSION ? 0x72 : 0x6F), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_54E9B0 + (FF8_US_VERSION ? 0x206 : 0x20C), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_54E9B0 + (FF8_US_VERSION ? 0x396 : 0x3A9), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_54E9B0 + (FF8_US_VERSION ? 0x3D2 : 0x3E5), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_54E9B0 + (FF8_US_VERSION ? 0x67D : (FF8_SP_VERSION ? 0x6CA : 0x68F)), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_54E9B0 + (FF8_US_VERSION ? 0x6A6 : (FF8_SP_VERSION ? 0x6F7 : 0x6BC)), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_54E9B0 + (FF8_US_VERSION ? 0xEED : (FF8_SP_VERSION ? 0xFAD : 0xF72)), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_54FDA0 + (FF8_US_VERSION ? 0xAE : 0xAC), ff8_world_dialog_assign_text);
		replace_call_function(ff8_externals.sub_54FDA0 + (FF8_US_VERSION ? 0x178 : 0x175), ff8_world_dialog_assign_text);
	}
}
