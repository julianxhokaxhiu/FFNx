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

#include "tim.h"

#include "../common.h"
#include "../log.h"
#include "../saveload.h"

bool tim_to_rgba32(uint32_t *target, uint8_t bpp, uint8_t *img_data, uint16_t img_w, uint16_t img_h, uint16_t *pal_data)
{
    if (bpp == 0)
    {
        if (pal_data == nullptr)
        {
            ffnx_error("%s bpp 0 without palette\n", __func__);

            return false;
        }

        for (int y = 0; y < img_h; ++y)
        {
            for (int x = 0; x < img_w / 2; ++x)
            {
                *target = fromR5G5B5Color(pal_data[*img_data & 0xF]);

                ++target;

                *target = fromR5G5B5Color(pal_data[*img_data >> 4]);

                ++target;
                ++img_data;
            }
        }
    }
    else if (bpp == 1)
    {
        if (pal_data == nullptr)
        {
            ffnx_error("%s bpp 1 without palette\n", __func__);

            return false;
        }

        for (int y = 0; y < img_h; ++y)
        {
            for (int x = 0; x < img_w; ++x)
            {
                *target = fromR5G5B5Color(pal_data[*img_data]);

                ++target;
                ++img_data;
            }
        }
    }
    else if (bpp == 2)
    {
        uint16_t *img_data16 = (uint16_t *)img_data;

        for (int y = 0; y < img_h; ++y)
        {
            for (int x = 0; x < img_w; ++x)
            {
                *target = fromR5G5B5Color(*img_data16);

                ++target;
                ++img_data16;
            }
        }
    }
    else
    {
        ffnx_error("%s unknown bpp %d\n", __func__, bpp);

        return false;
    }

    return true;
}

void save_tim(const char *fileName, uint8_t bpp, ff8_tim *tim_infos, uint32_t palette_index)
{
    uint32_t w = tim_infos->img_w;

    if (bpp == 0)
    {
        w *= 4;
    }
    else if (bpp == 1)
    {
        w *= 2;
    }

    // allocate PBO
    uint32_t image_data_size = w * tim_infos->img_h * 4;
    uint32_t *image_data = (uint32_t*)driver_malloc(image_data_size);

    // convert source data
    if (image_data != nullptr)
    {
        // TODO: multiple palettes
        if (tim_to_rgba32(image_data, bpp, tim_infos->img_data, w, tim_infos->img_h, tim_infos->pal_data))
        {
            // TODO: is animated
            save_texture(image_data, image_data_size, w, tim_infos->img_h, palette_index, fileName, false);
        }

        driver_free(image_data);
    }
}

void save_lzs(const char *fileName, uint8_t *uncompressed_data)
{
    uint16_t *header = (uint16_t *)uncompressed_data;
    ff8_tim tim_infos = ff8_tim();
    tim_infos.img_w = header[2];
    tim_infos.img_h = header[3];
    tim_infos.img_data = uncompressed_data + 8;

    save_tim(fileName, 2, &tim_infos);
}
