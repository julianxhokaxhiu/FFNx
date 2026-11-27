/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 HobbitDur                                          //
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

#include "pictures.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"
#include "../../common.h"
#include "../../log.h"
#include "../file.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// -------------------------------------------------------------------------
// AddMorePictures - new pictures for the magazines and tutorial books.
//
// A magazine or tutorial page shows the picture of a mngrp.bin slot. The menu
// loads that slot (a TIM, whose header gives the VRAM placement), then the PC
// port swaps in a standalone texture through two switches on the slot number:
// Menu_RegisterTexUpload only queues the 37 slots it knows, and Menu_images
// names one hard-coded file per slot (slot 28 is menu/mag00.TEX, ...). Both
// switches stop at slot 184, so a mod can only replace the 30 page pictures.
//
// Here a slot past the switches (185-255) is queued too when the direct
// folder provides menu/magNNN.TEX, and Menu_images loads that file. It runs
// the game's own code for it with slot 28 standing in, so every step but the
// file name is the vanilla one. The 37 vanilla slots are left untouched.
// -------------------------------------------------------------------------

#define VANILLA_FIRST_SLOT 9  // Menu_RegisterTexUpload: slot - 9 indexes its case table
#define VANILLA_LAST_SLOT 184 // both switches end here
#define ADDED_FIRST_SLOT (VANILLA_LAST_SLOT + 1)
#define ADDED_LAST_SLOT 255 // a page picks its slot with one byte; mngrphd.bin has 256 entries
#define CARRIER_SLOT 28     // a plain picture case of Menu_images (mag00), run for an added slot

struct ff8_menu_texture_upload
{
    int32_t file_id;   // mngrp.bin slot, -1 = free
    void *destination; // the buffer the slot is loaded into
};

// Whether a mod provides menu/magNNN.TEX for an added slot, checked the first time the slot is asked for
enum class AddedPicture : uint8_t
{
    Unchecked,
    Missing,
    Present
};

static struct
{
    uint32_t load_mngrp_texture_async; // Menu_LoadMngrpTextureAsync (0x4AC400 in US)
    uint32_t register_tex_upload;      // Menu_RegisterTexUpload (0x4B3C00)
    uint32_t upload_callback;          // its completion callback (0x4ABFF0), calls Menu_images twice
    uint32_t menu_images;              // Menu_images (0x4B3D00)
    const uint8_t *vanilla_case_table; // one byte per slot 9-184, which case of the switch it takes
    int32_t *upload_count;
    ff8_menu_texture_upload *uploads;
    int upload_capacity;                                   // the list ends where the count starts (11 entries)
    AddedPicture added_pictures[ADDED_LAST_SLOT + 1] = {}; // all Unchecked
    int current_added_slot = -1;                           // the added slot Menu_images is loading, -1 none
} pictures;

static void added_picture_path(int slot, const char *extension, bool in_archive, char *output, size_t output_size)
{
    _snprintf(output, output_size, in_archive ? "c:%smag%03d.%s" : "%smag%03d.%s", ff8_externals.archive_path_prefix_menu, slot, extension);
}

static bool is_vanilla_slot(int slot)
{
    if (slot < VANILLA_FIRST_SLOT || slot > VANILLA_LAST_SLOT)
    {
        return false;
    }

    // Slot 9 is queued, so its case is the one that queues
    return pictures.vanilla_case_table[slot - VANILLA_FIRST_SLOT] == pictures.vanilla_case_table[0];
}

static bool is_added_slot(int slot)
{
    if (slot < ADDED_FIRST_SLOT || slot > ADDED_LAST_SLOT)
    {
        return false;
    }

    if (pictures.added_pictures[slot] == AddedPicture::Unchecked)
    {
        char path[MAX_PATH] = {};
        added_picture_path(slot, "TEX", true, path, sizeof(path));
        pictures.added_pictures[slot] = ff8_fs_direct_file_exists(path) ? AddedPicture::Present : AddedPicture::Missing;

        if (pictures.added_pictures[slot] == AddedPicture::Present)
        {
            ffnx_info("AddMorePictures: mngrp.bin slot %d shows %s\n", slot, path);
        }
    }

    return pictures.added_pictures[slot] == AddedPicture::Present;
}

// Replaces Menu_RegisterTexUpload: the vanilla slots as before, plus the added ones
static int ff8_menu_register_tex_upload(int slot, void *destination)
{
    int count = *pictures.upload_count;

    if ((is_vanilla_slot(slot) || is_added_slot(slot)) && count < pictures.upload_capacity)
    {
        pictures.uploads[count].file_id = slot;
        pictures.uploads[count].destination = destination;
        *pictures.upload_count = count + 1;
    }

    return *pictures.upload_count;
}

// Calls Menu_images for a loaded slot. For an added slot, the queued entry is
// given the carrier slot, and the texture load below takes the added file.
static int ff8_menu_images(void *vram_rect, void *staging)
{
    pictures.current_added_slot = -1;

    for (int i = 0; i < *pictures.upload_count; ++i)
    {
        if (pictures.uploads[i].destination == staging)
        {
            if (pictures.uploads[i].file_id >= ADDED_FIRST_SLOT)
            {
                pictures.current_added_slot = pictures.uploads[i].file_id;
                pictures.uploads[i].file_id = CARRIER_SLOT;
            }

            break;
        }
    }

    int ret = ((int (*)(void *, void *))pictures.menu_images)(vram_rect, staging);

    pictures.current_added_slot = -1;

    return ret;
}

// The texture load at the end of Menu_images, the one every picture case goes through
static ff8_graphics_object *ff8_menu_images_load_texture(int a1, int a2, ff8_create_graphic_object *infos, char *path, void *a5)
{
    char added_path[MAX_PATH] = {};

    if (pictures.current_added_slot >= 0)
    {
        added_picture_path(pictures.current_added_slot, "tim", infos->file_container != nullptr, added_path, sizeof(added_path));

        if (trace_all || trace_files)
        {
            ffnx_trace("AddMorePictures: loading %s instead of %s\n", added_path, path);
        }

        path = added_path;
    }

    return ((ff8_graphics_object * (*)(int, int, ff8_create_graphic_object *, char *, void *))(ff8_externals._load_texture))(a1, a2, infos, path, a5);
}

void ff8_menu_pictures_init()
{
    if (ff8_remastered_edition)
    {
        return; // The Remastered menu textures are not these files
    }

    // The item menu loads a magazine page's picture through Menu_LoadMngrpTextureAsync
    pictures.load_mngrp_texture_async = get_relative_call(uint32_t(ff8_externals.menu_use_items_sub_4F81F0), JP_VERSION ? 0x3159 : 0x334F);
    pictures.register_tex_upload = get_relative_call(pictures.load_mngrp_texture_async, 0xC);
    pictures.upload_callback = get_absolute_value(pictures.load_mngrp_texture_async, 0x39); // mov [eax+0Ch], offset callback
    pictures.menu_images = get_relative_call(pictures.upload_callback, 0x1B);
    pictures.vanilla_case_table = (const uint8_t *)get_absolute_value(pictures.register_tex_upload, 0x12); // mov dl, ds:table[eax]
    pictures.upload_count = (int32_t *)get_absolute_value(pictures.register_tex_upload, 0x1E);             // mov eax, count
    pictures.uploads = (ff8_menu_texture_upload *)get_absolute_value(pictures.register_tex_upload, 0x25);  // mov uploads.file_id[eax*8], ecx
    pictures.upload_capacity = int((uint32_t(pictures.upload_count) - uint32_t(pictures.uploads)) / sizeof(ff8_menu_texture_upload));

    replace_function(pictures.register_tex_upload, ff8_menu_register_tex_upload);
    replace_call(pictures.upload_callback + 0x1B, ff8_menu_images);
    replace_call(pictures.upload_callback + 0x2C, ff8_menu_images);
    replace_call(pictures.menu_images + 0x1AEF, ff8_menu_images_load_texture);
}
