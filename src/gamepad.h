/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#include <cmath>
#include <Windows.h>
#include <Xinput.h>

// Kudos to https://katyscode.wordpress.com/2013/08/30/xinput-tutorial-part-1-adding-gamepad-support-to-your-windows-game/
class Gamepad
{
private:
    int cId;
    XINPUT_STATE state;
    XINPUT_VIBRATION vibration;

    float deadzoneX;
    float deadzoneY;

public:
    Gamepad() : deadzoneX(0.05f), deadzoneY(0.02f) {}
    Gamepad(float dzX, float dzY) : deadzoneX(dzX), deadzoneY(dzY) {}

    float leftStickX;
    float leftStickY;
    float rightStickX;
    float rightStickY;
    float leftTrigger;
    float rightTrigger;

    int  GetPort() const;
    XINPUT_GAMEPAD* GetState();
    void SetState(const XINPUT_VIBRATION &vibration);
    bool CheckConnection();
    // Get state from remote device
    bool Refresh();
    // Send vibration state to remote device
    bool Send();
    bool IsPressed(WORD) const;
};

extern Gamepad gamepad;
