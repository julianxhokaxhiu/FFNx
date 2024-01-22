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

#include "metadata.h"

Metadata metadataPatcher;

// PRIVATE
void Metadata::updateFF7()
{
    char currentSave[260]{ 0 };
    BYTE dataBuffer[64 * 1024 + 8];
    int dataSize = 0;

    // Hash existing save files
    for (uint32_t idx = 0; idx < 10; idx++)
    {
        dataSize = userID.length();
        memset(dataBuffer, 0, sizeof(dataBuffer));
        strcpy(currentSave, savePath);

        // Append save file name
        sprintf(currentSave + strlen(currentSave), R"(\save%02i.ff7)", idx);

        if (fileExists(currentSave))
        {
            FILE* file = fopen(currentSave, "rb");

            fseek(file, 0, SEEK_END);
            int fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);
            fread(dataBuffer, 1, fileSize, file);
            fclose(file);

            memcpy(dataBuffer + fileSize, userID.data(), userID.length());

            dataSize += fileSize;
        }
        else
        {
            memcpy(dataBuffer, userID.data(), userID.length());
        }

        // Hash to MD5
        MD5 md5(dataBuffer, dataSize);

        saveHash.push_back(md5.hexdigest());
    }

    // Update metadata
    int saveNumber;

    for (pugi::xml_node gamestatus : doc.children())
    {
        for (pugi::xml_node savefiles : gamestatus.children())
        {
            if (strcmp(savefiles.name(), "savefiles") == 0)
            {
                saveNumber = std::atoi(savefiles.attribute("block").value()) - 1;
            }

            for (pugi::xml_node child : savefiles.children())
            {
                if (strcmp(child.name(), "timestamp") == 0)
                {
                    child.text().set(now.data());
                }

                if (strcmp(child.name(), "signature") == 0)
                {
                    child.text().set(saveHash[saveNumber].data());
                }
            }
        }
    }
}

void Metadata::updateFF8()
{
    char currentSave[260]{ 0 };
    BYTE dataBuffer[8 * 1024 + 8];
    int dataSize = 0;
    int slotNumber = 1;

    // Hash existing save files
    for (uint32_t idx = 0; idx < 61; idx++)
    {
        dataSize = userID.length();
        memset(dataBuffer, 0, sizeof(dataBuffer));
        strcpy(currentSave, savePath);

        if (idx == 30) slotNumber = 2;

        // Append save file name
        if (idx == 60) {
            strcpy(currentSave + strlen(currentSave), "\\chocorpg.ff8");
        } else {
            sprintf(currentSave + strlen(currentSave), R"(\slot%d_save%02i.ff8)", slotNumber, (slotNumber == 2 ? idx - 30 : idx) + 1);
        }

        if (fileExists(currentSave))
        {
            FILE* file = fopen(currentSave, "rb");

            fseek(file, 0, SEEK_END);
            int fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);
            fread(dataBuffer, 1, fileSize, file);
            fclose(file);

            memcpy(dataBuffer + fileSize, userID.data(), userID.length());

            dataSize += fileSize;
        }
        else
        {
            memcpy(dataBuffer, userID.data(), userID.length());
        }

        // Hash to MD5
        MD5 md5(dataBuffer, dataSize);

        saveHash.push_back(md5.hexdigest());
    }

    // Update metadata
    int saveNumber;

    for (pugi::xml_node gamestatus : doc.children())
    {
        for (pugi::xml_node savefile : gamestatus.children())
        {
            if (strcmp(savefile.name(), "savefile") == 0)
            {
                if (strcmp(savefile.attribute("type").value(), "choco") == 0)
                {
                    saveNumber = 30;
                    slotNumber = 2;
                }
                else
                {
                    saveNumber = std::atoi(savefile.attribute("num").value()) - 1;
                    slotNumber = std::atoi(savefile.attribute("slot").value());
                }
            }

            for (pugi::xml_node child : savefile.children())
            {
                if (strcmp(child.name(), "timestamp") == 0)
                {
                    child.text().set(now.data());
                }

                if (strcmp(child.name(), "signature") == 0)
                {
                    child.text().set(saveHash[saveNumber + (slotNumber == 2 ? 30 : 0)].data());
                }
            }
        }
    }
}

// PUBLIC
void Metadata::apply()
{
    ffnx_trace("Applying required metadata.xml patch to preserve save files.\n");

    char metadataPath[260]{ 0 };
    std::chrono::milliseconds nowMS = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
        );

    now = std::to_string(nowMS.count());

    // Get Save Path
    get_userdata_path(savePath, sizeof(savePath), true);

    // Get Metadata Path
    strcpy(metadataPath, savePath);
    PathAppendA(metadataPath, "metadata.xml");

    // Save userID
    userID.assign(strrchr(savePath, '_') + 1);

    // Load Metadata
    doc.load_file(metadataPath);

    // Update Metadata
    if (ff8)
        updateFF8();
    else
        updateFF7();

    // Save Metadata
    doc.save_file(metadataPath);
}
