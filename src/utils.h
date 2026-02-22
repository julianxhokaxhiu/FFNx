/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include <regex>
#include <string>
#include <vector>
#include <chrono>
#include <random>

// Get the size of a vector in bytes
template<typename T>
size_t vectorSizeOf(const typename std::vector<T>& vec)
{
    return sizeof(T) * vec.size();
}

// trim from start (in place)
inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
        }));
}

// trim from end (in place)
inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}

inline bool contains(std::string const& value, std::string const& token)
{
    return value.find(token) != std::string::npos;
}

inline std::vector<std::string> split(const std::string& string, const std::string& regex)
{
    std::vector<std::string> result;
    std::string safeString(string);

    trim(safeString);

    const std::regex rgx(regex);
    std::sregex_token_iterator iter(safeString.begin(), safeString.end(), rgx, -1);

    for (std::sregex_token_iterator end; iter != end; ++iter)
    {
        result.push_back(iter->str());
    }

    return result;
}

inline bool starts_with(std::string const& value, std::string const& starting)
{
    if (starting.size() > value.size()) return false;
    return value.rfind(starting, 0) == 0;
}

inline bool ends_with(std::string const& value, std::string const& ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

inline bool replaceOnce(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);

    if (start_pos == std::string::npos)
        return false;

    str.replace(start_pos, from.length(), to);

    return true;
}

inline void replaceAll(std::string& str, const char from, const char to)
{
    std::replace(str.begin(), str.end(), from, to);
}

// Based on https://stackoverflow.com/a/13446015
inline int getRandomInt(int min, int max)
{
    std::random_device rd;
    std::mt19937::result_type seed = rd() ^ (
        (std::mt19937::result_type)
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count() +
        (std::mt19937::result_type)
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count()
    );

    std::mt19937 gen(seed);
    std::uniform_int_distribution<unsigned> distrib(min, max);

    return distrib(gen);
}

inline std::chrono::time_point<std::chrono::high_resolution_clock> highResolutionNow()
{
    return std::chrono::high_resolution_clock::now();
}

inline long double elapsedMicroseconds(std::chrono::time_point<std::chrono::high_resolution_clock> startTime)
{
    return std::chrono::duration<long double, std::micro>(highResolutionNow() - startTime).count();
}

bool fileExists(const char *filename);
bool dirExists(const char *dirname);
std::string getCopyrightInfoFromExe(const std::string& filePath);
std::wstring GetErrorMessage(unsigned long errorCode);
bool isFileSigned(const char* dllPath);
std::string sha1_file(const char* filename);
std::string md5_hash(const unsigned char* data, size_t length);
