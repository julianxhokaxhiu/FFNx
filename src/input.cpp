#include <windowsx.h>
#include <dinput.h>
#include <vector>
#include "input.h"
#include "globals.h"

std::vector<MouseListener*> mouseListeners;
std::vector<KeyListener*> keyListeners;
byte keys[256];
bool blockKeys = false;

void SetBlockKeysFromGame(bool block) {
    blockKeys = block;
}

__declspec(dllexport) void __stdcall RegisterMouseListener(MouseListener* listener)
{
    mouseListeners.push_back(listener);
}

__declspec(dllexport) void __stdcall RegisterKeyListener(KeyListener* listener)
{
    keyListeners.push_back(listener);
}

void MouseDown(MouseEventArgs& e)
{
    for (MouseListener* listener : mouseListeners)
        listener->MouseDown(e);
}

void MouseUp(MouseEventArgs& e)
{
    for (MouseListener* listener : mouseListeners)
        listener->MouseUp(e);
}

void MouseWheel(MouseEventArgs& e)
{
    for (MouseListener* listener : mouseListeners)
        listener->MouseWheel(e);
}

void KeyUp(KeyEventArgs& e)
{
    for (KeyListener* listener : keyListeners)
        listener->KeyUp(e);
}

void KeyDown(KeyEventArgs& e)
{
    for (KeyListener* listener : keyListeners)
        listener->KeyDown(e);
}

void RefreshDevices()
{

}

void HandleInputEvents(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
        MouseDown(MouseEventArgs{
            (int)msg,
            0,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam),
            });
        break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
        MouseUp(MouseEventArgs{
            (int)msg,
            0,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam),
            });
        break;
    case WM_MOUSEWHEEL:
        MouseWheel(MouseEventArgs{
            0,
            0,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam),
            });
        break;
    case WM_SETCURSOR:
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        KeyDown(KeyEventArgs{
            (int)wParam,
            });
            break;
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        KeyUp(KeyEventArgs{
            (int)wParam,
            });
            break;
        break;
    case WM_CHAR:
        break;
    case WM_DEVICECHANGE:
        RefreshDevices();
        break;
    }
}

byte* GetGameKeyState()
{
    IDirectInputDeviceA* keyboard_device = *common_externals.keyboard_device;
    if (keyboard_device != NULL)
    {
        if (blockKeys)
        {
            std::memset(keys, 0, 256);
            return keys;
        }

        if (keyboard_device->GetDeviceState(256, keys) == 0)
            return keys;

        *common_externals.keyboard_connected = 0;
        if (common_externals.dinput_acquire_keyboard() != 0 && keyboard_device->GetDeviceState(256, keys) != 0)
            return keys;
    }
    return NULL;
}