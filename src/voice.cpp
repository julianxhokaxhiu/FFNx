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

#include "voice.h"

#include "audio.h"
#include "field.h"
#include "patch.h"

#include <iomanip>
#include <sstream>
#include <queue>

enum class display_type
{
	NONE = 0,
	DIALOGUE,
	CHAR_CMD
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

int (*opcode_old_message)();
int (*opcode_old_ask)(int);
int (*opcode_old_wmode)();

std::map<int,bool> simulate_OK_disabled;

DWORD previous_master_music_volume = 0x64; // Assume maximum by default
void (*set_master_music_volume)(uint32_t);
float voice_volume = -1.0f;

std::array<battle_text_aux_data, 64> other_battle_display_text_queue;
std::queue<short> display_string_actor_queue;

//=============================================================================

void set_voice_volume()
{
	voice_volume = 2.0f + (100 - external_voice_music_fade_volume) / 100.0f;
}

void begin_voice()
{
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
				set_master_music_volume(external_voice_music_fade_volume);
			}
		}
	}
}

bool play_voice(char* field_name, byte window_id, byte dialog_id, byte page_count)
{
	char name[MAX_PATH];

	char page = 'a' + page_count;
	if (page > 'z') page = 'z';

	sprintf(name, "%s/%u_%u%c", field_name, window_id, dialog_id, page);

	if (!nxAudioEngine.canPlayVoice(name))
		sprintf(name, "%s/%u%c", field_name, dialog_id, page);

	if (!nxAudioEngine.canPlayVoice(name) && page_count == 0)
	{
		sprintf(name, "%s/%u_%u", field_name, window_id, dialog_id);

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

bool play_battle_action_voice(byte char_id, byte command_id, short action_id)
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

int opcode_wmode()
{
	byte window_id = get_field_parameter<byte>(0);
	byte window_permanent = get_field_parameter<byte>(2);

	simulate_OK_disabled[window_id] = window_permanent;

	return opcode_old_wmode();
}

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
		is_voice_acting = play_voice(field_name, window_id, dialog_id, message_page_count);
		if (trace_all || trace_opcodes) ffnx_trace("opcode[MESSAGE]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u\n", field_name, window_id, dialog_id, message_page_count);
	}
	else if (_is_dialog_closing)
	{
		end_voice(window_id);
		simulate_OK_disabled[window_id] = false;
	}

	// Auto close the message if it was voice acted and the audio file has finished playing
	if (is_voice_acting && !nxAudioEngine.isVoicePlaying(window_id))
	{
		is_voice_acting = false;
		if (!simulate_OK_disabled[window_id]) simulate_OK_button = true;
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
		play_voice(field_name, window_id, dialog_id, message_page_count);
		if (trace_all || trace_opcodes) ffnx_trace("opcode[ASK]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u\n", field_name, window_id, dialog_id, message_page_count);
	}
	else if (_is_dialog_option_changed)
	{
		play_option(field_name, window_id, dialog_id, opcode_ask_current_option);
		if (trace_all || trace_opcodes) ffnx_trace("opcode[ASK]: field=%s,window_id=%u,dialog_id=%u,option_id=%u\n", field_name, window_id, dialog_id, opcode_ask_current_option);
	}
	else if (_is_dialog_closing)
	{
		end_voice(window_id);
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
		text_data->n_frames = (n_frames == 0) ? ((int (*)())ff7_externals.get_n_frames_display_action_string)() : n_frames * battle_frame_multiplier;

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
				hasStarted = play_battle_action_voice(char_id, command_id, action_id);
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

void voice_init()
{
	// Prepare up to 10 voice slots
	nxAudioEngine.setVoiceMaxSlots(10);

	if (!ff8)
	{
		set_master_music_volume = (void (*)(uint32_t))common_externals.set_master_midi_volume;

		opcode_old_message = (int (*)())ff7_externals.opcode_message;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x40], (DWORD)&opcode_voice_message);

		opcode_old_ask = (int (*)(int))ff7_externals.opcode_ask;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x48], (DWORD)&opcode_voice_ask);
		replace_call_function((uint32_t)ff7_externals.opcode_ask + 0x8E, opcode_voice_parse_options);

		opcode_old_wmode = (int (*)())ff7_externals.opcode_wmode;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x52], (DWORD)&opcode_wmode);

		replace_function(ff7_externals.add_text_to_display_queue, ff7_add_text_to_display_queue);
		replace_function(ff7_externals.update_display_text_queue, ff7_update_display_text_queue);
		replace_function(ff7_externals.display_battle_action_text_42782A, ff7_display_battle_action_text);
		replace_call_function(ff7_externals.run_enemy_ai_script + 0xB7F, ff7_enqueue_script_display_string);
	}
}
