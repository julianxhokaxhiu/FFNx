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
    return sdlgamepad ? 1 : 0;
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
    if (!sdlgamepad) return "";
    const char *name = SDL_GetGamepadName(sdlgamepad);
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
                if (sdlgamepad && event.gaxis.which == sdlInstanceId)
                {
                    float leftDeadzone  = (float)left_analog_stick_deadzone;
                    float rightDeadzone = (float)right_analog_stick_deadzone;

                    auto normAxis = [](Sint16 v) -> float { return SDL_clamp((float)v / SDL_JOYSTICK_AXIS_MAX, -1.0f, 1.0f); };
                    auto applyDeadzone = [](float v, float dz) -> float {
                        if (fabsf(v) < dz) return 0.0f;
                        float sign = (v < 0.0f) ? -1.0f : 1.0f;
                        return ((fabsf(v) - dz) / (1.0f - dz)) * sign;
                    };
                    auto applyTriggerDeadzone = [](float v, float dz) -> float {
                        if (v <= dz) return 0.0f;
                        return (v - dz) / (1.0f - dz);
                    };

                    switch ((SDL_GamepadAxis)event.gaxis.axis)
                    {
                        case SDL_GAMEPAD_AXIS_LEFTX:
                            leftStickX =  applyDeadzone(normAxis(event.gaxis.value), leftDeadzone);
                            break;
                        case SDL_GAMEPAD_AXIS_LEFTY:
                            leftStickY = -applyDeadzone(normAxis(event.gaxis.value), leftDeadzone);
                            break;
                        case SDL_GAMEPAD_AXIS_RIGHTX:
                            rightStickX =  applyDeadzone(normAxis(event.gaxis.value), rightDeadzone);
                            break;
                        case SDL_GAMEPAD_AXIS_RIGHTY:
                            rightStickY = -applyDeadzone(normAxis(event.gaxis.value), rightDeadzone);
                            break;
                        case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                            leftTrigger  = applyTriggerDeadzone(SDL_clamp((float)event.gaxis.value / SDL_JOYSTICK_AXIS_MAX, 0.0f, 1.0f), (float)left_analog_trigger_deadzone);
                            break;
                        case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                            rightTrigger = applyTriggerDeadzone(SDL_clamp((float)event.gaxis.value / SDL_JOYSTICK_AXIS_MAX, 0.0f, 1.0f), (float)right_analog_trigger_deadzone);
                            break;
                        default:
                            break;
                    }
                }
                break;

            case SDL_EVENT_GAMEPAD_ADDED:
                if (!sdlgamepad)
                {
                    SDL_JoystickID id = event.gdevice.which;
                    SDL_Gamepad *gp = SDL_OpenGamepad(id);
                    if (gp)
                    {
                        sdlgamepad = gp;
                        sdlInstanceId = id;
                    }
                }
                break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                if (sdlgamepad && event.gdevice.which == sdlInstanceId)
                    closeGamepad();
                break;

            case SDL_EVENT_GAMEPAD_REMAPPED:
                if (sdlgamepad && event.gdevice.which == sdlInstanceId)
                {
                    closeGamepad();
                    openGamepad();
                    if (trace_all || trace_gamepad)
                        ffnx_trace("SDL gamepad: mapping remapped, reopened\n");
                }
                break;

            default:
                break;
        }
    }
}

bool SDLGamepad::openGamepad()
{
    if (!init())
        return false;

    int count = 0;
    SDL_JoystickID *ids = SDL_GetGamepads(&count);

    if (!ids || count == 0)
    {
        SDL_free(ids);
        return false;
    }

    for (int i = 0; i < count; ++i)
    {
        SDL_Gamepad *gp = SDL_OpenGamepad(ids[i]);
        if (!gp) continue;

        sdlgamepad = gp;
        sdlInstanceId = ids[i];
        break;
    }

    SDL_free(ids);
    return sdlgamepad != nullptr;
}

bool SDLGamepad::CheckConnection()
{
    if (!init())
        return false;

    GamepadEvents();

    return sdlgamepad != nullptr;
}

void SDLGamepad::closeGamepad()
{
    if (sdlgamepad)
    {
        SDL_CloseGamepad(sdlgamepad);
        sdlgamepad = nullptr;
    }

    sdlInstanceId = -1;

    leftStickX = leftStickY = rightStickX = rightStickY = 0.0f;
    leftTrigger = rightTrigger = 0.0f;
}

bool SDLGamepad::Refresh()
{
    if (!sdlgamepad)
    {
        if (!openGamepad())
            return false;
    }

    GamepadEvents();

    return true;
}

bool SDLGamepad::HasRumble() const
{
    if (!sdlgamepad)
        return false;
    SDL_PropertiesID props = SDL_GetGamepadProperties(sdlgamepad);
    return SDL_GetBooleanProperty(props, SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, false);
}

bool SDLGamepad::Vibrate(WORD wLeftMotorSpeed, WORD wRightMotorSpeed)
{
    if (!sdlgamepad && !openGamepad())
        return false;

    if (!SDL_RumbleGamepad(sdlgamepad, wLeftMotorSpeed, wRightMotorSpeed, SDL_HAPTIC_INFINITY))
        return false;

    return true;
}

bool SDLGamepad::IsPressed(SDL_GamepadButton button) const
{
    if (!sdlgamepad) return false;
    return SDL_GetGamepadButton(sdlgamepad, button);
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
