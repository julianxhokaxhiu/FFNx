/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 myst6re                                            //
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
#include "utils.h"

#include "globals.h"

#include <sys/stat.h>
#include <filesystem>

bool fileExists(const char *filename)
{
    struct stat dummy;

    // Use stat to keep compatibility with 7th Heaven
    return stat(filename, &dummy) == 0;
}

bool dirExists(const char *dirname)
{
    struct stat dummy;

    // Use stat to keep compatibility with 7th Heaven
    return stat(dirname, &dummy) == 0;
}

std::string getCopyrightInfoFromExe(const std::string& filePath)
{
    // Get the size of the version information
    DWORD handle = 0;
    DWORD versionInfoSize = GetFileVersionInfoSize(filePath.c_str(), &handle);
    if (versionInfoSize == 0) return "Failed to get version info size.";

    // Allocate memory to hold version information
    std::vector<char> versionData(versionInfoSize);
    if (!GetFileVersionInfo(filePath.c_str(), handle, versionInfoSize, versionData.data())) return "Failed to get version information.";

    // Query the translation table to locate the language and code page
    struct LANGANDCODEPAGE {
        WORD language;
        WORD codePage;
    } *translation = nullptr;

    UINT translationSize = 0;
    if (!VerQueryValue(versionData.data(), "\\VarFileInfo\\Translation", (LPVOID*)&translation, &translationSize)) return "Failed to query translation information.";

    if (translationSize == 0) return "No translation information available.";

    // Use the first language and code page in the translation table
    char subBlock[50];
    snprintf(subBlock, sizeof(subBlock), "\\StringFileInfo\\%04x%04x\\LegalCopyright", translation[0].language, translation[0].codePage);

    // Query the copyright information
    char* copyrightInfo = nullptr;
    UINT infoSize = 0;
    if (!VerQueryValue(versionData.data(), subBlock, (LPVOID*)&copyrightInfo, &infoSize) || infoSize == 0) return "Copyright information not found.";

    // Return the copyright information as a std::string
    return std::string(copyrightInfo, infoSize);
}
