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

#include <cmath>

#include "gamepad.h"

Gamepad gamepad;

int Gamepad::GetPort() const
{
    return cId + 1;
}

XINPUT_GAMEPAD* Gamepad::GetState()
{
    return &state.Gamepad;
}

const XINPUT_VIBRATION &Gamepad::GetVibrationState() const
{
    return vibration;
}

bool Gamepad::Vibrate(WORD wLeftMotorSpeed, WORD wRightMotorSpeed)
{
    vibration.wLeftMotorSpeed = wLeftMotorSpeed;
    vibration.wRightMotorSpeed = wRightMotorSpeed;

    if (cId == -1)
        CheckConnection();

    if (cId != -1)
    {
        if (XInputSetState(cId, &vibration) != ERROR_SUCCESS)
        {
            cId = -1;
            return false;
        }

        return true;
    }

    return false;
}

bool Gamepad::CheckConnection()
{
    int controllerId = -1;

    for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++)
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        if (XInputGetState(i, &state) == ERROR_SUCCESS)
            controllerId = i;
    }

    cId = controllerId;

    return controllerId != -1;
}

// Returns false if the controller has been disconnected
bool Gamepad::Refresh()
{
    if (cId == -1)
        CheckConnection();

    if (cId != -1)
    {
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        if (XInputGetState(cId, &state) != ERROR_SUCCESS)
        {
            cId = -1;
            return false;
        }

        float normLX = fmaxf(-1, (float)state.Gamepad.sThumbLX / 32767);
        float normLY = fmaxf(-1, (float)state.Gamepad.sThumbLY / 32767);

        leftStickX = (abs(normLX) < deadzoneX ? 0 : (abs(normLX) - deadzoneX) * (normLX / abs(normLX)));
        leftStickY = (abs(normLY) < deadzoneY ? 0 : (abs(normLY) - deadzoneY) * (normLY / abs(normLY)));

        if (deadzoneX > 0) leftStickX *= 1 / (1 - deadzoneX);
        if (deadzoneY > 0) leftStickY *= 1 / (1 - deadzoneY);

        float normRX = fmaxf(-1, (float)state.Gamepad.sThumbRX / 32767);
        float normRY = fmaxf(-1, (float)state.Gamepad.sThumbRY / 32767);

        rightStickX = (abs(normRX) < deadzoneX ? 0 : (abs(normRX) - deadzoneX) * (normRX / abs(normRX)));
        rightStickY = (abs(normRY) < deadzoneY ? 0 : (abs(normRY) - deadzoneY) * (normRY / abs(normRY)));

        if (deadzoneX > 0) rightStickX *= 1 / (1 - deadzoneX);
        if (deadzoneY > 0) rightStickY *= 1 / (1 - deadzoneY);

        leftTrigger = (float)state.Gamepad.bLeftTrigger / 255;
        rightTrigger = (float)state.Gamepad.bRightTrigger / 255;

        return true;
    }
    return false;
}

bool Gamepad::IsPressed(WORD button) const
{
    return (state.Gamepad.wButtons & button) != 0;
}

bool Gamepad::IsIdle()
{
  return  !(gamepad.leftStickY > 0.5f || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_UP)) &&
          !(gamepad.leftStickY < -0.5f || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_DOWN)) &&
          !(gamepad.leftStickX < -0.5f || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_LEFT)) &&
          !(gamepad.leftStickX > 0.5f || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_RIGHT)) &&
          !gamepad.IsPressed(XINPUT_GAMEPAD_X) &&
          !gamepad.IsPressed(XINPUT_GAMEPAD_A) &&
          !gamepad.IsPressed(XINPUT_GAMEPAD_B) &&
          !gamepad.IsPressed(XINPUT_GAMEPAD_Y) &&
          !gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER)&&
          !gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
          !(gamepad.leftTrigger > 0.85f) &&
          !(gamepad.rightTrigger > 0.85f) &&
          !gamepad.IsPressed(XINPUT_GAMEPAD_BACK) &&
          !gamepad.IsPressed(XINPUT_GAMEPAD_START) &&
          !gamepad.IsPressed(XINPUT_GAMEPAD_START) &&
          !gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_THUMB) &&
          !gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_THUMB) &&
          !gamepad.IsPressed(0x400);
}
