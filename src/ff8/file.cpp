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

#include "file.h"
#include "utils.h"
#include "../ff8.h"
#include "../log.h"
#include "../redirect.h"

#include <fcntl.h>
#include <io.h>
#include <lz4.h>

char next_direct_file[MAX_PATH] = "";
bool last_fopen_is_redirected = false;
uint32_t last_compression_type = 0;
size_t last_compressed_size = 0;
size_t last_uncompressed_size = 0;

size_t get_fl_prefix_size()
{
	return 2 + strlen(ff8_externals.archive_path_prefix);
}

void ff8_fs_lang_string(char *data)
{
	strncpy(data, ff8_externals.archive_path_prefix + 10, strlen(ff8_externals.archive_path_prefix) - 10 - 1);
}

bool set_direct_path(const char *fullpath, char *output, size_t output_size)
{
	if (strnicmp(fullpath + 2, ff8_externals.archive_path_prefix, strlen(ff8_externals.archive_path_prefix)) != 0)
	{
		if (trace_all || trace_direct) ffnx_warning("%s: file ignored for direct path %s (should match %s)\n", __func__, fullpath, ff8_externals.archive_path_prefix);

		return false;
	}

	_snprintf(output, output_size, "%s/%s/%s", basedir, direct_mode_path.c_str(), fullpath + get_fl_prefix_size());

	return true;
}

bool check_direct_sub_archive_exists(const char *ext, const char *path_without_ext)
{
	char archive_path[MAX_PATH] = {}, direct_path[MAX_PATH] = {};

	sprintf(archive_path, "%s.%s", path_without_ext, ext);

	set_direct_path(archive_path, direct_path, sizeof(direct_path));

	return fileExists(direct_path);
}

void ff8_fs_archive_sub_archive_get_filename(const char *filename, char *dirname)
{
	ff8_externals.sub_archive_get_filename(filename, dirname);

	char cleaned_dirname[MAX_PATH] = "c:";
	size_t path_strlen = strlen(dirname);

	strncpy(cleaned_dirname + 2, dirname, path_strlen);

	if (cleaned_dirname[2 + path_strlen - 1] == '\n') {
		cleaned_dirname[2 + path_strlen - 1] = '\0';
		path_strlen -= 1;
	}

	if (cleaned_dirname[2 + path_strlen - 1] == '\\' || cleaned_dirname[2 + path_strlen - 1] == '/') {
		cleaned_dirname[2 + path_strlen - 1] = '\0';
	}

	if (check_direct_sub_archive_exists("fi", cleaned_dirname) && check_direct_sub_archive_exists("fl", cleaned_dirname) && check_direct_sub_archive_exists("fs", cleaned_dirname)) {
		set_direct_path(cleaned_dirname, next_direct_file, sizeof(next_direct_file));
		// Bypass subarchive opening
		strncpy(ff8_externals.temp_fs_path_cache, cleaned_dirname + 2, path_strlen);
	} else {
		if (trace_all || trace_direct) ffnx_warning("Direct files not found %s.fs, .fl and .fi\n", cleaned_dirname);

		next_direct_file[0] = '\0';
	}

	if (trace_all || trace_files) ffnx_trace("%s: filename=%s cleaned_dirname=%s next_direct_file=%s\n", __func__, filename, cleaned_dirname, next_direct_file);
}

ff8_file_container *ff8_fs_archive_open_temp(char *fl_path, char *fs_path, char *fi_path)
{
	if (next_direct_file[0] != '\0') {
		if (trace_all || trace_direct) ffnx_info("Direct file using %s.fs, .fl and .fi\n", next_direct_file);

		sprintf(fl_path, "%s.fl", next_direct_file);
		sprintf(fs_path, "%s.fs", next_direct_file);
		sprintf(fi_path, "%s.fi", next_direct_file);

		*next_direct_file = '\0';
	}

	if (trace_all || trace_files) ffnx_trace("%s: fl_path=%s fs_path=%s fi_path=%s\n", __func__, fl_path, fs_path, fi_path);

	return ff8_externals.archive_open(fl_path, fs_path, fi_path);
}

int ff8_fs_archive_search_filename2(const char *fullpath, ff8_file_fi_infos *fi_infos_for_the_path, const ff8_file_container *file_container)
{
	if (trace_all || trace_files) ffnx_trace("%s: Looking in archive for %s\n", __func__, fullpath);

	int ret = ff8_externals.ff8_fs_archive_search_filename2(fullpath, fi_infos_for_the_path, file_container);

	if (ret != 1 && file_container != nullptr)
	{
		// Lookup without the language in the path
		size_t prefix_size = get_fl_prefix_size();

		if (trace_all || trace_files) ffnx_warning("%s: file not found, searching again with another language %s...\n", __func__, fullpath + prefix_size);

		for (int id = 0; id < file_container->fl_infos->file_count; ++id)
		{
			char *path = ff8_externals.fs_archive_get_fl_filepath(id, file_container->fl_infos);

			if (!_stricmp(fullpath + prefix_size, path + prefix_size))
			{
				*fi_infos_for_the_path = file_container->fi_infos[id];

				if (trace_all || trace_files) ffnx_trace("%s: found archive file in another language\n", __func__);

				return 1;
			}
		}
	}

	return ret;
}

int ff8_fs_archive_search_filename_sub_archive(const char *fullpath, ff8_file_fi_infos *fi_infos_for_the_path, const ff8_file_container *file_container)
{
	if (trace_all || trace_files) ffnx_trace("%s %s\n", __func__, fullpath);

	char direct_path[MAX_PATH];

	set_direct_path(fullpath, direct_path, sizeof(direct_path));

	if (fileExists(direct_path))
	{
		strncpy(next_direct_file, direct_path, sizeof(next_direct_file));

		return 0; // Bypass Moriya filesystem
	}
	else
	{
		if (trace_all || trace_direct) ffnx_warning("Direct file not found %s\n", direct_path);
	}

	return ff8_fs_archive_search_filename2(fullpath, fi_infos_for_the_path, file_container);
}

void ff8_fs_archive_free_file_container_sub_archive(ff8_file_container *file_container)
{
	if (trace_all || trace_files) ffnx_trace("%s\n", __func__);

	*next_direct_file = '\0';

	return ff8_externals.free_file_container(file_container);
}

void ff8_fs_archive_patch_compression(uint32_t compression_type)
{
	if (trace_all || trace_files) ffnx_trace("%s compression_type=%d\n", __func__, compression_type);

	last_compression_type = compression_type;
}

uint8_t *ff8_fs_archive_malloc_source_data(size_t size, char *source_code_path, int line)
{
	if (trace_all || trace_files) ffnx_trace("%s size=%d\n", __func__, size);

	last_compressed_size = size - 12;

	return (uint8_t *)common_externals.assert_malloc(size, source_code_path, line);
}

uint8_t *ff8_fs_archive_malloc_target_data(size_t size, char *source_code_path, int line)
{
	if (trace_all || trace_files) ffnx_trace("%s size=%d\n", __func__, size);

	if (last_compression_type == 2) // LZ4 compression
	{
		last_uncompressed_size = size;

		size += 10;
	}

	return (uint8_t *)common_externals.assert_malloc(size, source_code_path, line);
}

void ff8_fs_archive_uncompress_data(const uint8_t *source_data, uint8_t *target_data)
{
	if (trace_all || trace_files) ffnx_trace("%s\n", __func__);

	if (last_compression_type == 2) // LZ4 compression
	{
		if (trace_all || trace_files) ffnx_trace("%s LZ4 compression detected\n", __func__);

		int uncompressed_size = LZ4_decompress_safe_partial((const char *)source_data + 12, (char *)target_data, last_compressed_size, last_uncompressed_size, last_uncompressed_size + 10);

		if (uncompressed_size < 0)
		{
			ffnx_error("%s: cannot uncompress lz4 file data (compressed_size=%d, uncompressed_size=%d, error=%d)\n", __func__, last_compressed_size, last_uncompressed_size, uncompressed_size);

			return;
		}

		if (uncompressed_size != last_uncompressed_size)
		{
			ffnx_warning("%s: uncompressed size is different than expected: %d != %d\n", __func__, uncompressed_size, last_uncompressed_size);

			return;
		}
	}
	else
	{
		((void(*)(const uint8_t*, uint8_t*))ff8_externals.lzs_uncompress)(source_data, target_data);
	}
}

bool ff8_attempt_redirection(const char *in, char *out, size_t size)
{
	// Remove AppPath from input
	if (strncmp(in, ff8_externals.app_path, strlen(ff8_externals.app_path)) == 0) {
		// +1 because the last '\' is stripped from AppPath by the game
		in += strlen(ff8_externals.app_path) + 1;

		return attempt_redirection(in, out, size) != -1;
	}

	return false;
}

int ff8_open(const char *fileName, int oflag, ...)
{
	va_list va;

	va_start(va, oflag);
	int pmode = va_arg(va, DWORD);
	const int shflag = _SH_DENYNO;

	if (trace_all || trace_files) ffnx_trace("%s: %s oflag=%X pmode=%X\n", __func__, fileName, oflag, pmode);

	if (next_direct_file && *next_direct_file != '\0')
	{
		if (trace_all || trace_direct) ffnx_info("Direct file using %s\n", next_direct_file);

		int ret = ff8_externals._sopen(next_direct_file, oflag, shflag, pmode);

		*next_direct_file = '\0';

		return ret;
	}

	char _filename[MAX_PATH]{ 0 };
	bool is_redirected = ff8_attempt_redirection(fileName, _filename, sizeof(_filename));

	last_fopen_is_redirected = is_redirected;

	int ret = ff8_externals._sopen(is_redirected ? _filename : fileName, oflag, shflag, pmode);

	last_fopen_is_redirected = false;

	return ret;
}

FILE *ff8_fopen(const char *fileName, const char *mode)
{
	if (trace_all || trace_files) ffnx_trace("%s: %s mode=%s\n", __func__, fileName, mode);

	const int shflag = _SH_DENYNO;

	if (next_direct_file && *next_direct_file != '\0')
	{
		if (trace_all || trace_direct) ffnx_info("Direct file using %s\n", next_direct_file);

		FILE *file = ff8_externals._fsopen(next_direct_file, mode, shflag);

		*next_direct_file = '\0';

		return file;
	}

	char _filename[MAX_PATH]{ 0 };
	bool is_redirected = ff8_attempt_redirection(fileName, _filename, sizeof(_filename));

	last_fopen_is_redirected = is_redirected;

	FILE *file = ff8_externals._fsopen(is_redirected ? _filename : fileName, mode, shflag);

	last_fopen_is_redirected = false;

	return file;
}

ff8_file *ff8_open_file(ff8_file_context *infos, const char *fs_path)
{
	if (trace_all || trace_files) ffnx_trace("%s: %s mode=%d callback=%p noOpen=%d archive=%p field_4=%d\n", __func__, fs_path, infos->mode, infos->filename_callback, infos->field_4, infos->file_container, infos->field_4);

	ff8_file *file;
	char fullpath[MAX_PATH];
	const int shflag = _SH_DENYNO;

	if (infos->filename_callback != nullptr)
	{
		infos->filename_callback(fs_path, fullpath);
	}

	if (infos->field_4)
	{
		file = (ff8_file *)external_calloc(1u, sizeof(ff8_file));

		if (file)
		{
			file->filename = fs_path;
			memcpy(&file->file_context, infos, sizeof(ff8_file_context));

			if (infos->mode > 1 && infos->mode <= 3)
			{
				ffnx_error("%s: CANT CREATE/WRITE TO FILE %s; FILE SYSTEM IS READ ONLY \n", __func__, fs_path);
			}
		}
	}
	else
	{
		int pmode = _S_IREAD, oflag = _O_BINARY;

		if (infos->mode == 1)
		{
			oflag = _O_TEXT;
		}
		else if (infos->mode > 1)
		{
			oflag |= _O_CREAT | _O_RDWR;
			pmode |= _S_IWRITE;

			if (infos->mode == 3)
			{
				oflag |= _O_TRUNC;
			}
		}

		file = (ff8_file *)external_calloc(1u, sizeof(ff8_file));

		if (file != nullptr)
		{
			// Note: the code in FF8 is strange here, with a condition on infos->filename_callback
			strncpy(fullpath, fs_path, sizeof(fullpath));

			if (infos->file_container != nullptr)
			{
				char direct_path[MAX_PATH];

				set_direct_path(fullpath, direct_path, sizeof(direct_path));

				file->fd = ff8_externals._sopen(direct_path, oflag, shflag, pmode);

				if (file->fd != -1)
				{
					if (trace_all || trace_direct) ffnx_info("Direct file using %s\n", direct_path);
				}
				else
				{
					if (trace_all || trace_direct) ffnx_warning("Direct file not found %s\n", direct_path);

					if (ff8_externals.fs_archive_search_filename(fullpath, &file->fi_infos, infos->file_container))
					{
						file->fd = 0;
						file->file_container = infos->file_container;
					}
				}
			}
			else
			{
				char _filename[256]{ 0 };
				bool is_redirected = ff8_attempt_redirection(fullpath, _filename, sizeof(_filename));

				last_fopen_is_redirected = is_redirected;

				// We need to use the external _open, and not the official one
				file->fd = ff8_externals._sopen(is_redirected ? _filename : fullpath, oflag, shflag, pmode);

				last_fopen_is_redirected = false;
			}

			file->is_open = 1;
			file->filename = ff8_externals.strcpy_with_malloc(fullpath);
			memcpy(&(file->file_context), infos, sizeof(ff8_file_context));

			if (file->fd == -1)
			{
				ffnx_error("%s: COULD NOT OPEN FILE %s\n", __func__, fs_path);
				ff8_close_file(file);
				file = nullptr;
			}
		}
	}

	return file;
}

bool ff8_fs_last_fopen_is_redirected()
{
	return last_fopen_is_redirected;
}
