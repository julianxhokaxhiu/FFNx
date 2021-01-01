/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
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

#include <string>
#include <vector>
#include <dinput.h>

// Inspired by https://bell0bytes.eu/directinput/

// the joystick class (DirectInput)
class Joystick
{
private:
  LPDIRECTINPUT8 dev = nullptr;                   // dinput interface
  LPDIRECTINPUTDEVICE8 gameController = nullptr;  // the actual joystick device
  DIJOYSTATE2 currentState;			                  // the state of the joystick in the current frame

  BOOL enumerateGameControllers(LPCDIDEVICEINSTANCE devInst);
  std::vector<LPDIRECTINPUTDEVICE8> gameControllers;	// a vector of all available game controllers

  BOOL isXInputDevice(const GUID *pGuidProductFromDirectInput);

  static BOOL CALLBACK staticEnumerateGameControllers(LPCDIDEVICEINSTANCE devInst, LPVOID pvRef);
  static BOOL CALLBACK staticSetGameControllerProperties(LPCDIDEVICEOBJECTINSTANCE devObjInst, LPVOID pvRef);

public:
  LPDIJOYSTATE2 GetState();
  bool CheckConnection();
  bool Refresh();
  void Clean();

  LONG GetDeadZone(float percent);
};

extern Joystick joystick;
