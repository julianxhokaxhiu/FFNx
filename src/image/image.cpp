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

#include <stdio.h>
#include <libpng16/png.h>

#include "image.h"
#include "../common.h"
#include "../renderer.h"
#include "log.h"

static void LibPngErrorCb(png_structp png_ptr, const char* error)
{
    ffnx_error("libpng error: %s\n", error);
}

static void LibPngWarningCb(png_structp png_ptr, const char* warning)
{
    ffnx_info("libpng warning: %s\n", warning);
}

void read_png_file(png_structp png_ptr, png_bytep data, size_t size)
{
    bx::ReaderI *reader = (bx::ReaderI *)png_get_io_ptr(png_ptr);
    bx::Error err;

    int32_t r = bx::read(reader, data, size, &err);

    if (!err.isOk() || r != size) {
        png_error(png_ptr, "Cannot read data");
    }
}

bimg::ImageContainer *loadPng(bx::AllocatorI *allocator, const char *filename, bimg::TextureFormat::Enum targetFormat)
{
    bimg::ImageContainer *ret;
    bx::FileReader reader;
    bx::Error err;

    if (!bx::open(&reader, filename, &err) || !err.isOk()) {
        return nullptr;
    }

    if (trace_all || trace_loaders) ffnx_trace("%s: %s\n", __func__, filename);

    ret = loadPng(allocator, &reader, targetFormat);

    bx::close(&reader);

    return ret;
}

bimg::ImageContainer *loadPng(bx::AllocatorI *allocator, bx::ReaderI *reader, bimg::TextureFormat::Enum targetFormat)
{
    png_infop info_ptr = nullptr;
    png_structp png_ptr = nullptr;

    png_uint_32 _width = 0, _height = 0;
    png_byte color_type = 0, bit_depth = 0;

    png_bytepp rowptrs = nullptr;
    size_t rowbytes = 0;

    uint8_t* data = nullptr;
    size_t datasize = 0;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)0, LibPngErrorCb, LibPngWarningCb);

    if (!png_ptr)
    {
        return nullptr;
    }

    info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);

        return nullptr;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

        return nullptr;
    }

    png_set_read_fn(png_ptr, reader, read_png_file);

    png_set_filter(png_ptr, 0, PNG_FILTER_NONE);

    png_read_info(png_ptr, info_ptr);

    // Expand data to 24-bit RGB, or 8-bit grayscale, with alpha if available.
    png_set_expand(png_ptr);
    png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
    // Use 1 byte per pixel in 1, 2, or 4-bit depth files.
    png_set_packing(png_ptr);
    // Expand the grayscale to 24-bit RGB if necessary.
    png_set_gray_to_rgb(png_ptr);
    // Scale a 16-bit depth file down to 8-bit, accurately.
    png_set_scale_16(png_ptr);

    if (targetFormat == bimg::TextureFormat::BGRA8) {
        png_set_bgr(png_ptr);
    }

    if ((trace_all || trace_loaders) && (png_get_bit_depth(png_ptr, info_ptr) != 8 || png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)) {
        ffnx_warning("%s: PNG format is not RGBA32, it will be converted automatically (bit_depth=%d color_type=%X)\n", __func__, png_get_bit_depth(png_ptr, info_ptr), png_get_color_type(png_ptr, info_ptr));
    }

    int number_passes = png_set_interlace_handling(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    _width = png_get_image_width(png_ptr, info_ptr);
    _height = png_get_image_height(png_ptr, info_ptr);

    rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    datasize = rowbytes * _height;

    if (trace_all || trace_loaders) ffnx_trace("%s: data_size=%d width=%d height=%d bit_depth=%d color_type=%X\n", __func__, datasize, _width, _height, bit_depth, color_type);

    if (color_type != PNG_COLOR_TYPE_RGBA || bit_depth != 8)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

        ffnx_error("%s: Cannot convert PNG to RGBA32\n", __func__);

        return nullptr;
    }

    bimg::ImageContainer *image = bimg::imageAlloc(allocator, targetFormat == bimg::TextureFormat::Count ? bimg::TextureFormat::RGBA8 : targetFormat, _width, _height, 0, 1, false, false);

    if (image == nullptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

        return nullptr;
    }

    for (int pass = 0; pass < number_passes; pass++)
    {
        data = (uint8_t *)image->m_data;
        for (int y = 0; y < _height; y++)
        {
            png_read_row(png_ptr, data, NULL);
            data += rowbytes;
        }
    }

    png_read_end(png_ptr, info_ptr);

    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

    return image;
}

bool parseDds(const char *filename, DirectX::ScratchImage &image, DirectX::TexMetadata &metadata)
{
    wchar_t filenameW[MAX_PATH];

    mbstowcs(filenameW, filename, MAX_PATH);

    HRESULT hr = DirectX::LoadFromDDSFile(
        filenameW,
        DirectX::DDS_FLAGS_NONE, &metadata, image
    );
    if (FAILED(hr) || image.GetImageCount() == 0)
    {
        ffnx_error("%s: Load DDS from file error (%d)\n", __func__, HRESULT_CODE(hr));

        return false;
    }

    return true;
}

bimg::ImageContainer *convertDds(bx::AllocatorI *allocator, DirectX::ScratchImage &image, const DirectX::TexMetadata &metadata, bimg::TextureFormat::Enum targetFormat, int lod)
{
    if (lod >= image.GetImageCount())
    {
        lod = image.GetImageCount() - 1;
    }

    const DirectX::Image &mainImage = image.GetImages()[lod];
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if (targetFormat == bimg::TextureFormat::BGRA8) {
        format = DXGI_FORMAT_B8G8R8A8_UNORM;
    } else {
        targetFormat = bimg::TextureFormat::RGBA8;
    }

    DirectX::ScratchImage out;
    HRESULT hr;
    if (DirectX::IsCompressed(image.GetMetadata().format))
    {
        hr = DirectX::Decompress(mainImage, format, out);
        image.Release();
    }
    else if (image.GetMetadata().format != format)
    {
        hr = DirectX::Convert(mainImage, format, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, out);
        image.Release();
    }
    else
    {
        if (!Renderer::doesItFitInMemory(mainImage.rowPitch * mainImage.height + 1)) {
            return nullptr;
        }

        return bimg::imageAlloc(allocator, targetFormat, mainImage.width, mainImage.height, 0, 1, false, false, mainImage.pixels);
    }

    if (FAILED(hr) || out.GetImageCount() == 0)
    {
        ffnx_error("%s: Convert DDS error (%d)\n", __func__, HRESULT_CODE(hr));

        return nullptr;
    }

    const DirectX::Image &mainImage2 = out.GetImages()[0];

    if (!Renderer::doesItFitInMemory(mainImage2.rowPitch * mainImage2.height + 1)) {
        return nullptr;
    }

    return bimg::imageAlloc(allocator, targetFormat, mainImage2.width, mainImage2.height, 0, 1, false, false, mainImage2.pixels);
}

bimg::ImageContainer *loadImageContainer(bx::AllocatorI *allocator, const char *filename, bimg::TextureFormat::Enum targetFormat)
{
    FILE* file = fopen(filename, "rb");

    if (!file)
    {
        return nullptr;
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

    if (buffer == nullptr)
    {
        return nullptr;
    }

    bimg::ImageContainer* img = bimg::imageParse(allocator, buffer, filesize + 1, targetFormat);

    driver_free(buffer);

    return img;
}
