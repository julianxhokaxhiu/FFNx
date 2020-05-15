#include <string>
#include <stdio.h>
#include <windows.h>

#include "log.h"
#include "hext.h"

FILE *app_log;

void open_applog(char *path)
{
	app_log = fopen(path, "wb");

	if(!app_log) MessageBoxA(hwnd, "Failed to open log file", "Error", 0);
}

void plugin_trace(const char *fmt, ...)
{
	va_list args;
	char tmp_str[1024];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	trace("%s", tmp_str);
}

void plugin_info(const char *fmt, ...)
{
	va_list args;
	char tmp_str[1024];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	info("%s", tmp_str);
}

void plugin_glitch(const char *fmt, ...)
{
	va_list args;
	char tmp_str[1024];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	glitch("%s", tmp_str);
}

void plugin_error(const char *fmt, ...)
{
	va_list args;
	char tmp_str[1024];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	error("%s", tmp_str);
}

void debug_print(const char *str)
{
	char tmp_str[1024];

	sprintf(tmp_str, "[%08i] %s", frame_counter, str);

	fwrite(tmp_str, 1, strlen(tmp_str), app_log);
	fflush(app_log);
}

// filter out some less useful spammy messages
const char ff7_filter[] = "SET VOLUME ";
const char ff8_filter[] = "Patch ";

void external_debug_print(const char *str)
{
	if(!ff8 && !strncmp(str, ff7_filter, sizeof(ff7_filter) - 1)) return;
	if(ff8 && !strncmp(str, ff8_filter, sizeof(ff8_filter) - 1)) return;

	if(show_applog) debug_print(str);

	if(show_error_popup)
	{
		strcpy(popup_msg, str);
		popup_ttl = POPUP_TTL_MAX;
		popup_color = text_colors[TEXTCOLOR_GRAY];
	}

	std::string msg(str);
	msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
	hextPatcher.applyAll(msg);
}

void external_debug_print2(const char *fmt, ...)
{
	va_list args;
	char tmp_str[1024];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	external_debug_print(tmp_str);
}

void debug_printf(const char *prefix, uint color, const char *fmt, ...)
{
	va_list args;
	char tmp_str[1024];
	char tmp_str2[1024];

	va_start(args, fmt);

	vsnprintf(tmp_str, sizeof(tmp_str), fmt, args);

	va_end(args);

	if ( prefix == nullptr)
		_snprintf(tmp_str2, sizeof(tmp_str2), "%s", tmp_str);
	else
		_snprintf(tmp_str2, sizeof(tmp_str2), "%s: %s", prefix, tmp_str);
	debug_print(tmp_str2);
}

void windows_error(uint error)
{
	char tmp_str[200];

	if(FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, error == 0 ? GetLastError() : error, 0, tmp_str, sizeof(tmp_str), 0)) debug_print(tmp_str);
}
