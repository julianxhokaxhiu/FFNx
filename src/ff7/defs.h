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

#pragma once

// battle
void magic_thread_start(void (*func)());

// misc
uint32_t get_equipment_stats(uint32_t party_index, uint32_t type);
void kernel2_reset_counters();
char *kernel2_add_section(uint32_t size);
char *kernel2_get_text(uint32_t section_base, uint32_t string_id, uint32_t section_offset);
void ff7_wm_activateapp(bool hasFocus);
int ff7_get_gamepad();
struct ff7_gamepad_status* ff7_update_gamepad_status();

// field
void field_load_textures(struct ff7_game_obj *game_object, struct struc_3 *struc_3);
void field_layer2_pick_tiles(short x_offset, short y_offset);
uint32_t field_open_flevel_siz();

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
int attempt_redirection(char* in, char* out, size_t size, bool wantsSteamPath = false);

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
