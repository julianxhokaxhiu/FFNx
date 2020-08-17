/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

GameHacks gamehacks;

// PUBLIC

void GameHacks::processKeyboardInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if ((::GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			switch (wParam)
			{
			case 'R':
				if (!ff8) ff7_do_reset = true;
				resetSpeedhack();
				break;
			case VK_UP:
				increaseSpeedhack();
				break;
			case VK_DOWN:
				decreaseSpeedhack();
				break;
			}
		}
		break;
	}
}

void GameHacks::processGamepadInput()
{
	if ((lastFrame == 0) || (frame_counter - lastFrame > 3))
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
			{
				ff7_do_reset = true;

				resetSpeedhack();
			}
			// Increase in-game speed on L2+R2+DPAD UP
			else if (
				ff7_externals.gamepad_status->dpad_up &&
				ff7_externals.gamepad_status->button7 &&
				ff7_externals.gamepad_status->button8
				)
				increaseSpeedhack();
			// Decrease in-game speed on L2+R2+DPAD DOWN
			else if (
				ff7_externals.gamepad_status->dpad_down &&
				ff7_externals.gamepad_status->button7 &&
				ff7_externals.gamepad_status->button8
				)
				decreaseSpeedhack();
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
		}

		lastFrame = frame_counter;
	}
}

void GameHacks::resetSpeedhack()
{
	speedhack_current_speed = 1.0;
}

void GameHacks::increaseSpeedhack()
{
	if (speedhack_current_speed == speedhack_max) speedhack_current_speed = 1;
	else speedhack_current_speed += speedhack_step;

	show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Current Speedhack: %2.1lfx", speedhack_current_speed);
}

void GameHacks::decreaseSpeedhack()
{
	if (speedhack_current_speed == 1.0) speedhack_current_speed = speedhack_max;
	else speedhack_current_speed -= speedhack_step;

	show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Current Speedhack: %2.1lfx", speedhack_current_speed);
}

double GameHacks::getCurrentSpeedhack()
{
	return speedhack_current_speed;
}