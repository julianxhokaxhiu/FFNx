#include "hext.h"

Hext hextPatcher;

// PRIVATE

int Hext::getAddress(std::string& token)
{
    int ret;

    std::vector<std::string> sparts = split(token, "[+-]+");
    std::vector<int> iparts;

    if (ends_with(sparts[0], "^"))
    {
        std::stringstream ss;
        int *ptr = (int*)(std::stoi(sparts[0].substr(0, sparts[0].length() - 1), nullptr, 16) + inGlobalOffset);
        ss << std::hex << *ptr;
        sparts[0] = ss.str();
    }
    
    for (auto &part : sparts)
    {
        iparts.push_back(
            std::stoi(part, nullptr, 16)
        );
    }

    ret = iparts[0];

    if (contains(token, "+"))
    {
        ret += iparts[1];
    }
    else if (contains(token, "-"))
    {
        ret -= iparts[1];
    }

    return ret + inGlobalOffset;
}

std::vector<char> Hext::getBytes(std::string& token)
{
    std::vector<char> ret;

    if (contains(token, ":"))
    {
        std::vector<std::string> parts = split(token, "[:]+");
        int count = std::stoi(parts[1], nullptr, 0);
        while (count > 0)
        {
            ret.push_back(std::stoi(parts[0], nullptr, 16));
            count--;
        }
    }
    else
    {
        std::vector<std::string> bytes = split(token, "[\\s,\\t]+");

        for (auto byte : bytes)
        {
            ret.push_back(std::stoi(byte, nullptr, 16));
        }
    }

    return ret;
}

bool Hext::hasCheckpoint(std::string& token)
{
    if (starts_with(token, "!"))
    {
        return true;
    }

    return false;
}

bool Hext::parseCheckpoint(std::string& token, std::string& checkpoint)
{
    if (starts_with(token, "!"))
    {
        if (contains(token, checkpoint))
        {
            return true;
        }
    }

    return false;
}

bool Hext::parseCommands(std::string& token)
{
    if (starts_with(token, ">>"))
    {
        if (ends_with(token, "FF7_CENTER_FIELDS = 1"))
        {
            ff7_center_fields = true;

            return true;
        }
    }

    if (starts_with(token, "<<"))
    {
        replace(token, "<<", "");

        trim(token);
        
        trace("%s\n", token.data());

        return true;
    }

    return false;
}

bool Hext::parseComment(std::string& token)
{
    if (isMultilineComment)
    {
        if (ends_with(token, "}}")) isMultilineComment = false;
        return true;
    }

    if (starts_with(token, "{{"))
    {
        isMultilineComment = true;
        return true;
    }

    if (starts_with(token, "#")) return true;
    if (starts_with(token, "{")) return true;

    return false;
}

bool Hext::parseGlobalOffset(std::string& token)
{
    if (starts_with(token, "+"))
    {
        inGlobalOffset = std::stoi(token.substr(1), nullptr, 16);

        return true;
    }
    else if (starts_with(token, "-"))
    {
        inGlobalOffset = -std::stoi(token.substr(1), nullptr, 16);

        return true;
    }

    return false;
}

bool Hext::parseMemoryPermission(std::string& token)
{
    if (contains(token, ":"))
    {
        DWORD dummy;

        std::vector<std::string> parts = split(token, "[:]+");
        int addr = getAddress(parts[0]);
        int length = std::stoi(parts[1], nullptr, 16);

        VirtualProtect((LPVOID)addr, length, PAGE_EXECUTE_READWRITE, &dummy);

        return true;
    }

    return false;
}

bool Hext::parseMemoryPatch(std::string& token)
{
    if (contains(token, "="))
    {
        DWORD dummy;

        std::vector<std::string> parts = split(token, "[=]+");
        int addr = getAddress(parts[0]);
        std::vector<char> bytes = getBytes(parts[1]);

        memcpy_code(addr, bytes.data(), bytes.size());

        return true;
    }

    return false;
}

// PUBLIC

void Hext::apply(std::string& filename)
{
    std::string line;
    std::ifstream ifs(filename);

    while (std::getline(ifs, line))
    {
        if (line.empty()) continue;

        // Check if delayed, if so it should not be applied
        if (hasCheckpoint(line)) {
            ifs.close();
            return;
        }

        // Check if is a comment
        if (parseComment(line)) continue;

        // Check if is a command
        if (parseCommands(line)) continue;

        // Check if is a global offset
        if (parseGlobalOffset(line)) continue;

        // Check if is a memory permission range
        if (parseMemoryPermission(line)) continue;

        // Check if is a memory patch instruction
        if (parseMemoryPatch(line)) continue;
    }

    ifs.close();

    trace("Applied Hext patch: %s\n", filename.c_str());
}

void Hext::applyDelayed(std::string& filename, std::string& checkpoint)
{
    std::string line;
    std::ifstream ifs(filename);

    bool matchCheckpoint = false;

    while (std::getline(ifs, line))
    {
        if (line.empty()) continue;

        // Check if is a comment
        if (parseComment(line)) continue;

        // Check if is a delayed patch.
        if (parseCheckpoint(line, checkpoint)) {
            matchCheckpoint = true;

            continue;
        }

        if (matchCheckpoint)
        {
            // Check if is a command
            if (parseCommands(line)) continue;

            // Check if is a global offset
            if (parseGlobalOffset(line)) continue;

            // Check if is a memory permission range
            if (parseMemoryPermission(line)) continue;

            // Check if is a memory patch instruction
            if (parseMemoryPatch(line)) continue;
        }
    }

    ifs.close();

    if (matchCheckpoint) trace("Applied Hext patch: %s\n", filename.c_str());
}

void Hext::applyAll(std::string& checkpoint)
{
    if (_access(hext_patching_path, 0) == 0)
    {
        if (!checkpoint.empty())
        {
            for (const auto& entry : std::filesystem::directory_iterator(hext_patching_path))
            {
                if (entry.is_regular_file()) {
                    applyDelayed(entry.path().string(), checkpoint);
                }

                inGlobalOffset = 0;
            }
        }
        else
        {
            for (const auto& entry : std::filesystem::directory_iterator(hext_patching_path))
            {
                if (entry.is_regular_file()) {
                    apply(entry.path().string());
                }

                inGlobalOffset = 0;
            }
        }
    }
}
