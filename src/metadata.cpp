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

#include <shlwapi.h>
#include <chrono>
#include <cerrno>
#include <vector>

#include "metadata.h"
#include "log.h"
#include "utils.h"

Metadata metadataPatcher;

// PRIVATE
bool Metadata::loadXml()
{
    ffnx_trace("Metadata: loading metadata.xml\n");

    auto result = doc.load_file(savePath);
    bool missing = false;
    if (!result && result.status == pugi::status_file_not_found)
    {
        DWORD attributes = GetFileAttributesA(savePath);
        DWORD error = GetLastError();
        missing = attributes == INVALID_FILE_ATTRIBUTES &&
            (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND);
    }
    if (!result && !missing)
    {
        ffnx_error("Metadata: could not load %s: %s\n", savePath, result.description());
        return false;
    }

    if (missing)
    {
        doc.reset();
        auto declaration = doc.append_child(pugi::node_declaration);
        declaration.append_attribute("version") = "1.0";
        declaration.append_attribute("encoding") = "UTF-8";
        doc.append_child("gamestatus");
    }
    else if (!doc.child("gamestatus"))
    {
        ffnx_error("Metadata: missing gamestatus in %s\n", savePath);
        return false;
    }

    return true;
}

void Metadata::saveXml()
{
    ffnx_trace("Metadata: saving metadata.xml\n");

    std::string temporaryPath = std::string(savePath) + ".tmp";
    if (!doc.save_file(temporaryPath.c_str()))
    {
        ffnx_error("Metadata: could not write %s\n", temporaryPath.c_str());
        DeleteFileA(temporaryPath.c_str());
        return;
    }

    if (!MoveFileExA(temporaryPath.c_str(), savePath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        ffnx_error("Metadata: could not replace %s (error %lu)\n", savePath, GetLastError());
        DeleteFileA(temporaryPath.c_str());
    }
}

bool Metadata::updateSignature(pugi::xml_node savefile, const char* filename, bool current)
{
    std::string path = std::string(userPath) + "\\" + filename;
    std::vector<unsigned char> data;
    FILE* file = fopen(path.c_str(), "rb");
    bool exists = file != nullptr;
    if (file)
    {
        ffnx_trace("Metadata: calculating hash for %s\n", path.c_str());
        if (fseek(file, 0, SEEK_END) != 0)
        {
            ffnx_error("Metadata: could not seek %s\n", path.c_str());
            fclose(file);
            return false;
        }
        long size = ftell(file);
        if (size < 0 || fseek(file, 0, SEEK_SET) != 0)
        {
            ffnx_error("Metadata: could not determine size of %s\n", path.c_str());
            fclose(file);
            return false;
        }
        data.resize(size);
        bool read = fread(data.data(), 1, data.size(), file) == data.size() && !ferror(file);
        fclose(file);
        if (!read)
        {
            ffnx_error("Metadata: could not read %s\n", path.c_str());
            return false;
        }
    }
    else if (errno != ENOENT)
    {
        ffnx_error("Metadata: could not open %s (error %d)\n", path.c_str(), errno);
        return false;
    }

    // Missing saves are signed with just the user ID, not an empty signature.
    data.insert(data.end(), userID.begin(), userID.end());
    std::string md5 = md5_hash(data.data(), data.size());
    auto signature = savefile.child("signature");
    if (!signature) signature = savefile.append_child("signature");
    signature.text().set(md5.c_str());

    if (exists || current)
    {
        for (auto timestamp : savefile.children("timestamp"))
            timestamp.text().set(now.c_str());
    }
    return true;
}

void Metadata::calcNow()
{
    std::chrono::milliseconds nowMS = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );

    now = std::to_string(nowMS.count());
}

// PUBLIC
void Metadata::init()
{
    ffnx_trace("Metadata: Initializing manager.\n");

    // Get Save Path
    get_userdata_path(userPath, sizeof(userPath), true);

    // Get Metadata Path
    strcpy(savePath, userPath);
    PathAppendA(savePath, "metadata.xml");

    // Save userID
    userID.assign(strrchr(userPath, '_') + 1);
}

void Metadata::updateFF7(uint8_t save)
{
    if (save >= 10)
    {
        ffnx_error("Metadata: invalid FF7 save number %u\n", save);
        return;
    }
    if (!loadXml()) return;

    calcNow();
    auto gamestatus = doc.child("gamestatus");
    for (int block = 1; block <= 10; ++block)
    {
        std::string blockNumber = std::to_string(block);
        auto savefiles = gamestatus.find_child_by_attribute("savefiles", "block", blockNumber.c_str());
        if (!savefiles)
        {
            savefiles = gamestatus.append_child("savefiles");
            savefiles.append_attribute("block") = block;
        }
        for (int slot = 1; slot <= 15; ++slot)
        {
            std::string slotNumber = std::to_string(slot);
            if (!savefiles.find_child_by_attribute("timestamp", "slot", slotNumber.c_str()))
                savefiles.append_child("timestamp").append_attribute("slot") = slot;
        }
        if (block == save + 1 || !savefiles.child("signature") || savefiles.child("signature").text().empty())
        {
            char filename[32];
            sprintf(filename, "save%02i.ff7", block - 1);
            if (!updateSignature(savefiles, filename, block == save + 1)) return;
        }
    }

    saveXml();
}

void Metadata::updateFF8(uint8_t slot, uint8_t save)
{
    if (slot == 0 || (slot <= 2 && (save == 0 || save > 30)))
    {
        ffnx_error("Metadata: invalid FF8 slot %u save number %u\n", slot, save);
        return;
    }
    if (!loadXml()) return;

    calcNow();
    auto gamestatus = doc.child("gamestatus");
    for (int card = 1; card <= 2; ++card)
    {
        for (int number = 1; number <= 30; ++number)
        {
            auto savefile = gamestatus.find_child([card, number](pugi::xml_node node)
            {
                return strcmp(node.name(), "savefile") == 0 &&
                    strcmp(node.attribute("type").value(), "ff8") == 0 &&
                    node.attribute("slot").as_int() == card && node.attribute("num").as_int() == number;
            });
            if (!savefile)
            {
                savefile = gamestatus.append_child("savefile");
                savefile.append_attribute("num") = number;
                savefile.append_attribute("type") = "ff8";
                savefile.append_attribute("slot") = card;
            }
            if (!savefile.child("timestamp")) savefile.append_child("timestamp");
            bool current = card == slot && number == save;
            if (current || !savefile.child("signature") || savefile.child("signature").text().empty())
            {
                char filename[32];
                sprintf(filename, "slot%d_save%02i.ff8", card, number);
                if (!updateSignature(savefile, filename, current)) return;
            }
        }
    }

    auto choco = gamestatus.find_child_by_attribute("savefile", "type", "choco");
    if (!choco)
    {
        choco = gamestatus.append_child("savefile");
        choco.append_attribute("type") = "choco";
    }
    if (!choco.child("timestamp")) choco.append_child("timestamp");
    if (slot > 2 || !choco.child("signature") || choco.child("signature").text().empty())
    {
        if (!updateSignature(choco, "chocorpg.ff8", slot > 2)) return;
    }

    saveXml();
}
