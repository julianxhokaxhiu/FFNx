#include "metadata.h"

Metadata metadataPatcher;

// PRIVATE
void Metadata::updateFF7()
{
    char currentSave[260]{ 0 };
    BYTE dataBuffer[64 * 1024 + 8];
    int dataSize = 0;

    // Hash existing save files
    for (uint idx = 0; idx < 10; idx++)
    {
        dataSize = userID.length();
        memset(dataBuffer, 0, sizeof(dataBuffer));
        strcpy(currentSave, savePath);

        // Append save file name
        sprintf(currentSave + strlen(currentSave), R"(\save%02i.ff7)", idx);

        if (_access(currentSave, 0) == 0)
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
    // TODO
}

// PUBLIC
void Metadata::apply()
{
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