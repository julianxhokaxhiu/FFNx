/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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
#include "../patch.h"
#include "remaster.h"

#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <lz4.h>
#include <map>
#include "Shlwapi.h"

char next_direct_file[MAX_PATH] = "";
bool last_fopen_is_redirected = false;
uint32_t last_compression_type = 0;
size_t last_compressed_size = 0;
size_t last_uncompressed_size = 0;
std::map<int, Zzz::File *> openedZzzFiles;

size_t get_fl_prefix_size(bool with_lang = true)
{
	return 2 + strlen(with_lang ? ff8_externals.archive_path_prefix : "\\FF8\\Data\\");
}

void ff8_fs_lang_string(char *data)
{
	strncpy(data, ff8_externals.archive_path_prefix + 10, strlen(ff8_externals.archive_path_prefix) - 10 - 1);
}

bool set_direct_path(const char *fullpath, char *output, size_t output_size)
{
	if (strnicmp(fullpath + 2, "\\FF8\\Data\\Magic\\", strlen("\\FF8\\Data\\Magic\\")) == 0)
	{
		_snprintf(output, output_size, "%s/%s/%s", basedir, direct_mode_path.c_str(), fullpath + get_fl_prefix_size(false));

		return true;
	}

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

int ff8_remastered_open_from_zzz_archives(const char *fileName)
{
	if (trace_all || trace_files) ffnx_trace("%s: fileName=%s\n", __func__, fileName);

	Zzz *archive = &g_FF8ZzzArchiveMain;

	if (strstr(fileName, "data\\sound\\") != nullptr
		|| strstr(fileName, "data\\music\\") != nullptr) {
		archive = &g_FF8ZzzArchiveOther;
	}

	Zzz::File *file = archive->openFile(fileName);
	if (file != nullptr) {
		openedZzzFiles[file->fd()] = file;

		return file->fd();
	}

	return -1;
}

int ff8_fs_archive_search_filename2(const char *fullpath, ff8_file_fi_infos *fi_infos_for_the_path, const ff8_file_container *file_container)
{
	if (trace_all || trace_files) ffnx_trace("%s: Looking in archive for %s\n", __func__, fullpath);

	int ret = ff8_externals.ff8_fs_archive_search_filename2(fullpath, fi_infos_for_the_path, file_container);

	if (ret != 1 && file_container != nullptr && !remastered_edition)
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
	else if (ret != 1 && remastered_edition)
	{
		int fullpath_len = strlen(fullpath);
		if (fullpath_len > 4) {
			char extension[5] = {};
			strncpy(extension, fullpath + fullpath_len - 4, 4);

			char *modifiablePath = const_cast<char *>(fullpath);
			modifiablePath[fullpath_len - 4] = '_';
			modifiablePath[fullpath_len - 3] = '\0';
			concat_lang_str(modifiablePath);
			strcat(modifiablePath, extension);

			ffnx_error("%s: retry with %s...\n", __func__, modifiablePath);

			ret = ff8_externals.ff8_fs_archive_search_filename2(modifiablePath, fi_infos_for_the_path, file_container);
		}

		if (ret != 1) {
			ffnx_error("%s: file not found: %s\n", __func__, fullpath);
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

void ff8_fs_archive_field_concat_extension(char *fileName, char *extension)
{
	// Remastered edition only
	if (strstr(extension, ".msd") != NULL || strstr(extension, ".jsm") != NULL
		|| (JP_VERSION && strstr(extension, ".inf") != NULL))
	{
		strcat(fileName, "_");
		concat_lang_str(fileName);
		strcat(fileName, extension);
	}
	else
	{
		strcat(fileName, extension);
	}
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

	if (remastered_edition)
	{
		bool isZzzFile = false;
		bool is_redirected = ff8_steam_redirection(fileName, _filename, &isZzzFile);

		if (oflag == (_O_BINARY | _O_RDONLY) && isZzzFile) {
			int ret = ff8_remastered_open_from_zzz_archives(is_redirected ? _filename : fileName);

			if (ret != -1) {
				return ret;
			}

			if (trace_all || trace_files) ffnx_info("Fallback to Steam path mode %s\n", _filename);
		}

		return ff8_externals._sopen(is_redirected ? _filename : fileName, oflag, shflag, pmode);
	}

	bool is_redirected = ff8_attempt_redirection(fileName, _filename, sizeof(_filename));

	last_fopen_is_redirected = is_redirected;

	int ret = ff8_externals._sopen(is_redirected ? _filename : fileName, oflag, shflag, pmode);

	last_fopen_is_redirected = false;

	return ret;
}

int ff8_read(int fd, void *buffer, unsigned int bufferSize)
{
	if (trace_all || trace_files) ffnx_info("%s: fd=%X bufferSize=%d\n", __func__, fd, bufferSize);

	if (remastered_edition && openedZzzFiles.contains(fd))
	{
		return openedZzzFiles.at(fd)->read(buffer, bufferSize);
	}

	if (fd < *ff8_externals._io_fd_number && (*(uint8_t *)(ff8_externals._io_known_fds[fd >> 5] + 36 * (fd & 0x1F) + 4) & 1))
	{
		ff8_externals._lock_fhandle(fd);
		int ret = ff8_externals._read_lk(fd, buffer, bufferSize);
		ff8_externals._unlock_fhandle(fd);

		return ret;
	}

	*(ff8_externals._errno()) = EBADF;
	*(ff8_externals.__doserrno()) = 0;

	return -1;
}

int ff8_write(int fd, void *buffer, unsigned int bufferSize)
{
	if (trace_all || trace_files) ffnx_info("%s: fd=%X bufferSize=%d\n", __func__, fd, bufferSize);

	if (remastered_edition && openedZzzFiles.contains(fd))
	{
		ffnx_error("%s: Trying to write in a ZZZ archive is forbidden\n");

		return -1;
	}

	if (fd < *ff8_externals._io_fd_number && (*(uint8_t *)(ff8_externals._io_known_fds[fd >> 5] + 36 * (fd & 0x1F) + 4) & 1))
	{
		ff8_externals._lock_fhandle(fd);
		int ret = ff8_externals._write_lk(fd, buffer, bufferSize);
		ff8_externals._unlock_fhandle(fd);

		return ret;
	}

	*(ff8_externals._errno()) = EBADF;
	*(ff8_externals.__doserrno()) = 0;

	return -1;
}

__int32 ff8_lseek(int fd, __int32 offset, int whence)
{
	if (trace_all || trace_files) ffnx_info("%s: fd=%X, offset=%d, whence=%d\n", __func__, fd, offset, whence);

	if (remastered_edition && openedZzzFiles.contains(fd)) {
		Zzz::File *file = openedZzzFiles.at(fd);
		uint32_t pos = offset;
		bx::Whence::Enum zzzWhence = bx::Whence::Begin;

		if (whence == SEEK_END) {
			zzzWhence = bx::Whence::End;
		} else if (whence == SEEK_CUR) {
			zzzWhence = bx::Whence::Current;
		} else if (whence != SEEK_SET) {
			ffnx_error("%s: seek type not supported: %d\n", __func__, whence);

			return -1;
		}

		return file->seek(pos, zzzWhence);
	}

	// Original implementation
	if (fd < *ff8_externals._io_fd_number && (*(uint8_t *)(ff8_externals._io_known_fds[fd >> 5] + 36 * (fd & 0x1F) + 4) & 1))
	{
		ff8_externals._lock_fhandle(fd);
		int ret = ff8_externals._lseek_lk(fd, offset, whence);
		ff8_externals._unlock_fhandle(fd);

		return ret;
	}

	*(ff8_externals._errno()) = EBADF;
	*(ff8_externals.__doserrno()) = 0;

	return -1;
}

__int32 ff8_filelength(int fd)
{
	if (trace_all) ffnx_info("%s: fd=%X\n", __func__, fd);

	if (remastered_edition && openedZzzFiles.contains(fd)) {
		Zzz::File *file = openedZzzFiles.at(fd);

		ffnx_info("%s: length=%d\n", __func__, file->size());

		return file->size();
	}

	// Original implementation
	if (fd < *ff8_externals._io_fd_number && (*(uint8_t *)(ff8_externals._io_known_fds[fd >> 5] + 36 * (fd & 0x1F) + 4) & 1))
	{
		ff8_externals._lock_fhandle(fd);
		int currentPos = ff8_externals._lseek_lk(fd, 0, SEEK_CUR);
		int fileSize = -1;
		if (currentPos != -1)
		{
			fileSize = ff8_externals._lseek_lk(fd, 0, SEEK_END);

			if (fileSize != currentPos)
			{
				ff8_externals._lseek_lk(fd, currentPos, SEEK_SET);
			}
		}
		ff8_externals._unlock_fhandle(fd);

		return fileSize;
	}

	*(ff8_externals._errno()) = EBADF;
	*(ff8_externals.__doserrno()) = 0;

	return -1;
}

int ff8_close(int fd)
{
	if (trace_all || trace_files) ffnx_info("%s: fd=%X\n", __func__, fd);

	if (remastered_edition && openedZzzFiles.contains(fd)) {
		Zzz::closeFile(openedZzzFiles.at(fd));
		openedZzzFiles.erase(fd);

		return 0;
	}

	// Original implementation
	if (fd < *ff8_externals._io_fd_number && (*(uint8_t *)(ff8_externals._io_known_fds[fd >> 5] + 36 * (fd & 0x1F) + 4) & 1))
	{
		ff8_externals._lock_fhandle(fd);
		int ret = ff8_externals._close_lk(fd);
		ff8_externals._unlock_fhandle(fd);

		return ret;
	}

	*(ff8_externals._errno()) = EBADF;
	*(ff8_externals.__doserrno()) = 0;

	return -1;
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

	if (remastered_edition)
	{
		bool is_redirected = ff8_steam_redirection(fileName, _filename);
		return ff8_externals._fsopen(is_redirected ? _filename : fileName, mode, shflag);
	}

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
				file->fd = -1;
				char _filename[256]{ 0 };

				if (remastered_edition)
				{
					bool isZzzFile = false;
					bool is_redirected = ff8_steam_redirection(fullpath, _filename, &isZzzFile);
					isZzzFile = isZzzFile && oflag == (_O_BINARY | _O_RDONLY);

					if (isZzzFile) {
						file->fd = ff8_remastered_open_from_zzz_archives(is_redirected ? _filename : fullpath);
					}

					if (file->fd == -1 || !isZzzFile)
					{
						if (trace_all || trace_files) ffnx_info("Fallback to Steam path mode %s\n", is_redirected ? _filename : fullpath);

						file->fd = ff8_externals._sopen(is_redirected ? _filename : fullpath, oflag, _SH_DENYNO, pmode);

						if (file->fd == -1)
						{
							if (trace_all || trace_files) ffnx_info("Fallback to original path mode %s\n", fullpath);
						}
					}
				}

				if (file->fd == -1)
				{
					bool is_redirected = ff8_attempt_redirection(fullpath, _filename, sizeof(_filename));

					last_fopen_is_redirected = is_redirected;

					// We need to use the external _open, and not the official one
					file->fd = ff8_externals._sopen(is_redirected ? _filename : fullpath, oflag, shflag, pmode);

					last_fopen_is_redirected = false;
				}
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

bool ff8_steam_redirection(const char *lpFileName, char *newPath, bool *isZzzFile)
{
	bool redirected = false;

	if (isZzzFile != nullptr)
	{
		*isZzzFile = false;
	}

	if (strstr(lpFileName, "CD:") != NULL)
	{
		uint8_t requiredDisk = (*ff8_externals.savemap_field)->curr_disk;
		CHAR diskAsChar[2];

		itoa(requiredDisk, diskAsChar, 10);

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		if (strstr(lpFileName, "DISK1") != NULL || strstr(lpFileName, "DISK2") != NULL || strstr(lpFileName, "DISK3") != NULL || strstr(lpFileName, "DISK4") != NULL)
		{
			if (isZzzFile == nullptr)
			{
				strcpy(newPath, ff8_externals.app_path);
			}
			PathAppendA(newPath, R"(data\disk)");
			PathAppendA(newPath, pos);

			if (strstr(lpFileName, diskAsChar) != NULL)
			{
				redirected = true;
			}
		}

		if (isZzzFile != nullptr)
		{
			*isZzzFile = true;
		}
	}
	else if (strstr(lpFileName, "app.log") || strstr(lpFileName, "ff8input.cfg"))
	{
		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_userdata_path(newPath, MAX_PATH, false);
		PathAppendA(newPath, JP_VERSION ? "ff8input_jp.cfg" : pos);

		redirected = true;
	}
	else if (strstr(lpFileName, "temp.fi") || strstr(lpFileName, "temp.fl") || strstr(lpFileName, "temp.fs") || strstr(lpFileName, "temp_evn.") || strstr(lpFileName, "temp_odd."))
	{
		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_userdata_path(newPath, MAX_PATH, false);
		PathAppendA(newPath, pos);

		redirected = true;
	}
	else if (strstr(lpFileName, ".fi") != NULL || strstr(lpFileName, ".fl") != NULL || strstr(lpFileName, ".fs") != NULL)
	{
		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		if (remastered_edition && (strstr(lpFileName, "field") != NULL || strstr(lpFileName, "magic") != NULL || strstr(lpFileName, "world") != NULL))
		{
			PathAppendA(newPath, R"(data)");
		}
		else
		{
			get_data_lang_path(newPath, isZzzFile == nullptr);
		}

		PathAppendA(newPath, pos);

		if (isZzzFile != nullptr)
		{
			*isZzzFile = true;
		}

		redirected = true;
	}
	else if (StrStrIA(lpFileName, R"(SAVE\)") != NULL) // SAVE\SLOTX\saveN or save\chocorpg
	{
		CHAR saveFileName[50]{ 0 };

		// Search for the next character pointer after "SAVE\"
		const char* pos = StrStrIA(lpFileName, R"(SAVE\)") + 5;
		strcpy(saveFileName, pos);
		_strlwr(saveFileName);
		char* posSeparator = strstr(saveFileName, R"(\)");
		if (posSeparator != NULL)
		{
			*posSeparator = '_';
		}
		strcat(saveFileName, R"(.ff8)");

		get_userdata_path(newPath, MAX_PATH, true);
		PathAppendA(newPath, saveFileName);

		redirected = true;
	}
	else if (isZzzFile != nullptr)
	{
		if (strncmp(lpFileName, ff8_externals.app_path, strlen(ff8_externals.app_path)) == 0) {
			// Remove app_path
			strcpy(newPath, lpFileName + strlen(ff8_externals.app_path) + 1);

			redirected = true;
		}

		*isZzzFile = true;
	}

	if (redirected && (trace_all || trace_files)) ffnx_info("Redirected: %s -> %s (is in ZZZ archive: %s)\n", lpFileName, newPath, isZzzFile == nullptr ? "not asked" : (*isZzzFile ? "yes" : "no"));

	return redirected;
}
