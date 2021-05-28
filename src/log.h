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

#pragma once

#include "cfg.h"
#include "common.h"
#include "globals.h"

#define ffnx_error(x, ...) debug_printf("ERROR", text_colors[TEXTCOLOR_RED], (x), __VA_ARGS__)
#define ffnx_warning(x, ...) debug_printf("WARNING", text_colors[TEXTCOLOR_YELLOW], (x), __VA_ARGS__)
#define ffnx_info(x, ...) debug_printf("INFO", text_colors[TEXTCOLOR_WHITE], (x), __VA_ARGS__)
#define ffnx_dump(x, ...) debug_printf("DUMP", text_colors[TEXTCOLOR_PINK], (x), __VA_ARGS__)
#define ffnx_trace(x, ...) debug_printf("TRACE", text_colors[TEXTCOLOR_GREEN], (x), __VA_ARGS__)
#define ffnx_glitch(x, ...) debug_printf("GLITCH", text_colors[TEXTCOLOR_GRAY], (x), __VA_ARGS__)
#define ffnx_unexpected(x, ...) debug_printf("UNEXPECTED", text_colors[TEXTCOLOR_LIGHT_BLUE], (x), __VA_ARGS__)

#define ffnx_glitch_once(x, ...) { static uint32_t glitch_ ## __LINE__ = false; if(!glitch_ ## __LINE__) { ffnx_glitch(x, __VA_ARGS__); glitch_ ## __LINE__ = true; } }
#define ffnx_unexpected_once(x, ...) { static uint32_t unexpected_ ## __LINE__ = false; if(!unexpected_ ## __LINE__) { ffnx_unexpected(x, __VA_ARGS__); unexpected_ ## __LINE__ = true; } }

void open_applog(char *path);

void plugin_trace(const char *fmt, ...);
void plugin_info(const char *fmt, ...);
void plugin_glitch(const char *fmt, ...);
void plugin_error(const char *fmt, ...);

void external_debug_print(const char *str);
void external_debug_print2(const char *fmt, ...);

void show_popup_msg(uint8_t text_color, const char* fmt, ...);

void debug_printf(const char *, uint32_t, const char *, ...);

void windows_error(uint32_t error);
