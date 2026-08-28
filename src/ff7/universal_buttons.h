#pragma once

#include "../ff7.h"

void universal_buttons_load(struc_3* graphics_context, char* template_path, ff7_game_obj* game_object);
void universal_buttons_unload();
void universal_buttons_draw(ff7_game_obj* game_object);
void universal_buttons_reset();
void universal_buttons_flush_vanilla_field();
void universal_buttons_flush_menu(ff7_graphics_object* graphics_object, ff7_game_obj* game_object);

bool universal_buttons_parse_field_prompt(const byte* buffer, int* button, int* byte_count);
int universal_buttons_draw_field_prompt(int button, int x, int y, float z);
int universal_buttons_draw_menu_prompt(int button, int x, int y, float z);
int universal_buttons_draw_field_jp_control(int control, int x, int y, float z);
int universal_buttons_draw_menu_jp_control(int control, int x, int y, float z);
int universal_buttons_field_prompt_width();
int universal_buttons_draw_config_binding(int x, int y, byte* buffer, byte color, float z);

int universal_buttons_submit_vanilla_prompt(void* caller_frame, int use_alpha,
	struct graphics_object* graphics_object);