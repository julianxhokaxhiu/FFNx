/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#include <filesystem>
#include <io.h>
#include "log.h"

#include "redirect.h"

int attempt_redirection(char* in, char* out, size_t size, bool wantsSteamPath)
{
	std::string newIn(in);

	std::transform(newIn.begin(), newIn.end(), newIn.begin(), ::tolower);

	bool isSavegame = strstr(newIn.data(), ".ff7") != NULL;
	bool isCacheFile = strstr(newIn.data(), ".p") != NULL;

	if (wantsSteamPath && _access(in, 0) == -1)
	{
		if (
			strcmp(newIn.data(), "scene.bin") == 0 ||
			strcmp(newIn.data(), "camdat0.bin") == 0 ||
			strcmp(newIn.data(), "camdat1.bin") == 0 ||
			strcmp(newIn.data(), "camdat2.bin") == 0 ||
			strcmp(newIn.data(), "co.bin") == 0
			)
		{
			get_data_lang_path(out);
			PathAppendA(out, R"(battle)");
			PathAppendA(out, newIn.data());

			if (_access(out, 0) == -1)
				return 1;
		}
		else
		{
			const char* pos = strstr(newIn.data(), "data");

			if (pos != NULL)
			{
				pos += 5;
			}
			else
			{
				// Search for the last '\' character and get a pointer to the next char
				pos = strrchr(in, 92);

				if (pos != NULL) pos += 1;
			}

			get_data_lang_path(out);
			if (pos != NULL) PathAppendA(out, pos);

			if ((_access(out, 0) == -1 || pos == NULL))
			{
				// If steam edition, do one more try in the user data path
				if (steam_edition) get_userdata_path(out, size, isSavegame);
				else strcpy(out, "");

				if (isCacheFile)
				{
					if (steam_edition)
					{
						PathAppendA(out, "cache");
						std::filesystem::create_directories(out);
					}
					PathAppendA(out, newIn.data());
				}
				else
				{
					if (isSavegame)
					{
						pos = strrchr(newIn.data(), 47) + 1;
						PathAppendA(out, pos);
					}
					else
					{
						PathAppendA(out, pos);
						if (_access(out, 0) == -1)
							return 1;
					}
				}
			}
		}

		if (trace_all || trace_files) ffnx_trace("Redirected: %s -> %s\n", newIn.data(), out);

		return 0;
	}
	else
	{
		if (isSavegame && !save_path.empty())
		{
			char* pos = strrchr(newIn.data(), 47);

			// This case may happen if we have already redirected the path
			if (strstr(newIn.data(), "save/save") == NULL)
			{
				// Allow the game to continue by forward the redirected path again
				strcpy(out, newIn.data());
			}
			// This one means we still have to redirect it
			else if (pos != NULL)
			{
				strcpy(out, basedir);
				PathAppendA(out, save_path.c_str());
				PathAppendA(out, ++pos);

				if (trace_all || trace_files) ffnx_trace("Redirected: %s -> %s\n", newIn.data(), out);
			}

			// Always return as found in order to allow non existing save files to be saved under the new redirected path
			return 0;
		}
		else if (!isCacheFile)
		{
			const char* pos = strstr(newIn.data(), "data");

			if (pos != NULL)
			{
				pos += 5;
			}
			else
			{
				// Search for the last '\' character and get a pointer to the next char
				pos = strrchr(newIn.data(), 92);

				if (pos != NULL) pos += 1;
			}

			strcpy(out, basedir);
			PathAppendA(out, override_path.c_str());
			if (pos != NULL)
				PathAppendA(out, pos);
			else
			{
				PathAppendA(out, R"(battle)");
				PathAppendA(out, newIn.data());
			}

			if (_access(out, 0) == -1)
				return -1;

			if (trace_all || trace_files) ffnx_trace("Redirected: %s -> %s\n", newIn.data(), out);

			return 0;
		}
	}

	return -1;
}

int redirect_path_with_override(char* in, char* out, size_t out_size)
{
  char _newFilename[260]{ 0 };

  // Attempt another redirection based on Steam/eStore logic
  int redirect_status = attempt_redirection(in, _newFilename, sizeof(_newFilename), steam_edition || estore_edition);

  // File was found
  if (redirect_status == 0)
  {
    // Attemp override redirection on top of Steam/eStore new path
    redirect_status = attempt_redirection(_newFilename, out, out_size);

    if (redirect_status == -1)
    {
      redirect_status = 0;

      // If was not found, use original redirected path
      strcpy(out, _newFilename);
    }
  }
  // File was not found
  else if (redirect_status == -1)
  {
    // Attemp override redirection on top of classic path
    redirect_status = attempt_redirection(in, out, out_size);

    if (redirect_status == -1)
    {
      redirect_status = 0;

      // If was not found, use original filename
      strcpy(out, in);
    }
  }

  return redirect_status;
}