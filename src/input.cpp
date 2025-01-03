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

#include <windowsx.h>
#include <dinput.h>
#include "input.h"
#include "gamehacks.h"
#include "globals.h"

byte keys[256];
bool blockKeys = false;

void SetBlockKeysFromGame(bool block) {
    blockKeys = block;
}

void MouseDown(MouseEventArgs e)
{
    for (MouseListener* listener : mouseListeners)
        listener->MouseDown(e);
}

void MouseUp(MouseEventArgs e)
{
    for (MouseListener* listener : mouseListeners)
        listener->MouseUp(e);
}

void MouseWheel(MouseEventArgs e)
{
    for (MouseListener* listener : mouseListeners)
        listener->MouseWheel(e);
}

void MouseMove(MouseEventArgs e)
{
    for (MouseListener* listener : mouseListeners)
        listener->MouseMove(e);
}

void KeyUp(KeyEventArgs e)
{
    for (KeyListener* listener : keyListeners)
        listener->KeyUp(e);
}

void KeyDown(KeyEventArgs e)
{
    for (KeyListener* listener : keyListeners)
        listener->KeyDown(e);
}

void KeyPress(KeyPressEventArgs e)
{
    for (KeyListener* listener : keyListeners)
        listener->KeyPress(e);
}

void RefreshDevices()
{

}

// Handles inputs coming from the WindowProc function - originates from DispatchMessage in the message loop
bool HandleInputEvents(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
    {
        int button = 0;
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 1; }
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 2; }
        if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 3; }
        if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 4 : 5; }
        MouseDown(MouseEventArgs{
            (int)button,
            0,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam),
            });
        return true;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    {
        int button = 0;
        if (msg == WM_LBUTTONUP) { button = 1; }
        if (msg == WM_RBUTTONUP) { button = 2; }
        if (msg == WM_MBUTTONUP) { button = 3; }
        if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 4 : 5; }
        MouseUp(MouseEventArgs{
            (int)button,
            0,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam)
            });
        return true;
    }
    case WM_MOUSEWHEEL:
        MouseWheel(MouseEventArgs{
            0,
            GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam)
            });
        return true;
    case WM_MOUSEMOVE:
        MouseMove(MouseEventArgs{
            0,
            0,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam)
            });
        return true;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        KeyDown(KeyEventArgs{
            (int)wParam,
            (bool)((lParam >> 24) & 1)
            });
        return true;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        KeyUp(KeyEventArgs{
            (int)wParam,
            (bool)((lParam >> 24) & 1)
            });
        return true;
    case WM_CHAR:
        KeyPress(KeyPressEventArgs{
            (int)wParam
            });
        return true;
    case WM_DEVICECHANGE:
        RefreshDevices();
        return true;
    }
    return false;
}

// Handles polling the keyboard input using DirectInput
byte* GetGameKeyState()
{
    IDirectInputDeviceA* keyboard_device = *common_externals.keyboard_device;
    if (keyboard_device != NULL)
    {
        if (blockKeys || !gamehacks.canInputBeProcessed())
        {
            std::memset(keys, 0, 256);
            return keys;
        }

        // This is the existing functionality but retries on any error rather than just DIERR_INPUTLOST
        if (keyboard_device->GetDeviceState(256, keys) == 0)
            return keys;

        *common_externals.keyboard_connected = 0;
        if (common_externals.dinput_acquire_keyboard() != 0 && keyboard_device->GetDeviceState(256, keys) != 0)
            return keys;
    }
    return NULL;
}
