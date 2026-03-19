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
#include "gamepad.h"

Gamepad gamepad;

Gamepad::Gamepad()
    : cId(-1), deadzoneX(0.05f), deadzoneY(0.02f), sdlGamepad(nullptr), sdlInstanceId(-1), sdlInitialized(false),
      leftStickX(0.0f), leftStickY(0.0f), rightStickX(0.0f), rightStickY(0.0f), leftTrigger(0.0f), rightTrigger(0.0f), controllerName("")
{
}

Gamepad::Gamepad(float dzX, float dzY)
    : cId(-1), deadzoneX(dzX), deadzoneY(dzY), sdlGamepad(nullptr), sdlInstanceId(-1), sdlInitialized(false),
      leftStickX(0.0f), leftStickY(0.0f), rightStickX(0.0f), rightStickY(0.0f), leftTrigger(0.0f), rightTrigger(0.0f), controllerName("")
{
}

Gamepad::~Gamepad()
{
    closeGamepad();
    if (sdlInitialized)
    {
        SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
        sdlInitialized = false;
    }
}

int Gamepad::GetPort() const
{
    return sdlGamepad ? 1 : 0;
}

GamepadInput* Gamepad::GetState()
{
    return &state.Gamepad;
}

const GamepadVibration &Gamepad::GetVibrationState() const
{
    return vibration;
}

bool Gamepad::Gamepad_Init()
{
    if (sdlInitialized)
        return true;

    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS3, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS3_SIXAXIS_DRIVER, "1");

    if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD))
    {
        if (trace_all || trace_gamepad)
            ffnx_trace("Gamepad: SDL_InitSubSystem(SDL_INIT_GAMEPAD) failed: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetGamepadEventsEnabled(true);

    int mapped = SDL_AddGamepadMappingsFromFile("SDL_GameControllerDB.txt");
    if (mapped > 0)
    {
        if (trace_all || trace_gamepad)
            ffnx_trace("Gamepad: loaded %d gamepad mappings from SDL_GameControllerDB\n", mapped);
    }
    else if (mapped < 0)
    {
        if (trace_all || trace_gamepad)
            ffnx_trace("Gamepad: SDL_GameControllerDB mapping files failed: %s\n", SDL_GetError());
    }

    sdlInitialized = true;

    if (trace_all || trace_gamepad)
        ffnx_trace("Gamepad: initialized gamepad subsystem\n");

    return true;
}

void Gamepad::GetDeviceName(SDL_Gamepad *gp, SDL_JoystickID id)
{
    sdlGamepad = gp;
    sdlInstanceId = id;
    cId = 0;
    const char *name = SDL_GetGamepadName(gp);
    controllerName = name ? name : "";
    if (trace_all || trace_gamepad)
        ffnx_trace("Gamepad connected: %s\n", controllerName.c_str());
}

void Gamepad::handleSDLEvents()
{
    // Pump OS events into the SDL queue once, then peek only at gamepad events.
    // This avoids consuming unrelated events (window, keyboard, mouse) that the
    // rest of the engine still needs to process.
    SDL_PumpEvents();

    SDL_Event events[8];
    int count;
    while ((count = SDL_PeepEvents(events, SDL_arraysize(events), SDL_GETEVENT,
                                   SDL_EVENT_GAMEPAD_AXIS_MOTION,
                                   SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED)) > 0)
    {
        for (int i = 0; i < count; ++i)
        {
            const SDL_Event &event = events[i];
            switch (event.type)
            {
                case SDL_EVENT_GAMEPAD_ADDED:
                    if (!sdlGamepad)
                    {
                        SDL_JoystickID id = event.gdevice.which;
                        SDL_Gamepad *gp = SDL_OpenGamepad(id);
                        if (gp)
                            GetDeviceName(gp, id);
                    }
                    break;

                case SDL_EVENT_GAMEPAD_REMOVED:
                    if (sdlGamepad && event.gdevice.which == sdlInstanceId)
                    {
                        if (trace_all || trace_gamepad)
                            ffnx_trace("Gamepad disconnected: %s\n", controllerName.empty() ? "unknown" : controllerName.c_str());
                        closeGamepad();
                    }
                    break;

                case SDL_EVENT_GAMEPAD_REMAPPED:
                    if (sdlGamepad && event.gdevice.which == sdlInstanceId)
                    {
                        if (trace_all || trace_gamepad)
                            ffnx_trace("Gamepad remapped: %s\n", controllerName.c_str());
                    }
                    break;

                default:
                    break;
            }
        }
    }
}

bool Gamepad::openGamepad()
{
    if (!Gamepad_Init())
        return false;

    int count = 0;
    SDL_JoystickID *ids = SDL_GetGamepads(&count);

    if (!ids || count == 0)
    {
        if (trace_all || trace_gamepad)
            ffnx_trace("Gamepad: no gamepads, count=%d\n", count);
        SDL_free(ids);
        return false;
    }

    for (int i = 0; i < count; ++i)
    {
        SDL_Gamepad *gp = SDL_OpenGamepad(ids[i]);
        if (!gp)
        {
            if (trace_all || trace_gamepad)
                ffnx_trace("Gamepad: SDL_OpenGamepad failed: %s\n", SDL_GetError());
            continue;
        }

        GetDeviceName(gp, ids[i]);
        break;
    }

    SDL_free(ids);
    return sdlGamepad != nullptr;
}

void Gamepad::closeGamepad()
{
    if (sdlGamepad)
    {
        SDL_CloseGamepad(sdlGamepad);
        sdlGamepad = nullptr;
    }

    sdlInstanceId = -1;
    cId = -1;
    controllerName.clear();

    ZeroMemory(&state, sizeof(state));
    leftStickX = leftStickY = rightStickX = rightStickY = 0.0f;
    leftTrigger = rightTrigger = 0.0f;
}

bool Gamepad::CheckConnection()
{
    if (!Gamepad_Init())
        return false;

    handleSDLEvents();

    if (sdlGamepad)
        return true;

    if (!SDL_HasGamepad())
        return false;

    return openGamepad();
}

bool Gamepad::Refresh()
{
    if (!Gamepad_Init())
        return false;

    handleSDLEvents();

    if (!sdlGamepad)
    {
        if (!openGamepad())
            return false;
    }


    deadzoneX = (float)left_analog_stick_deadzone;
    deadzoneY = (float)right_analog_stick_deadzone;

    auto normAxis = [](Sint16 v) -> float { return fmaxf(-1.0f, (float)v / 32767.0f); };
    auto applyDeadzone = [](float v, float dz) -> float
    {
        if (fabsf(v) < dz) return 0.0f;
        float sign = (v < 0.0f) ? -1.0f : 1.0f;
        return ((fabsf(v) - dz) / (1.0f - dz)) * sign;
    };

    leftStickX  =  applyDeadzone(normAxis(SDL_GetGamepadAxis(sdlGamepad, SDL_GAMEPAD_AXIS_LEFTX)),  deadzoneX);
    leftStickY  = -applyDeadzone(normAxis(SDL_GetGamepadAxis(sdlGamepad, SDL_GAMEPAD_AXIS_LEFTY)),  deadzoneY);
    rightStickX =  applyDeadzone(normAxis(SDL_GetGamepadAxis(sdlGamepad, SDL_GAMEPAD_AXIS_RIGHTX)), deadzoneX);
    rightStickY = -applyDeadzone(normAxis(SDL_GetGamepadAxis(sdlGamepad, SDL_GAMEPAD_AXIS_RIGHTY)), deadzoneY);

    auto applyTriggerDeadzone = [](float v, float dz) -> float
    {
        if (v <= dz) return 0.0f;
        return (v - dz) / (1.0f - dz);
    };

    leftTrigger  = applyTriggerDeadzone(SDL_clamp((float)SDL_GetGamepadAxis(sdlGamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER)  / 32767.0f, 0.0f, 1.0f), (float)left_analog_trigger_deadzone);
    rightTrigger = applyTriggerDeadzone(SDL_clamp((float)SDL_GetGamepadAxis(sdlGamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) / 32767.0f, 0.0f, 1.0f), (float)right_analog_trigger_deadzone);

    static const WORD buttonMasks[] = {
        GAMEPAD_BUTTON_DPAD_UP, GAMEPAD_BUTTON_DPAD_DOWN, GAMEPAD_BUTTON_DPAD_LEFT,  GAMEPAD_BUTTON_DPAD_RIGHT,
        GAMEPAD_BUTTON_START,   GAMEPAD_BUTTON_BACK,      GAMEPAD_BUTTON_LEFT_THUMB, GAMEPAD_BUTTON_RIGHT_THUMB,
        GAMEPAD_BUTTON_LEFT_SHOULDER, GAMEPAD_BUTTON_RIGHT_SHOULDER,
        GAMEPAD_BUTTON_A, GAMEPAD_BUTTON_B, GAMEPAD_BUTTON_X, GAMEPAD_BUTTON_Y
    };
    static const SDL_GamepadButton sdlButtons[] = {
        SDL_GAMEPAD_BUTTON_DPAD_UP,    SDL_GAMEPAD_BUTTON_DPAD_DOWN,  SDL_GAMEPAD_BUTTON_DPAD_LEFT,  SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
        SDL_GAMEPAD_BUTTON_START,      SDL_GAMEPAD_BUTTON_BACK,       SDL_GAMEPAD_BUTTON_LEFT_STICK, SDL_GAMEPAD_BUTTON_RIGHT_STICK,
        SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
        SDL_GAMEPAD_BUTTON_SOUTH, SDL_GAMEPAD_BUTTON_EAST, SDL_GAMEPAD_BUTTON_WEST, SDL_GAMEPAD_BUTTON_NORTH
    };
    static_assert(sizeof(buttonMasks) == sizeof(sdlButtons) / sizeof(sdlButtons[0]) * sizeof(buttonMasks[0]),
        "buttonMasks and sdlButtons must have the same number of entries");

    WORD buttons = 0;
    for (int i = 0; i < (int)(sizeof(buttonMasks) / sizeof(buttonMasks[0])); i++)
    {
        if (SDL_GetGamepadButton(sdlGamepad, sdlButtons[i]))
            buttons |= buttonMasks[i];
    }
    if (SDL_GetGamepadButton(sdlGamepad, SDL_GAMEPAD_BUTTON_GUIDE))
        buttons |= GAMEPAD_BUTTON_GUIDE;

    ZeroMemory(&state, sizeof(GamepadState));
    state.Gamepad.sThumbLX      = SDL_GetGamepadAxis(sdlGamepad, SDL_GAMEPAD_AXIS_LEFTX);
    state.Gamepad.sThumbLY      = SDL_GetGamepadAxis(sdlGamepad, SDL_GAMEPAD_AXIS_LEFTY);
    state.Gamepad.sThumbRX      = SDL_GetGamepadAxis(sdlGamepad, SDL_GAMEPAD_AXIS_RIGHTX);
    state.Gamepad.sThumbRY      = SDL_GetGamepadAxis(sdlGamepad, SDL_GAMEPAD_AXIS_RIGHTY);
    state.Gamepad.bLeftTrigger  = (BYTE)(leftTrigger  * 255.0f);
    state.Gamepad.bRightTrigger = (BYTE)(rightTrigger * 255.0f);
    state.Gamepad.wButtons      = buttons;

    return true;
}

bool Gamepad::Vibrate(WORD wLeftMotorSpeed, WORD wRightMotorSpeed)
{
    vibration.wLeftMotorSpeed = wLeftMotorSpeed;
    vibration.wRightMotorSpeed = wRightMotorSpeed;

    if (!Gamepad_Init())
        return false;

    if (!sdlGamepad && !openGamepad())
        return false;

    // SDL requires a duration; calling each frame keeps rumble alive.
    if (!SDL_RumbleGamepad(sdlGamepad, wLeftMotorSpeed, wRightMotorSpeed, 100))
    {
        closeGamepad();
        return false;
    }

    return true;
}

bool Gamepad::IsPressed(WORD button) const
{
    return (state.Gamepad.wButtons & button) != 0;
}

bool Gamepad::IsIdle() const
{
    return !(leftStickY > 0.5f  || IsPressed(GAMEPAD_BUTTON_DPAD_UP))    &&
           !(leftStickY < -0.5f || IsPressed(GAMEPAD_BUTTON_DPAD_DOWN))  &&
           !(leftStickX < -0.5f || IsPressed(GAMEPAD_BUTTON_DPAD_LEFT))  &&
           !(leftStickX > 0.5f  || IsPressed(GAMEPAD_BUTTON_DPAD_RIGHT)) &&
           !IsPressed(GAMEPAD_BUTTON_X)              &&
           !IsPressed(GAMEPAD_BUTTON_A)              &&
           !IsPressed(GAMEPAD_BUTTON_B)              &&
           !IsPressed(GAMEPAD_BUTTON_Y)              &&
           !IsPressed(GAMEPAD_BUTTON_LEFT_SHOULDER)  &&
           !IsPressed(GAMEPAD_BUTTON_RIGHT_SHOULDER) &&
           !(leftTrigger  > 0.85f)                  &&
           !(rightTrigger > 0.85f)                  &&
           !IsPressed(GAMEPAD_BUTTON_BACK)           &&
           !IsPressed(GAMEPAD_BUTTON_START)          &&
           !IsPressed(GAMEPAD_BUTTON_LEFT_THUMB)     &&
           !IsPressed(GAMEPAD_BUTTON_RIGHT_THUMB)    &&
           !IsPressed(GAMEPAD_BUTTON_GUIDE);
}
