/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
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

void set_game_paths(int install_options, char *_app_path, const char *_dataDrive)
{
	char fileName[MAX_PATH] = {};

	if (!app_path.empty())
	{
		ffnx_info("Overriding AppPath with %s\n", app_path.c_str());
		strncpy(fileName, app_path.c_str(), sizeof(fileName));
		_app_path = fileName;
	}

	if (!steam_edition && !data_drive.empty())
	{
		ffnx_info("Overriding DataDrive with %s\n", data_drive.c_str());
		_dataDrive = data_drive.c_str();
	}

	ff8_externals.set_game_paths(install_options, _app_path, _dataDrive);
}

void game_cfg_init()
{
	if (ff8)
	{
		replace_call(ff8_externals.init_config + 0x3E, set_game_paths);
	}
}
