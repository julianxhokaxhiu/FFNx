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

#pragma once

// battle
void magic_thread_start(void (*func)());
void ff7_load_battle_stage(int param_1, int battle_location_id, int **param_3);
void ff7_battle_sub_5C7F94(int param_1, int param_2);
void ff7_display_battle_action_text_sub_6D71FA(short command_id, short action_id);
void ifrit_first_wave_effect_widescreen_fix_sub_66A47E(int wave_data_pointer);
void ifrit_second_third_wave_effect_widescreen_fix_sub_66A47E(int wave_data_pointer);

// menu
void ff7_menu_battle_end_sub_6C9543();
void ff7_menu_sub_71AAA3(int param_1);
int ff7_get_materia_gil(uint32_t materia);
void ff7_opcode_increase_gil_call(int param_1);
byte ff7_menu_sub_6CBCF3(uint32_t materia_id);
void ff7_menu_sub_6CC17F(uint32_t materia);
uint32_t ff7_menu_decrease_item_quantity(uint32_t item_data);
void ff7_menu_sub_6CDC09(DWORD param_1);
void ff7_battle_menu_sub_6DB0EE();
void ff7_set_battle_menu_state_data_at_full_atb(short param_1, short param_2, short menu_state);

// misc
void ff7_core_game_loop();
uint32_t get_equipment_stats(uint32_t party_index, uint32_t type);
void kernel2_reset_counters();
char *kernel2_add_section(uint32_t size);
char *kernel2_get_text(uint32_t section_base, uint32_t string_id, uint32_t section_offset);
void ff7_wm_activateapp(bool hasFocus);
int ff7_get_gamepad();
struct ff7_gamepad_status* ff7_update_gamepad_status();
void* ff7_engine_exit_game_mode(ff7_game_obj* game_object);
void ff7_on_gameover_enter();
void ff7_on_gameover_exit();
BYTE ff7_toggle_battle_field();
BYTE ff7_toggle_battle_worldmap();
bool ff7_skip_movies();
void *ff7_menu_sub_6F5C0C(uint32_t param1, uint32_t param2, uint8_t param3, uint8_t param4, uint32_t param5);
void *ff7_menu_sub_6FAC38(uint32_t param1, uint32_t param2, uint8_t param3, uint8_t param4, uint32_t param5);
void ff7_limit_fps();
void ff7_handle_ambient_playback();
void ff7_handle_voice_playback();
BOOL ff7_write_save_file(char slot);
int ff7_field_load_models_atoi(const char* str);
void ff7_chocobo_field_entity_60FA7D(WORD param1, short param2, short param3);
void ff7_character_regularly_field_entity_60FA7D(WORD param1, short param2, short param3);
int ff7_load_save_file(int param_1);

// camera
void ff7_battle_camera_hook_init();
void ff7_update_battle_camera(short cameraScriptIndex);
void ff7_update_idle_battle_camera();

// animation
void ff7_battle_animations_hook_init();

// field
void ff7_field_hook_init();
void field_load_textures(struct ff7_game_obj *game_object, struct struc_3 *struc_3);
void field_layer1_pick_tiles(short x_offset, short y_offset);
void field_layer2_pick_tiles(short x_offset, short y_offset);
void field_layer3_pick_tiles(short x_offset, short y_offset);
void field_layer4_pick_tiles(short x_offset, short y_offset);
void ff7_field_clip_with_camera_range(vector2<short>* point);
void ff7_field_layer3_clip_with_camera_range(field_trigger_header* trigger_header, vector2<short>* point);
uint32_t field_open_flevel_siz();
void field_init_scripted_bg_movement();
void field_update_scripted_bg_movement();
bool ff7_field_do_draw_3d_model(short x, short y);
void ff7_field_set_fade_quad_size(int x, int y, int width, int height);

// world
void ff7_world_hook_init();
void ff7_world_update_model_movement(int delta_position_x, int delta_position_z);

// file
FILE *open_lgp_file(char *filename, uint32_t mode);
void close_lgp_file(FILE *fd);
extern char lgp_names[18][256];
uint32_t lgp_chdir(char *path);
struct lgp_file *lgp_open_file(char *filename, uint32_t lgp_num);
uint32_t lgp_seek_file(uint32_t offset, uint32_t lgp_num);
uint32_t lgp_read(uint32_t lgp_num, char *dest, uint32_t size);
uint32_t lgp_read_file(struct lgp_file *file, uint32_t lgp_num, char *dest, uint32_t size);
uint32_t lgp_get_filesize(struct lgp_file *file, uint32_t lgp_num);
void close_file(struct ff7_file *file);
struct ff7_file *open_file(struct file_context *file_context, char *filename);
uint32_t __read_file(uint32_t count, void *buffer, struct ff7_file *file);
uint32_t read_file(uint32_t count, void *buffer, struct ff7_file *file);
uint32_t __read(FILE *file, char *buffer, uint32_t count);
uint32_t write_file(uint32_t count, void *buffer, struct ff7_file *file);
uint32_t get_filesize(struct ff7_file *file);
uint32_t tell_file(struct ff7_file *file);
void seek_file(struct ff7_file *file, uint32_t offset);
char *make_pc_name(struct file_context *file_context, struct ff7_file *file, char *filename);

// graphics
void destroy_d3d2_indexed_primitive(struct indexed_primitive *ip);
uint32_t ff7gl_load_group(uint32_t group_num, struct matrix_set *matrix_set, struct p_hundred *_hundred_data, struct p_group *_group_data, struct polygon_data *polygon_data, struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object);
struct tex_header *sub_673F5C(struct struc_91 *struc91);
void draw_single_triangle(struct nvertex *vertices);
void sub_6B2720(struct indexed_primitive *ip);
void draw_3d_model(uint32_t current_frame, struct anim_header *anim_header, struct struc_110 *struc_110, struct hrc_data *hrc_data, struct ff7_game_obj *game_object);

// loaders
struct anim_header *load_animation(struct file_context *file_context, char *filename);
struct battle_hrc_header *read_battle_hrc(uint32_t use_file_context, struct file_context *file_context, char *filename);
struct polygon_data *load_p_file(struct file_context *file_context, uint32_t create_lists, char *filename);
void destroy_tex_header(struct ff7_tex_header *tex_header);
struct ff7_tex_header *load_tex_file(struct file_context *file_context, char *filename);

// dsound
int ff7_dsound_create(HWND hwnd, LPGUID guid);
void ff7_dsound_release();
int ff7_dsound_createsoundbuffer(const WAVEFORMATEX *waveFormatEx);
