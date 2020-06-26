#pragma once

#include <windows.h>
#include <functional>

struct KeyEventArgs
{
	int keyValue;
	bool isExtended;
};

struct KeyPressEventArgs
{
	int keyChar;
};

struct MouseEventArgs
{
	int button;
	int delta;
	int x;
	int y;
};

class MouseListener
{
public:
	virtual void MouseDown(MouseEventArgs& e) = 0;
	virtual void MouseUp(MouseEventArgs& e) = 0;
	virtual void MouseWheel(MouseEventArgs& e) = 0;
	virtual void MouseMove(MouseEventArgs& e) = 0;
};

class KeyListener
{
public:
	virtual void KeyUp(KeyEventArgs& e) = 0;
	virtual void KeyDown(KeyEventArgs& e) = 0;
	virtual void KeyPress(KeyPressEventArgs& e) = 0;
};

void SetBlockKeysFromGame(bool block = false);
__declspec(dllexport) void __stdcall RegisterMouseListener(MouseListener* listener);
__declspec(dllexport) void __stdcall RegisterKeyListener(KeyListener* listener);
bool HandleInputEvents(UINT msg, WPARAM wParam, LPARAM lParam);
byte* GetGameKeyState();