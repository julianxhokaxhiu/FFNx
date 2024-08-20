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

/*
	HEXT Specification by DLPB

	See https://forums.qhimm.com/index.php?topic=13574.0

	This implementation may differ from the original specification as new required functionalities will be implemented,
	in order to ease modders life.
*/

#pragma once

#include <string>
#include <vector>

class Hext {
private:
	int inGlobalOffset;
	bool isMultilineComment = false;

	int getAddress(std::string token);
	std::vector<char> getBytes(std::string token);

	bool hasCheckpoint(std::string token);
	bool parseCheckpoint(std::string token, std::string checkpoint);
	bool parseCommands(std::string token);
	bool parseComment(std::string token);
	bool parseGlobalOffset(std::string token);
	bool parseMemoryPermission(std::string token);
	bool parseMemoryPatch(std::string token);

public:
	void apply(std::string filename);
	void applyDelayed(std::string filename, std::string checkpoint);
	void applyAll(std::string checkpoint = std::string());
};

extern Hext hextPatcher;
