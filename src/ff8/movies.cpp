/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 myst6re                                            //
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

#include "movies.h"

#include "../globals.h"
#include "../log.h"

struct FF8Bink {
	pak_pointers_entry entry;
	FILE *f;
};

void *ff8_bink_open(uint8_t disc, uint32_t movie)
{
	const pak_pointers_entry &pak_pointer = ff8_externals.disc_pak_offsets[disc][movie];
	char filename[MAX_PATH] = {};
	strcpy(filename, ff8_externals.data_drive_path);
	strcat(filename, ff8_externals.disc_pak_filenames[disc]);

	if (trace_all || trace_files || trace_movies) ffnx_trace("%s: disc=%d movie=%d filename=%s\n", __func__, disc, movie, filename);

	FF8Bink *ret = new FF8Bink();

	ret->entry = pak_pointer;
	ret->f = fopen(filename, "rb");

	if (ret->f == nullptr) {
		delete ret;

		return nullptr;
	}

	if (fseek(ret->f, pak_pointer.bik_offset, SEEK_SET) != 0) {
		fclose(ret->f);

		delete ret;

		return nullptr;
	}

	return ret;
}

int ff8_bink_read(void *opaque, uint8_t *buf, int buf_size)
{
	if (trace_all || true) ffnx_trace("%s: buf_size=%d\n", __func__, buf_size);

	if (opaque == nullptr) {
		return AVERROR_EXIT;
	}

	if (buf_size == 0) {
		return 0;
	}

	FF8Bink *f = (FF8Bink *)opaque;

	int r = fread(buf, 1, buf_size, f->f);

	if (feof(f->f) || ftell(f->f) >= f->entry.bik_lowres_offset) {
		return AVERROR_EOF;
	}

	if (r < 0) {
		return AVERROR_EXIT;
	}

	return r;
}

int64_t ff8_bink_seek(void *opaque, int64_t offset, int whence)
{
	if (trace_all || true) ffnx_trace("%s: offset=%d, whence=%X\n", __func__, offset, whence);

	if (opaque == nullptr) {
		return AVERROR_EXIT;
	}

	FF8Bink *f = (FF8Bink *)opaque;

	if (whence == AVSEEK_SIZE) {
		return f->entry.bik_lowres_offset - f->entry.bik_offset;
	}

	whence &= 0xFFFF;

	if (whence == SEEK_END) {
		offset = f->entry.bik_lowres_offset + offset;
		whence = SEEK_SET;
	} else if (whence == SEEK_SET) {
		offset = f->entry.bik_offset + offset;
	} else if (whence != SEEK_CUR) {
		ffnx_error("%s: seek type not supported: %d\n", __func__, whence);

		return AVERROR_EXIT;
	}

	if (fseek(f->f, offset, whence) != 0) {
		return AVERROR_EXIT;
	}

	return ftell(f->f);
}

void ff8_bink_close(void *opaque)
{
	if (trace_all || trace_files || trace_movies) ffnx_trace("%s\n", __func__);

	if (opaque == nullptr) {
		return;
	}

	FF8Bink *f = (FF8Bink *)opaque;

	fclose(f->f);

	delete f;
}
