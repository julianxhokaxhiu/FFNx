/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include "game_cfg.h"

#include "patch.h"
#include "globals.h"
#include "cfg.h"
#include "log.h"

void normalize_path_win(char *name)
{
	int idx = 0;
	while (name[idx] != 0)
	{
		if (name[idx] == '/') name[idx] = '\\';
		idx++;
	}
}

void ff8_set_game_paths(int install_options, char *_app_path, const char *_dataDrive)
{
	char fileName[MAX_PATH] = {};

	if (!app_path.empty())
	{
		ffnx_info("Overriding AppPath with %s\n", app_path.c_str());
		strncpy(fileName, app_path.c_str(), sizeof(fileName));
		_app_path = fileName;
		normalize_path_win(_app_path);
	}

	if (!steam_edition && !remastered_edition && !data_drive.empty())
	{
		ffnx_info("Overriding DataDrive with %s\n", data_drive.c_str());
		_dataDrive = data_drive.c_str();
	}

	ff8_externals.set_game_paths(install_options, _app_path, _dataDrive);

	if (remastered_edition)
	{
		strncpy(ff8_externals.music_path, "data\\music\\stream\\", sizeof("data\\music\\stream\\"));
	}
}

bool ff8_reg_set_graphics(uint32_t graphics)
{
	HKEY phkResult = 0;
	LSTATUS ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Square Soft, Inc\\Final Fantasy VIII\\1.00", 0, KEY_SET_VALUE, &phkResult);

	if (ret != ERROR_SUCCESS)
	{
		return false;
	}

	ret = RegSetValueExA(phkResult, "Graphics", 0, REG_DWORD, (const BYTE *)&graphics, sizeof(uint32_t));
	RegFlushKey(phkResult);
	RegCloseKey(phkResult);

	return ret == ERROR_SUCCESS;
}

int ff8_reg_get_midiguid(LPDWORD midi_guid)
{
	int ret = remastered_edition ? 0 : ff8_externals.reg_get_midiguid((LPBYTE)midi_guid);
	LPDWORD default_midi_guid[4] = {0, 0, 0, 0};

	if (remastered_edition || memcmp(midi_guid, default_midi_guid, 16) == 0) {
		ffnx_info("MIDI GUID is zero, force to Microsoft Synthesizer\n");
		// Use default Microsoft synthesizer, instead of starting FF8Config.exe
		uint32_t buf[4] = {0x58C2B4D0, 0x11D146E7, 0xA000AC89, 0x294105C9};
		memcpy(midi_guid, buf, 16);

		if (!steam_edition && !remastered_edition) {
			ff8_externals.reg_set_midiguid((uint8_t *)buf);
			ff8_reg_set_graphics(0x100021); // High res by default
		}

		return 1;
	}

	return ret;
}

uint32_t ff8_reg_get_graphics()
{
	uint32_t ret = remastered_edition ? 0x00100021 : ff8_externals.reg_get_graphics();

	ret |= 0x00100000; // Force this flag to prevent graphical glitches

	if (ff8_high_res_font == 1) {
		ret |= 0x00000020;
	} else if (ff8_high_res_font == 0) {
		ret &= 0xFFFFFFDF;
	}

	return ret;
}

void ff8_reg_get_app_path(CHAR *lpData, DWORD cbData)
{
	char buf[MAX_PATH]{ 0 };
	GetCurrentDirectory(sizeof(buf), buf);
	strcat(buf, R"(\)");
	strcpy(lpData, buf);
}

void ff8_reg_get_data_drive(CHAR *lpData, DWORD cbData)
{
	strcpy(lpData, "CD:");
}

int ff8_reg_get_installoptions()
{
	return 0x000000ff;
}

int ff8_reg_get_guid(LPBYTE lpData)
{
	LPSTR buf[16]{ 0 };
	memcpy(lpData, buf, 16);

	return 1;
}

int ff8_reg_get_soundoptions()
{
	return 0x00000000;
}

int ff8_reg_get_midioptions()
{
	return 0x00000001;
}

void game_cfg_init()
{
	if (ff8)
	{
		replace_call(ff8_externals.init_config + 0x3E, ff8_set_game_paths);
		replace_call(ff8_externals.init_config + 0x48, ff8_reg_get_midiguid);
		replace_call(ff8_externals.init_config + 0x16B, ff8_reg_get_graphics);

		if (remastered_edition)
		{
			replace_call(ff8_externals.init_config + 0x15, ff8_reg_get_app_path);
			replace_call(ff8_externals.init_config + 0x21, ff8_reg_get_data_drive);
			replace_call(ff8_externals.init_config + 0x26, ff8_reg_get_installoptions);
			replace_call(ff8_externals.pubintro_init + 0x14, ff8_reg_get_guid); // Graphics
			replace_call(ff8_externals.pubintro_init + 0x28, ff8_reg_get_guid); // Sound
			replace_call(ff8_externals.pubintro_init + 0x1E, ff8_reg_get_soundoptions);
			replace_call(ff8_externals.pubintro_init + 0x32, ff8_reg_get_midioptions);
		}
	}
}
