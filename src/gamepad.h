/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#pragma once

#include <Windows.h>
#include <SDL3/SDL.h>
#include <string>

// Kudos to https://katyscode.wordpress.com/2013/08/30/xinput-tutorial-part-1-adding-gamepad-support-to-your-windows-game/
// for the foundation of FFNx's original XInput implementation

// SDL-style button bitmask constants
#define GAMEPAD_BUTTON_DPAD_UP        0x0001
#define GAMEPAD_BUTTON_DPAD_DOWN      0x0002
#define GAMEPAD_BUTTON_DPAD_LEFT      0x0004
#define GAMEPAD_BUTTON_DPAD_RIGHT     0x0008
#define GAMEPAD_BUTTON_START          0x0010
#define GAMEPAD_BUTTON_BACK           0x0020
#define GAMEPAD_BUTTON_LEFT_THUMB     0x0040
#define GAMEPAD_BUTTON_RIGHT_THUMB    0x0080
#define GAMEPAD_BUTTON_LEFT_SHOULDER  0x0100
#define GAMEPAD_BUTTON_RIGHT_SHOULDER 0x0200
#define GAMEPAD_BUTTON_GUIDE          0x0400
#define GAMEPAD_BUTTON_A              0x1000
#define GAMEPAD_BUTTON_B              0x2000
#define GAMEPAD_BUTTON_X              0x4000
#define GAMEPAD_BUTTON_Y              0x8000

typedef struct GamepadInput
{
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
} GamepadInput;

typedef struct GamepadState
{
    DWORD dwPacketNumber;
    GamepadInput Gamepad;
} GamepadState;

typedef struct GamepadVibration
{
    WORD wLeftMotorSpeed;
    WORD wRightMotorSpeed;
} GamepadVibration;

// SDL3 state type means we still use SDL_JoystickID as the device identifier.
// It is not used as the legacy SI joystick path, only for SDL_GetGamepads / event IDs.

class Gamepad
{
private:
    int cId;
    GamepadState state;
    GamepadVibration vibration;

    float deadzoneX;
    float deadzoneY;

    SDL_Gamepad *sdlGamepad = nullptr;
    SDL_JoystickID sdlInstanceId = -1;
    bool sdlInitialized = false;
    std::string controllerName;

    bool Gamepad_Init();
    void handleSDLEvents();
    bool openGamepad();
    void GetDeviceName(SDL_Gamepad *gp, SDL_JoystickID id);
    void closeGamepad();

public:
    Gamepad();
    Gamepad(float dzX, float dzY);
    ~Gamepad();

    float leftStickX;
    float leftStickY;
    float rightStickX;
    float rightStickY;
    float leftTrigger;
    float rightTrigger;

    int  GetPort() const;
    bool HasRumble() const;
    GamepadInput* GetState();
    const GamepadVibration &GetVibrationState() const;
    bool CheckConnection();
    bool Refresh();
    bool Vibrate(WORD wLeftMotorSpeed, WORD wRightMotorSpeed);
    bool IsPressed(WORD) const;
    bool IsIdle() const;
};

extern Gamepad gamepad;
