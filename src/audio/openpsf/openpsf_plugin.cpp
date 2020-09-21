/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#include "openpsf_plugin.h"

OpenPsfPlugin::OpenPsfPlugin() : handle(nullptr), mod(nullptr)
{
}

OpenPsfPlugin::~OpenPsfPlugin()
{
	close();
}

bool OpenPsfPlugin::open(const char* libFileName)
{
	close();
	this->handle = LoadLibraryA(libFileName);

	if (nullptr != this->handle)
	{
		FARPROC procAddress = GetProcAddress(this->handle, "get_openpsf");

		if (nullptr != procAddress)
		{
			get_openpsf f = (get_openpsf)procAddress;
			mod = f();
			if (nullptr != mod) {
				if (mod->initialize_psx_core("bios.bin")) {
					return true;
				}
				else {
					error("Cannot open bios file from %s\n", "bios.bin");
				}
			}
		}

		error("couldn't load function get_openpsf in external library (error %u)\n", GetLastError());

		close();
	}
	else {
		error("couldn't load external library (error %u)\n", GetLastError());
	}

	return false;
}

void OpenPsfPlugin::close()
{
	if (nullptr != this->handle) {
		FreeLibrary(this->handle);
		this->handle = nullptr;
	}
}
