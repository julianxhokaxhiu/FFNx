/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
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
#include "../ff8.h"
#include "../log.h"

#include <fcntl.h>
#include <io.h>

char next_direct_file[MAX_PATH] = "";

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
	if (strncmp(fullpath + 2, ff8_externals.archive_path_prefix, strlen(ff8_externals.archive_path_prefix)) != 0)
	{
		if (trace_all || trace_direct) ffnx_warning("%s: file ignored for direct path %s\n", __func__, fullpath);

		return false;
	}

	_snprintf(output, output_size, "%s\\%s\\%s", basedir, direct_mode_path.c_str(), fullpath + get_fl_prefix_size());

	return true;
}

int ff8_fs_archive_search_filename2(const char *fullpath, ff8_file_fi_infos *fi_infos_for_the_path, const ff8_file_container *file_container)
{
	if (trace_all || trace_files) ffnx_trace("%s: Looking in archive for %s\n", __func__, fullpath);

	int ret = ff8_externals.ff8_fs_archive_search_filename2(fullpath, fi_infos_for_the_path, file_container);

	if (ret != 1 && file_container != nullptr)
	{
		// Lookup without the language in the path
		size_t prefix_size = get_fl_prefix_size();

		if (trace_all || trace_files) ffnx_trace("%s: file not found, searching again with another language %s...\n", __func__, fullpath + prefix_size);

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

int ff8_open(const char *fileName, int oflag, ...)
{
	va_list va;

	va_start(va, oflag);
	int pmode = va_arg(va, int);

	if (trace_all || trace_files) ffnx_trace("%s: %s pmode=%X\n", __func__, fileName, pmode);

	if (next_direct_file && *next_direct_file != '\0')
	{
		if (trace_all || trace_direct) ffnx_info("Direct file using %s\n", next_direct_file);

		int ret = ff8_externals._sopen(next_direct_file, oflag, 64, pmode);

		*next_direct_file = '\0';

		return ret;
	}

	return ff8_externals._sopen(fileName, oflag, 64, pmode);
}

FILE *ff8_fopen(const char *fileName, const char *mode)
{
	if (trace_all || trace_files) ffnx_trace("%s: %s mode=%s\n", __func__, fileName, mode);

	if (next_direct_file && *next_direct_file != '\0')
	{
		if (trace_all || trace_direct) ffnx_info("Direct file using %s\n", next_direct_file);

		FILE *file = ff8_externals._fsopen(next_direct_file, mode, 64);

		*next_direct_file = '\0';

		return file;
	}

	return ff8_externals._fsopen(fileName, mode, 64);
}

ff8_file *ff8_open_file(ff8_file_context *infos, const char *fs_path)
{
	if (trace_all || trace_files) ffnx_trace("%s: %s mode=%d callback=%p noOpen=%d archive=%p field_4=%d\n", __func__, fs_path, infos->mode, infos->filename_callback, infos->field_4, infos->file_container, infos->field_4);

	ff8_file *file;
	char fullpath[256];

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

				file->fd = ff8_externals._sopen(direct_path, oflag, 64, pmode);

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
				// We need to use the external _open, and not the official one
				file->fd = ff8_externals._sopen(fullpath, oflag, 64, pmode);
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
