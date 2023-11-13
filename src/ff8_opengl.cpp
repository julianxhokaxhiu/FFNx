/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
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

#include "renderer.h"
#include "globals.h"
#include "common.h"
#include "ff8.h"
#include "fake_dd.h"
#include "patch.h"
#include "log.h"
#include "macro.h"
#include "movies.h"
#include "gl.h"
#include "saveload.h"
#include "gamepad.h"
#include "joystick.h"
#include "gamehacks.h"
#include "vibration.h"
#include "ff8/file.h"

unsigned char texture_reload_fix1[] = {0x5B, 0x5F, 0x5E, 0x5D, 0x81, 0xC4, 0x10, 0x01, 0x00, 0x00};
unsigned char texture_reload_fix2[] = {0x5F, 0x5E, 0x5D, 0x5B, 0x81, 0xC4, 0x8C, 0x00, 0x00, 0x00};

void ff8gl_field_78(struct ff8_polygon_set *polygon_set, struct ff8_game_obj *game_object)
{
	struct matrix_set *matrix_set;
	struct p_hundred *hundred_data = 0;
	uint32_t group_counter;

	if(trace_all) ffnx_trace("dll_gfx: field_78\n");

	if(!game_object->in_scene) return;

	if(polygon_set == 0) return;

	if(polygon_set->field_0 == 0) return;

	matrix_set = polygon_set->matrix_set;

	hundred_data = 0;

	if(polygon_set->field_2C) hundred_data = polygon_set->hundred_data;

	group_counter = 0;

	while(group_counter < polygon_set->numgroups)
	{
		uint32_t defer = false;
		uint32_t zsort = false;

		if(polygon_set->per_group_hundreds) hundred_data = polygon_set->hundred_data_group_array[group_counter];

		if(hundred_data)
		{
			if(game_object->field_91C && hundred_data->zsort) zsort = true;
			else if(!game_object->field_928) defer = (hundred_data->options & (BIT(V_ALPHABLEND) | BIT(V_TMAPBLEND)));
		}

		if(!defer) common_setrenderstate(hundred_data, (struct game_obj *)game_object);

		if(matrix_set && matrix_set->matrix_projection) gl_set_d3dprojection_matrix(matrix_set->matrix_projection);

		if(hundred_data) hundred_data = &hundred_data[1];

		group_counter++;
	}
}

void ff8gl_field_54(struct texture_set *texture_set, struct game_obj *game_object)
{
	if (trace_all) ffnx_trace("field_54\n");
}

void ff8gl_field_58(struct texture_set *texture_set, struct game_obj *game_object)
{
	if (trace_all) ffnx_trace("field_58\n");
}

void ff8gl_field_5C(struct texture_set *texture_set, struct game_obj *game_object)
{
	if (trace_all) ffnx_trace("field_5C\n");
}

void ff8gl_field_60(struct palette *palette, struct texture_set *texture_set)
{
	if(trace_all) ffnx_trace("field_60\n");
}

void ff8gl_field_84(uint32_t unknown, struct game_obj *game_object)
{
	if (trace_all) ffnx_trace("field_84\n");
}

void ff8gl_field_88()
{
	if (trace_all) ffnx_trace("field_88\n");
}

void ff8_destroy_tex_header(struct ff8_tex_header *tex_header)
{
	if(!tex_header) return;

	if((uint32_t)tex_header->file.pc_name > 32) external_free(tex_header->file.pc_name);

	external_free(tex_header->old_palette_data);
	external_free(tex_header->palette_colorkey);
	external_free(tex_header->tex_format.palette_data);
	external_free(tex_header->image_data);

	external_free(tex_header);
}

struct ff8_tex_header *ff8_load_tex_file(struct ff8_file_context* file_context, char *filename)
{
	struct ff8_tex_header *ret = (struct ff8_tex_header *)common_externals.create_tex_header();
	struct ff8_file* file = ff8_open_file(file_context, filename);
	uint32_t i, len;

	if(!file) goto error;
	if(!ff8_read_file(sizeof(*ret), ret, file)) goto error;

	ret->image_data = 0;
	ret->old_palette_data = 0;
	ret->palette_colorkey = 0;
	ret->tex_format.palette_data = 0;

	if(ret->version != 2) goto error;
	else
	{
		if(ret->tex_format.use_palette)
		{
			ret->tex_format.palette_data = (uint32_t*)common_externals.alloc_read_file(4, ret->tex_format.palette_size, (struct file *)file);
			if(!ret->tex_format.palette_data) goto error;
		}

		ret->image_data = (unsigned char*)common_externals.alloc_read_file(ret->tex_format.bytesperpixel, ret->tex_format.width * ret->tex_format.height, (struct file *)file);
		if(!ret->image_data) goto error;

		if(ret->use_palette_colorkey)
		{
			ret->palette_colorkey = (char*)common_externals.alloc_read_file(1, ret->palettes, (struct file *)file);
			if(!ret->palette_colorkey) goto error;
		}
	}

	ret->file.pc_name = (char*)external_malloc(1024);

	len = _snprintf(ret->file.pc_name, 1024, "%s", &filename[7]);

	for(i = 0; i < len; i++)
	{
		if(ret->file.pc_name[i] == '.')
		{
			if(!_strnicmp(&ret->file.pc_name[i], ".TEX", 4)) ret->file.pc_name[i] = 0;
			else ret->file.pc_name[i] = '_';
		}
	}

	ff8_close_file(file);
	return ret;

error:
	ff8_destroy_tex_header(ret);
	ff8_close_file(file);
	return 0;
}

#define TEXRELOAD_BUFFER_SIZE 64

struct
{
	char *image_data;
	uint32_t size;
	struct ff8_texture_set *texture_set;
} reload_buffer[TEXRELOAD_BUFFER_SIZE] = {};
uint32_t reload_buffer_index = 0;

// this function is wedged into the middle of a function designed to reload a Direct3D texture
// when the image data changes
void texture_reload_hack(struct ff8_texture_set *texture_set)
{
	uint32_t i;
	uint32_t size;
	VOBJ(tex_header, tex_header, texture_set->tex_header);

	size = VREF(tex_header, tex_format.width) * VREF(tex_header, tex_format.height) * VREF(tex_header, tex_format.bytesperpixel);

	// a circular buffer holds the last TEXRELOAD_BUFFER_SIZE textures that went through here
	// and their respective image data so that we can see if anything actually changed and avoid
	// unnecessary texture reloads
	for(i = 0; i < TEXRELOAD_BUFFER_SIZE; i++)
	{
		if(reload_buffer[i].texture_set == texture_set && reload_buffer[i].size == size && memcmp(reload_buffer[i].image_data, VREF(tex_header, image_data), size) == 0)
		{
			return;
		}
	}

	TexturePacker::TiledTex tiledTex = texturePacker.getTiledTex(VREF(tex_header, image_data));
	Tim::Bpp texBpp = VREF(tex_header, tex_format.bytesperpixel) == 2 ? Tim::Bpp16 : (VREF(tex_header, palette_entries) == 256 ? Tim::Bpp8 : Tim::Bpp4);

	if (tiledTex.isValid() && texBpp != tiledTex.bpp()) {
		if(trace_all || trace_vram) ffnx_trace("%s: ignore reload because BPP does not match 0x%X (bpp vram=%d, bpp tex=%d, source bpp tex=%d) image_data=0x%X\n", __func__, texture_set, tiledTex.bpp(), VREF(tex_header, tex_format.bytesperpixel), texBpp, VREF(tex_header, image_data));

		return;
	}

	common_unload_texture((struct texture_set *)texture_set);
	common_load_texture((struct texture_set *)texture_set, texture_set->tex_header, texture_set->texture_format);

	reload_buffer[reload_buffer_index].texture_set = texture_set;
	if (reload_buffer[reload_buffer_index].image_data != nullptr && reload_buffer[reload_buffer_index].size != size) {
		driver_free(reload_buffer[reload_buffer_index].image_data);
		reload_buffer[reload_buffer_index].image_data = nullptr;
	}
	if (reload_buffer[reload_buffer_index].image_data == nullptr) {
		reload_buffer[reload_buffer_index].image_data = (char*)driver_malloc(size);
	}
	memcpy(reload_buffer[reload_buffer_index].image_data, VREF(tex_header, image_data), size);
	reload_buffer[reload_buffer_index].size = size;
	reload_buffer_index = (reload_buffer_index + 1) % TEXRELOAD_BUFFER_SIZE;

	stats.texture_reloads++;

	if(trace_all || trace_vram) ffnx_trace("texture_reload_hack: 0x%X (bpp=%d, sourceBpp=%d) image_data=0x%X\n", texture_set, VREF(tex_header, tex_format.bytesperpixel), texBpp, VREF(tex_header, image_data));
}

void texture_reload_hack1(struct texture_page *texture_page, uint32_t unknown1, uint32_t unknown2)
{
	struct ff8_texture_set *texture_set = (struct ff8_texture_set *)texture_page->tri_gfxobj->hundred_data->texture_set;

	texture_reload_hack(texture_set);
}

void texture_reload_hack2(struct texture_page *texture_page, uint32_t unknown1, uint32_t unknown2)
{
	struct ff8_texture_set *texture_set = (struct ff8_texture_set *)texture_page->sub_tri_gfxobj->hundred_data->texture_set;

	texture_reload_hack(texture_set);
}

void ff8_unload_texture(struct ff8_texture_set *texture_set)
{
	uint32_t i;

	// remove any references to this texture
	for(i = 0; i < TEXRELOAD_BUFFER_SIZE; i++) if(reload_buffer[i].texture_set == texture_set) reload_buffer[i].texture_set = 0;
}

void swirl_sub_56D390(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
	static struct tex_header *last_tex_header = 0;
	struct tex_header *tex_header = make_framebuffer_tex(256, 256, x, y, w, h, false);

	if(last_tex_header) ff8_destroy_tex_header((struct ff8_tex_header *)last_tex_header);

	if(trace_all) ffnx_trace("swirl_sub_56D390: (%i, %i) %ix%i 0x%x (0x%x)\n", x, y, w, h, *ff8_externals.swirl_texture1, tex_header);

	common_unload_texture((*ff8_externals.swirl_texture1)->hundred_data->texture_set);
	common_load_texture((*ff8_externals.swirl_texture1)->hundred_data->texture_set, tex_header, texture_format);

	last_tex_header = tex_header;
}

int ff8_init_gamepad()
{
	if (xinput_connected)
	{
		if (gamepad.Refresh())
			return TRUE;
	}
	else
	{
		if (joystick.Refresh())
    	return TRUE;
	}

	return FALSE;
}

LPDIJOYSTATE2 ff8_update_gamepad_status()
{
	ff8_externals.dinput_gamepad_state->rgdwPOV[0] = -1;
	ff8_externals.dinput_gamepad_state->lX = 0;
	ff8_externals.dinput_gamepad_state->lY = 0;
	ff8_externals.dinput_gamepad_state->lRx = 0;
	ff8_externals.dinput_gamepad_state->lRy = 0;

	nxVibrationEngine.rumbleUpdate();

	if (xinput_connected)
	{
		if (!gamepad.Refresh() || !gamehacks.canInputBeProcessed()) return 0;

		if ((gamepad.leftStickY > 0.5f) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_UP))
		{
			ff8_externals.dinput_gamepad_state->lY = 0xFFFFFFFFFFFFFFFF;
			ff8_externals.dinput_gamepad_state->rgdwPOV[0] = 0;
		}
		else if ((gamepad.leftStickY < -0.5f) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_DOWN))
		{
			ff8_externals.dinput_gamepad_state->lY = -0xFFFFFFFFFFFFFFFF;
			ff8_externals.dinput_gamepad_state->rgdwPOV[0] = 18000;
		}

		if ((gamepad.leftStickX < -0.5f) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_LEFT))
		{
			ff8_externals.dinput_gamepad_state->lX = 0xFFFFFFFFFFFFFFFF;
			ff8_externals.dinput_gamepad_state->rgdwPOV[0] = 27000;
		}
		else if ((gamepad.leftStickX > 0.5f) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_RIGHT))
		{
			ff8_externals.dinput_gamepad_state->lX = -0xFFFFFFFFFFFFFFFF;
			ff8_externals.dinput_gamepad_state->rgdwPOV[0] = 9000;
		}

		if (gamepad.rightStickY > 0.5f)
			ff8_externals.dinput_gamepad_state->lRy = 0xFFFFFFFFFFFFFFFF;
		else if (gamepad.rightStickY < -0.5f)
			ff8_externals.dinput_gamepad_state->lRy = -0xFFFFFFFFFFFFFFFF;

		if (gamepad.rightStickX > 0.5f)
			ff8_externals.dinput_gamepad_state->lRx = -0xFFFFFFFFFFFFFFFF;
		else if (gamepad.rightStickX < -0.5f)
			ff8_externals.dinput_gamepad_state->lRx = 0xFFFFFFFFFFFFFFFF;

		ff8_externals.dinput_gamepad_state->lZ = 0;
		ff8_externals.dinput_gamepad_state->lRz = 0;
		ff8_externals.dinput_gamepad_state->rglSlider[0] = 0;
		ff8_externals.dinput_gamepad_state->rglSlider[1] = 0;
		ff8_externals.dinput_gamepad_state->rgdwPOV[1] = -1;
		ff8_externals.dinput_gamepad_state->rgdwPOV[2] = -1;
		ff8_externals.dinput_gamepad_state->rgdwPOV[3] = -1;
		ff8_externals.dinput_gamepad_state->rgbButtons[0] = gamepad.IsPressed(steam_edition ? XINPUT_GAMEPAD_A : XINPUT_GAMEPAD_X) ? 0x80 : 0; // Cross (Steam)/Square
		ff8_externals.dinput_gamepad_state->rgbButtons[1] = gamepad.IsPressed(steam_edition ? XINPUT_GAMEPAD_B : XINPUT_GAMEPAD_A) ? 0x80 : 0; // Circle (Steam)/Cross
		ff8_externals.dinput_gamepad_state->rgbButtons[2] = gamepad.IsPressed(steam_edition ? XINPUT_GAMEPAD_X : XINPUT_GAMEPAD_B) ? 0x80 : 0; // Square (Steam)/Circle
		ff8_externals.dinput_gamepad_state->rgbButtons[3] = gamepad.IsPressed(XINPUT_GAMEPAD_Y) ? 0x80 : 0; // Triangle
		ff8_externals.dinput_gamepad_state->rgbButtons[4] = gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER) ? 0x80 : 0; // L1
		ff8_externals.dinput_gamepad_state->rgbButtons[5] = gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 0x80 : 0; // R1
		ff8_externals.dinput_gamepad_state->rgbButtons[6] = (steam_edition ? gamepad.IsPressed(XINPUT_GAMEPAD_BACK) : gamepad.leftTrigger > 0.85f) ? 0x80 : 0; // SELECT (Steam)/L2
		ff8_externals.dinput_gamepad_state->rgbButtons[7] = (steam_edition ? gamepad.IsPressed(XINPUT_GAMEPAD_START) : gamepad.rightTrigger > 0.85f) ? 0x80 : 0; // START (Steam)/R2
		ff8_externals.dinput_gamepad_state->rgbButtons[8] = (steam_edition ? gamepad.leftTrigger > 0.85f : gamepad.IsPressed(XINPUT_GAMEPAD_BACK)) ? 0x80 : 0; // L2 (Steam)/SELECT
		ff8_externals.dinput_gamepad_state->rgbButtons[9] = (steam_edition ? gamepad.rightTrigger > 0.85f : gamepad.IsPressed(XINPUT_GAMEPAD_START)) ? 0x80 : 0; // R2 (Steam)/START
		ff8_externals.dinput_gamepad_state->rgbButtons[10] = gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_THUMB) ? 0x80 : 0; // L3
		ff8_externals.dinput_gamepad_state->rgbButtons[11] = gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_THUMB) ? 0x80 : 0; // R3
		ff8_externals.dinput_gamepad_state->rgbButtons[12] = gamepad.IsPressed(0x400) ? 0x80 : 0; // PS Button
	}
	else
	{
		if (!joystick.Refresh() || !gamehacks.canInputBeProcessed()) return 0;

		if ((joystick.GetState()->lY < joystick.GetDeadZone(-0.5f)) || joystick.GetState()->rgdwPOV[0] == 0)
		{
			ff8_externals.dinput_gamepad_state->lY = 0xFFFFFFFFFFFFFFFF;
			ff8_externals.dinput_gamepad_state->rgdwPOV[0] = 0;
		}
		else if ((joystick.GetState()->lY > joystick.GetDeadZone(0.5f)) || joystick.GetState()->rgdwPOV[0] == 18000)
		{
			ff8_externals.dinput_gamepad_state->lY = -0xFFFFFFFFFFFFFFFF;
			ff8_externals.dinput_gamepad_state->rgdwPOV[0] = 18000;
		}

		if ((joystick.GetState()->lX < joystick.GetDeadZone(-0.5f)) || joystick.GetState()->rgdwPOV[0] == 27000)
		{
			ff8_externals.dinput_gamepad_state->lX = 0xFFFFFFFFFFFFFFFF;
			ff8_externals.dinput_gamepad_state->rgdwPOV[0] = 27000;
		}
		else if ((joystick.GetState()->lX > joystick.GetDeadZone(0.5f)) || joystick.GetState()->rgdwPOV[0] == 9000)
		{
			ff8_externals.dinput_gamepad_state->lX = -0xFFFFFFFFFFFFFFFF;
			ff8_externals.dinput_gamepad_state->rgdwPOV[0] = 9000;
		}

		if (joystick.GetState()->lRy < joystick.GetDeadZone(-0.5f))
			ff8_externals.dinput_gamepad_state->lRy = 0xFFFFFFFFFFFFFFFF;
		else if (joystick.GetState()->lRy > joystick.GetDeadZone(0.5f))
			ff8_externals.dinput_gamepad_state->lRy = -0xFFFFFFFFFFFFFFFF;

		if (joystick.GetState()->lRx > joystick.GetDeadZone(0.5f))
			ff8_externals.dinput_gamepad_state->lRx = -0xFFFFFFFFFFFFFFFF;
		else if (joystick.GetState()->lRx < joystick.GetDeadZone(-0.5f))
			ff8_externals.dinput_gamepad_state->lRx = 0xFFFFFFFFFFFFFFFF;

		ff8_externals.dinput_gamepad_state->lZ = 0;
		ff8_externals.dinput_gamepad_state->lRz = 0;
		ff8_externals.dinput_gamepad_state->rglSlider[0] = 0;
		ff8_externals.dinput_gamepad_state->rglSlider[1] = 0;
		ff8_externals.dinput_gamepad_state->rgdwPOV[1] = -1;
		ff8_externals.dinput_gamepad_state->rgdwPOV[2] = -1;
		ff8_externals.dinput_gamepad_state->rgdwPOV[3] = -1;
		ff8_externals.dinput_gamepad_state->rgbButtons[0] = joystick.GetState()->rgbButtons[0] & 0x80 ? 0x80 : 0; // Square
		ff8_externals.dinput_gamepad_state->rgbButtons[1] = joystick.GetState()->rgbButtons[1] & 0x80 ? 0x80 : 0; // Cross
		ff8_externals.dinput_gamepad_state->rgbButtons[2] = joystick.GetState()->rgbButtons[2] & 0x80 ? 0x80 : 0; // Circle
		ff8_externals.dinput_gamepad_state->rgbButtons[3] = joystick.GetState()->rgbButtons[3] & 0x80 ? 0x80 : 0; // Triangle
		ff8_externals.dinput_gamepad_state->rgbButtons[4] = joystick.GetState()->rgbButtons[4] & 0x80 ? 0x80 : 0; // L1
		ff8_externals.dinput_gamepad_state->rgbButtons[5] = joystick.GetState()->rgbButtons[5] & 0x80 ? 0x80 : 0; // R1
		ff8_externals.dinput_gamepad_state->rgbButtons[6] = joystick.GetState()->rgbButtons[6] & 0x80 ? 0x80 : 0; // L2
		ff8_externals.dinput_gamepad_state->rgbButtons[7] = joystick.GetState()->rgbButtons[7] & 0x80 ? 0x80 : 0; // R2
		ff8_externals.dinput_gamepad_state->rgbButtons[8] = joystick.GetState()->rgbButtons[8] & 0x80 ? 0x80 : 0; // SELECT
		ff8_externals.dinput_gamepad_state->rgbButtons[9] = joystick.GetState()->rgbButtons[9] & 0x80 ? 0x80 : 0; // START
		ff8_externals.dinput_gamepad_state->rgbButtons[10] = joystick.GetState()->rgbButtons[10] & 0x80 ? 0x80 : 0; // L3
		ff8_externals.dinput_gamepad_state->rgbButtons[11] = joystick.GetState()->rgbButtons[11] & 0x80 ? 0x80 : 0; // R3
		ff8_externals.dinput_gamepad_state->rgbButtons[12] = joystick.GetState()->rgbButtons[12] & 0x80 ? 0x80 : 0; // PS Button
	}

	return ff8_externals.dinput_gamepad_state;
}

int ff8_is_window_active()
{
	if (gameHwnd == GetActiveWindow())
	{
		ff8_externals.engine_eval_keyboard_gamepad_input();
		ff8_externals.has_keyboard_gamepad_input();

		if (simulate_OK_button)
		{
			// Flag the button OK as pressed
			ff8_externals.engine_input_confirmed_buttons[1] = ff8_externals.engine_input_valid_buttons[1] = 0x40;

			// End simulation right here before we press this button by mistake in other windows
			simulate_OK_button = false;
		}
	}

	return 0;
}

bool ff8_skip_movies()
{
	uint32_t mode = getmode_cached()->driver_mode;

	if (ff8_externals.movie_object->movie_is_playing)
	{
		if (mode == MODE_FIELD)
		{
			// Prevent game acting weird or wrong if movie is skipped
			if (
				*common_externals.current_field_id == 339 // dosea_2
			)
			{
				return false;
			}

			// Force last frame for field scripts
			ff8_externals.movie_object->movie_current_frame = 0xFFFF;
			(*ff8_externals.savemap)[80 / 4] = 0xFFFF;
			ff8_externals.sub_5304B0();
		}
		else if (mode == MODE_CREDITS)
		{
			if (enable_ffmpeg_videos)
				ff8_stop_movie();
			else
				((void(*)())common_externals.stop_movie)();
		}

		return true;
	}
	else
	{
		if (mode == MODE_CREDITS)
		{
			*ff8_externals.credits_counter = 256;
			*ff8_externals.credits_loop_state = 18;

			return true;
		}
	}

	return false;
}

int ff8_toggle_battle_field()
{
	int ret = 1;

	if (gamehacks.wantsBattle()) ret = ff8_externals.sub_52B3A0();

	return ret;
}

int ff8_toggle_battle_worldmap(int param)
{
	int ret = 0;

	if (gamehacks.wantsBattle()) ret = ff8_externals.sub_541C80(param);

	return ret;
}

uint32_t ff8_retry_configured_drive(char* filename, uint8_t* data)
{
	int32_t res = ff8_externals.sm_pc_read(filename, data);

	if (!res) {
		char dataDrive[8];
		char modifiedFilename[MAX_PATH];

		ff8_externals.reg_get_data_drive(dataDrive, 4);
		dataDrive[7] = '\0'; // For safety

		if (trace_files || trace_all) ffnx_trace("%s: filename=%s, dataDrive=%s, diskDataPath=%s\n", __func__, filename, dataDrive, ff8_externals.disk_data_path);

		if (GetDriveTypeA(dataDrive) == DRIVE_CDROM) {
			strcpy(modifiedFilename, dataDrive);
			char* filenameNoDrive = strrchr(filename, ':');
			if (filenameNoDrive != nullptr) {
				strncat(modifiedFilename, filenameNoDrive + 1, MAX_PATH);

				if (strncmp(modifiedFilename, filename, MAX_PATH) != 0) {
					res = ff8_externals.sm_pc_read(modifiedFilename, data);

					if (res) {
						strncpy(ff8_externals.disk_data_path, dataDrive, 260);
						strncat(ff8_externals.disk_data_path, "\\", 260);

						if (trace_files || trace_all) ffnx_trace("%s: diskDataPath changed %s\n", __func__, ff8_externals.disk_data_path);
					}
				}
			}
		}
	}

	return res;
}

uint32_t ff8_credits_main_loop_gfx_begin_scene(uint32_t unknown, struct game_obj *game_object)
{
	if (drawFFNxLogoFrame(game_object)) {
		ff8_externals.input_fill_keystate();

		if (((ff8_externals.input_get_keyscan(0, 0) & ff8_externals.input_get_keyscan(1, 0)) & 0xF0) != 0) {
			stopDrawFFNxLogo();
		}

		return 0;
	}

	return common_begin_scene(unknown, game_object);
}

void ff8_init_hooks(struct game_obj *_game_object)
{
	struct ff8_game_obj *game_object = (struct ff8_game_obj *)_game_object;

	game_object->dddevice = &_fake_dddevice;
	game_object->front_surface[0] = &_fake_dd_front_surface;
	game_object->front_surface[1] = &_fake_dd_back_surface;
	game_object->dd2interface = &_fake_dddevice;
	game_object->d3d2device = &_fake_d3d2device;

	if (ff8_ssigpu_debug)
		ff8_externals.show_vram_window();

	replace_function(ff8_externals.engine_eval_process_input, ff8_is_window_active);

	replace_function(ff8_externals.swirl_sub_56D390, swirl_sub_56D390);

	replace_function(common_externals.destroy_tex_header, ff8_destroy_tex_header);
	replace_function(common_externals.load_tex_file, ff8_load_tex_file);

	replace_function(common_externals.open_file, ff8_open_file);
	replace_call(uint32_t(ff8_externals.fs_archive_search_filename) + 0x10, ff8_fs_archive_search_filename2);
	// Search file in temp.fs archive (field)
	replace_call(ff8_externals.moriya_filesytem_open + 0x776, ff8_fs_archive_search_filename_sub_archive);
	// Search file in FS archive
	replace_call(ff8_externals.moriya_filesytem_open + 0x83C, ff8_fs_archive_search_filename_sub_archive);
	replace_function(ff8_externals._open, ff8_open);
	replace_function(ff8_externals.fopen, ff8_fopen);
	replace_call(ff8_externals.moriya_filesytem_close + 0x1F, ff8_fs_archive_free_file_container_sub_archive);

	ff8_read_file = (uint32_t(*)(uint32_t, void *, struct ff8_file *))common_externals.read_file;
	ff8_close_file = (void (*)(struct ff8_file *))common_externals.close_file;

	memset_code(ff8_externals.movie_hack1, 0x90, 14);
	memset_code(ff8_externals.movie_hack2, 0x90, 8);

	ff8_externals.d3dcaps[0] = true;
	ff8_externals.d3dcaps[1] = true;
	ff8_externals.d3dcaps[2] = true;
	ff8_externals.d3dcaps[3] = true;

	if (version == VERSION_FF8_12_FR_NV || version == VERSION_FF8_12_SP_NV || version == VERSION_FF8_12_IT_NV)
	{
		unsigned char ff8fr_savefix1[] = "\xC0\xEA\x03\x8A\x41\x6D\x80\xE2"
																		 "\x01\x24\xFE\x0A\xD0\x88\x51\x6D";
		unsigned char ff8fr_savefix2[] = "\x8A\x50\x6D\xC0\xE9\x03\x80\xE1"
																		 "\x01\x80\xE2\xFE\x0A\xCA\x88\x48"
																		 "\x6D";

		memcpy_code(ff8_externals.sub_53BB90 + 0x952, ff8fr_savefix1, sizeof(ff8fr_savefix1) - 1);
		memcpy_code(ff8_externals.sub_53C750 + 0x8B0, ff8fr_savefix1, sizeof(ff8fr_savefix1) - 1);

		memcpy_code(ff8_externals.sub_544630 + 0xE2, ff8fr_savefix2, sizeof(ff8fr_savefix2) - 1);

		patch_code_byte(ff8_externals.sub_544630 + 0x12F, 0x7D);

		patch_code_byte(ff8_externals.sub_548080 + 0x174, 0x6E);
		patch_code_byte(ff8_externals.sub_548080 + 0x1A3, 0x6E);
		patch_code_byte(ff8_externals.sub_548080 + 0x1C7, 0x6E);
		patch_code_byte(ff8_externals.sub_548080 + 0x1E5, 0x6E);

		patch_code_byte(ff8_externals.sub_549E80 + 0x1CE, 0x6E);

		patch_code_byte(ff8_externals.sub_546100 + 0x952, 0x6D);
		patch_code_byte(ff8_externals.sub_546100 + 0x9A6, 0x74);

		patch_code_byte(ff8_externals.sub_546100 + 0xA23, 0x7C);
		patch_code_byte(ff8_externals.sub_546100 + 0xA67, 0x7C);
		patch_code_byte(ff8_externals.sub_546100 + 0xA90, 0x7C);

		patch_code_byte(ff8_externals.sub_54A0D0 + 0x151, 0x6E);

		patch_code_byte(ff8_externals.sub_54D7E0 + 0xB6, 0x74);
		patch_code_byte(ff8_externals.sub_54D7E0 + 0xE0, 0x74);
		patch_code_byte(ff8_externals.sub_54D7E0 + 0x14A, 0x7C);

		patch_code_byte(ff8_externals.sub_54FDA0 + 0x51, 0x70);
		patch_code_byte(ff8_externals.sub_54FDA0 + 0xB6, 0x70);
		patch_code_byte(ff8_externals.sub_54FDA0 + 0x17F, 0x70);
		patch_code_byte(ff8_externals.sub_54FDA0 + 0x1C7, 0x70);
		patch_code_byte(ff8_externals.sub_54FDA0 + 0x1DB, 0x70);
	}

	// don't set system speaker config to stereo
	memset_code(common_externals.directsound_create + 0x6D, 0x90, 34);

	if (ff8_externals.nvidia_hack1)
		patch_code_double(ff8_externals.nvidia_hack1, 0.0);
	if (ff8_externals.nvidia_hack2)
		patch_code_float(ff8_externals.nvidia_hack2, 0.0f);

	memcpy_code(ff8_externals.sub_4653B0 + 0xA5, texture_reload_fix1, sizeof(texture_reload_fix1));
	replace_function(ff8_externals.sub_4653B0 + 0xA5 + sizeof(texture_reload_fix1), texture_reload_hack1);

	memcpy_code(ff8_externals.sub_465720 + 0xB3, texture_reload_fix2, sizeof(texture_reload_fix2));
	replace_function(ff8_externals.sub_465720 + 0xB3 + sizeof(texture_reload_fix2), texture_reload_hack2);

	// replace rdtsc timing
	replace_function((uint32_t)common_externals.get_time, qpc_get_time);

	// override the timer calibration
	QueryPerformanceFrequency((LARGE_INTEGER *)&game_object->_countspersecond);
	game_object->countspersecond = (double)game_object->_countspersecond;

	// Add speedhack support
	replace_function((uint32_t)common_externals.diff_time, qpc_diff_time);

	// Gamepad
	replace_function(ff8_externals.dinput_init_gamepad, ff8_init_gamepad);
	replace_function(ff8_externals.dinput_update_gamepad_status, ff8_update_gamepad_status);

	// Do not alter worldmap texture UVs (Maki's patch) http://forums.qhimm.com/index.php?topic=16327.0
	if (FF8_US_VERSION)
	{
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x180, 0); // +-2 replaced by 0
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x18A, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x198, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1A2, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1B2, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1BC, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1CC, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1D6, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1E6, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1F0, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1F8, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x202, 0);
	}
	else
	{
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x19F, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1AA, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1BB, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1C6, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1D9, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1E4, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x1F7, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x202, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x215, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x220, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x229, 0);
		patch_code_byte(ff8_externals.worldmap_alter_uv_sub_553B40 + 0x234, 0);
	}

	// Fix blue color in battle with fire spells
	patch_code_byte(ff8_externals.read_vram_palette_sub_467370 + 0x6F, 0);

	// #####################
	// battle toggle
	// #####################
	replace_call_function(ff8_externals.battle_trigger_field, ff8_toggle_battle_field);
	replace_call_function(ff8_externals.battle_trigger_worldmap, ff8_toggle_battle_worldmap);

	// Allow squaresoft logo skip by pressing a button
	patch_code_byte(ff8_externals.load_credits_image + 0x5FD, 0); // if (intro_step >= 0) ...
	// Add FFNx Logo
	replace_call(ff8_externals.credits_main_loop + 0x6D, ff8_credits_main_loop_gfx_begin_scene);

	if (!steam_edition) {
		// Look again with the DataDrive specified in the register
		replace_call(ff8_externals.get_disk_number + 0x6E, ff8_retry_configured_drive);
		replace_call(ff8_externals.cdcheck_sub_52F9E0 + 0x15E, ff8_retry_configured_drive);
	}
}

struct ff8_gfx_driver *ff8_load_driver(void* _game_object)
{
	struct ff8_gfx_driver *ret = (ff8_gfx_driver *)external_calloc(1, sizeof(*ret));

	ret->init = common_init;
	ret->cleanup = common_cleanup;
	ret->lock = common_lock;
	ret->unlock = common_unlock;
	ret->flip = common_flip;
	ret->clear = common_clear;
	ret->clear_all= common_clear_all;
	ret->setviewport = common_setviewport;
	ret->setbg = common_setbg;
	ret->prepare_polygon_set = common_prepare_polygon_set;
	ret->load_group = common_externals.generic_load_group;
	ret->setmatrix = common_setmatrix;
	ret->unload_texture = common_unload_texture;
	ret->load_texture = common_load_texture;
	ret->field_54 = ff8gl_field_54;
	ret->field_58 = ff8gl_field_58;
	ret->field_5C = ff8gl_field_5C;
	ret->field_60 = ff8gl_field_60;
	ret->palette_changed = common_palette_changed;
	ret->write_palette = common_write_palette;
	ret->blendmode = common_blendmode;
	ret->light_polygon_set = common_light_polygon_set;
	ret->field_64 = common_field_64;
	ret->setrenderstate = common_setrenderstate;
	ret->_setrenderstate = common_setrenderstate;
	ret->__setrenderstate = common_setrenderstate;
	ret->field__84 = ff8gl_field_84;
	ret->field_88 = ff8gl_field_88;
	ret->field_74 = common_field_74;
	ret->field_78 = common_field_78;
	ret->draw_deferred = common_draw_deferred;
	ret->field_80 = common_field_80;
	ret->field_84 = common_field_84;
	ret->begin_scene = common_begin_scene;
	ret->end_scene = common_end_scene;
	ret->field_90 = common_field_90;
	ret->setrenderstate_flat2D = common_setrenderstate_2D;
	ret->setrenderstate_smooth2D = common_setrenderstate_2D;
	ret->setrenderstate_textured2D = common_setrenderstate_2D;
	ret->setrenderstate_paletted2D = common_setrenderstate_2D;
	ret->_setrenderstate_paletted2D = common_setrenderstate_2D;
	ret->draw_flat2D = common_draw_2D;
	ret->draw_smooth2D = common_draw_2D;
	ret->draw_textured2D = common_draw_2D;
	ret->draw_paletted2D = common_draw_paletted2D;
	ret->setrenderstate_flat3D = common_setrenderstate_3D;
	ret->setrenderstate_smooth3D = common_setrenderstate_3D;
	ret->setrenderstate_textured3D = common_setrenderstate_3D;
	ret->setrenderstate_paletted3D = common_setrenderstate_3D;
	ret->_setrenderstate_paletted3D = common_setrenderstate_3D;
	ret->draw_flat3D = common_draw_3D;
	ret->draw_smooth3D = common_draw_3D;
	ret->draw_textured3D = common_draw_3D;
	ret->draw_paletted3D = common_draw_paletted3D;
	ret->setrenderstate_flatlines = common_setrenderstate_2D;
	ret->setrenderstate_smoothlines = common_setrenderstate_2D;
	ret->draw_flatlines = common_draw_lines;
	ret->draw_smoothlines = common_draw_lines;
	ret->field_EC = common_field_EC;

	return ret;
}
