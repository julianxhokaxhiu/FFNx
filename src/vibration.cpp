/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 myst6re                                            //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
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

#include "vibration.h"

#include "gamepad.h"
#include "joystick.h"
#include "globals.h"
#include "log.h"

NxVibrationEngine nxVibrationEngine;

NxVibrationEngine::NxVibrationEngine() :
	_leftMotorStopTimeFrame(0),
	_left(0), _right(0),
	_currentLeft(0), _currentRight(0)
{
}

void NxVibrationEngine::setLeftMotorValue(uint8_t force)
{
	if (force > 0)
	{
		_leftMotorStopTimeFrame = xinput_connected ? frame_counter + LEFT_MOTOR_DURATION_FRAMES : 0;
		_left = force;
	}
}

void NxVibrationEngine::setRightMotorValue(uint8_t force)
{
	_right = force;
}

void NxVibrationEngine::stopAll()
{
	_leftMotorStopTimeFrame = 0;
	_left = 0;
	_right = 0;

	rumbleUpdate();
}

bool NxVibrationEngine::hasChanged() const
{
	return _currentLeft != _left || _currentRight != _right;
}

void NxVibrationEngine::updateLeftMotorValue()
{
	if (xinput_connected && _leftMotorStopTimeFrame > 0 && frame_counter > _leftMotorStopTimeFrame)
	{
		if (trace_all) ffnx_trace("NxVibrationEngine::%s stop\n", __func__);
		_leftMotorStopTimeFrame = 0;
		_left = 0;
	}
	else if (!xinput_connected)
	{
		_leftMotorStopTimeFrame = 0;
	}
}

bool NxVibrationEngine::rumbleUpdate()
{
	updateLeftMotorValue();

	if (! hasChanged())
	{
		return false;
	}

	if (trace_all) ffnx_trace("NxVibrationEngine::%s left=%d right=%d\n", __func__, _left, _right);

	const DWORD maxVibration = xinput_connected ? UINT16_MAX : joystick.GetMaxVibration();
	DWORD left = _left * maxVibration / LEFT_MOTOR_MAX_VALUE;
	DWORD right = _right * maxVibration / RIGHT_MOTOR_MAX_VALUE;

	if (left > maxVibration) {
		left = maxVibration;
	}
	if (right > maxVibration) {
		right = maxVibration;
	}

	if (xinput_connected)
	{
		gamepad.Vibrate(left, right);
	}
	else
	{
		joystick.Vibrate(left, right);
	}

	_currentLeft = _left;
	_currentRight = _right;

	return true;
}

bool NxVibrationEngine::canRumble() const
{
	if (xinput_connected)
	{
		return gamepad.GetPort() > 0;
	}

	return joystick.CheckConnection() && joystick.HasForceFeedback();
}
