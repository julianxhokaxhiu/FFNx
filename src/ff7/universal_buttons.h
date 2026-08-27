#pragma once

#include "../ff7.h"

void universal_buttons_load(struc_3* graphics_context, char* template_path, ff7_game_obj* game_object);
void universal_buttons_unload();
void universal_buttons_draw(ff7_game_obj* game_object);
void universal_buttons_reset();
void universal_buttons_flush_vanilla_field();

bool universal_buttons_parse_field_prompt(const byte* buffer, int* button, int* byte_count);
int universal_buttons_draw_field_prompt(int button, int x, int y, float z);
int universal_buttons_draw_field_action(int action, int x, int y, float z);
int universal_buttons_field_prompt_width();

int universal_buttons_submit_vanilla_prompt(void* caller_frame, int use_alpha,
	struct graphics_object* graphics_object);