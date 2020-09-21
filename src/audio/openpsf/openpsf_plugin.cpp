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

OpenPsfPlugin::OpenPsfPlugin() : handle(nullptr)
{
	trace("OpenPsfPlugin\n");
}

OpenPsfPlugin::~OpenPsfPlugin()
{
	trace("~OpenPsfPlugin\n");
	close();
	trace("/~OpenPsfPlugin\n");
}

bool OpenPsfPlugin::open(const char* libFileName)
{
	trace("OpenPsfPlugin::open %s\n", libFileName);
	close();
	this->handle = LoadLibraryA(libFileName);

	if (nullptr != this->handle)
	{
		FARPROC procAddress = GetProcAddress(this->handle, "get_openpsf");

		if (nullptr != procAddress)
		{
			get_openpsf f = (get_openpsf)procAddress;
			mod = f();
			trace("/OpenPsfPlugin::open ok\n");
			return true;
		}

		error("couldn't load function get_openpsf in external library (error %u)\n", GetLastError());

		close();
	}
	else {
		error("couldn't load external library (error %u)\n", GetLastError());
	}

	trace("/OpenPsfPlugin::open error\n");
	return false;
}

void OpenPsfPlugin::close()
{
	trace("OpenPsfPlugin::close\n");
	if (nullptr != this->handle) {
		FreeLibrary(this->handle);
		this->handle = nullptr;
	}
	trace("/OpenPsfPlugin::close\n");
}
