/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
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

#include <wbemidl.h>
#include <oleauto.h>

#include "joystick.h"
#include "log.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

Joystick joystick;

BOOL CALLBACK Joystick::staticEnumerateGameControllers(LPCDIDEVICEINSTANCE devInst, LPVOID pvRef)
{
	Joystick* inputHandlerInstance = (Joystick*)pvRef;
	return inputHandlerInstance->enumerateGameControllers(devInst);
}

BOOL CALLBACK Joystick::staticSetGameControllerProperties(LPCDIDEVICEOBJECTINSTANCE devObjInst, LPVOID pvRef)
{
  // the game controller
	LPDIRECTINPUTDEVICE8 gameController = (LPDIRECTINPUTDEVICE8)pvRef;
	gameController->Unacquire();

  // structure to hold game controller range properties
	DIPROPRANGE gameControllerRange;

	// set the range to -32768 and 32768
	gameControllerRange.lMin = SHRT_MIN;
	gameControllerRange.lMax = SHRT_MAX;

	// set the size of the structure
	gameControllerRange.diph.dwSize = sizeof(DIPROPRANGE);
	gameControllerRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);

	// set the object that we want to change
	gameControllerRange.diph.dwHow = DIPH_BYID;
	gameControllerRange.diph.dwObj = devObjInst->dwType;

	// now set the range for the axis
	if (FAILED(gameController->SetProperty(DIPROP_RANGE, &gameControllerRange.diph)))
		return DIENUM_STOP;

	return DIENUM_CONTINUE;
}

BOOL Joystick::enumerateGameControllers(LPCDIDEVICEINSTANCE devInst)
{
  if(isXInputDevice(&devInst->guidProduct))
    return DIENUM_CONTINUE;

	// enumerate devices
	LPDIRECTINPUTDEVICE8 gameController;

	// create interface for the current game controller
	if (FAILED(dev->CreateDevice(devInst->guidInstance, &gameController, NULL)))
		return DIENUM_CONTINUE;
	else
	{
    if (trace_all || trace_gamepad)
    {
      // get game controller name
      DIDEVICEINSTANCE deviceInfo;
      deviceInfo.dwSize = sizeof(DIDEVICEINSTANCE);
      gameController->GetDeviceInfo(&deviceInfo);
      ffnx_trace("Found DInput Gamepad #%d: %s\n", gameControllers.size(), deviceInfo.tszInstanceName);
    }

		// store the game controller
		gameControllers.push_back(gameController);
		return DIENUM_CONTINUE;
	}
}

LPDIJOYSTATE2 Joystick::GetState()
{
  return &currentState;
}

LPDIDEVCAPS Joystick::GetCaps()
{
  return &caps;
}

bool Joystick::CheckConnection()
{
  if (dev == nullptr)
  {
    // initialize the main DirectInput 8 device
    if (FAILED(DirectInput8Create(gameHinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&dev, NULL)))
      return false;

    gameControllers.clear();

    // enumerate all available game controllers. Attempt to fetch the ones that support force feedback
    if (FAILED(dev->EnumDevices(DI8DEVCLASS_GAMECTRL, &staticEnumerateGameControllers, this, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK)))
      return false;

    // If the collection is empty it means the API call was successful but no device was found. Re-iterate without force feedback this time.
    if (!gameControllers.empty())
      gameControllerSupportsVibration = true;
    else
    {
      if (FAILED(dev->EnumDevices(DI8DEVCLASS_GAMECTRL, &staticEnumerateGameControllers, this, DIEDFL_ATTACHEDONLY)))
        return false;
    }

    if (gameControllers.empty())
      return false;

    gameController = gameControllers.at(0);

    // Get controller capabilities
    caps.dwSize = sizeof(DIDEVCAPS);

    if (FAILED(gameController->GetCapabilities(&caps)))
      return false;

    if (trace_all || trace_gamepad)
    {
      // get game controller name
      DIDEVICEINSTANCE deviceInfo;
      deviceInfo.dwSize = sizeof(DIDEVICEINSTANCE);
      gameController->GetDeviceInfo(&deviceInfo);
      ffnx_trace("Using Gamepad: %s (Supports Force Feedback: %s)\n", deviceInfo.tszInstanceName, gameControllerSupportsVibration ? "yes" : "no");
    }

    // set cooperative level
    if (FAILED(gameController->SetCooperativeLevel(gameHwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
      return false;

    // set data format
    if (FAILED(gameController->SetDataFormat(&c_dfDIJoystick2)))
      return false;

    // set range and dead zone of joystick axes
    if (FAILED(gameController->EnumObjects(&staticSetGameControllerProperties, gameController, DIDFT_AXIS)))
      return false;

    // get game controller max force feedback supported magnitude
    if (gameControllerSupportsVibration)
    {
      gameControllerInfo.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);

      if (FAILED(gameController->GetObjectInfo(&gameControllerInfo, DIDFT_AXIS, DIPH_BYID)))
        return false;
    }
  }

  // clean joystick states
  ZeroMemory(&currentState, sizeof(DIJOYSTATE2));

  return gameController != nullptr;
}

// poll
bool Joystick::Refresh()
{
  HRESULT hr;

  // Return if no joystick detected
  if (!CheckConnection()) return false;

  // poll the device to read the current state
  hr = gameController->Poll();

  if (FAILED(hr))
  {
    // DirectInput lost the device, try to re-acquire it
    hr = gameController->Acquire();
    while (hr == DIERR_INPUTLOST)
      hr = gameController->Acquire();

    // return if a fatal error is encountered
    if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED))
      return false;

    // if another application has control of this device, we have to wait for our turn
    if (hr == DIERR_OTHERAPPHASPRIO)
      return false;
  }

  // now if everything is okay, we can get the state of the device
  if (FAILED(hr = gameController->GetDeviceState(sizeof(DIJOYSTATE2), &currentState)))
    return false;

  return true;
}

bool Joystick::HasAnalogTriggers()
{
  return caps.dwAxes >=6;
}

void Joystick::Clean()
{
  for (LPDIRECTINPUTDEVICE8 controller : gameControllers)
    SAFE_RELEASE(controller);

  SAFE_RELEASE(dev);

  dev = nullptr;
  gameController = nullptr;
  gameControllers.clear();
}

LONG Joystick::GetDeadZone(float percent)
{
  return SHRT_MAX * percent;
}

bool Joystick::HasForceFeedback()
{
  return gameControllerSupportsVibration;
}

DWORD Joystick::GetMaxVibration()
{
  return gameControllerSupportsVibration ? gameControllerInfo.dwFFMaxForce : 0;
}

void Joystick::Vibrate(WORD leftMotorSpeed, WORD rightMotorSpeed)
{
  if (gameControllerSupportsVibration)
  {
    DWORD      dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
    LONG       lDirection[2] = { leftMotorSpeed, rightMotorSpeed };

    DIPERIODIC diPeriodic;      // type-specific parameters
    DIENVELOPE diEnvelope;      // envelope
    DIEFFECT   diEffect;        // general parameters

    // setup the periodic structure
    diPeriodic.dwMagnitude = DI_FFNOMINALMAX;
    diPeriodic.lOffset = 0;
    diPeriodic.dwPhase = 0;
    diPeriodic.dwPeriod = (DWORD) (0.05 * DI_SECONDS);

    // set the modulation envelope
    diEnvelope.dwSize = sizeof(DIENVELOPE);
    diEnvelope.dwAttackLevel = 0;
    diEnvelope.dwAttackTime = (DWORD) (0.01 * DI_SECONDS);
    diEnvelope.dwFadeLevel = 0;
    diEnvelope.dwFadeTime = (DWORD) (3.0 * DI_SECONDS);

    // set up the effect structure itself
    diEffect.dwSize = sizeof(DIEFFECT);
    diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
    diEffect.dwDuration = (DWORD) INFINITE; // (1 * DI_SECONDS);

    // set up details of effect
    diEffect.dwSamplePeriod = 0;               // = default
    diEffect.dwGain = DI_FFNOMINALMAX;         // no scaling
    diEffect.dwTriggerButton = DIJOFS_BUTTON0; // connect effect to trigger button
    diEffect.dwTriggerRepeatInterval = 0;
    diEffect.cAxes = 2;
    diEffect.rgdwAxes = dwAxes;
    diEffect.rglDirection = &lDirection[0];
    diEffect.lpEnvelope = &diEnvelope;
    diEffect.cbTypeSpecificParams = sizeof(diPeriodic);
    diEffect.lpvTypeSpecificParams = &diPeriodic;

    // create the effect and get the interface to it
    if (SUCCEEDED(gameController->CreateEffect(GUID_Square, &diEffect, &gameControllerEffect, NULL)))
      // Play the effect
      gameControllerEffect->Start(1, DIES_SOLO);
  }
}

bool Joystick::IsIdle()
{
  return  !(joystick.GetState()->lY < joystick.GetDeadZone(-0.5f) || joystick.GetState()->rgdwPOV[0] == 0) &&
          !(joystick.GetState()->lY > joystick.GetDeadZone(0.5f) || joystick.GetState()->rgdwPOV[0] == 18000) &&
          !(joystick.GetState()->lX < joystick.GetDeadZone(-0.5f) || joystick.GetState()->rgdwPOV[0] == 27000) &&
          !(joystick.GetState()->lX > joystick.GetDeadZone(0.5f) || joystick.GetState()->rgdwPOV[0] == 9000) &&
          !(joystick.GetState()->rgbButtons[0] & 0x80) &&
          !(joystick.GetState()->rgbButtons[1] & 0x80) &&
          !(joystick.GetState()->rgbButtons[2] & 0x80) &&
          !(joystick.GetState()->rgbButtons[3] & 0x80) &&
          !(joystick.GetState()->rgbButtons[4] & 0x80) &&
          !(joystick.GetState()->rgbButtons[5] & 0x80) &&
          !(joystick.GetState()->rgbButtons[6] & 0x80) &&
          !(joystick.GetState()->rgbButtons[7] & 0x80) &&
          !(joystick.GetState()->rgbButtons[8] & 0x80) &&
          !(joystick.GetState()->rgbButtons[9] & 0x80) &&
          !(joystick.GetState()->rgbButtons[10] & 0x80) &&
          !(joystick.GetState()->rgbButtons[11] & 0x80) &&
          !(joystick.GetState()->rgbButtons[12] & 0x80);
}

//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains
// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
// Unfortunately this information can not be found by just using DirectInput
//-----------------------------------------------------------------------------
BOOL Joystick::isXInputDevice(const GUID* pGuidProductFromDirectInput)
{
  IWbemLocator*           pIWbemLocator  = NULL;
  IEnumWbemClassObject*   pEnumDevices   = NULL;
  IWbemClassObject*       pDevices[20]   = {0};
  IWbemServices*          pIWbemServices = NULL;
  BSTR                    bstrNamespace  = NULL;
  BSTR                    bstrDeviceID   = NULL;
  BSTR                    bstrClassName  = NULL;
  DWORD                   uReturned      = 0;
  bool                    bIsXinputDevice= false;
  UINT                    iDevice        = 0;
  VARIANT                 var;
  HRESULT                 hr;

  // CoInit if needed
  hr = CoInitialize(NULL);
  bool bCleanupCOM = SUCCEEDED(hr);

  // So we can call VariantClear() later, even if we never had a successful IWbemClassObject::Get().
  VariantInit(&var);

  // Create WMI
  hr = CoCreateInstance(__uuidof(WbemLocator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IWbemLocator), (LPVOID*) &pIWbemLocator);

  if(FAILED(hr) || pIWbemLocator == NULL)
    goto LCleanup;

  bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2"); if(bstrNamespace == NULL) goto LCleanup;
  bstrClassName = SysAllocString(L"Win32_PNPEntity");    if(bstrClassName == NULL) goto LCleanup;
  bstrDeviceID  = SysAllocString(L"DeviceID");           if(bstrDeviceID == NULL)  goto LCleanup;

  // Connect to WMI
  hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L, 0L, NULL, NULL, &pIWbemServices);
  if(FAILED(hr) || pIWbemServices == NULL)
    goto LCleanup;

  // Switch security level to IMPERSONATE.
  CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

  hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
  if(FAILED(hr) || pEnumDevices == NULL)
    goto LCleanup;

  // Loop over all devices
  for(;;)
  {
    // Get 20 at a time
    hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);

    if(FAILED(hr))
      goto LCleanup;

    if(uReturned == 0)
      break;

    for(iDevice=0; iDevice<uReturned; iDevice++)
    {
      // For each device, get its device ID
      hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
      if(SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL)
      {
        // Check if the device ID contains "IG_".  If it does, then it's an XInput device
        // This information can not be found from DirectInput
        if(wcsstr(var.bstrVal, L"IG_"))
        {
          // If it does, then get the VID/PID from var.bstrVal
          DWORD dwPid = 0, dwVid = 0;
          WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");

          if(strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1)
            dwVid = 0;

          WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");

          if(strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1)
            dwPid = 0;

          // Compare the VID/PID to the DInput device
          DWORD dwVidPid = MAKELONG(dwVid, dwPid);
          if(dwVidPid == pGuidProductFromDirectInput->Data1)
          {
            bIsXinputDevice = true;
            goto LCleanup;
          }
        }
      }
      VariantClear(&var);
      SAFE_RELEASE(pDevices[iDevice]);
    }
  }

LCleanup:
  VariantClear(&var);
  if(bstrNamespace)
    SysFreeString(bstrNamespace);
  if(bstrDeviceID)
    SysFreeString(bstrDeviceID);
  if(bstrClassName)
    SysFreeString(bstrClassName);
  for(iDevice=0; iDevice<20; iDevice++)
    SAFE_RELEASE(pDevices[iDevice]);
  SAFE_RELEASE(pEnumDevices);
  SAFE_RELEASE(pIWbemLocator);
  SAFE_RELEASE(pIWbemServices);

  if(bCleanupCOM)
    CoUninitialize();

  return bIsXinputDevice;
}
