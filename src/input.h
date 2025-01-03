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

#pragma once

#include <windows.h>
#include <vector>

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
	virtual void MouseDown(MouseEventArgs e) = 0;
	virtual void MouseUp(MouseEventArgs e) = 0;
	virtual void MouseWheel(MouseEventArgs e) = 0;
	virtual void MouseMove(MouseEventArgs e) = 0;
};

class KeyListener
{
public:
	virtual void KeyUp(KeyEventArgs e) = 0;
	virtual void KeyDown(KeyEventArgs e) = 0;
	virtual void KeyPress(KeyPressEventArgs e) = 0;
};

std::vector<MouseListener*> mouseListeners;
std::vector<KeyListener*> keyListeners;

void SetBlockKeysFromGame(bool block = false);
bool HandleInputEvents(UINT msg, WPARAM wParam, LPARAM lParam);
byte* GetGameKeyState();
