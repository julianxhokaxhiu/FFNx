#pragma once

#include <regex>
#include <string>

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

inline bool replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);

    if (start_pos == std::string::npos)
        return false;

    str.replace(start_pos, from.length(), to);

    return true;
}
