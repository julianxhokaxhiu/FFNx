/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#include <stdint.h>

#include "defs.h"
#include "battle/camera.h"
#include "field/camera.h"
#include "world/camera.h"
#include "world/world.h"

#include "../audio.h"
#include "../gamepad.h"
#include "../gamehacks.h"
#include "../joystick.h"
#include "../music.h"
#include "../ff7.h"
#include "../log.h"
#include "../metadata.h"
#include "../achievement.h"

#include <bx/math.h>

// CORE GAME LOOP
void ff7_core_game_loop()
{
	struct ff7_game_obj *game_object = (ff7_game_obj *)common_externals.get_game_object();
	uint64_t start_t, end_t;

	common_externals.get_time(&start_t);

	*(double *)&game_object->field_28 = *(double *)&game_object->field_28 + 1.0;
	ff7_engine_exit_game_mode(game_object);

	if ( !game_object->window_minimized && game_object->engine_loop_obj.main_loop )
	{
		game_object->engine_loop_obj.main_loop((game_obj*)game_object);
		*(double *)&game_object->field_44 = *(double *)&game_object->field_44 + 1.0;
	}

	if ( !game_object->window_minimized )
		common_flip((game_obj*)game_object);

	if ( game_object->field_950 || game_object->window_minimized )
		WaitMessage();

	common_externals.get_time(&end_t);
	if ( game_object->field_794 ) common_externals.diff_time(&end_t, &start_t, (uint64_t*)(game_object->field_794 + 0x98));
}

// MDEF fix
uint32_t get_equipment_stats(uint32_t party_index, uint32_t type)
{
	uint32_t character = ff7_externals.party_member_to_char_map[ff7_externals.savemap->party_members[party_index]];

	switch(type)
	{
		case 0:
			return ff7_externals.weapon_data_array[ff7_externals.savemap->chars[character].equipped_weapon].attack_stat;
			break;
		case 1:
			return ff7_externals.armor_data_array[ff7_externals.savemap->chars[character].equipped_armor].defense_stat;
			break;
		case 2:
			return 0;
			break;
		case 3:
			return mdef_fix ? ff7_externals.armor_data_array[ff7_externals.savemap->chars[character].equipped_armor].mdef_stat : 0;
			break;

		default: return 0;
	}
}

// WM_ACTIVATEAPP
void ff7_wm_activateapp(bool hasFocus)
{

}

// Analogue controls
int ff7_get_control_direction()
{
	byte* level_data = *ff7_externals.field_level_data_pointer;

	if (level_data != nullptr)
	{
		uint32_t triggers_offset = *(uint32_t*)(level_data + 0x22);
		signed short* control_direction_data = (signed short*)(level_data + triggers_offset + 4 + 9);

		return static_cast<int>(*control_direction_data);
	}

	return 0;
}

void ff7_set_control_direction(int x)
{
	byte* level_data = *ff7_externals.field_level_data_pointer;

	if (level_data != nullptr)
	{
		uint32_t triggers_offset = *(uint32_t*)(level_data + 0x22);
		signed short* control_direction_data = (signed short*)(level_data + triggers_offset + 4 + 9);

		*control_direction_data = static_cast<signed short>(x);
	}
}

void ff7_use_analogue_controls(float analog_threshold)
{
	static WORD last_field_id = 0;
	static int base_control_direction = 0;
	static bool isCameraReset = false;
	if (last_field_id != *ff7_externals.field_id)
	{
		last_field_id = *ff7_externals.field_id;
		base_control_direction = ff7_get_control_direction();
	}

	vector3<float> joyDir = {0.0f, 0.0f, 0.0f};
	vector3<float> inputDir = {0.0f, 0.0f, 0.0f};
	float horizontalScroll = 0.0f;
	float verticalScroll = 0.0f;
	const float rotSpeedMax = 4.0f;
	float verticalRotSpeed = 0.0f;
	float horizontalRotSpeed = 0.0f;
	const float zoomSpeedMax = 1000.0f;
	float zoomSpeed = 0.0f;

	float invertedVerticalCameraScale = -1.0;
	if(enable_inverted_vertical_camera_controls) invertedVerticalCameraScale = 1.0;

	float invertedHorizontalCameraScale = -1.0;
	if(enable_inverted_horizontal_camera_controls) invertedHorizontalCameraScale = 1.0;

	if(xinput_connected)
	{
		if (gamepad.Refresh())
		{
			if(std::abs(gamepad.leftStickX) > left_analog_stick_deadzone ||
			   std::abs(gamepad.leftStickY) > left_analog_stick_deadzone)
				joyDir = {gamepad.leftStickX, gamepad.leftStickY, 0.0f};
			else
				joyDir = {0.0f, 0.0, 0.0};


			if(gamepad.leftStickY > analog_threshold && !(gamepad.leftStickX < -analog_threshold || gamepad.leftStickX > analog_threshold))
				inputDir = {0.0f, 1.0f, 0.0f};
			else if(gamepad.leftStickY > analog_threshold && gamepad.leftStickX < -analog_threshold)
				inputDir = {-0.707f, 0.707f, 0.0f};
			else if(gamepad.leftStickY > analog_threshold && gamepad.leftStickX > analog_threshold)
				inputDir = {0.707f, 0.707f, 0.0f};
			else if(gamepad.leftStickX < -analog_threshold &&!(gamepad.leftStickY > analog_threshold || gamepad.leftStickY < -analog_threshold))
				inputDir = {-1.0f, 0.0f, 0.0f};
			else if(gamepad.leftStickX > analog_threshold && !(gamepad.leftStickY > analog_threshold || gamepad.leftStickY < -analog_threshold))
				inputDir = {1.0f, 0.0f, 0.0f};
			else if(gamepad.leftStickY < -analog_threshold && gamepad.leftStickX < -analog_threshold)
				inputDir = {-0.707f, -0.707f, 0.0f};
			else if(gamepad.leftStickY < -analog_threshold && gamepad.leftStickX > analog_threshold)
				inputDir = {0.707f, -0.707f, 0.0f};
			else if(gamepad.leftStickY < -analog_threshold && !(gamepad.leftStickX < -analog_threshold || gamepad.leftStickX > analog_threshold))
				inputDir = {0.0f, -1.0f, 0.0f};

			if (gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_THUMB)
			    && std::abs(gamepad.rightStickX) < right_analog_stick_deadzone
				&& std::abs(gamepad.rightStickY) < right_analog_stick_deadzone)
			{
				if(!isCameraReset)
				{
					ff7::world::camera.requestResetCameraRotation(true);
					ff7::battle::camera.reset();
					isCameraReset = true;
				}
			} else
			{
				isCameraReset = false;

				if(gamepad.rightTrigger > right_analog_trigger_deadzone)
					zoomSpeed += zoomSpeedMax * (0.5f * gamepad.rightTrigger);
				if(gamepad.leftTrigger > left_analog_trigger_deadzone)
					zoomSpeed -= zoomSpeedMax * (0.5f * gamepad.leftTrigger);

				bx::Vec3 rightAnalogDir(gamepad.rightStickX, gamepad.rightStickY, 0.0f);
				float length = std::min(bx::length(rightAnalogDir), 1.0f);
				if(length > right_analog_stick_deadzone)
				{
					rightAnalogDir = bx::normalize(rightAnalogDir);
					float scale = (length - right_analog_stick_deadzone) / (1.0 - right_analog_stick_deadzone);
					rightAnalogDir.x *= scale;
					rightAnalogDir.y *= scale;
					verticalRotSpeed = invertedVerticalCameraScale * -rotSpeedMax * rightAnalogDir.y;
					horizontalRotSpeed = invertedHorizontalCameraScale * rotSpeedMax * rightAnalogDir.x;
					horizontalScroll = rightAnalogDir.x;
					verticalScroll = -rightAnalogDir.y;
				}
			}
		}
	}
	else
	{
		if (joystick.Refresh())
		{
			if(std::abs(joystick.GetState()->lX) > joystick.GetDeadZone(left_analog_trigger_deadzone) ||
			   std::abs(joystick.GetState()->lY) > joystick.GetDeadZone(left_analog_trigger_deadzone))
				joyDir = {static_cast<float>(joystick.GetState()->lX) / static_cast<float>(SHRT_MAX),
				         -static_cast<float>(joystick.GetState()->lY) / static_cast<float>(SHRT_MAX), 0.0f};
			else
				joyDir = {0.0f, 0.0, 0.0};

			if(joystick.GetState()->lY < joystick.GetDeadZone(-analog_threshold) &&
			!(joystick.GetState()->lX < joystick.GetDeadZone(-analog_threshold) || joystick.GetState()->lX > joystick.GetDeadZone(analog_threshold)))
				inputDir = {0.0f, 1.0f, 0.0f};
			else if(joystick.GetState()->lY < joystick.GetDeadZone(-analog_threshold) && joystick.GetState()->lX < joystick.GetDeadZone(-analog_threshold))
				inputDir = {-0.707f, 0.707f, 0.0f};
			else if(joystick.GetState()->lY < joystick.GetDeadZone(-analog_threshold) && joystick.GetState()->lX > joystick.GetDeadZone(analog_threshold))
				inputDir = {0.707f, 0.707f, 0.0f};
			else if(joystick.GetState()->lX < joystick.GetDeadZone(-analog_threshold) &&
			!(joystick.GetState()->lY < joystick.GetDeadZone(-analog_threshold) || joystick.GetState()->lY > joystick.GetDeadZone(analog_threshold)))
				inputDir = {-1.0f, 0.0f, 0.0f};
			else if(joystick.GetState()->lX > joystick.GetDeadZone(analog_threshold) &&
				!(joystick.GetState()->lY < joystick.GetDeadZone(-analog_threshold) || joystick.GetState()->lY > joystick.GetDeadZone(analog_threshold)))
				inputDir = {1.0f, 0.0f, 0.0f};
			else if(joystick.GetState()->lY > joystick.GetDeadZone(analog_threshold) && joystick.GetState()->lX < joystick.GetDeadZone(-analog_threshold))
				inputDir = {-0.707f, -0.707f, 0.0f};
			else if(joystick.GetState()->lY > joystick.GetDeadZone(analog_threshold) && joystick.GetState()->lX > joystick.GetDeadZone(analog_threshold))
				inputDir = {0.707f, -0.707f, 0.0f};
			else if(joystick.GetState()->lY > joystick.GetDeadZone(analog_threshold) &&
				!(joystick.GetState()->lX < joystick.GetDeadZone(-analog_threshold) || joystick.GetState()->lX > joystick.GetDeadZone(analog_threshold)))
				inputDir = {0.0f, -1.0f, 0.0f};

			if ((joystick.GetState()->rgbButtons[11] & 0x80)
			    && std::abs(joystick.GetState()->lRz) < joystick.GetDeadZone(right_analog_stick_deadzone)
				&& std::abs(joystick.GetState()->lZ) < joystick.GetDeadZone(right_analog_stick_deadzone))
			{
				if(!isCameraReset)
				{
					ff7::world::camera.requestResetCameraRotation(true);
					ff7::battle::camera.reset();
					isCameraReset = true;
				}
			} else
			{
				isCameraReset = false;

				bx::Vec3 rightAnalogDir(
					static_cast<float>(joystick.GetState()->lZ) / static_cast<float>(SHRT_MAX),
					static_cast<float>(joystick.GetState()->lRz) / static_cast<float>(SHRT_MAX), 0.0f);
				float length = std::min(bx::length(rightAnalogDir), 1.0f);
				if(length > right_analog_stick_deadzone)
				{
					rightAnalogDir = bx::normalize(rightAnalogDir);
					float scale = (length - right_analog_stick_deadzone) / (1.0 - right_analog_stick_deadzone);
					rightAnalogDir.x *=  scale;
					rightAnalogDir.y *=  scale;
					verticalRotSpeed = invertedVerticalCameraScale * rotSpeedMax * rightAnalogDir.y;
					horizontalRotSpeed = invertedHorizontalCameraScale * rotSpeedMax * rightAnalogDir.x;
					horizontalScroll = rightAnalogDir.x;
					verticalScroll = rightAnalogDir.y;
				}

				if(joystick.HasAnalogTriggers())
				{
					if(joystick.GetState()->lRy > -static_cast<float>(SHRT_MAX) + joystick.GetDeadZone(right_analog_trigger_deadzone))
						zoomSpeed += zoomSpeedMax * (0.5f + 0.5f * static_cast<float>(joystick.GetState()->lRy) / static_cast<float>(SHRT_MAX));
					if(joystick.GetState()->lRx > -static_cast<float>(SHRT_MAX) + joystick.GetDeadZone(left_analog_trigger_deadzone))
						zoomSpeed -= zoomSpeedMax * (0.5f + 0.5f * static_cast<float>(joystick.GetState()->lRx) / static_cast<float>(SHRT_MAX));
				}
				else
				{
					if(joystick.GetState()->rgbButtons[7] & 0x80) zoomSpeed += zoomSpeedMax;
					if(joystick.GetState()->rgbButtons[6] & 0x80) zoomSpeed -= zoomSpeedMax;
				}
			}
		}
	}

	ff7::world::world.SetJoystickDirection(joyDir);

	float inputDirLength = vector_length(&inputDir);
	if(inputDirLength > 0.0f)
	{
		normalize_vector(&joyDir);
		float angle = atan2( inputDir.x*joyDir.y - inputDir.y*joyDir.x, inputDir.x*joyDir.x + inputDir.y*joyDir.y );
		int offset = std::max(-128, std::min(128, static_cast<int>(128.0f * angle / M_PI)));
		ff7_set_control_direction(base_control_direction + offset);
	}
	else
	{
		ff7_set_control_direction(base_control_direction);
	}

	ff7::battle::camera.setRotationSpeed(verticalRotSpeed, horizontalRotSpeed, 0.0f);
	ff7::battle::camera.setZoomSpeed(zoomSpeed);
	ff7::field::camera.setScrollingDir(horizontalScroll, verticalScroll);
	ff7::world::camera.setRotationSpeed(verticalRotSpeed, horizontalRotSpeed, 0.0f);
	ff7::world::camera.setZoomSpeed(zoomSpeed);
}

int ff7_get_gamepad()
{
	if (simulate_OK_button)
	{
		return TRUE;
	}
	else if (xinput_connected)
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

struct ff7_gamepad_status* ff7_update_gamepad_status()
{
	float analog_threshold = enable_auto_run ? left_analog_stick_deadzone + 0.25f * (1.0f - left_analog_stick_deadzone) : 0.5f;
	float run_threshold = left_analog_stick_deadzone + 0.75f * (1.0f - left_analog_stick_deadzone);

	// Reset
	ZeroMemory(ff7_externals.gamepad_status, sizeof(ff7_gamepad_status));
	gamepad_analogue_intent = INTENT_NONE;

	if (simulate_OK_button)
	{
		// Flag the button OK ( no matter to what is mapped in the controller ) as pressed
		*ff7_externals.input_ok_button_status = 1;

		// End simulation right here before we press this button by mistake in other windows
		simulate_OK_button = false;
	}
	else if (xinput_connected && gamehacks.canInputBeProcessed())
	{
		if (gamepad.Refresh())
		{
			ff7_externals.gamepad_status->pos_x = gamepad.leftStickX;
			ff7_externals.gamepad_status->pos_y = gamepad.leftStickY;
			ff7_externals.gamepad_status->dpad_up = (gamepad.leftStickY > analog_threshold) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_UP); // UP
			ff7_externals.gamepad_status->dpad_down = (gamepad.leftStickY < -analog_threshold) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_DOWN); // DOWN
			ff7_externals.gamepad_status->dpad_left = (gamepad.leftStickX < -analog_threshold) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_LEFT); // LEFT
			ff7_externals.gamepad_status->dpad_right = (gamepad.leftStickX > analog_threshold) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_RIGHT); // RIGHT
			ff7_externals.gamepad_status->button1 = gamepad.IsPressed(XINPUT_GAMEPAD_X); // Square
			ff7_externals.gamepad_status->button2 = gamepad.IsPressed(XINPUT_GAMEPAD_A); // Cross
			ff7_externals.gamepad_status->button3 = gamepad.IsPressed(XINPUT_GAMEPAD_B); // Circle
			ff7_externals.gamepad_status->button4 = gamepad.IsPressed(XINPUT_GAMEPAD_Y); // Triangle
			ff7_externals.gamepad_status->button5 = gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER); // L1
			ff7_externals.gamepad_status->button6 = gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER); // R1
			ff7_externals.gamepad_status->button7 = gamepad.leftTrigger > 0.85f; // L2
			ff7_externals.gamepad_status->button8 = gamepad.rightTrigger > 0.85f; // R2

			ff7_externals.gamepad_status->button9 = gamepad.IsPressed(XINPUT_GAMEPAD_BACK); // SELECT
			ff7_externals.gamepad_status->button10 = gamepad.IsPressed(XINPUT_GAMEPAD_START); // START
			ff7_externals.gamepad_status->button11 = gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_THUMB); // L3
			ff7_externals.gamepad_status->button12 = gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_THUMB); // R3
			ff7_externals.gamepad_status->button13 = gamepad.IsPressed(0x400); // PS Button

			// Update the player intent based on the analogue movement
			if (enable_auto_run)
			{
				bx::Vec3 joyDir = {gamepad.leftStickX, gamepad.leftStickY, 0.0f};
				auto joyLength = std::min(bx::length(joyDir), 1.0f);
				if (joyLength > run_threshold) gamepad_analogue_intent = INTENT_RUN;
				else if(joyLength > analog_threshold) gamepad_analogue_intent = INTENT_WALK;
			}
		}
	}
	else if (gamehacks.canInputBeProcessed())
	{
		if (joystick.Refresh())
		{
			ff7_externals.gamepad_status->pos_x = joystick.GetState()->lX;
			ff7_externals.gamepad_status->pos_y = joystick.GetState()->lY;
			ff7_externals.gamepad_status->dpad_up = (joystick.GetState()->lY < joystick.GetDeadZone(-analog_threshold)) || joystick.GetState()->rgdwPOV[0] == 0
			                                         || joystick.GetState()->rgdwPOV[0] == 4500 || joystick.GetState()->rgdwPOV[0] == 31500; // UP
			ff7_externals.gamepad_status->dpad_down = (joystick.GetState()->lY > joystick.GetDeadZone(analog_threshold)) || joystick.GetState()->rgdwPOV[0] == 18000
			                                         || joystick.GetState()->rgdwPOV[0] == 13500 || joystick.GetState()->rgdwPOV[0] == 22500; // DOWN
			ff7_externals.gamepad_status->dpad_left = (joystick.GetState()->lX < joystick.GetDeadZone(-analog_threshold)) || joystick.GetState()->rgdwPOV[0] == 27000
			                                         || joystick.GetState()->rgdwPOV[0] == 22500 || joystick.GetState()->rgdwPOV[0] == 31500; // LEFT
			ff7_externals.gamepad_status->dpad_right = (joystick.GetState()->lX > joystick.GetDeadZone(analog_threshold)) || joystick.GetState()->rgdwPOV[0] == 9000
			                                         || joystick.GetState()->rgdwPOV[0] == 4500 || joystick.GetState()->rgdwPOV[0] == 13500; // RIGHT
			ff7_externals.gamepad_status->button1 = joystick.GetState()->rgbButtons[0] & 0x80; // Square
			ff7_externals.gamepad_status->button2 = joystick.GetState()->rgbButtons[1] & 0x80; // Cross
			ff7_externals.gamepad_status->button3 = joystick.GetState()->rgbButtons[2] & 0x80; // Circle
			ff7_externals.gamepad_status->button4 = joystick.GetState()->rgbButtons[3] & 0x80; // Triangle
			ff7_externals.gamepad_status->button5 = joystick.GetState()->rgbButtons[4] & 0x80; // L1
			ff7_externals.gamepad_status->button6 = joystick.GetState()->rgbButtons[5] & 0x80; // R1
			ff7_externals.gamepad_status->button7 = joystick.GetState()->rgbButtons[6] & 0x80; // L2
			ff7_externals.gamepad_status->button8 = joystick.GetState()->rgbButtons[7] & 0x80; // R2

			ff7_externals.gamepad_status->button9 = joystick.GetState()->rgbButtons[8] & 0x80; // SELECT
			ff7_externals.gamepad_status->button10 = joystick.GetState()->rgbButtons[9] & 0x80; // START
			ff7_externals.gamepad_status->button11 = joystick.GetState()->rgbButtons[10] & 0x80; // L3
			ff7_externals.gamepad_status->button12 = joystick.GetState()->rgbButtons[11] & 0x80; // R3
			ff7_externals.gamepad_status->button13 = joystick.GetState()->rgbButtons[12] & 0x80; // PS Button

			// Update the player intent based on the analogue movement
			if (enable_auto_run)
			{
				bx::Vec3 joyDir = {static_cast<float>(joystick.GetState()->lX) / static_cast<float>(joystick.GetDeadZone(1.0f)),
				static_cast<float>(joystick.GetState()->lY) / static_cast<float>(joystick.GetDeadZone(1.0f)), 0.0f};
				auto joyLength = std::min(bx::length(joyDir), 1.0f);
				if (joyLength > run_threshold) gamepad_analogue_intent = INTENT_RUN;
				else if(joyLength > analog_threshold) gamepad_analogue_intent = INTENT_WALK;
			}
		}
	}

	if(enable_analogue_controls) ff7_use_analogue_controls(analog_threshold);

	return ff7_externals.gamepad_status;
}

void* ff7_engine_exit_game_mode(ff7_game_obj* game_object)
{
	void* result;

	if (game_object)
	{
		result = game_object;
		if (game_object->field_924)
		{
			if (game_object->engine_loop_obj.exit_main)
				game_object->engine_loop_obj.exit_main((struct game_obj*)game_object);

			ff7_externals.sub_666C13((struct game_obj*)game_object);
			result = ff7_externals.sub_670F9B(game_object->dx_sfx_something);

			if (ff7_do_reset)
			{
				// Trigger game over and ensure battle mode can be retriggered
				*ff7_externals.word_CC0828 = 0;
				*ff7_externals.byte_CC0D89 = 26;
				*ff7_externals.word_DB958A = 0;
				*ff7_externals.byte_CC164C = 0;
				*ff7_externals.word_CC0DC6 = 0;

				// Fix possible weird 3D issues that may happens in movies if resetted after some of those
				ff7_externals.modules_global_object->MVCAM_flag = 0;

				ff7_do_reset = false;
			}

			if (game_object->engine_loop_obj.enter_main)
				result = game_object->engine_loop_obj.enter_main((struct game_obj*)game_object);

			game_object->field_924 = 0;
		}
	}
	return result;
}

void ff7_on_gameover_enter()
{
	// Stop current music
	((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(/* stop_music */ 0xF0, 0, 0, 0, 0, 0);
	// Stop current sound
	((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(/* stop_music */ 0xF1, 0, 0, 0, 0, 0);
	// Play the gameover music
	((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(/* play_music */ 0x14, /* midi_id */ 0x3A, 0, 0, 0, 0);

	ff7_externals.start_gameover();
}

void ff7_on_gameover_exit()
{
	// Stop current music
	((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(/* stop_music */ 0xF0, 0, 0, 0, 0, 0);
	// Stop current sound
	((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(/* stop_music */ 0xF1, 0, 0, 0, 0, 0);

	ff7_externals.gameover_sub_6C12B1();
}

BYTE ff7_toggle_battle_field()
{
	BYTE ret = ff7_externals.sub_60B260();

	if (!gamehacks.wantsBattle()) ret = 255;

	return ret;
}

BYTE ff7_toggle_battle_worldmap()
{
	BYTE ret = ff7_externals.sub_767C55();

	if (!gamehacks.wantsBattle()) ret = 0;

	return ret;
}

bool ff7_skip_movies()
{
	uint32_t mode = getmode_cached()->driver_mode;

	// Prevent game acting weird or wrong if movie is skipped
	if (
		*ff7_externals.field_id == 399 ||
		*ff7_externals.field_id == 489 ||
		*ff7_externals.field_id == 490 ||
		*ff7_externals.field_id == 543
	)
	{
		return false;
	}

	if (ff7_externals.movie_object->is_playing)
	{
		ff7_externals.movie_object->movie_end = 1;

		if (mode == MODE_FIELD)
		{
			*ff7_externals.word_CC0DD4 = 5;
			*ff7_externals.word_CC1638 = 0;
		}

		if (*ff7_externals.field_id == 116)
		{
			if (use_external_music)
				ff7_play_midi(2);
			else
				ff7_externals.play_midi(2);
		}

		return true;
	}

	return false;
}

void* ff7_menu_sub_6FAC38(uint32_t param1, uint32_t param2, uint8_t param3, uint8_t param4, uint32_t param5)
{
	return ff7_externals.menu_sub_6FAC38(param1, param2, param3, *ff7_externals.millisecond_counter < 0x8000 ? 7 : 0, param5);
}

void* ff7_menu_sub_6F5C0C(uint32_t param1, uint32_t param2, uint8_t param3, uint8_t param4, uint32_t param5)
{
	return ff7_externals.menu_sub_6F5C0C(param1, param2, param3, *ff7_externals.millisecond_counter < 0x8000 ? 7 : 0, param5);
}

void ff7_limit_fps()
{
	static time_t last_gametime;
	time_t gametime;
	double framerate = 30.0f;

	struct ff7_game_obj *game_object = (ff7_game_obj *)common_externals.get_game_object();
	struct game_mode *mode = getmode_cached();

	switch(mode->driver_mode)
	{
	case MODE_FIELD:
		if (ff7_externals.movie_object->is_playing && !*ff7_externals.field_limit_fps)
		{
			// Some movies do not expect to be frame limited
			qpc_get_time(&last_gametime);
			return;
		}
		break;
	case MODE_GAMEOVER:
		// Gameover screen has nothing to limit
		qpc_get_time(&last_gametime);
		return;
	case MODE_SUBMARINE:
		last_gametime = *ff7_externals.submarine_last_gametime;
		break;
	}

	if (ff7_fps_limiter < FPS_LIMITER_60FPS)
	{
		switch (mode->driver_mode)
		{
		case MODE_BATTLE:
			if (ff7_fps_limiter < FPS_LIMITER_30FPS) framerate = 15.0f;
			break;
		case MODE_SNOWBOARD:
		case MODE_COASTER:
		case MODE_CONDOR:
		case MODE_CREDITS:
		case MODE_MENU:
		case MODE_MAIN_MENU:
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
		case MODE_SNOWBOARD:
		case MODE_SUBMARINE:
		case MODE_COASTER:
		case MODE_CONDOR:
		case MODE_CREDITS:
		case MODE_MENU:
		case MODE_MAIN_MENU:
			framerate = 60.0f;
			break;
		}
	}

	switch(mode->driver_mode)
	{
	case MODE_SUBMARINE:
		if (*ff7_externals.submarine_minigame_status)
			*ff7_externals.submarine_minigame_status = 0;
		else
			*ff7_externals.submarine_minigame_status = 1;
		break;
	}

	framerate *= gamehacks.getCurrentSpeedhack();
	double frame_time = game_object->countspersecond / framerate;

	do qpc_get_time(&gametime);
	while (gametime > last_gametime && qpc_diff_time(&gametime, &last_gametime, nullptr) < frame_time);

	last_gametime = gametime;
}

void ff7_handle_ambient_playback()
{
	struct game_mode *mode = getmode_cached();
	static char filename[64]{0};
	static WORD last_field_id = 0, last_triangle_id = 0, last_battle_id = 0;
	bool playing = false;

	switch (mode->driver_mode)
	{
	case MODE_BATTLE:
		if (last_battle_id != ff7_externals.modules_global_object->battle_id)
		{
			last_battle_id = ff7_externals.modules_global_object->battle_id;

			sprintf(filename, "bat_%d", last_battle_id);
			nxAudioEngine.playAmbient(filename);
		}
		if (*ff7_externals.is_battle_paused && nxAudioEngine.isAmbientPlaying())
			nxAudioEngine.pauseAmbient();
		else if (!(*ff7_externals.is_battle_paused) && !(nxAudioEngine.isAmbientPlaying()))
			nxAudioEngine.resumeAmbient();
		break;
	case MODE_FIELD:
		if (last_field_id != *common_externals.current_field_id)
		{
			last_field_id = *common_externals.current_field_id;
			last_triangle_id = *common_externals.current_triangle_id;

			sprintf(filename, "field_%d_%d", last_field_id, *common_externals.current_triangle_id);
			playing = nxAudioEngine.playAmbient(filename);

			if (!playing)
			{
				sprintf(filename, "field_%d", last_field_id);
				playing = nxAudioEngine.playAmbient(filename);
			}
		}
		else if (common_externals.current_triangle_id != 0 && last_field_id == *common_externals.current_field_id && last_triangle_id != *common_externals.current_triangle_id)
		{
			last_triangle_id = *common_externals.current_triangle_id;

			sprintf(filename, "field_%d_%d", last_field_id, *common_externals.current_triangle_id);
			playing = nxAudioEngine.playAmbient(filename);
		}
		break;
	default:
		if (last_field_id != 0 || last_battle_id != 0)
		{
			nxAudioEngine.stopAmbient();
			last_field_id = 0;
			last_battle_id = 0;
		}
		break;
	}
}

void ff7_handle_voice_playback()
{
	switch (getmode_cached()->driver_mode)
	{
	case MODE_BATTLE:
		if (*ff7_externals.g_is_battle_paused && nxAudioEngine.isVoicePlaying())
			nxAudioEngine.pauseVoice();
		else if (!*ff7_externals.g_is_battle_paused && !nxAudioEngine.isVoicePlaying())
			nxAudioEngine.resumeVoice();
		break;
	default:
		break;
	}
}

BOOL ff7_write_save_file(char slot)
{
	BOOL ret = ff7_externals.write_save_file(slot);

	uint8_t savefile_num = ((slot & 0xF0) >> 4);
	ffnx_trace("Save: user saved in save%02i\n", savefile_num);
	metadataPatcher.updateFF7(savefile_num);

	return ret;
}

// The function that load the model size sometimes gets a buffer that is non-null terminated, returning weird values.
// Replace the native `atoi` call with a custom function that always uses the first 4 bytes to convert the buffer to int
int ff7_field_load_models_atoi(const char* str)
{
	std::string buf(str, 4);

	return std::stol(buf);
}

//#########################
// steam achievement hooks
//#########################

int ff7_load_save_file(int param_1){
	int returnValue = ((int(*)(int))ff7_externals.load_save_file)(param_1);
	g_FF7SteamAchievements->initStatsFromSaveFile(*ff7_externals.savemap);
	return returnValue;
}

void ff7_chocobo_field_entity_60FA7D(WORD param1, short param2, short param3){
	((void(*)(WORD, short, short)) ff7_externals.sub_60FA7D)(param1, param2, param3);

	if(param3 == 0x04)
		g_FF7SteamAchievements->unlockGoldChocoboAchievement(ff7_externals.savemap->chocobo_slots_first, ff7_externals.savemap->chocobo_slots_last);
}

void ff7_character_regularly_field_entity_60FA7D(WORD param1, short param2, short param3){
	((void(*)(WORD, short, short)) ff7_externals.sub_60FA7D)(param1, param2, param3);

	if(param3 & (1 << 0) || param3 & (1 << 2))
		g_FF7SteamAchievements->unlockYuffieAndVincentAchievement(ff7_externals.savemap->yuffie_reg_mask, ff7_externals.savemap->vincent_reg_mask);
}
