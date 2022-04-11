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
#pragma once

#include <stdio.h>
#include <map>
#include <string>
#include <windows.h>

struct ZzzTocEntry
{
	uint64_t filePos;
	uint32_t fileSize;
	char fileName[128];
	uint32_t fileNameSize;
};

class Zzz
{
public:
	class File
	{
	public:
		enum Whence {
			SeekSet = SEEK_SET,
			SeekEnd = SEEK_END,
			SeekCur = SEEK_CUR
		};
		int64_t seek(int32_t pos, Whence whence);
		int read(void *data, unsigned int size);
		inline int64_t relativePos() const {
			return int64_t(_pos - _tocEntry.filePos);
		}
		inline uint64_t absolutePos() const {
			return _tocEntry.filePos;
		}
		inline uint32_t size() const {
			return _tocEntry.fileSize;
		}
		inline const char *fileName() const {
			return _tocEntry.fileName;
		}
		inline uint32_t fileNameSize() const {
			return _tocEntry.fileNameSize;
		}
		inline int fd() const {
			return _fd;
		}
	private:
		friend class Zzz;
		File(const ZzzTocEntry &tocEntry, int fd);
		~File();
		const ZzzTocEntry _tocEntry;
		int _fd;
		int64_t _pos;
	};

	Zzz();
	~Zzz() {}
	errno_t open(const char *fileName);
	bool isOpen() const;
	File *openFile(const char *fileName, size_t fileNameSize) const;
	static void closeFile(File *file);
	bool fileExists(const char *fileName, size_t fileNameSize) const;
	inline const char *fileName() const {
		return _fileName;
	}
private:
	friend class File;
	bool lookup(const char *fileName, size_t fileNameSize, ZzzTocEntry &tocEntry) const;
	bool openHeader();

	std::map<std::string, ZzzTocEntry> _toc;
	char _fileName[MAX_PATH];
	FILE *_f;
};
