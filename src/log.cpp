/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
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

#include <string>
#include <stdio.h>
#include <windows.h>

#include "log.h"
#include "hext.h"

#include "renderer.h"

#define FFNX_DEBUG_BUFFER_SIZE 4096

FILE *app_log;

void open_applog(char *path)
{
	app_log = fopen(path, "wb");

	if(!app_log) MessageBoxA(gameHwnd, "Failed to open log file", "Error", 0);
}

void plugin_trace(const char *fmt, ...)
{
	va_list args;
	char tmp_str[FFNX_DEBUG_BUFFER_SIZE];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	ffnx_trace("%s", tmp_str);
}

void plugin_info(const char *fmt, ...)
{
	va_list args;
	char tmp_str[FFNX_DEBUG_BUFFER_SIZE];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	ffnx_info("%s", tmp_str);
}

void plugin_glitch(const char *fmt, ...)
{
	va_list args;
	char tmp_str[FFNX_DEBUG_BUFFER_SIZE];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	ffnx_glitch("%s", tmp_str);
}

void plugin_error(const char *fmt, ...)
{
	va_list args;
	char tmp_str[FFNX_DEBUG_BUFFER_SIZE];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	ffnx_error("%s", tmp_str);
}

void debug_print(const char *str)
{
	char tmp_str[FFNX_DEBUG_BUFFER_SIZE + 16];

	sprintf(tmp_str, "[%08i] %s", frame_counter, str);

	fwrite(tmp_str, 1, strlen(tmp_str), app_log);
	fflush(app_log);
}

void show_popup_msg(uint8_t text_color, const char* fmt, ...)
{
	va_list args;
	char tmp_str[1024];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	strcpy(popup_msg, tmp_str);
	popup_ttl = POPUP_TTL_MAX;
	popup_color = text_colors[text_color];
}

void clear_popup_msg()
{
	popup_ttl = 0;
}

uint32_t get_popup_time()
{
	return popup_ttl;
}

void external_debug_print(const char *str)
{
	std::string msg(str);
	msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
	trim(msg);

	if (msg.length() == 0) return;

	if (ff8)
	{
		if (starts_with(msg, "Patch")) return;
	}
	else
	{
		if (starts_with(msg, "SET VOLUME")) return;
	}

	hextPatcher.applyAll(msg);

	msg += "\n";

	if (show_applog) debug_print(msg.c_str());
	if (show_error_popup) show_popup_msg(TEXTCOLOR_GRAY, str);
}

void external_debug_print2(const char *fmt, ...)
{
	va_list args;
	char tmp_str[FFNX_DEBUG_BUFFER_SIZE];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	external_debug_print(tmp_str);
}

void debug_printf(const char *prefix, uint32_t color, const char *fmt, ...)
{
	va_list args;
	char tmp_str[FFNX_DEBUG_BUFFER_SIZE];
	char tmp_str2[FFNX_DEBUG_BUFFER_SIZE];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	if ( prefix == nullptr)
		_snprintf(tmp_str2, sizeof(tmp_str2), "%s", tmp_str);
	else
		_snprintf(tmp_str2, sizeof(tmp_str2), "%s: %s", prefix, tmp_str);
	debug_print(tmp_str2);
}

void windows_error(uint32_t error)
{
	char tmp_str[200];

	if(FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, error == 0 ? GetLastError() : error, 0, tmp_str, sizeof(tmp_str), 0)) debug_print(tmp_str);
}
