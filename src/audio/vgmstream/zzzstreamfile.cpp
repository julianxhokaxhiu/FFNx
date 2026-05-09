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
/************************************************************************************/
// From vgmstream COPYING file:                                                     //
// Copyright (c) 2008-2019 Adam Gashlin, Fastelbja, Ronny Elfert, bnnm,             //
//                      Christopher Snowhill, NicknineTheEagle, bxaimc,             //
//                      Thealexbarney, CyberBotX, et al                             //
//                                                                                  //
// Portions Copyright (c) 2004-2008, Marko Kreen                                    //
// Portions Copyright 2001-2007  jagarl / Kazunori Ueno <jagarl@creator.club.ne.jp> //
// Portions Copyright (c) 1998, Justin Frankel/Nullsoft Inc.                        //
// Portions Copyright (C) 2006 Nullsoft, Inc.                                       //
// Portions Copyright (c) 2005-2007 Paul Hsieh                                      //
// Portions Public Domain originating with Sun Microsystems                         //
//                                                                                  //
// Permission to use, copy, modify, and distribute this software for any            //
// purpose with or without fee is hereby granted, provided that the above           //
// copyright notice and this permission notice appear in all copies.                //
//                                                                                  //
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES         //
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF                 //
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR          //
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES           //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN            //
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF          //
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.                   //
/************************************************************************************/

#include "zzzstreamfile.h"
#include "../../log.h"
#include "../../ff8/remaster.h"

/* a STREAMFILE that operates via standard IO using a buffer */
struct ZZZ_STREAMFILE {
    STREAMFILE vt;          /* callbacks */

    Zzz::File* infile;      /* actual FILE */
    offv_t offset;          /* last read offset (info) */
    offv_t buf_offset;      /* current buffer data start */
    uint8_t* buf;           /* data buffer */
    size_t buf_size;        /* max buffer size */
    size_t valid_size;      /* current buffer size */
};

size_t zzz_read(ZZZ_STREAMFILE* sf, uint8_t* dst, offv_t offset, size_t length)
{
    if (trace_all) ffnx_trace("%s: %s\n", __func__, sf->infile->fileName());
    size_t read_total = 0;

    if (dst == nullptr || length <= 0 || offset < 0)
    {
        return read_total;
    }

    /* is the part of the requested length in the buffer? */
    if (offset >= sf->buf_offset && offset < sf->buf_offset + sf->valid_size)
    {
        size_t buf_limit;
        int buf_into = int(offset - sf->buf_offset);

        buf_limit = sf->valid_size - buf_into;
        if (buf_limit > length) {
            buf_limit = length;
        }

        memcpy(dst, sf->buf + buf_into, buf_limit);
        read_total += buf_limit;
        length -= buf_limit;
        offset += buf_limit;
        dst += buf_limit;
    }

    /* read the rest of the requested length */
    while (length > 0)
    {
        size_t length_to_read;

        /* ignore requests at EOF */
        if (offset >= sf->infile->size() || sf->infile->seek(offset, bx::Whence::Begin) < 0)
        {
            break;
        }

        /* fill the buffer (offset now is beyond buf_offset) */
        sf->buf_offset = offset;
        sf->valid_size = sf->infile->read((char *)sf->buf, sf->buf_size);

        /* decide how much must be read this time */
        length_to_read = length > sf->buf_size ? sf->buf_size : length;

        /* give up on partial reads (EOF) */
        if (sf->valid_size < length_to_read) {
            memcpy(dst, sf->buf, sf->valid_size);
            offset += sf->valid_size;
            read_total += sf->valid_size;
            break;
        }

        /* use the new buffer */
        memcpy(dst, sf->buf, length_to_read);
        offset += length_to_read;
        read_total += length_to_read;
        length -= length_to_read;
        dst += length_to_read;
    }

    sf->offset = offset; /* last fread offset */
    return read_total;
}

size_t zzz_get_size(ZZZ_STREAMFILE* sf)
{
    return sf->infile->size();
}

offv_t zzz_get_offset(ZZZ_STREAMFILE* sf)
{
    return sf->infile->relativePos();
}

void zzz_get_name(ZZZ_STREAMFILE* sf, char* name, size_t name_size)
{
    int copy_size = sf->infile->fileNameSize() + 1;
    if (copy_size > name_size)
    {
        copy_size = name_size;
    }

    memcpy(name, sf->infile->fileName(), copy_size);
    name[copy_size - 1] = '\0';
    if (trace_all) ffnx_trace("%s: %s %d %d %s\n", __func__, name, copy_size, name_size, sf->infile->fileName());
}

STREAMFILE* zzz_open(ZZZ_STREAMFILE* sf, const char* const filename, size_t buf_size)
{
    if (trace_all || trace_files) ffnx_trace("%s: %s\n", __func__, filename);
    return open_ZZZ_STREAMFILE(filename, buf_size);
}

void zzz_close(ZZZ_STREAMFILE* sf)
{
    if (trace_all || trace_files) ffnx_trace("%s: %s\n", __func__, sf->infile->fileName());
    Zzz::closeFile(sf->infile);
    delete[] sf->buf;
    delete sf;
}

STREAMFILE* open_ZZZ_STREAMFILE(const char* const filename, size_t buf_size)
{
    if (trace_all || trace_files) ffnx_trace("%s: %s\n", __func__, filename);
    if (filename == nullptr)
    {
        return nullptr;
    }

    Zzz::File* zzz_file = g_FF8ZzzArchiveOther.openFile(filename);

    if (zzz_file == nullptr) {
        return nullptr;
    }

    uint8_t* buf = new uint8_t[buf_size];
    memset(buf, 0, buf_size);

    ZZZ_STREAMFILE* this_sf = new ZZZ_STREAMFILE();

    this_sf->vt.read = (size_t (*)(STREAMFILE*, uint8_t*, offv_t, size_t))zzz_read;
    this_sf->vt.get_size = (size_t (*)(STREAMFILE*))zzz_get_size;
    this_sf->vt.get_offset = (offv_t (*)(STREAMFILE*))zzz_get_offset;
    this_sf->vt.get_name = (void (*)(STREAMFILE*, char*, size_t))zzz_get_name;
    this_sf->vt.open = (STREAMFILE* (*)(STREAMFILE*, const char* const, size_t))zzz_open;
    this_sf->vt.close = (void (*)(STREAMFILE*))zzz_close;

    this_sf->infile = zzz_file;
    this_sf->buf_size = buf_size;
    this_sf->buf = buf;

    return &this_sf->vt;
}
