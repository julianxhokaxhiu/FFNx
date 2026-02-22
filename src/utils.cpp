/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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
#include "log.h"

#include <sys/stat.h>
#include <filesystem>
#include <Softpub.h>
#include <wintrust.h>

#include <cryptopp/md5.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>

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

std::wstring GetErrorMessage(unsigned long errorCode)
{
    LPWSTR messageBuffer = nullptr;

    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&messageBuffer,
        0,
        NULL
    );

    std::wstring message = messageBuffer ? messageBuffer : L"Unknown error";
    LocalFree(messageBuffer);
    return message;
}

bool isFileSigned(const char* dllPath)
{
    WINTRUST_FILE_INFO fileInfo = {};
    WINTRUST_DATA trustData = {};
    WINTRUST_SIGNATURE_SETTINGS signatureSettings = {};

    // Open the file with proper sharing flags
    fileInfo.cbStruct = sizeof(WINTRUST_FILE_INFO);
    fileInfo.pcwszFilePath = NULL;
    fileInfo.hFile = CreateFileA(dllPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    fileInfo.pgKnownSubject = NULL;

    GUID actionID = WINTRUST_ACTION_GENERIC_VERIFY_V2;

    trustData.cbStruct = sizeof(WINTRUST_DATA);
    trustData.dwUIChoice = WTD_UI_NONE;
    trustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    trustData.dwUnionChoice = WTD_CHOICE_FILE;
    trustData.pFile = &fileInfo;
    trustData.dwStateAction = WTD_STATEACTION_VERIFY;
    trustData.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL;

    LONG status = WinVerifyTrust(NULL, &actionID, &trustData);

    if (status != ERROR_SUCCESS) ffnx_error("Unable to verify '%ls': %ls", dllPath, GetErrorMessage(status));

    trustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(NULL, &actionID, &trustData);

    CloseHandle(fileInfo.hFile);

    return status == ERROR_SUCCESS;
}

std::string sha1_file(const char *filename)
{
    CryptoPP::SHA1 hash;
    std::string digest;

    CryptoPP::FileSource(filename, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(digest), false)));

    return digest;
}

std::string md5_hash(const unsigned char* data, size_t length)
{
    using namespace CryptoPP;

    std::string digest;

    MD5 hash;
    StringSource(data, length, true,
        new HashFilter(hash,
            new HexEncoder(
                new StringSink(digest), false)));

    return digest;
}
