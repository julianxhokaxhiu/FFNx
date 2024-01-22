/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include "../common.h"
#include "../renderer.h"
#include <stdio.h>
#include <libpng16/png.h>

static void LibPngErrorCb(png_structp png_ptr, const char* error)
{
    ffnx_error("libpng error: %s\n", error);
}

static void LibPngWarningCb(png_structp png_ptr, const char* warning)
{
    ffnx_info("libpng warning: %s\n", warning);
}

bool loadPng(const char *filename, bimg::ImageMip &mip)
{
    FILE* file = fopen(filename, "rb");

    if (!file)
    {
        return false;
    }

    png_infop info_ptr = nullptr;
    png_structp png_ptr = nullptr;

    png_uint_32 _width = 0, _height = 0;
    png_byte color_type = 0, bit_depth = 0;

    png_bytepp rowptrs = nullptr;
    size_t rowbytes = 0;

    uint8_t* data = nullptr;
    size_t datasize = 0;

    fseek(file, 0, SEEK_END);
    datasize = ftell(file);
    fseek(file, 0, SEEK_SET);

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)0, LibPngErrorCb, LibPngWarningCb);

    if (!png_ptr)
    {
        fclose(file);

        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);

        fclose(file);

        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

        fclose(file);

        return false;
    }

    png_init_io(png_ptr, file);

    png_set_filter(png_ptr, 0, PNG_FILTER_NONE);

    if (!Renderer::doesItFitInMemory(datasize))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

        fclose(file);

        return false;
    }

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);

    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    _width = png_get_image_width(png_ptr, info_ptr);
    _height = png_get_image_height(png_ptr, info_ptr);

    rowptrs = png_get_rows(png_ptr, info_ptr);
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    datasize = rowbytes * _height;

    if (!Renderer::doesItFitInMemory(datasize))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

        fclose(file);

        return false;
    }

    data = (uint8_t*)driver_calloc(datasize, sizeof(uint8_t));

    for (png_uint_32 y = 0; y < _height; y++) memcpy(data + (rowbytes * y), rowptrs[y], rowbytes);

    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

    fclose(file);

    // ------------------------------------------------------------

    bimg::TextureFormat::Enum texFmt = bimg::TextureFormat::Unknown;

    switch (bit_depth)
    {
    case 8:
    {
        switch (color_type)
        {
        case PNG_COLOR_TYPE_GRAY:
            texFmt = bimg::TextureFormat::R8;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            texFmt = bimg::TextureFormat::RG8;
            break;
        case PNG_COLOR_TYPE_RGB:
            texFmt = bimg::TextureFormat::RGB8;
            break;
        case PNG_COLOR_TYPE_RGBA:
        case PNG_COLOR_TYPE_PALETTE:
            texFmt = bimg::TextureFormat::RGBA8;
            break;
        }
        break;
    }
    case 16:
    {
        switch (color_type)
        {
        case PNG_COLOR_TYPE_GRAY:
            texFmt = bimg::TextureFormat::R16;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            texFmt = bimg::TextureFormat::RG16;
            break;
        case PNG_COLOR_TYPE_RGB:
        case PNG_COLOR_TYPE_RGBA:
            texFmt = bimg::TextureFormat::RGBA16;
            break;
        case PNG_COLOR_TYPE_PALETTE:
            break;
        }
        break;
    }
    default:
        break;
    }

    if (texFmt != bimg::TextureFormat::Unknown)
    {
        mip.m_blockSize = 0;
        mip.m_bpp = 0;
        mip.m_data = data;
        mip.m_depth = 0;
        mip.m_format = texFmt;
        mip.m_hasAlpha = true;
        mip.m_size = datasize;
        mip.m_width = _width;
        mip.m_height = _height;

        return true;
    }
    else
    {
        driver_free(data);
    }

    return false;
}

bimg::ImageContainer *loadImageContainer(bx::AllocatorI *allocator, const char *filename, bimg::TextureFormat::Enum targetFormat, bool isPng)
{
    bimg::ImageContainer* img = nullptr;

    if (isPng)
    {
        bimg::ImageMip mip;
        if (loadPng(filename, mip))
        {
            img = bimg::imageAlloc(allocator, mip.m_format, mip.m_width, mip.m_height, mip.m_depth, 1, false, false, mip.m_data);

            driver_free((void *)mip.m_data);

            return img;
        }
    }

    FILE* file = fopen(filename, "rb");

    if (!file)
    {
        return img;
    }

    size_t filesize = 0;
    char* buffer = nullptr;

    fseek(file, 0, SEEK_END);
    filesize = ftell(file);

    if (Renderer::doesItFitInMemory(filesize + 1))
    {
        buffer = (char*)driver_malloc(filesize + 1);
        fseek(file, 0, SEEK_SET);
        fread(buffer, filesize, 1, file);
    }

    fclose(file);

    if (buffer != nullptr)
    {
        img = bimg::imageParse(allocator, buffer, filesize + 1, targetFormat);

        driver_free(buffer);
    }

    return img;
}
