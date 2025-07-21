/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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

#include "../ff8.h"

#include <stdio.h>

int ff8_fs_archive_search_filename2(const char *fullpath, ff8_file_fi_infos *fi_infos_for_the_path, const ff8_file_container *file_container);
void ff8_fs_archive_sub_archive_get_filename(const char *filename, char *path);
ff8_file_container *ff8_fs_archive_open_temp(char *fl_path, char *fs_path, char *fi_path);
int ff8_fs_archive_search_filename_sub_archive(const char *fullpath, ff8_file_fi_infos *fi_infos_for_the_path, const ff8_file_container *file_container);
void ff8_fs_archive_free_file_container_sub_archive(ff8_file_container *file_container);
void ff8_fs_archive_patch_compression(uint32_t compression_type);
uint8_t *ff8_fs_archive_malloc_source_data(size_t size, char *source_code_path, int line);
uint8_t *ff8_fs_archive_malloc_target_data(size_t size, char *source_code_path, int line);
void ff8_fs_archive_uncompress_data(const uint8_t *source_data, uint8_t *target_data);

int ff8_open(const char *fileName, int oflag, ...);
FILE *ff8_fopen(const char *fileName, const char *mode);
ff8_file *ff8_open_file(ff8_file_context *infos, const char *fs_path);
uint32_t(*ff8_read_file)(uint32_t count, void* buffer, struct ff8_file* file);
void (*ff8_close_file)(struct ff8_file* file);

void ff8_fs_lang_string(char *data);

bool ff8_fs_last_fopen_is_redirected();
