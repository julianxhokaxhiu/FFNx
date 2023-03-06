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

#pragma once

#include "log.h"

class GameHacks
{
private:
	uint16_t hold_input_for_frames = 0;
	bool enable_hold_input = true;

	bool speedhack_enabled;
	double speedhack_current_speed;
	bool battle_wanted = true;
	bool auto_attack_mode = false;

	// SPEEDHACK
	void toggleSpeedhack();
	void resetSpeedhack();
	void increaseSpeedhack();
	void decreaseSpeedhack();

	// BATTLE
	void toggleBattleMode();
	void toggleAutoAttackMode();
	void toggleMusicOnBattlePause();

	// MOVIES
	void skipMovies();

	// SOFT RESET
	void softReset();

	// INPUT VALIDATION
	void holdInput();
	void drawnInput();

	// VOICE AUTO TEXT
	void toggleAutoText();

public:
	void init();

	// GLOBALS
	void processKeyboardInput(UINT msg, WPARAM wParam, LPARAM lParam);
	void processGamepadInput();

	// SPEEDHACK
	double getCurrentSpeedhack();

	// BATTLE
	bool wantsBattle();
	bool isAutoAttack();

	// INPUT VALIDATION
	bool canInputBeProcessed();

private:
	bool isKeyboardShortcutMode = false;
	bool isGamepadShortcutMode = false;
};

extern GameHacks gamehacks;
