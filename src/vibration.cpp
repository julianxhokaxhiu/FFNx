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

#include <vector>

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

NxVibrationEngine::~NxVibrationEngine()
{
	for (auto data: _vibrateData) {
		delete[] data.second;
	}
}

void NxVibrationEngine::setLeftMotorValue(uint8_t force)
{
	if (trace_all || trace_gamepad) ffnx_trace("NxVibrationEngine::%s force=%d\n", __func__, force);

	if (force > 0)
	{
		_leftMotorStopTimeFrame = xinput_connected ? frame_counter + LEFT_MOTOR_DURATION_FRAMES : 0;
		_left = force;
	}
}

void NxVibrationEngine::setRightMotorValue(uint8_t force)
{
	if (trace_all || trace_gamepad) ffnx_trace("NxVibrationEngine::%s force=%d\n", __func__, force);

	_right = force;
}

void NxVibrationEngine::stopAll()
{
	if (trace_all || trace_gamepad) ffnx_trace("NxVibrationEngine::%s\n", __func__);

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
		if (trace_all || trace_gamepad) ffnx_trace("NxVibrationEngine::%s stop\n", __func__);
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

	if (trace_all || trace_gamepad) ffnx_trace("NxVibrationEngine::%s left=%d right=%d\n", __func__, _left, _right);

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

uint8_t *NxVibrationEngine::createVibrateDataFromConfig(const toml::parse_result &config)
{
	char sectionName[6] = "";
	uint32_t header[64] = {};
	std::vector<uint16_t> executionData;

	for (int set = 0; set < 64; ++set)
	{
		snprintf(sectionName, sizeof(sectionName), "set%d", set);
		const toml::array *leftMotor = config[sectionName]["left_motor"].as_array();
		const toml::array *rightMotor = config[sectionName]["right_motor"].as_array();
		if (leftMotor && rightMotor
			&& (leftMotor->empty() || leftMotor->is_homogeneous(toml::node_type::array))
			&& (rightMotor->empty() || rightMotor->is_homogeneous(toml::node_type::array))
		) {
			header[set] = sizeof(header) + executionData.size() * sizeof(uint16_t);

			uint16_t leftSize = leftMotor->empty() ? 0 : (leftMotor->size() + 1) * 2,
				rightSize = rightMotor->empty() ? 0 : (rightMotor->size() + 1) * 2;
			executionData.push_back(leftSize);
			executionData.push_back(rightSize);

			for (const toml::node& elem: *leftMotor)
			{
				const toml::array *values = elem.as_array();

				if (values && values->is_homogeneous(toml::node_type::integer) && values->size() == 2)
				{
					executionData.push_back(
						(((*values)[1].value_or(0) & 0xFF) << 8) | ((*values)[0].value_or(0) & 0xFF)
					);
				}
				else
				{
					ffnx_warning("NxVibrationEngine::%s: Invalid values in section %s/left_motor\n", __func__, sectionName);
				}
			}

			if (leftSize > 0) {
				// End of sequence
				executionData.push_back(0xFF00);
			}

			for (const toml::node& elem: *rightMotor)
			{
				const toml::array *values = elem.as_array();

				if (values && values->is_homogeneous(toml::node_type::integer) && values->size() == 2)
				{
					executionData.push_back(
						(((*values)[1].value_or(0) & 0xFF) << 8) | ((*values)[0].value_or(0) & 0xFF)
					);
				}
				else
				{
					ffnx_warning("NxVibrationEngine::%s: Invalid values in section %s/right_motor\n", __func__, sectionName);
				}
			}

			if (rightSize > 0) {
				// End of sequence
				executionData.push_back(0xFF00);
			}
		}
		else if (config[sectionName])
		{
			ffnx_warning("NxVibrationEngine::%s: Missing or invalid left_motor or right_motor in section %s\n", __func__, sectionName);
		}
	}

	if (executionData.empty())
	{
		return nullptr;
	}

	uint8_t *data = new uint8_t[sizeof(header) + executionData.size() * sizeof(uint16_t)];

	memcpy(data, header, sizeof(header));
	memcpy(data + sizeof(header), executionData.data(), executionData.size() * sizeof(uint16_t));

	return data;
}

const uint8_t *NxVibrationEngine::vibrateDataOverride(const char *name)
{
	char fullpath[MAX_PATH];
	snprintf(fullpath, sizeof(fullpath), "%s/%s/%s.toml", basedir, external_vibrate_path.c_str(), name);

	if (trace_all || trace_gamepad) ffnx_trace("NxVibrationEngine::%s: looking for %s\n", __func__, fullpath);

	std::string nameStr(name);

	if (_vibrateData.contains(nameStr))
	{
		return _vibrateData[nameStr];
	}

	try
	{
		toml::parse_result result = toml::parse_file(fullpath);
		uint8_t *data = createVibrateDataFromConfig(result);
		if (data != nullptr)
		{
			if (trace_all || trace_gamepad) ffnx_trace("NxVibrationEngine::%s: data created\n", __func__, fullpath);
			_vibrateData[nameStr] = data;
		}

		return data;
	}
	catch (const toml::parse_error &err)
	{
		return nullptr;
	}

	return nullptr;
}
