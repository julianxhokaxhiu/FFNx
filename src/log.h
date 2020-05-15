#pragma once

#include "cfg.h"
#include "common.h"
#include "globals.h"

#define glitch_once(x, ...) { static uint glitch_ ## __LINE__ = false; if(!glitch_ ## __LINE__) { glitch(x, __VA_ARGS__); glitch_ ## __LINE__ = true; } }
#define unexpected_once(x, ...) { static uint unexpected_ ## __LINE__ = false; if(!unexpected_ ## __LINE__) { unexpected(x, __VA_ARGS__); unexpected_ ## __LINE__ = true; } }

#define error(x, ...) debug_printf("ERROR", text_colors[TEXTCOLOR_RED], (x), __VA_ARGS__)
#define warning(x, ...) debug_printf("WARNING", text_colors[TEXTCOLOR_YELLOW], (x), __VA_ARGS__)
#define info(x, ...) debug_printf("INFO", text_colors[TEXTCOLOR_WHITE], (x), __VA_ARGS__)
#define dump(x, ...) debug_printf("DUMP", text_colors[TEXTCOLOR_PINK], (x), __VA_ARGS__)
#define trace(x, ...) debug_printf("TRACE", text_colors[TEXTCOLOR_GREEN], (x), __VA_ARGS__)
#define glitch(x, ...) debug_printf("GLITCH", text_colors[TEXTCOLOR_GRAY], (x), __VA_ARGS__)
#define unexpected(x, ...) debug_printf("UNEXPECTED", text_colors[TEXTCOLOR_LIGHT_BLUE], (x), __VA_ARGS__)

void open_applog(char *path);

void plugin_trace(const char *fmt, ...);
void plugin_info(const char *fmt, ...);
void plugin_glitch(const char *fmt, ...);
void plugin_error(const char *fmt, ...);

void external_debug_print(const char *str);
void external_debug_print2(const char *fmt, ...);

void debug_printf(const char *, uint, const char *, ...);

void windows_error(uint error);
