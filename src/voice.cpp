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

#include <queue>

struct battle_text_aux_data{
	bool has_started;
	bool is_dialogue;
	bool is_paused;
	uint16_t enemy_id;
};

int (*opcode_old_message)();
int (*opcode_old_ask)(int);

DWORD previous_master_music_volume = 0x64; // Assume maximum by default
void (*set_master_music_volume)(uint32_t);
float voice_volume = -1.0f;

std::array<battle_text_aux_data, 64> other_battle_display_text_queue;
std::queue<short> display_string_actor_queue;

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

bool play_battle_voice(short enemyID, std::string filename)
{
	char name[MAX_PATH];

	sprintf(name, "_battle/enemy_%04X/%s", enemyID, filename.c_str());

	if(!nxAudioEngine.canPlayVoice(name))
		return false;

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

	byte window_id = get_field_parameter<byte>(0);
	byte dialog_id = get_field_parameter<byte>(1);
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
		if (trace_all || trace_opcodes) ffnx_trace("opcode[MESSAGE]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u\n", field_name, window_id, dialog_id, message_page_count);
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

byte opcode_ask_current_option = 0;

int opcode_voice_parse_options(uint8_t window_id, uint8_t dialog_id, uint8_t first_question_id, uint8_t last_question_id, WORD *current_question_id)
{
	opcode_ask_current_option = *current_question_id;

	return ff7_externals.sub_6310A1(window_id, dialog_id, first_question_id, last_question_id, current_question_id);
}

int opcode_voice_ask(int unk)
{
	static byte message_page_count = 0;
	static WORD message_last_opcode = 0;
	static WORD message_last_option = 0;

	int ret = opcode_old_ask(unk);

	byte window_id = get_field_parameter<byte>(1);
	byte dialog_id = get_field_parameter<byte>(2);
	byte message_current_opcode = get_dialog_opcode(window_id);
	char* field_name = get_current_field_name();

	bool _is_dialog_opening = is_dialog_opening(message_current_opcode);
	bool _is_dialog_starting = is_dialog_starting(message_last_opcode, message_current_opcode);
	bool _is_dialog_paging = is_dialog_paging(message_last_opcode, message_current_opcode);
	bool _is_dialog_closing = is_dialog_closing(message_last_opcode, message_current_opcode);
	bool _is_dialog_closed = is_dialog_closed(message_last_opcode, message_current_opcode);
	bool _is_dialog_option_changed = (message_last_option != opcode_ask_current_option);

	if (_is_dialog_paging) message_page_count++;

	if (_is_dialog_opening)
	{
		message_page_count = 0;
		begin_voice();
	}
	if (_is_dialog_starting || _is_dialog_paging)
	{
		play_voice(field_name, dialog_id, message_page_count);
		if (trace_all || trace_opcodes) ffnx_trace("opcode[ASK]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u\n", field_name, window_id, dialog_id, message_page_count);
	}
	else if (_is_dialog_option_changed)
	{
		play_option(field_name, dialog_id, opcode_ask_current_option);
		if (trace_all || trace_opcodes) ffnx_trace("opcode[ASK]: field=%s,window_id=%u,dialog_id=%u,option_id=%u\n", field_name, window_id, dialog_id, opcode_ask_current_option);
	}
	else if (_is_dialog_closing)
	{
		end_voice();
	}

	message_last_option = opcode_ask_current_option;
	message_last_opcode = message_current_opcode;

	return ret;
}

std::string decode_ff7_text(const char *encoded_text)
{
	std::string decoded_text{};
	int index = 0;
	char current_char;
	while (current_char = encoded_text[index++], current_char != char(0xFF))
	{
		switch (current_char)
		{
		case 0xF8:
			index += 2;
			break;
		default:
			if (current_char >= 0xEA)
				ffnx_trace("special char: %c", current_char);
			decoded_text.push_back(current_char + 0x20);
			break;
		}
	}
	return decoded_text;
}

std::string get_battle_voice_filename(std::string decoded_text)
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
		text_data->wait_frames = wait_frames * frame_multiplier - 1;
		text_data->n_frames = (n_frames == 0) ? ((int (*)())ff7_externals.get_n_frames_display_action_string)() : n_frames * frame_multiplier;

		int index = std::distance(ff7_externals.battle_display_text_queue.begin(), text_data);
		int attacker_id = ff7_externals.anim_event_queue[*ff7_externals.anim_event_index].attackerID;
		if (attacker_id == 10 && n_frames == 0)
		{
			other_battle_display_text_queue[index].has_started = false;
			other_battle_display_text_queue[index].is_dialogue = true;
			if (!display_string_actor_queue.empty())
			{
				short actor_id = display_string_actor_queue.front();
				display_string_actor_queue.pop();
				other_battle_display_text_queue[index].enemy_id = ff7_externals.battle_actor_vars[actor_id].formationID;
			}
		}

		if (trace_all || trace_battle_text)
			ffnx_trace("Add text string to be displayed: (text_id: %d, field_2: %d, wait_frames: %d, n_frames: %d)\n", text_data->buffer_idx, text_data->field_2, text_data->wait_frames, text_data->n_frames);
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
			if (other_text_data_first.is_dialogue)
			{
				if (!other_text_data_first.has_started)
				{
					const char *encoded_text = ff7_externals.get_kernel_text(8, text_data_first.buffer_idx, 8);
					std::string decoded_text = decode_ff7_text(encoded_text);
					std::string filename = get_battle_voice_filename(decoded_text);

					if (trace_all || trace_battle_text)
						ffnx_trace("Begin voice of EnemyID: %04X for text: %s (filename: %s)\n", other_text_data_first.enemy_id, decoded_text.c_str(), filename.c_str());

					begin_voice();
					other_text_data_first.has_started = play_battle_voice(other_text_data_first.enemy_id, filename);
				}
			}

			if (text_data_first.field_2 != 0)
			{
				((void (*)(uint16_t))ff7_externals.battle_sub_430D14)(0x2D7);
				text_data_first.field_2 = 0;
			}

			if (text_data_first.n_frames == 0)
			{
				text_data_first.buffer_idx = -1;

				// End voice
				if (other_text_data_first.is_dialogue && other_text_data_first.has_started)
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

			int show_text = ((int (*)())ff7_externals.battle_sub_66C3BF)();
			if (show_text)
				((void (*)(short))ff7_externals.set_battle_text_active)(text_data_first.buffer_idx);

			if (*ff7_externals.g_is_battle_paused || !*ff7_externals.g_is_battle_running)
			{
				// TODO Pause voice
				return;
			}

			// Ending voice
			if (other_text_data_first.is_dialogue && other_text_data_first.has_started)
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
		if (!*ff7_externals.g_is_battle_paused && *ff7_externals.g_is_battle_running)
			text_data_first.wait_frames--;
	}

	((void (*)(short))ff7_externals.set_battle_text_active)(-1);
	*ff7_externals.field_battle_word_BF2E08 = *ff7_externals.field_battle_word_BF2E08 & 0xFFFD;
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
		replace_call_function((uint32_t)ff7_externals.opcode_ask + 0x8E, opcode_voice_parse_options);

		replace_function(ff7_externals.add_text_to_display_queue, ff7_add_text_to_display_queue);
		replace_function(ff7_externals.update_display_text_queue, ff7_update_display_text_queue);
		replace_call_function(ff7_externals.run_enemy_ai_script + 0xB7F, ff7_enqueue_script_display_string);
	}
}
