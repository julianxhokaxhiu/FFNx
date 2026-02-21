/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 myst6re                                            //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
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
#include "zzz_archive.h"

#include <algorithm>
#include <io.h>
#include <fcntl.h>

#include "../log.h"

Zzz::Zzz() :
	_f(nullptr)
{
	_fileName[0] = '\0';
}

errno_t Zzz::open(const char *fileName)
{
	strncpy(_fileName, fileName, MAX_PATH - 1);
	errno_t err = fopen_s(&_f, fileName, "rb");

	if (err != 0) {
		return err;
	}

	if (!openHeader()) {
		return EILSEQ;
	}

	fclose(_f);

	return 0;
}

bool Zzz::isOpen() const
{
	return !_toc.empty();
}

bool Zzz::lookup(const char *fileName, ZzzTocEntry *tocEntry) const
{
	if (trace_all || trace_files) ffnx_trace("Zzz::%s: %s\n", __func__, fileName);
	char transformedFileName[ZZZ_FILENAME_MAX_SIZE];
	toWindowsSeparators(fileName, transformedFileName);

	auto it = _toc.find(transformedFileName);

	if (it == _toc.end()) {
		if (trace_all || trace_files) ffnx_error("Zzz::%s: file %s not found in %s\n", __func__, transformedFileName, _fileName);

		return false;
	}

	if (tocEntry != nullptr) {
		*tocEntry = it->second;
	}

	return true;
}

void Zzz::toWindowsSeparators(const char *fileName, char *transformedFileName)
{
	for (int i = 0; i < ZZZ_FILENAME_MAX_SIZE; ++i) {
		if (fileName[i] == '/') {
			transformedFileName[i] = '\\';
		} else if (fileName[i] == '\0') {
			transformedFileName[i] = '\0';

			return;
		} else {
			transformedFileName[i] = tolower(fileName[i]);
		}
	}

	transformedFileName[ZZZ_FILENAME_MAX_SIZE - 1] = '\0';
}

bool Zzz::openHeader()
{
	if (!_toc.empty())
	{
		return true;
	}

	// Optimization: use a buffer to minimize the number of fread calls
	constexpr int bufferSize = 0x4000;
	char buffer[bufferSize], *cur = buffer, *end = buffer + bufferSize;

	if (fread(cur, bufferSize, 1, _f) != 1)
	{
		ffnx_error("Zzz::%s: cannot read toc entries\n", __func__);

		return false;
	}

	uint32_t fileCount = *(uint32_t *)buffer;
	cur += sizeof(uint32_t);

	if (trace_all || trace_files) ffnx_trace("Zzz::%s: found %d files\n", __func__, fileCount);

	for (uint64_t i = 0; i < fileCount; ++i)
	{
		size_t remainingSize = size_t(end - cur);

		if (remainingSize < 128 + 16) {
			// Read data again
			if (remainingSize > 0) {
				memcpy(buffer, cur, remainingSize);
			}
			if (fread(buffer + remainingSize, bufferSize - remainingSize, 1, _f) != 1) {
				ffnx_error("Zzz::%s: cannot read toc entry %d\n", __func__, i);

				return false;
			}
			cur = buffer;
			end = buffer + bufferSize;
		}

		/**
		 * Toc entry format:
		 * | Offset | Size   | Description     |
		 * -------------------------------------
		 * | 0      | 4      | Filename length |
		 * | 4      | varies | Filename        |
		 * | varies | 8      | File pos        |
		 * | varies | 4      | File size       |
		 */

		ZzzTocEntry tocEntry;
		uint32_t fileNameSize = *(uint32_t *)cur;
		cur += sizeof(uint32_t);
		if (fileNameSize > 128) {
			ffnx_error("Zzz::%s: cannot read toc entry %d, fileNameSize > 128\n", __func__, i);

			return false;
		}
		memcpy(tocEntry.fileName, cur, fileNameSize);
		cur += fileNameSize;
		tocEntry.fileName[fileNameSize] = '\0';
		tocEntry.fileNameSize = fileNameSize;
		tocEntry.filePos = *(uint64_t *)cur;
		cur += sizeof(uint64_t);
		tocEntry.fileSize = *(uint32_t *)cur;
		cur += sizeof(uint32_t);

		_toc[tocEntry.fileName] = tocEntry;
	}

	return true;
}

Zzz::File *Zzz::openFile(const char *fileName) const
{
	int fd = 0;

	if (trace_all || trace_files) ffnx_trace("Zzz::%s: %s\n", __func__, fileName);

	errno_t err = _sopen_s(&fd, _fileName, _O_BINARY, _SH_DENYNO, _S_IREAD);
	if (err != 0)
	{
		return nullptr;
	}

	ZzzTocEntry tocEntry;
	if (!lookup(fileName, &tocEntry))
	{
		_close(fd);

		return nullptr;
	}

	if (_lseeki64(fd, tocEntry.filePos, SEEK_SET) != tocEntry.filePos)
	{
		_close(fd);

		return nullptr;
	}

	return new File(tocEntry, fd);
}

void Zzz::closeFile(File *file)
{
	if (file != nullptr)
	{
		delete file;
	}
}

Zzz::File::File(const ZzzTocEntry &tocEntry, int fd) :
	_tocEntry(tocEntry), _fd(fd), _pos(tocEntry.filePos)
{
}

Zzz::File::~File()
{
	if (trace_all) ffnx_trace("Zzz::File::close: %s\n", _tocEntry.fileName);

	_close(_fd);
}

int64_t Zzz::File::seek(int64_t pos, bx::Whence::Enum whence)
{
	if (trace_all) ffnx_trace("Zzz::File::%s: %u\n", __func__, pos);

	switch (whence) {
		case bx::Whence::End:
			pos = _tocEntry.fileSize + (pos < 0 ? pos : 0);
			break;
		case bx::Whence::Current:
			pos += relativePos();
			break;
		case bx::Whence::Begin:
			break;
	}

	if (pos < 0) {
		return -1;
	}

	_pos = _lseeki64(_fd, _tocEntry.filePos + pos, SEEK_SET);

	if (trace_all) ffnx_trace("Zzz::File::%s: new pos=%lld filePos=%lld\n", __func__, _pos, _tocEntry.filePos);

	return relativePos();
}

int Zzz::File::read(void *data, unsigned int size)
{
	int64_t pos = relativePos();

	if (trace_all) ffnx_trace("Zzz::File::%s: size=%u pos=%lld fileSize=%u\n", __func__, size, pos, _tocEntry.fileSize);

	if (pos < 0) {
		return -1; // Before the beginning of the file
	}

	if (pos >= _tocEntry.fileSize) {
		return 0; // End Of File
	}

	int r = _read(_fd, data, std::min(size, uint32_t(_tocEntry.fileSize - pos)));

	if (trace_all) ffnx_trace("Zzz::File::%s: read %d bytes\n", __func__, r);

	if (r > 0) {
		_pos += r;
	}

	return r;
}
