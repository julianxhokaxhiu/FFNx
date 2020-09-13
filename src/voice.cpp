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

#include "voice.h"

#include "audio.h"
#include "field.h"
#include "patch.h"

int (*opcode_old_message)();
int (*opcode_old_ask)(int);

void play_voice(char* field_name, byte dialog_id, byte page_count)
{
	char name[MAX_PATH];

	char page = 'a' + page_count;
	if (page > 'z') page = 'z';
	sprintf(name, "%s/%u%c", field_name, dialog_id, page);

	if (!nxAudioEngine.canPlayVoice(name) && page_count == 0)
		sprintf(name, "%s/%u", field_name, dialog_id);

	nxAudioEngine.playVoice(name);
}

void play_option(char* field_name, byte dialog_id, byte option_count)
{
	char name[MAX_PATH];

	sprintf(name, "%s/%u_%u", field_name, dialog_id, option_count);

	nxAudioEngine.playVoice(name);
}

void stop_voice(uint32_t time = 0)
{
	nxAudioEngine.stopVoice(time);
}

int opcode_voice_message()
{
	static byte message_page_count = 0;
	static WORD message_last_opcode = 0;

	byte window_id = get_field_parameter(0);
	byte dialog_id = get_field_parameter(1);
	byte message_current_opcode = ff7_externals.opcode_message_loop_code[24 * window_id];
	char* field_name = strrchr(ff7_externals.field_file_name, 92) + 1;

	bool is_new_dialog = (message_current_opcode == 0);
	bool is_page_changing = ((message_last_opcode == 14 && message_current_opcode == 2) || (message_last_opcode == 4 && message_current_opcode == 8));
	bool is_dialog_closing = (message_last_opcode != message_current_opcode && message_current_opcode == 7);
	bool is_dialog_closed = (message_last_opcode == 7 && message_current_opcode == 0);

	if (is_new_dialog) message_page_count = 0;
	if (is_page_changing) message_page_count++;
	if (is_new_dialog || is_page_changing)
	{
		play_voice(field_name, dialog_id, message_page_count);
		if (trace_all || trace_opcodes) trace("opcode[MESSAGE]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u\n", field_name, window_id, dialog_id, message_page_count);
	}
	else if (is_dialog_closing)
	{
		stop_voice();
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
	byte message_current_opcode = ff7_externals.opcode_message_loop_code[24 * window_id];
	byte message_current_option = (ff7_externals.opcode_ask_question_code[24 * window_id] - 6) / 16;
	char* field_name = strrchr(ff7_externals.field_file_name, 92) + 1;

	bool is_new_dialog = (message_current_opcode == 0);
	bool is_page_changing = ((message_last_opcode == 14 && message_current_opcode == 2) || (message_last_opcode == 4 && message_current_opcode == 8));
	bool is_dialog_closing = (message_last_opcode != message_current_opcode && message_current_opcode == 7);
	bool is_dialog_closed = (message_last_opcode == 7 && message_current_opcode == 0);
	bool is_dialog_option_changed = (message_last_option != message_current_option);

	if (is_new_dialog) message_page_count = 0;
	if (is_page_changing) message_page_count++;
	if (is_new_dialog || is_page_changing)
	{
		play_voice(field_name, dialog_id, message_page_count);
		if (trace_all || trace_opcodes) trace("opcode[ASK]: field=%s,window_id=%u,dialog_id=%u,paging_id=%u\n", field_name, window_id, dialog_id, message_page_count);
	}
	else if (is_dialog_option_changed)
	{
		play_option(field_name, dialog_id, message_current_option);
		if (trace_all || trace_opcodes) trace("opcode[ASK]: field=%s,window_id=%u,dialog_id=%u,option_id=%u\n", field_name, window_id, dialog_id, message_current_option);
	}
	else if (is_dialog_closing)
	{
		stop_voice();
	}

	message_last_option = message_current_option;
	message_last_opcode = message_current_opcode;

	return opcode_old_ask(unk);
}

void voice_init()
{
	if (!ff8)
	{
		opcode_old_message = (int (*)())ff7_externals.opcode_message;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x40], (DWORD)&opcode_voice_message);

		opcode_old_ask = (int (*)(int))ff7_externals.opcode_ask;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x48], (DWORD)&opcode_voice_ask);
	}
}