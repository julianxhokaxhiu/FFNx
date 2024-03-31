/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include <chrono>
#include <cstring>
#include <io.h>
#include <sstream>

#include <pugiconfig.hpp>
#include <pugixml.hpp>

#include "log.h"
#include "md5.h"

class Metadata
{
private:
	pugi::xml_document doc;

	std::string now;
	std::string userID;
	char userPath[260]{ 0 };
	char savePath[260]{ 0 };

	void calcNow();
	void loadXml();
	void saveXml();

public:
	void init();
	void updateFF7(uint8_t save);
	void updateFF8(uint8_t slot, uint8_t save);
};

extern Metadata metadataPatcher;
