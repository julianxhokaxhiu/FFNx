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
#include <bx/file.h>

constexpr size_t ZZZ_FILENAME_MAX_SIZE = 128;

struct ZzzTocEntry
{
	uint64_t filePos;
	uint32_t fileSize;
	char fileName[ZZZ_FILENAME_MAX_SIZE];
	uint32_t fileNameSize;
};

class Zzz
{
public:
	class File : public bx::ReaderSeekerI
	{
	public:
		int64_t seek(int64_t offset = 0, bx::Whence::Enum whence = bx::Whence::Current) override;
		int read(void *data, unsigned int size);
		inline virtual int32_t read(void* data, int32_t size, bx::Error* _err) override {
			return read(data, size);
		}
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
		virtual ~File();
		const ZzzTocEntry _tocEntry;
		int _fd;
		int64_t _pos;
	};

	Zzz();
	~Zzz() {}
	errno_t open(const char *fileName);
	bool isOpen() const;
	File *openFile(const char *fileName) const;
	static void closeFile(File *file);
	inline bool fileExists(const char *fileName) const {
		return lookup(fileName);
	}
	inline const char *fileName() const {
		return _fileName;
	}
private:
	friend class File;
	bool lookup(const char *fileName, ZzzTocEntry *tocEntry = nullptr) const;
	static void toWindowsSeparators(const char *fileName, char *transformedFileName);
	bool openHeader();

	std::map<std::string, ZzzTocEntry> _toc;
	char _fileName[MAX_PATH];
	FILE *_f;
};
