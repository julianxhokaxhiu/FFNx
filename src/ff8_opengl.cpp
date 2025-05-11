/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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

#include "globals.h"
#include "common.h"
#include "ff8.h"
#include "fake_dd.h"
#include "patch.h"
#include "log.h"
#include "macro.h"
#include "movies.h"
#include "gl.h"
#include "gamepad.h"
#include "joystick.h"
#include "gamehacks.h"
#include "utils.h"
#include "vibration.h"
#include "ff8/file.h"
#include "ff8/vram.h"
#include "metadata.h"
#include "achievement.h"

unsigned char texture_reload_fix1[] = {0x5B, 0x5F, 0x5E, 0x5D, 0x81, 0xC4, 0x10, 0x01, 0x00, 0x00};
unsigned char texture_reload_fix2[] = {0x5F, 0x5E, 0x5D, 0x5B, 0x81, 0xC4, 0x8C, 0x00, 0x00, 0x00};
int left_stick_y = 0x80;
int left_stick_x = 0x80;
int right_stick_y = 0x80;
int right_stick_x = 0x80;

std::chrono::time_point<std::chrono::high_resolution_clock> intro_credits_music_start_time;
constexpr int intro_credits_fade_frames = 33;
constexpr int intro_credits_adjusted_frames = 438; // Instead of 374 in the Game
constexpr int intro_credits_frames_between_music_start_and_first_image = 180;

int (*ff8_opcode_old_battle)(int);

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
void texture_reload_hack(struct texture_page *texture_page, struct ff8_texture_set *texture_set)
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
	Tim::Bpp texBpp = Tim::Bpp(texture_page->color_key);

	if (tiledTex.isValid() && texBpp != tiledTex.bpp()) {
		if(trace_all || trace_vram) ffnx_trace("%s: ignore reload because BPP does not match 0x%X (bpp vram=%d, bpp tex=%d, source bpp tex=%d) image_data=0x%X\n", __func__, texture_set, tiledTex.bpp(), VREF(tex_header, tex_format.bytesperpixel), texBpp, VREF(tex_header, image_data));

		return;
	}

	if (texBpp != Tim::Bpp16) {
		const std::list<TexturePacker::IdentifiedTexture> &textures = texturePacker.matchTextures(tiledTex, false, true);

		if (textures.empty()) {
			if(trace_all || trace_vram) ffnx_trace("%s: ignore reload because no animated texture matches the current texture set 0x%X (bpp vram=%d, bpp tex=%d, source bpp tex=%d) image_data=0x%X\n", __func__, texture_set, tiledTex.bpp(), VREF(tex_header, tex_format.bytesperpixel), texBpp, VREF(tex_header, image_data));

			return;
		}
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

	texture_reload_hack(texture_page, texture_set);
}

void texture_reload_hack2(struct texture_page *texture_page, uint32_t unknown1, uint32_t unknown2)
{
	struct ff8_texture_set *texture_set = (struct ff8_texture_set *)texture_page->sub_tri_gfxobj->hundred_data->texture_set;

	texture_reload_hack(texture_page, texture_set);
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

	struct ff8_texture_set *texture_set = (struct ff8_texture_set *)(*ff8_externals.swirl_texture1)->hundred_data->texture_set;

	common_unload_texture((*ff8_externals.swirl_texture1)->hundred_data->texture_set);
	common_load_texture((*ff8_externals.swirl_texture1)->hundred_data->texture_set, tex_header, texture_set->texture_format);

	last_tex_header = tex_header;
}

void ff8_wm_set_render_to_vram_current_screen_flag_before_battle()
{
	if(trace_all) ffnx_trace("%s\n", __func__);

	// There is currently an visual issue in the last worldmap frame before swirl if this flag is enabled
	// We lose the shadows, but keep the full battle transition effect
	*ff8_externals.sub_blending_capability = false;

	// Disable software frame rendering to VRAM (by not doing anything here) because it is not needed anymore
}

void ff8_swirl_init(float a1)
{
	if(trace_all) ffnx_trace("%s\n", __func__);

	// Reenable the flag that was disabled in worldmap
	*ff8_externals.sub_blending_capability = true;

	((void(*)(float))ff8_externals.sub_460B60)(a1);
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

int ff8_get_analog_value(int8_t port, int type, int8_t offset)
{
	if (type == 0) {
		return right_stick_x;
	}

	if (type == 1) {
		return right_stick_y;
	}

	if (type == 2) {
		return left_stick_x;
	}

	if (type == 3) {
		return left_stick_y;
	}

	return -1;
}

int ff8_get_analog_value_wm(int8_t port, int type, int8_t offset)
{
	if (left_stick_x != 0x80 || left_stick_y != 0x80) {
		int *keyscans = *(int **)(ff8_externals.worldmap_input_update_sub_559240 + (FF8_US_VERSION ? 0x64 : 0x61));
		int index = **(int **)(ff8_externals.worldmap_input_update_sub_559240 + (FF8_US_VERSION ? 0x9 : 0x6));
		keyscans[index] &= 0x0FFF; // Remove d-pad keys
	}

	return ff8_get_analog_value(port, type, offset);
}

LPDIJOYSTATE2 ff8_update_gamepad_status()
{
	ff8_externals.dinput_gamepad_state->rgdwPOV[0] = -1;
	ff8_externals.dinput_gamepad_state->lX = 0;
	ff8_externals.dinput_gamepad_state->lY = 0;
	ff8_externals.dinput_gamepad_state->lRx = 0;
	ff8_externals.dinput_gamepad_state->lRy = 0;

	nxVibrationEngine.rumbleUpdate();

	int lX = 0, lY = 0, rX = 0, rY = 0;

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

		lY = int(gamepad.leftStickY * 0x80);
		lX = int(gamepad.leftStickX * 0x80);
		rY = int(gamepad.rightStickY * 0x80);
		rX = int(gamepad.rightStickX * 0x80);

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
		ff8_externals.dinput_gamepad_state->rgbButtons[0] = gamepad.IsPressed(steam_stock_launcher ? XINPUT_GAMEPAD_A : XINPUT_GAMEPAD_X) ? 0x80 : 0; // Cross (Steam)/Square
		ff8_externals.dinput_gamepad_state->rgbButtons[1] = gamepad.IsPressed(steam_stock_launcher ? XINPUT_GAMEPAD_B : XINPUT_GAMEPAD_A) ? 0x80 : 0; // Circle (Steam)/Cross
		ff8_externals.dinput_gamepad_state->rgbButtons[2] = gamepad.IsPressed(steam_stock_launcher ? XINPUT_GAMEPAD_X : XINPUT_GAMEPAD_B) ? 0x80 : 0; // Square (Steam)/Circle
		ff8_externals.dinput_gamepad_state->rgbButtons[3] = gamepad.IsPressed(XINPUT_GAMEPAD_Y) ? 0x80 : 0; // Triangle
		ff8_externals.dinput_gamepad_state->rgbButtons[4] = gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER) ? 0x80 : 0; // L1
		ff8_externals.dinput_gamepad_state->rgbButtons[5] = gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 0x80 : 0; // R1
		ff8_externals.dinput_gamepad_state->rgbButtons[6] = (steam_stock_launcher ? gamepad.IsPressed(XINPUT_GAMEPAD_BACK) : gamepad.leftTrigger > 0.85f) ? 0x80 : 0; // SELECT (Steam)/L2
		ff8_externals.dinput_gamepad_state->rgbButtons[7] = (steam_stock_launcher ? gamepad.IsPressed(XINPUT_GAMEPAD_START) : gamepad.rightTrigger > 0.85f) ? 0x80 : 0; // START (Steam)/R2
		ff8_externals.dinput_gamepad_state->rgbButtons[8] = (steam_stock_launcher ? gamepad.leftTrigger > 0.85f : gamepad.IsPressed(XINPUT_GAMEPAD_BACK)) ? 0x80 : 0; // L2 (Steam)/SELECT
		ff8_externals.dinput_gamepad_state->rgbButtons[9] = (steam_stock_launcher ? gamepad.rightTrigger > 0.85f : gamepad.IsPressed(XINPUT_GAMEPAD_START)) ? 0x80 : 0; // R2 (Steam)/START
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

		lY = -int(joystick.GetState()->lY * 0x80 / SHRT_MAX);
		lX = int(joystick.GetState()->lX * 0x80 / SHRT_MAX);
		rY = -int(joystick.GetState()->lRy * 0x80 / SHRT_MAX);
		rX = int(joystick.GetState()->lRx * 0x80 / SHRT_MAX);

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

	left_stick_y = -lY + 0x80;
	if (left_stick_y > 255) left_stick_y = 255;
	if (left_stick_y < 0) left_stick_y = 0;

	left_stick_x = lX + 0x80;
	if (left_stick_x > 255) left_stick_x = 255;
	if (left_stick_x < 0) left_stick_x = 0;

	int mul = (left_stick_x - 128) * (left_stick_x - 128) + (left_stick_y - 128) * (left_stick_y - 128);
	if (mul < 1600) {
		left_stick_y = 0x80;
		left_stick_x = 0x80;
	}

	right_stick_y = -rY + 0x80;
	if (right_stick_y > 255) right_stick_y = 255;
	if (right_stick_y < 0) right_stick_y = 0;

	right_stick_x = rX + 0x80;
	if (right_stick_x > 255) right_stick_x = 255;
	if (right_stick_x < 0) right_stick_x = 0;

	mul = (right_stick_x - 128) * (right_stick_x - 128) + (right_stick_y - 128) * (right_stick_y - 128);
	if (mul < 1600) {
		right_stick_y = 0x80;
		right_stick_x = 0x80;
	}

	return ff8_externals.dinput_gamepad_state;
}

int ff8_get_input_device_capabilities_number_of_buttons(int a1)
{
	return xinput_connected ? 10 : std::min<DWORD>(joystick.GetCaps()->dwButtons, 10);
}

int ff8_draw_gamepad_icon_or_keyboard_key(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos, int icon_id, uint16_t x, uint16_t y)
{
	// Keep the "keys" if it is a keyboard and not a gamepad
	if (icon_id >= 128 && icon_id < 140)
	{
		BYTE is_gamepad = *ff8_externals.engine_gamepad_button_pressed != 0;

		if (is_gamepad)
		{
			int val = ((int(*)(int,int,int))ff8_externals.get_command_key)(is_gamepad, icon_id - 128, 0);

			if (val == 0) {
				val = ((int(*)(int,int,int))ff8_externals.get_command_key)(!is_gamepad, icon_id - 128, 0);
			}

			int rgbButton = val - 224;

			switch (rgbButton)
			{
				case 0: // Cross (Steam)/Square
					return steam_stock_launcher ? 134 : 135;
				case 1: // Circle (Steam)/Cross
					return steam_stock_launcher ? 133 : 134;
				case 2: // Square (Steam)/Circle
					return steam_stock_launcher ? 135 : 133;
				case 3: // Triangle
					return 132;
				case 4: // L1
					return 130;
				case 5: // R1
					return 131;
				case 6: // SELECT (Steam)/L2
					return steam_stock_launcher ? 136 : 128;
				case 7: // START (Steam)/R2
					return steam_stock_launcher ? 139 : 129;
				case 8: // L2 (Steam)/SELECT
					return steam_stock_launcher ? 128 : 136;
				case 9: // R2 (Steam)/START
					return steam_stock_launcher ? 129 : 139;
			}
		}

		((void(*)(int, ff8_draw_menu_sprite_texture_infos*, int, uint16_t, uint16_t))ff8_externals.draw_controller_or_keyboard_icons)(a1, draw_infos, icon_id, x, y);

		return -1;
	}

	return icon_id;
}

unsigned int *ff8_draw_icon_get_icon_sp1_infos(int icon_id, int &states_count)
{
	int *icon_sp1_data = ((int*(*)())ff8_externals.get_icon_sp1_data)();

	if (icon_id >= icon_sp1_data[0])
	{
		states_count = 0;

		return nullptr;
	}

	states_count = HIWORD(icon_sp1_data[icon_id + 1]);

	return (unsigned int *)((char *)icon_sp1_data + uint16_t(icon_sp1_data[icon_id + 1]));
}

ff8_draw_menu_sprite_texture_infos *ff8_draw_icon_or_key(
	int a1, ff8_draw_menu_sprite_texture_infos *draw_infos,
	int icon_id, uint16_t x, uint16_t y, int a6, int field10_modifier = 0,
	bool no_a6_mask = false,
	bool override_field4_8_with_a6 = false,
	bool yfix = false
) {
	icon_id = ff8_draw_gamepad_icon_or_keyboard_key(a1, draw_infos, icon_id, x, y);
	if (icon_id < 0)
	{
		return draw_infos;
	}

	int states_count = 0;
	unsigned int *sp1_section_data = ff8_draw_icon_get_icon_sp1_infos(icon_id, states_count);

	if (sp1_section_data == nullptr)
	{
		return draw_infos;
	}

	for (int i = states_count; i > 0; --i)
	{
		draw_infos->field_0 = 0x5000000;
		draw_infos->field_10 = (sp1_section_data[0] & 0x7CFFFFF) + ((0x3810 + field10_modifier) << 16);
		if (override_field4_8_with_a6)
		{
			draw_infos->field_8 = ((a6 & 0xFFFFFF) | 0x64000000) | (((HIBYTE(a6) >> 1) & 2) << 24);
			draw_infos->field_4 = ((HIBYTE(a6) & 3) << 5) | 0xE100041E;
		}
		else
		{
			draw_infos->field_8 = no_a6_mask ? a6 | (((sp1_section_data[0] >> 26) & 2) << 24) : (a6 & 0x3FFFFFF) | (((sp1_section_data[0] >> 26) & 2 | 0x64) << 24);
			draw_infos->field_4 = (sp1_section_data[0] >> 25) & 0x60 | 0xE100041E;
		}
		draw_infos->field_14 = sp1_section_data[1] & 0xFF00FF;
		draw_infos->x_related = x + (int16_t(sp1_section_data[1]) >> 8);
		draw_infos->y_related = y + (int32_t(sp1_section_data[1]) >> 24);
		if (yfix && *ff8_externals.battle_boost_cross_icon_display_1D76604) {
			*((uint8_t *)draw_infos + 11) |= 2u;
		}
		((void(*)(int, ff8_draw_menu_sprite_texture_infos*))ff8_externals.sub_49BB30)(a1, draw_infos);
		if (!no_a6_mask) {
			draw_infos += 1;
		}
		sp1_section_data += 2;
	}

	return draw_infos;
}

ff8_draw_menu_sprite_texture_infos *ff8_draw_icon_or_key1(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos, int icon_id, uint16_t x, uint16_t y, int a6)
{
	return ff8_draw_icon_or_key(a1, draw_infos, icon_id, x, y, a6);
}

ff8_draw_menu_sprite_texture_infos *ff8_draw_icon_or_key2(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos, int *icon_sp1_data, int icon_id, uint16_t x, uint16_t y)
{
	return ff8_draw_icon_or_key(a1, draw_infos, icon_id, x, y, *ff8_externals.dword_1D2B808);
}

ff8_draw_menu_sprite_texture_infos *ff8_draw_icon_or_key3(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos, int *icon_sp1_data, int icon_id, uint16_t x, uint16_t y, int a6)
{
	return ff8_draw_icon_or_key(a1, draw_infos, icon_id, x, y, a6, 0, true);
}

ff8_draw_menu_sprite_texture_infos *ff8_draw_icon_or_key4(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos, int *icon_sp1_data, int icon_id, uint16_t x, uint16_t y, int a6, int a7)
{
	return ff8_draw_icon_or_key(a1, draw_infos, icon_id, x, y, a6, a7, false, true);
}

ff8_draw_menu_sprite_texture_infos *ff8_draw_icon_or_key5(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos, int icon_id, uint16_t x, uint16_t y, int a6, int a7)
{
	return ff8_draw_icon_or_key(a1, draw_infos, icon_id, x, y, a6, a7, true, false, true);
}

ff8_draw_menu_sprite_texture_infos_short *ff8_draw_icon_or_key6(int a1, ff8_draw_menu_sprite_texture_infos_short *draw_infos, int icon_id, uint16_t x, uint16_t y, int a6, int a7) {
	// We should not cast like this, but that's what the game does
	icon_id = ff8_draw_gamepad_icon_or_keyboard_key(a1, reinterpret_cast<ff8_draw_menu_sprite_texture_infos *>(draw_infos), icon_id, x, y);
	if (icon_id < 0)
	{
		return draw_infos;
	}

	int states_count = 0;
	unsigned int *sp1_section_data = ff8_draw_icon_get_icon_sp1_infos(icon_id, states_count);

	if (sp1_section_data == nullptr)
	{
		return draw_infos;
	}

	for (int i = states_count; i > 0; --i)
	{
		draw_infos->field_0 = 0x4000000;
		draw_infos->field_C = (sp1_section_data[0] & 0x7CFFFFF) + ((0x3810 + a7) << 16);
		draw_infos->field_4 = a6 | (((sp1_section_data[0] >> 26) & 2) << 24);
		draw_infos->field_10 = sp1_section_data[1] & 0xFF00FF;
		draw_infos->x_related = x + (int16_t(sp1_section_data[1]) >> 8);
		draw_infos->y_related = y + (sp1_section_data[1] >> 24);
		((void(*)(int, ff8_draw_menu_sprite_texture_infos_short*))ff8_externals.sub_49FE60)(a1, draw_infos);
		draw_infos += 1;
		sp1_section_data += 2;
	}

	return draw_infos;
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
			(*ff8_externals.savemap_field)->current_frame = 0xFFFF;
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

int ff8_opcode_battle(int unk)
{
	int ret = ff8_opcode_old_battle(unk);

	next_battle_scene_id = *ff8_externals.battle_encounter_id;
	next_music_is_battle = true;

	return ret;
}

int ff8_toggle_battle_field()
{
	int ret = 0;

	if (gamehacks.wantsBattle()) ret = ff8_externals.sub_47CA90();

	if (ret > 0)
	{
		next_battle_scene_id = *ff8_externals.battle_encounter_id;
		next_music_is_battle = true;
	}

	return ret;
}

int ff8_toggle_battle_worldmap(WORD* battle_id)
{
	int ret = 0;

	if (gamehacks.wantsBattle()) ret = ff8_externals.sub_541C80(battle_id);

	if (ret > 0)
	{
		next_battle_scene_id = *battle_id;
		next_music_is_battle = true;
	}

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

int credits_controller_music_play(void *data)
{
	int ret = ((int(*)(void*))ff8_externals.sdmusicplay)(data);

	intro_credits_music_start_time = highResolutionNow();

	return ret;
}

int credits_controller_input_call()
{
	if (*ff8_externals.credits_counter == 0) {
		int frameCountAdjusted =
			intro_credits_frames_between_music_start_and_first_image
			+ *ff8_externals.credits_current_step_image * intro_credits_adjusted_frames
			+ intro_credits_fade_frames;

		int realFramesEllapsed = int((60.0 / 1000.0) * (elapsedMicroseconds(intro_credits_music_start_time) / 1000.0));
		int waitFor = frameCountAdjusted - realFramesEllapsed;

		// Add frames on each image display
		if (waitFor > 0) {
			*ff8_externals.credits_current_image_global_counter_start += waitFor;
		}
	}

	return ((int(*)())ff8_externals.sub_52FE80)();
}

char new_game_text_cache[64] = "";
char load_game_text_cache[64] = "";

char *ff8_get_text_cached(int pool_id, int cat_id, int text_id, int a4, char *cache)
{
	if (*cache == '\0') {
		memcpy(cache, ((char*(*)(int,int,int,int))ff8_externals.get_text_data)(pool_id, cat_id, text_id, a4), sizeof(new_game_text_cache));
	}

	return cache;
}

char *ff8_get_text_cached_new_game(int pool_id, int cat_id, int text_id, int a4)
{
	return ff8_get_text_cached(pool_id, cat_id, text_id, a4, new_game_text_cache);
}

char *ff8_get_text_cached_load_game(int pool_id, int cat_id, int text_id, int a4)
{
	return ff8_get_text_cached(pool_id, cat_id, text_id, a4, load_game_text_cache);
}

int ff8_create_save_file(int slot, char* save)
{
	int ret = ((int(*)(int,char*))ff8_externals.create_save_file_sub_4C6E50)(slot, save);

	uint8_t savefile_slot = slot > 1 ? 2 : 1;
	uint8_t savefile_save = atoi(&save[strlen(save) - 2]) + 1;
	ffnx_trace("Save: user saved in slot%d_save%02i\n", savefile_slot, savefile_save);
	metadataPatcher.updateFF8(savefile_slot, savefile_save);

	return ret;
}

int ff8_create_save_file_chocobo_world(int unused, int data_source, int offset, size_t size)
{
	int ret = ((int(*)(int,int,int,size_t))ff8_externals.create_save_chocobo_world_file_sub_4C6620)(unused, data_source, offset, size);

	ffnx_trace("Save: user saved in slot:choco\n");
	if (ret > 0) metadataPatcher.updateFF8(3, 0);

	return ret;
}

int ff8_cardgame_postgame_func_534BC0()
{
	g_FF8SteamAchievements->unlockPlayTripleTriadAchievement();
	return ff8_externals.cardgame_func_534BC0();
}

void ff8_cardgame_enter_hook_sub_460B60(float a1)
{
	g_FF8SteamAchievements->initOwnedTripleTriadRareCards(ff8_externals.savemap->triple_triad);
	((void(*)(float))ff8_externals.sub_460B60)(a1);
}

void ff8_cardgame_exit_hook_sub_4972A0()
{
	g_FF8SteamAchievements->unlockLoserTripleTriadAchievement(ff8_externals.savemap->triple_triad);
	((void(*)())ff8_externals.sub_4972A0)();
}

int ff8_cardgame_add_card_to_squall_original(int card_idx)
{
	// update known cards
	if (card_idx >= 77)
		ff8_externals.savemap->triple_triad.cards_rare[(card_idx - 77) / 8] |= 1 << ((card_idx - 77) % 8);
	else
		ff8_externals.savemap->triple_triad.cards[card_idx] |= 0x80u;

	// add card to squall
	if (card_idx >= 77)
	{
		ff8_externals.savemap->triple_triad.card_locations[card_idx - 77] = 240; // SQUALL
		return 0;
	}
	else if ((ff8_externals.savemap->triple_triad.cards[card_idx] & 0x7Fu) >= 100)
	{
		return -1;
	}
	else
	{
		++ff8_externals.savemap->triple_triad.cards[card_idx];
		return 0;
	}
}

int ff8_cardgame_add_card_to_squall(int card_idx)
{
	int ret = ff8_cardgame_add_card_to_squall_original(card_idx);
	g_FF8SteamAchievements->unlockCollectorTripleTriadAchievement(ff8_externals.savemap->triple_triad);
	return ret;
}

int ff8_cardgame_update_card_with_location_original(int card_idx, int card_location)
{
	if ( card_idx >= 77 )
	{
		ff8_externals.savemap->triple_triad.card_locations[card_idx - 77] = card_location;
		return 0;
	}
	else
	{
		byte card_value = ff8_externals.savemap->triple_triad.cards[card_idx];
		if ( card_location == 240 ) // SQUALL location
		{
			if ((card_value & 0x7Fu) < 100)
			{
				ff8_externals.savemap->triple_triad.cards[card_idx] = card_value + 1;
				return 0;
			}
		}
		else if ((card_value & 0x7F) != 0)
		{
			ff8_externals.savemap->triple_triad.cards[card_idx] = card_value - 1;
			return 0;
		}
		return -1;
	}
}

int ff8_cardgame_update_card_with_location(int card_idx, int card_location)
{
	int ret = ff8_cardgame_update_card_with_location_original(card_idx, card_location);
	if (card_location == 240) // Squall location
	{
		g_FF8SteamAchievements->unlockCollectorTripleTriadAchievement(ff8_externals.savemap->triple_triad);
	}
	return ret;
}

int ff8_cardgame_sub_535D00(void* tt_data)
{
	uint16_t prev_card_wins = ff8_externals.savemap->triple_triad.victory_count;
	int ret = ff8_externals.cardgame_sub_535D00(tt_data);
	if (ff8_externals.savemap->triple_triad.victory_count > prev_card_wins)
	{
		g_FF8SteamAchievements->increaseCardWinsAndUnlockProfessionalAchievement();
	}
	return ret;
}

void ff8_enable_gf_sub_47E480(int gf_idx)
{
	ff8_externals.savemap->gfs[gf_idx].exists |= 1u;
	g_FF8SteamAchievements->unlockGuardianForceAchievement(gf_idx);
}

void ff8_update_seed_exp_4C30E0(int seed_lvl)
{
	ff8_externals.update_seed_exp_4C30E0(seed_lvl);
	g_FF8SteamAchievements->unlockTopSeedRankAchievement(ff8_externals.savemap->field_header.seedExp);
}

int ff8_field_opcode_POPM_W(void* field_data, int memory_offset)
{
	int ret = ff8_externals.opcode_popm_w(field_data, memory_offset);
	g_FF8SteamAchievements->unlockTopSeedRankAchievement(ff8_externals.savemap->field_header.seedExp);
	return ret;
}

int ff8_field_opcode_ADDSEEDLEVEL(void* field_data)
{
	int ret = ff8_externals.opcode_addseedlevel(field_data);
	g_FF8SteamAchievements->unlockTopSeedRankAchievement(ff8_externals.savemap->field_header.seedExp);
	return ret;
}

void ff8_field_update_seed_level()
{
	((void(*)())ff8_externals.field_update_seed_level_52B140)();
	g_FF8SteamAchievements->unlockTopSeedRankAchievement(ff8_externals.savemap->field_header.seedExp);
	g_FF8SteamAchievements->unlockMaxGilAchievement(ff8_externals.savemap->gil);
}

void ff8_worldmap_update_seed_level()
{
	((void(*)())ff8_externals.worldmap_update_seed_level_651C10)();
	g_FF8SteamAchievements->unlockTopSeedRankAchievement(ff8_externals.savemap->field_header.seedExp);
	g_FF8SteamAchievements->unlockMaxGilAchievement(ff8_externals.savemap->gil);
}

// Replacing a specific call that is called when player remodel weapon just before assigning the new
// weapon id to the character
int ff8_menu_junkshop_get_char_id_hook_4ABC40(int chars_available_bitmap, int char_idx)
{
	int char_id = ff8_externals.sub_4ABC40(chars_available_bitmap, char_idx);
	g_FF8SteamAchievements->initPreviousWeaponIdBeforeUpgrade(char_id, ff8_externals.savemap->chars[char_id].weapon_id);
	return char_id;
}

// Replacing a specific call that is called when player remodel weapon just after assigning the new
// weapon id to the character
int ff8_menu_junkshop_hook_4EA770(int a1, uint32_t a2)
{
	int ret = ff8_externals.sub_4EA770(a1, a2);
	g_FF8SteamAchievements->unlockUpgradeWeaponAchievement(*ff8_externals.savemap);
	return ret;
}

// Replacing a call done before computing max HP for a character in order to get the 
// index "party_char_id"
void ff8_hook_sub_4954B0(int party_char_id)
{
	ff8_externals.sub_4954B0(party_char_id);
	g_FF8SteamAchievements->initStatCharIdUnderStatCompute(party_char_id);
}

int ff8_compute_char_max_hp_496310(int multiplier, int char_id)
{
	int max_hp_mul = ff8_externals.compute_char_max_hp_496310(multiplier, char_id);
	byte stat_char_id = g_FF8SteamAchievements->getStatCharIdUnderStatCompute();
	if (stat_char_id != 0xFFu) {
		int max_hp = ff8_externals.char_comp_stats_1CFF000[stat_char_id].unk3[14] * max_hp_mul / 100;
		g_FF8SteamAchievements->unlockMaxHpAchievement(max_hp);
	}
	return max_hp_mul;
}

int ff8_field_opcode_ADDGIL(void* field_data)
{
	int ret = ff8_externals.opcode_addgil(field_data);
	g_FF8SteamAchievements->unlockMaxGilAchievement(ff8_externals.savemap->gil);
	return ret;
}

void ff8_menu_shop_sub_4EBE40(byte* menu_data)
{
	uint16_t menu_op = *(uint16_t*)(menu_data + 16);
	bool is_menu_sell_buy = ((*ff8_externals.menu_data_1D76A9C) & 0x40) != 0;
	((void(*)(byte*))ff8_externals.menu_shop_sub_4EBE40)(menu_data);
	byte is_sell = *(byte*)(menu_data + 70);
	if (menu_op == 12 && is_menu_sell_buy && is_sell)
	{
		uint32_t gil = *(uint32_t*)(menu_data + 40);
		g_FF8SteamAchievements->unlockMaxGilAchievement(gil);
	}
}

int ff8_battle_menu_add_exp_and_bonus_496CB0(int party_char_id, uint16_t exp)
{
	byte char_id = *(ff8_externals.character_data_1CFE74C + party_char_id);
	int ret = ff8_externals.battle_menu_add_exp_and_stat_bonus_496CB0(party_char_id, exp);
	if (char_id != 0xFF) {
		int level = ff8_externals.get_char_level_4961D0(ff8_externals.savemap->chars[char_id].exp, char_id);
		g_FF8SteamAchievements->unlockTopLevelAchievement(level);
	}
	return ret;
}

// Replace a function that is called before increasing the kills of a character
void ff8_battle_after_enemy_kill_sub_496CB0(int party_char_id, int a1, int current_actor_second_byte, int a2)
{
	ff8_externals.battle_sub_494AF0(party_char_id, a1, current_actor_second_byte, a2);
	g_FF8SteamAchievements->increaseKillsAndTryUnlockAchievement();
}

int ff8_limit_fps()
{
	static time_t last_gametime;
	time_t gametime;
	double framerate = 30.0f;

	struct ff8_game_obj *game_object = (ff8_game_obj *)common_externals.get_game_object();
	struct game_mode *mode = getmode_cached();

	// For cross music play (vanilla music only)
	qpc_get_time(&gametime);
	*ff8_externals.time_volume_change_related_1A78BE0 = (1000.0 / game_object->countspersecond) * qpc_diff_time(&gametime, &last_gametime, nullptr);

	if (ff8_fps_limiter < FPS_LIMITER_60FPS)
	{
		switch (mode->driver_mode)
		{
		case MODE_BATTLE:
			if (ff8_fps_limiter < FPS_LIMITER_30FPS) framerate = 15.0f;
			break;
		case MODE_CREDITS:
		case MODE_CARDGAME:
			framerate = 60.0f;
			break;
		}
	}
	else
	{
		switch (mode->driver_mode)
		{
		case MODE_FIELD:
		case MODE_WORLDMAP:
		case MODE_BATTLE:
		case MODE_SWIRL:
		case MODE_CREDITS:
			framerate = 60.0f;
			break;
		}
	}

	framerate *= gamehacks.getCurrentSpeedhack();
	double frame_time = game_object->countspersecond / framerate;

	do qpc_get_time(&gametime);
	while (gametime > last_gametime && qpc_diff_time(&gametime, &last_gametime, nullptr) < frame_time);

	last_gametime = gametime;

	return 0;
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
	replace_call(ff8_externals.worldmap_with_fog_sub_53FAC0 + (FF8_US_VERSION ? 0xB3C: 0xB2F), ff8_wm_set_render_to_vram_current_screen_flag_before_battle);
	replace_call(ff8_externals.swirl_enter + 0x9, ff8_swirl_init);

	replace_function(common_externals.destroy_tex_header, ff8_destroy_tex_header);
	replace_function(common_externals.load_tex_file, ff8_load_tex_file);

	replace_function(common_externals.open_file, ff8_open_file);
	replace_call(uint32_t(ff8_externals.fs_archive_search_filename) + 0x10, ff8_fs_archive_search_filename2);
	// Search file in temp.fs archive (field)
	replace_call(ff8_externals.moriya_filesystem_open + 0x776, ff8_fs_archive_search_filename_sub_archive);
	// Search file in FS archive
	replace_call(ff8_externals.moriya_filesystem_open + 0x83C, ff8_fs_archive_search_filename_sub_archive);
	replace_function(ff8_externals._open, ff8_open);
	replace_function(ff8_externals.fopen, ff8_fopen);
	replace_call(ff8_externals.moriya_filesystem_close + 0x1F, ff8_fs_archive_free_file_container_sub_archive);

	ff8_read_file = (uint32_t(*)(uint32_t, void *, struct ff8_file *))common_externals.read_file;
	ff8_close_file = (void (*)(struct ff8_file *))common_externals.close_file;

	// #####################
	// Adding LZ4 support to FS archives
	// #####################

	// Insert a call to ff8_fs_archive_patch_compression to pass the compression type
	// 83 BD D4 FD FF FF 01|0F 84(addrCompre)|E9(addrUnkComp)
	// 51|E8 (addrPatch)|83 C4 04|E9(addrCompre) 90 90 90 90
	uint32_t read_or_uncompress_fs_data_jump_to_uncompress = *(uint32_t *)(ff8_externals.read_or_uncompress_fs_data + 0x54 + 2);
	memcpy_code(ff8_externals.read_or_uncompress_fs_data + 0x4D, "\x51\xE8\x00\x00\x00\x00\x83\xC4\x04\xE9\x00\x00\x00\x00\x90\x90\x90\x90", 18);
	replace_call(ff8_externals.read_or_uncompress_fs_data + 0x4D + 1, ff8_fs_archive_patch_compression);
	patch_code_dword(ff8_externals.read_or_uncompress_fs_data + 0x4D + 10, read_or_uncompress_fs_data_jump_to_uncompress - 1);
	// Obtain the compressed and the uncompressed sizes
	replace_call(ff8_externals.read_or_uncompress_fs_data + 0x153, ff8_fs_archive_malloc_source_data);
	replace_call(ff8_externals.read_or_uncompress_fs_data + 0x188, ff8_fs_archive_malloc_target_data);
	// Replace the LZS algorithm by LZ4 if compression type is 2
	replace_call(ff8_externals.read_or_uncompress_fs_data + 0x1E6, ff8_fs_archive_uncompress_data);

	memset_code(ff8_externals.movie_hack1, 0x90, 14);
	memset_code(ff8_externals.movie_hack2, 0x90, 8);

	ff8_externals.d3dcaps[0] = true; // Has DDSCAPS_OVERLAY capability in Hardware Emulation Layer + Enable alpha on textures
	ff8_externals.d3dcaps[1] = true; // Enable alpha on textures
	ff8_externals.d3dcaps[2] = false; // Emulate substractive blending via non-paletted texture if enabled
	ff8_externals.d3dcaps[3] = true; // Seems to divide by 2 one vertex if disabled
	ff8_externals.d3dcaps[4] = false;
	*ff8_externals.sub_blending_capability = true;
	patch_code_byte(ff8_externals.sub_45CDD0 + 0x12, 2); // Force comparison to current driver, to enable substractive blending in field fade in/out

	// Fix save format
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

	// Update the metadata file when a save file is modified
	if (steam_edition)
	{
		replace_call(ff8_externals.main_menu_controller + (JP_VERSION ? 0x1004 : 0xF8D), ff8_create_save_file);
		replace_call(ff8_externals.menu_chocobo_world_controller + 0x9F6, ff8_create_save_file_chocobo_world);
		replace_call(ff8_externals.menu_chocobo_world_controller + 0xFA3, ff8_create_save_file_chocobo_world);
		replace_call(ff8_externals.menu_chocobo_world_controller + 0x11BB, ff8_create_save_file_chocobo_world);
		replace_call(ff8_externals.menu_chocobo_world_controller + 0x13EC, ff8_create_save_file_chocobo_world);
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

	// #####################
	// new timer calibration
	// #####################

	// replace time diff
	replace_function((uint32_t)common_externals.diff_time, qpc_diff_time);

	if (ff8_fps_limiter >= FPS_LIMITER_DEFAULT)
	{
		// replace rdtsc timing
		replace_function((uint32_t)common_externals.get_time, qpc_get_time);

		// override the timer calibration
		QueryPerformanceFrequency((LARGE_INTEGER *)&game_object->_countspersecond);
		game_object->countspersecond = (double)game_object->_countspersecond;

		replace_function(ff8_externals.fps_limiter, ff8_limit_fps);
	}

	// Gamepad
	replace_function(ff8_externals.dinput_init_gamepad, ff8_init_gamepad);
	replace_function(ff8_externals.dinput_update_gamepad_status, ff8_update_gamepad_status);
	replace_function(ff8_externals.dinput_get_input_device_capabilities_number_of_buttons, ff8_get_input_device_capabilities_number_of_buttons);

	if (steam_stock_launcher)
	{
		// Create ff8input.cfg with the same default values than the FF8_Launcher

		// When the game starts without ff8input.cfg file
		patch_code_byte(ff8_externals.input_init + 0x29, 225); // 226 => 225
		patch_code_byte(ff8_externals.input_init + 0x3C, 224); // 225 => 224
		patch_code_byte(ff8_externals.input_init + 0x53, 226); // 224 => 226
		patch_code_byte(ff8_externals.input_init + 0xA7, 232); // 230 => 232
		patch_code_byte(ff8_externals.input_init + 0xBA, 233); // 231 => 233
		patch_code_byte(ff8_externals.input_init + 0xD1, 230); // 232 => 230
		patch_code_byte(ff8_externals.input_init + 0xE4, 231); // 233 => 231

		// When the player reset the controls in the game menu
		patch_code_byte(ff8_externals.ff8input_cfg_reset + 0xD8, 225); // 226 => 225
		patch_code_byte(ff8_externals.ff8input_cfg_reset + 0xEB, 224); // 225 => 224
		patch_code_byte(ff8_externals.ff8input_cfg_reset + 0x102, 226); // 224 => 226
		patch_code_byte(ff8_externals.ff8input_cfg_reset + 0x156, 232); // 230 => 232
		patch_code_byte(ff8_externals.ff8input_cfg_reset + 0x169, 233); // 231 => 233
		patch_code_byte(ff8_externals.ff8input_cfg_reset + 0x180, 230); // 232 => 230
		patch_code_byte(ff8_externals.ff8input_cfg_reset + 0x193, 231); // 233 => 231
	}

	// #####################
	// Analog 360 patch
	// #####################
	// Field
	replace_call(ff8_externals.sub_4789A0 + (JP_VERSION ? 0x320 : 0x336), ff8_get_analog_value); // Test if available
	replace_call(ff8_externals.sub_4789A0 + (JP_VERSION ? 0x331 : 0x347), ff8_get_analog_value); // lX
	replace_call(ff8_externals.sub_4789A0 + (JP_VERSION ? 0x345 : 0x35B), ff8_get_analog_value); // lY
	// Worldmap
	replace_call(ff8_externals.worldmap_input_update_sub_559240 + (FF8_US_VERSION ? 0xC2 : 0xBF), ff8_get_analog_value_wm); // lX
	replace_call(ff8_externals.worldmap_input_update_sub_559240 + (FF8_US_VERSION ? 0xD2 : 0xCF), ff8_get_analog_value); // lY
	replace_call(ff8_externals.worldmap_input_update_sub_559240 + (FF8_US_VERSION ? 0xE2 : 0xDF), ff8_get_analog_value); // rX
	replace_call(ff8_externals.worldmap_input_update_sub_559240 + (FF8_US_VERSION ? 0xF2 : 0xEF), ff8_get_analog_value); // rY

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

	// #####################
	// battle toggle
	// #####################
	replace_call_function(ff8_externals.sub_4789A0 + (JP_VERSION ? 0x674 : 0x68B), ff8_toggle_battle_field);
	replace_call_function(ff8_externals.battle_trigger_worldmap, ff8_toggle_battle_worldmap);

	// Allow squaresoft logo skip by pressing a button
	patch_code_byte(ff8_externals.load_credits_image + 0x5FD, 0); // if (intro_step >= 0) ...
	// Add FFNx Logo
	replace_call(ff8_externals.credits_main_loop + 0x6D, ff8_credits_main_loop_gfx_begin_scene);
	// Fix credits intro synchronization with the music
	replace_call(ff8_externals.load_credits_image + 0x164, credits_controller_music_play);
	replace_call(ff8_externals.load_credits_image + 0x305, credits_controller_input_call);

	if (!steam_edition) {
		// Look again with the DataDrive specified in the register
		replace_call(ff8_externals.get_disk_number + 0x6E, ff8_retry_configured_drive);
		replace_call(ff8_externals.cdcheck_sub_52F9E0 + 0x15E, ff8_retry_configured_drive);
	}

	// Force SFX IDs for Quezacotl
	patch_code_dword(int(ff8_externals.vibrate_data_summon_quezacotl) - 16, 240030); // 240030 - 240000 + 370 = ID 400
	patch_code_dword(int(ff8_externals.vibrate_data_summon_quezacotl) - 12, 240033); // 240033 - 240000 + 370 = ID 403
	patch_code_dword(int(ff8_externals.vibrate_data_summon_quezacotl) - 8, 240036); // 240036 - 240000 + 370 = ID 406
	patch_code_dword(int(ff8_externals.vibrate_data_summon_quezacotl) - 4, 240039); // 240039 - 240000 + 370 = ID 409

	if (!FF8_US_VERSION && !JP_VERSION) {
		// Fix "New Game" and "Load Game" texts converted to "Doomtrain" and "Alexander" when starting a new game
		replace_call(ff8_externals.main_menu_render_sub_4E5550 + 0x203, ff8_get_text_cached_new_game);
		replace_call(ff8_externals.main_menu_render_sub_4E5550 + 0x222, ff8_get_text_cached_load_game);
	}

	if (ff8_use_gamepad_icons) {
		// Replace the whole function to conditionnally show PlayStation icons or keyboard keys
		replace_function(ff8_externals.ff8_draw_icon_or_key1, ff8_draw_icon_or_key1);
		replace_function(ff8_externals.ff8_draw_icon_or_key2, ff8_draw_icon_or_key2);
		replace_function(ff8_externals.ff8_draw_icon_or_key3, ff8_draw_icon_or_key3);
		replace_function(ff8_externals.ff8_draw_icon_or_key4, ff8_draw_icon_or_key4);
		replace_function(ff8_externals.ff8_draw_icon_or_key5, ff8_draw_icon_or_key5);
		replace_function(ff8_externals.ff8_draw_icon_or_key6, ff8_draw_icon_or_key6);
	}

	// All possible message and ask windows
	ff8_opcode_old_battle = (int (*)(int))ff8_externals.opcode_battle;
	patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x69], (DWORD)&ff8_opcode_battle);

	//###############################
	// steam achievement unlock calls
	//###############################
	if(steam_achievements_debug_mode)
	{
		// triple triad
		patch_code_dword((uint32_t)&ff8_externals.cardgame_funcs[4], (uint32_t)&ff8_cardgame_postgame_func_534BC0);
		replace_call(ff8_externals.sub_534640 + 0x8D, (void*)ff8_cardgame_enter_hook_sub_460B60);
		replace_call(ff8_externals.sub_534640 + 0x51, (void*)ff8_cardgame_exit_hook_sub_4972A0);
		replace_function(ff8_externals.cardgame_add_card_to_squall_534840, (void*)ff8_cardgame_add_card_to_squall);
		replace_function(ff8_externals.cardgame_update_card_with_location_5347F0, (void*)ff8_cardgame_update_card_with_location);
		patch_code_dword(ff8_externals.cargame_func_535C90 + 0x19, (uint32_t)&ff8_cardgame_sub_535D00);

		// guardian forces
		replace_function(ff8_externals.enable_gf_sub_47E480, (void*)ff8_enable_gf_sub_47E480);

		// seed rank A (also max GIL)
		replace_call(ff8_externals.menu_sub_4D4D30 + 0x928, (void*)ff8_update_seed_exp_4C30E0);
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x0D], (uint32_t)&ff8_field_opcode_POPM_W);
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x153], (uint32_t)&ff8_field_opcode_ADDSEEDLEVEL);
		replace_call(ff8_externals.sub_529FF0 + 0x120, (void*)ff8_field_update_seed_level);
		replace_call(ff8_externals.worldmap_update_steps_sub_6519D0 + 0x152, (void*)ff8_worldmap_update_seed_level);

		// handyman: upgrade weapon
		replace_call(ff8_externals.menu_junkshop_sub_4EA890 + 0x5C1, (void*)ff8_menu_junkshop_get_char_id_hook_4ABC40);
		replace_call(ff8_externals.menu_junkshop_sub_4EA890 + 0x60B, (void*)ff8_menu_junkshop_hook_4EA770);

		// max HP
		replace_call(ff8_externals.compute_char_stats_sub_495960 + 0x68, (void*)ff8_hook_sub_4954B0);
		replace_call(ff8_externals.compute_char_stats_sub_495960 + 0x94, (void*)ff8_compute_char_max_hp_496310);

		// max GIL
		replace_call((uint32_t)ff8_externals.menu_callbacks[11].func + 0x1F0, (void*)ff8_menu_shop_sub_4EBE40);
		patch_code_dword((uint32_t)ff8_externals.menu_callbacks[11].func + 0x39, (uint32_t)ff8_menu_shop_sub_4EBE40);
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x151], (uint32_t)&ff8_field_opcode_ADDGIL);

		// max LEVEL
		replace_call(ff8_externals.battle_menu_sub_4A3EE0 + 0x581, (void*)ff8_battle_menu_add_exp_and_bonus_496CB0);

		// kills
		replace_call(ff8_externals.battle_sub_494410 + 0x525, (void*)ff8_battle_after_enemy_kill_sub_496CB0);
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
