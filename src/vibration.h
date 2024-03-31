/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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

#include <stdint.h>
#include <unordered_map>
#include <string>

#include "cfg.h"

constexpr int LEFT_MOTOR_DURATION_FRAMES = 5;
constexpr int LEFT_MOTOR_MAX_VALUE = 240;
constexpr int RIGHT_MOTOR_MAX_VALUE = 128;

class NxVibrationEngine {
public:
	NxVibrationEngine();
	~NxVibrationEngine();
	void setLeftMotorValue(uint8_t force);
	void setRightMotorValue(uint8_t force);
	void stopAll();
	bool rumbleUpdate();
	bool canRumble() const;
	const uint8_t *vibrateDataOverride(const char *name);
private:
	bool hasChanged() const;
	void updateLeftMotorValue();
	uint8_t *createVibrateDataFromConfig(const toml::parse_result &config);

	uint32_t _leftMotorStopTimeFrame;
	uint8_t _left, _right;
	uint8_t _currentLeft, _currentRight;
	std::unordered_map<std::string, uint8_t *> _vibrateData;
};

extern NxVibrationEngine nxVibrationEngine;
