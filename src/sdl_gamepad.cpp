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

static float applyDeadzone(float value, float deadzone)
{
    float norm = fmaxf(-1.0f, value / SDL_JOYSTICK_AXIS_MAX);
    if (fabsf(norm) < deadzone) return 0.0f;
    float sign = (norm < 0.0f) ? -1.0f : 1.0f;
    return ((fabsf(norm) - deadzone) / (1.0f - deadzone)) * sign;
}

static float applyTriggerDeadzone(float value, float deadzone)
{
    float raw = SDL_clamp(value / SDL_JOYSTICK_AXIS_MAX, 0.0f, 1.0f);
    if (raw <= deadzone) return 0.0f;
    return (raw - deadzone) / (1.0f - deadzone);
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
                    float leftDz  = (float)left_analog_stick_deadzone;
                    float rightDz = (float)right_analog_stick_deadzone;

                    switch ((SDL_GamepadAxis)event.gaxis.axis)
                    {
                        case SDL_GAMEPAD_AXIS_LEFTX:
                            leftStickX = applyDeadzone((float)event.gaxis.value, leftDz);
                            break;
                        case SDL_GAMEPAD_AXIS_LEFTY:
                            leftStickY = -applyDeadzone((float)event.gaxis.value, leftDz);
                            break;
                        case SDL_GAMEPAD_AXIS_RIGHTX:
                            rightStickX = applyDeadzone((float)event.gaxis.value, rightDz);
                            break;
                        case SDL_GAMEPAD_AXIS_RIGHTY:
                            rightStickY = -applyDeadzone((float)event.gaxis.value, rightDz);
                            break;
                        case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                            leftTrigger = applyTriggerDeadzone((float)event.gaxis.value, (float)left_analog_trigger_deadzone);
                            break;
                        case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                            rightTrigger = applyTriggerDeadzone((float)event.gaxis.value, (float)right_analog_trigger_deadzone);
                            break;
                        default:
                            break;
                    }
                }
                break;

            case SDL_EVENT_GAMEPAD_ADDED:
                if (!sdlgamepad)
                {
                    SDL_Gamepad *gp = SDL_OpenGamepad(event.gdevice.which);
                    if (gp)
                    {
                        sdlgamepad = gp;
                        sdlInstanceId = event.gdevice.which;
                    }
                }
                break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                if (sdlgamepad && event.gdevice.which == sdlInstanceId)
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
    if (sdlgamepad)
    {
        SDL_CloseGamepad(sdlgamepad);
        sdlgamepad = nullptr;
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

    return sdlgamepad != nullptr;
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
    if (!sdlgamepad)
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
