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

#pragma once

#include <windows.h>

#include "../../log.h"

#include "include/openpsf.h"

#define UNUSED_PARAM(x) (void)x;

class OpenPsfPlugin {
private:
	HINSTANCE handle;
	OPENPSF* mod;
	OpenPsfPlugin(const OpenPsfPlugin& copy);
protected:
	inline HINSTANCE getHandle() const {
		return handle;
	}
public:
	OpenPsfPlugin();
	virtual ~OpenPsfPlugin();
	bool open(const char* libFileName);
	void close();
	inline OPENPSF* getModule() const {
		return mod;
	}
};
