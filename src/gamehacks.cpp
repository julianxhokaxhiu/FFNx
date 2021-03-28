/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
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
#include "music.h"

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

void GameHacks::skipMovies()
{
	if (!ff8)
	{
		if (ff7_skip_movies())
		{
			show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "FMV Skipped");

			holdInput();
		}
	}
	else
	{
		if (ff8_skip_movies())
		{
			show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "FMV Skipped");

			holdInput();
		}
	}
}

void GameHacks::softReset()
{
	if (!canInputBeProcessed()) return;

	if (!ff8) ff7_do_reset = true;

	resetSpeedhack();

	holdInput();
}

// PUBLIC

void GameHacks::init()
{
	resetSpeedhack();

	if (speedhack_current_speed > 1.0) speedhack_enabled = true;
}

void GameHacks::processKeyboardInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if ((::GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			switch (wParam)
			{
			case 'B':
				toggleBattleMode();
				break;
			case 'R':
				softReset();
				break;
			case 'S':
				skipMovies();
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
	if (!ff8)
	{
		// Soft reset on L1+L2+R1+R2+START+SELECT
		if (
			ff7_externals.gamepad_status->button5 &&
			ff7_externals.gamepad_status->button6 &&
			ff7_externals.gamepad_status->button7 &&
			ff7_externals.gamepad_status->button8 &&
			ff7_externals.gamepad_status->button9 &&
			ff7_externals.gamepad_status->button10
			)
			softReset();
		// Increase in-game speed on L2+R2+UP
		else if (
			ff7_externals.gamepad_status->dpad_up &&
			ff7_externals.gamepad_status->button7 &&
			ff7_externals.gamepad_status->button8
			)
			increaseSpeedhack();
		// Decrease in-game speed on L2+R2+DOWN
		else if (
			ff7_externals.gamepad_status->dpad_down &&
			ff7_externals.gamepad_status->button7 &&
			ff7_externals.gamepad_status->button8
			)
			decreaseSpeedhack();
		// Toggle Speedhack on L2+R2+LEFT/RIGHT
		else if (
			(
				ff7_externals.gamepad_status->dpad_left ||
				ff7_externals.gamepad_status->dpad_right
			) &&
			ff7_externals.gamepad_status->button7 &&
			ff7_externals.gamepad_status->button8
			)
			toggleSpeedhack();
		// Toggle battle mode on L3+R3
		else if (
			ff7_externals.gamepad_status->button11 &&
			ff7_externals.gamepad_status->button12
			)
			toggleBattleMode();
		// Skip Movies on SELECT+START
		else if (
			ff7_externals.gamepad_status->button9 &&
			ff7_externals.gamepad_status->button10
			)
			skipMovies();
		else
			drawnInput();
	}
	else
	{
		// Increase in-game speed on L2+R2+DPAD UP
		if (
			ff8_externals.dinput_gamepad_state->rgdwPOV[0] == 0 &&
			ff8_externals.dinput_gamepad_state->rgbButtons[6] == 0x80 &&
			ff8_externals.dinput_gamepad_state->rgbButtons[7] == 0x80
			)
			increaseSpeedhack();
		// Decrease in-game speed on L2+R2+DPAD DOWN
		else if (
			ff8_externals.dinput_gamepad_state->rgdwPOV[0] == 18000 &&
			ff8_externals.dinput_gamepad_state->rgbButtons[6] == 0x80 &&
			ff8_externals.dinput_gamepad_state->rgbButtons[7] == 0x80
			)
			decreaseSpeedhack();
		// Toggle Speedhack on L2+R2+LEFT/RIGHT
		else if (
			(
				ff8_externals.dinput_gamepad_state->rgdwPOV[0] == 27000 ||
				ff8_externals.dinput_gamepad_state->rgdwPOV[0] == 9000
			) &&
			ff8_externals.dinput_gamepad_state->rgbButtons[6] == 0x80 &&
			ff8_externals.dinput_gamepad_state->rgbButtons[7] == 0x80
			)
			toggleSpeedhack();
		// Toggle battle mode on L3+R3
		else if (
			ff8_externals.dinput_gamepad_state->rgbButtons[10] &&
			ff8_externals.dinput_gamepad_state->rgbButtons[11]
			)
			toggleBattleMode();
		// Skip Movies on SELECT+START
		else if (
			ff8_externals.dinput_gamepad_state->rgbButtons[8] == 0x80 &&
			ff8_externals.dinput_gamepad_state->rgbButtons[9] == 0x80
			)
			skipMovies();
		else
			drawnInput();
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

void GameHacks::holdInput()
{
	hold_input_for_frames = 30; // ~1 sec
}

void GameHacks::drawnInput()
{
	if (hold_input_for_frames > 0) hold_input_for_frames--;
}

bool GameHacks::canInputBeProcessed()
{
	return hold_input_for_frames == 0;
}