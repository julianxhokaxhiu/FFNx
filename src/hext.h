#pragma once

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream> 
#include <locale>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "globals.h"
#include "patch.h"

class Hext {
private:
	int inGlobalOffset;
	bool isMultilineComment = false;

	int getAddress(std::string token);
	std::vector<char> getBytes(std::string token);

	bool parseCommands(std::string token);
	bool parseComment(std::string token);
	bool parseGlobalOffset(std::string token);
	bool parseMemoryPermission(std::string token);
	bool parseMemoryPatch(std::string token);

public:
	void apply(std::string filename);
	void apply();
};

extern Hext hextPatcher;
