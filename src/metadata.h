#pragma once

#include <chrono>
#include <cstring>
#include <io.h>
#include <sstream>

#include <pugiconfig.hpp>
#include <pugixml.hpp>

#include "log.h"
#include "md5.h"
#include "common.h"
#include "globals.h"
#include "utils.h"

class Metadata
{
private:
	pugi::xml_document doc;

	std::string now;
	std::string userID;
	char savePath[260]{ 0 };
	std::vector<std::string> saveHash;

	void updateFF7();
	void updateFF8();

public:
	void apply();
};

extern Metadata metadataPatcher;