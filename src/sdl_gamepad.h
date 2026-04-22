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

// Kudos to https://katyscode.wordpress.com/2013/08/30/xinput-tutorial-part-1-adding-gamepad-support-to-your-windows-game/
// for the foundation of FFNx's original XInput implementation

class SDLGamepad
{
private:
    SDL_Gamepad *handle = nullptr;
    SDL_JoystickID sdlInstanceId = 0;
    bool sdlInitialized = false;

    bool init();
    void GamepadEvents();
    void closeGamepad();

public:
    ~SDLGamepad();

    float leftStickX = 0.0f;
    float leftStickY = 0.0f;
    float rightStickX = 0.0f;
    float rightStickY = 0.0f;
    float leftTrigger = 0.0f;
    float rightTrigger = 0.0f;

    int  GetPort() const;
    const char* GetName() const;
    bool CheckConnection();
    bool HasRumble() const;
    bool Refresh();
    bool Vibrate(WORD wLeftMotorSpeed, WORD wRightMotorSpeed);
    bool IsPressed(SDL_GamepadButton) const;
    bool IsIdle() const;
};

extern SDLGamepad sdlgamepad;
