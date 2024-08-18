/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include "gamehacks.h"
#include "audio.h"
#include "ff7/defs.h"
#include "gamepad.h"
#include "joystick.h"

GameHacks gamehacks;

// PRIVATE

void GameHacks::toggleSpeedhack()
{
	speedhack_enabled = !speedhack_enabled;

	show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Current Speedhack: %s", speedhack_enabled ? "ENABLED" : "DISABLED");

	holdInput();
}

void GameHacks::resetSpeedhack()
{
	speedhack_current_speed = speedhack_min;
}

void GameHacks::increaseSpeedhack()
{
	speedhack_enabled = true;

	if ((speedhack_current_speed + speedhack_step) <= speedhack_max) speedhack_current_speed += speedhack_step;

	show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Current Speedhack: %2.1lfx", speedhack_current_speed);

	holdInput();
}

void GameHacks::decreaseSpeedhack()
{
	speedhack_enabled = true;

	if ((speedhack_current_speed - speedhack_step) >= speedhack_min) speedhack_current_speed -= speedhack_step;

	show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Current Speedhack: %2.1lfx", speedhack_current_speed);

	holdInput();
}

void GameHacks::toggleBattleMode()
{
	battle_wanted = !battle_wanted;

	show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Battle mode: %s", battle_wanted ? "ENABLED" : "DISABLED");

	holdInput();
}

void GameHacks::toggleAutoAttackMode()
{
	auto_attack_mode = !auto_attack_mode;

	show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Auto attack mode: %s", auto_attack_mode ? "ENABLED" : "DISABLED");

	holdInput();
}

void GameHacks::toggleMusicOnBattlePause()
{
	if (!ff8) {
		if (*ff7_externals.is_battle_paused && use_external_music) {
			if (nxAudioEngine.isMusicPlaying())
				nxAudioEngine.pauseMusic();
			else
				nxAudioEngine.resumeMusic();
		}
	}
}

void GameHacks::toggleAutoText()
{
	enable_voice_auto_text = !enable_voice_auto_text;

	show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Voice auto text mode: %s", enable_voice_auto_text ? "ENABLED" : "DISABLED");

	holdInput();
}

void GameHacks::skipMovies()
{
	if (!ff8)
	{
		if (ff7_skip_movies())
		{
			show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "FMV Skipped");

			holdInput();
		}
		else clear_popup_msg();
	}
	else
	{
		if (ff8_skip_movies())
		{
			show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "FMV Skipped");

			holdInput();
		}
		else clear_popup_msg();
	}
}

void GameHacks::softReset()
{
	if (!ff8) ff7_do_reset = true;

	resetSpeedhack();

	holdInput();

	clear_popup_msg();
}

// PUBLIC

void GameHacks::init()
{
	resetSpeedhack();

	if (speedhack_current_speed > 1.0) speedhack_enabled = true;
}

void GameHacks::processKeyboardInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	isKeyboardShortcutMode = false;
	switch (msg)
	{
	case WM_KEYDOWN:
		if ((::GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			isKeyboardShortcutMode = true;
			switch (wParam)
			{
			case 'A':
				toggleAutoAttackMode();
				break;
			case 'B':
				toggleBattleMode();
				break;
			case 'M':
				toggleMusicOnBattlePause();
				break;
			case 'R':
				softReset();
				break;
			case 'S':
				skipMovies();
				break;
			case 'T':
				toggleAutoText();
				break;
			case VK_UP:
				increaseSpeedhack();
				break;
			case VK_DOWN:
				decreaseSpeedhack();
				break;
			case VK_LEFT:
			case VK_RIGHT:
				toggleSpeedhack();
				break;
			}
		}
		break;
	}
}

void GameHacks::processGamepadInput()
{
	if(isGamepadShortcutMode && get_popup_time() == 0) isGamepadShortcutMode = false;

	if (xinput_connected)
	{
		if (gamepad.Refresh())
		{
			if(gamepad.IsIdle())
			{
				hold_input_for_frames = 0;
				enable_hold_input = true;
			}

			if(hold_input_for_frames > 0)
			{
				drawnInput();
				return;
			}

			if (gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_THUMB)) // L2
			{
				isGamepadShortcutMode = !isGamepadShortcutMode;
				if(isGamepadShortcutMode) show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Waiting for shortcut input..");
				else clear_popup_msg();
				holdInput();
			}

			if(!isGamepadShortcutMode) return;

			// Soft reset on START+SELECT
			if (
				gamepad.IsPressed(XINPUT_GAMEPAD_BACK) &&
				gamepad.IsPressed(XINPUT_GAMEPAD_START)
				)
				softReset();
			// Increase in-game speed on R1
			else if (
				gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER)
				)
				increaseSpeedhack();
			// Decrease in-game speed on L1
			else if (
				gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER)
				)
				decreaseSpeedhack();
			// Toggle Speedhack on L2/R2
			else if (
				gamepad.leftTrigger > 0.85f ||
				gamepad.rightTrigger > 0.85f
				)
				toggleSpeedhack();
			// Toggle battle mode on Circle
			else if (
				gamepad.IsPressed(XINPUT_GAMEPAD_B)
				)
				toggleBattleMode();
			// Toggle auto attack mode on Triangle
			else if (
				gamepad.IsPressed(XINPUT_GAMEPAD_Y)
				)
				toggleAutoAttackMode();
			// Skip Movies on Square
			else if (
				gamepad.IsPressed(XINPUT_GAMEPAD_X)
				)
				skipMovies();
		}
	}
	else
	{
		if (joystick.Refresh())
		{
			if(joystick.IsIdle())
			{
				hold_input_for_frames = 0;
				enable_hold_input = true;
			}

			if(hold_input_for_frames > 0)
			{
				drawnInput();
				return;
			}

			if (joystick.GetState()->rgbButtons[10] & 0x80) // L2
			{
				isGamepadShortcutMode = !isGamepadShortcutMode;
				if(isGamepadShortcutMode) show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Waiting for shortcut input..");
				else clear_popup_msg();
				holdInput();
			}

			if(!isGamepadShortcutMode) return;

			// Soft reset on START+SELECT
			if (
				(joystick.GetState()->rgbButtons[8] & 0x80) &&
				(joystick.GetState()->rgbButtons[9] & 0x80)
				)
				softReset();
			// Increase in-game speed on R1
			else if (
				joystick.GetState()->rgbButtons[5] & 0x80
				)
				increaseSpeedhack();
			// Decrease in-game speed on L1
			else if (
				joystick.GetState()->rgbButtons[4] & 0x80
				)
				decreaseSpeedhack();
			// Toggle Speedhack on L2/R2
			else if (
				(joystick.GetState()->rgbButtons[6] & 0x80) ||
				(joystick.GetState()->rgbButtons[7] & 0x80)
				)
				toggleSpeedhack();
			// Toggle battle mode on Circle
			else if (
				joystick.GetState()->rgbButtons[2] & 0x80
				)
				toggleBattleMode();
			// Toggle auto attack mode on Triangle
			else if (
				joystick.GetState()->rgbButtons[3] & 0x80
				)
				toggleAutoAttackMode();
			// Skip Movies on Square
			else if (
				joystick.GetState()->rgbButtons[0] & 0x80
				)
				skipMovies();
		}
	}
}

double GameHacks::getCurrentSpeedhack()
{
	return speedhack_enabled ? speedhack_current_speed : 1.0;
}

bool GameHacks::wantsBattle()
{
	return battle_wanted;
}

bool GameHacks::isAutoAttack()
{
	return auto_attack_mode;
}

void GameHacks::holdInput()
{
	if(!enable_hold_input) return;
	hold_input_for_frames = 30; // ~1 sec
	enable_hold_input = false;
}

void GameHacks::drawnInput()
{
	if (hold_input_for_frames > 0) hold_input_for_frames--;
}

bool GameHacks::canInputBeProcessed()
{
	return !isGamepadShortcutMode && !isKeyboardShortcutMode;
}
