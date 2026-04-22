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

#include <cmath>
#include <SDL3/SDL.h>

#include "cfg.h"
#include "log.h"
#include "sdl_gamepad.h"

SDLGamepad sdlgamepad;

SDLGamepad::~SDLGamepad()
{
    closeGamepad();
    if (sdlInitialized)
    {
        SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
        sdlInitialized = false;
    }
}

int SDLGamepad::GetPort() const
{
    return handle ? 1 : 0;
}


bool SDLGamepad::init()
{
    if (sdlInitialized)
        return true;

    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS3, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS3_SIXAXIS_DRIVER, "1");

    if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD))
    {
        ffnx_error("SDL gamepad: failed to initialize subsystem\n");
        return false;
    }

    int GamepadMappingLoaded = SDL_AddGamepadMappingsFromFile("gamecontrollerdb.txt");
    sdlInitialized = true;

    if (trace_all || trace_gamepad)
    {
        ffnx_trace("SDL gamepad: subsystem initialized\n");
        if (GamepadMappingLoaded > 0) ffnx_trace("SDL gamepad: loaded %d mappings from gamecontrollerdb.txt\n", GamepadMappingLoaded);
    }

    return true;
}

const char* SDLGamepad::GetName() const
{
    if (!handle) return "";
    const char *name = SDL_GetGamepadName(handle);
    return name ? name : "";
}

void SDLGamepad::GamepadEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                if (handle && event.gaxis.which == sdlInstanceId)
                {
                    float leftDz  = (float)left_analog_stick_deadzone;
                    float rightDz = (float)right_analog_stick_deadzone;

                    switch ((SDL_GamepadAxis)event.gaxis.axis)
                    {
                        case SDL_GAMEPAD_AXIS_LEFTX:
                        case SDL_GAMEPAD_AXIS_LEFTY:
                        {
                            float normLX = fmaxf(-1.0f, (float)SDL_GetGamepadAxis(handle, SDL_GAMEPAD_AXIS_LEFTX) / SDL_JOYSTICK_AXIS_MAX);
                            float normLY = fmaxf(-1.0f, (float)SDL_GetGamepadAxis(handle, SDL_GAMEPAD_AXIS_LEFTY) / SDL_JOYSTICK_AXIS_MAX);
                            leftStickX = (fabsf(normLX) < leftDz ? 0.0f : (fabsf(normLX) - leftDz) * (normLX / fabsf(normLX)));
                            leftStickY = (fabsf(normLY) < leftDz ? 0.0f : (fabsf(normLY) - leftDz) * (normLY / fabsf(normLY)));
                            if (leftDz > 0.0f) { leftStickX *= 1.0f / (1.0f - leftDz); leftStickY *= 1.0f / (1.0f - leftDz); }
                            leftStickY = -leftStickY;
                            break;
                        }
                        case SDL_GAMEPAD_AXIS_RIGHTX:
                        case SDL_GAMEPAD_AXIS_RIGHTY:
                        {
                            float normRX = fmaxf(-1.0f, (float)SDL_GetGamepadAxis(handle, SDL_GAMEPAD_AXIS_RIGHTX) / SDL_JOYSTICK_AXIS_MAX);
                            float normRY = fmaxf(-1.0f, (float)SDL_GetGamepadAxis(handle, SDL_GAMEPAD_AXIS_RIGHTY) / SDL_JOYSTICK_AXIS_MAX);
                            rightStickX = (fabsf(normRX) < rightDz ? 0.0f : (fabsf(normRX) - rightDz) * (normRX / fabsf(normRX)));
                            rightStickY = (fabsf(normRY) < rightDz ? 0.0f : (fabsf(normRY) - rightDz) * (normRY / fabsf(normRY)));
                            if (rightDz > 0.0f) { rightStickX *= 1.0f / (1.0f - rightDz); rightStickY *= 1.0f / (1.0f - rightDz); }
                            rightStickY = -rightStickY;
                            break;
                        }
                        case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                        {
                            float raw = SDL_clamp((float)event.gaxis.value / SDL_JOYSTICK_AXIS_MAX, 0.0f, 1.0f);
                            float dz  = (float)left_analog_trigger_deadzone;
                            leftTrigger = (raw <= dz) ? 0.0f : (raw - dz) / (1.0f - dz);
                            break;
                        }
                        case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                        {
                            float raw = SDL_clamp((float)event.gaxis.value / SDL_JOYSTICK_AXIS_MAX, 0.0f, 1.0f);
                            float dz  = (float)right_analog_trigger_deadzone;
                            rightTrigger = (raw <= dz) ? 0.0f : (raw - dz) / (1.0f - dz);
                            break;
                        }
                        default:
                            break;
                    }
                }
                break;

            case SDL_EVENT_GAMEPAD_ADDED:
                if (!handle)
                {
                    SDL_Gamepad *gp = SDL_OpenGamepad(event.gdevice.which);
                    if (gp)
                    {
                        handle = gp;
                        sdlInstanceId = event.gdevice.which;
                    }
                }
                break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                if (handle && event.gdevice.which == sdlInstanceId)
                    closeGamepad();
                break;

            default:
                break;
        }
    }
}


bool SDLGamepad::CheckConnection()
{
    return Refresh();
}

void SDLGamepad::closeGamepad()
{
    if (handle)
    {
        SDL_CloseGamepad(handle);
        handle = nullptr;
    }

    sdlInstanceId = 0;

    leftStickX = leftStickY = rightStickX = rightStickY = 0.0f;
    leftTrigger = rightTrigger = 0.0f;
}

bool SDLGamepad::Refresh()
{
    if (!init())
        return false;

    GamepadEvents();

    return handle != nullptr;
}

bool SDLGamepad::HasRumble() const
{
    if (!handle)
        return false;
    SDL_PropertiesID props = SDL_GetGamepadProperties(handle);
    return SDL_GetBooleanProperty(props, SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, false);
}

bool SDLGamepad::Vibrate(WORD wLeftMotorSpeed, WORD wRightMotorSpeed)
{
    if (!handle)
        return false;

    if (!SDL_RumbleGamepad(handle, wLeftMotorSpeed, wRightMotorSpeed, SDL_HAPTIC_INFINITY))
        return false;

    return true;
}

bool SDLGamepad::IsPressed(SDL_GamepadButton button) const
{
    if (!handle) return false;
    return SDL_GetGamepadButton(handle, button);
}

bool SDLGamepad::IsIdle() const
{
    return !(leftStickY > 0.5f  || IsPressed(SDL_GAMEPAD_BUTTON_DPAD_UP))    &&
           !(leftStickY < -0.5f || IsPressed(SDL_GAMEPAD_BUTTON_DPAD_DOWN))  &&
           !(leftStickX < -0.5f || IsPressed(SDL_GAMEPAD_BUTTON_DPAD_LEFT))  &&
           !(leftStickX > 0.5f  || IsPressed(SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) &&
           !IsPressed(SDL_GAMEPAD_BUTTON_WEST)              &&
           !IsPressed(SDL_GAMEPAD_BUTTON_SOUTH)             &&
           !IsPressed(SDL_GAMEPAD_BUTTON_EAST)              &&
           !IsPressed(SDL_GAMEPAD_BUTTON_NORTH)             &&
           !IsPressed(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)     &&
           !IsPressed(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)    &&
           !(leftTrigger  > 0.85f)                         &&
           !(rightTrigger > 0.85f)                         &&
           !IsPressed(SDL_GAMEPAD_BUTTON_BACK)              &&
           !IsPressed(SDL_GAMEPAD_BUTTON_START)             &&
           !IsPressed(SDL_GAMEPAD_BUTTON_LEFT_STICK)        &&
           !IsPressed(SDL_GAMEPAD_BUTTON_RIGHT_STICK)       &&
           !IsPressed(SDL_GAMEPAD_BUTTON_GUIDE);
}
