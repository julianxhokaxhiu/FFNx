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

    int  GetPort();
    XINPUT_GAMEPAD* GetState();
    bool CheckConnection();
    bool Refresh();
    bool IsPressed(WORD);
};

extern Gamepad gamepad;