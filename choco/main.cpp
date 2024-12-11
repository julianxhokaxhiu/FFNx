/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2020 Marcin Gomulak                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#include <windows.h>
#include <Shlwapi.h>
#include <ShlObj_core.h>

void get_userdata_path(PCHAR buffer, size_t bufSize, bool isSavegameFile)
{
	PWSTR outPath = NULL;

	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &outPath);

	if (SUCCEEDED(hr))
	{
		wcstombs(buffer, outPath, bufSize);

		CoTaskMemFree(outPath);
		PathAppendA(buffer, R"(Square Enix\FINAL FANTASY VIII Steam)");

		if (isSavegameFile)
		{
      // Search for the first "user_" match in the game path
      CHAR searchPath[MAX_PATH];
      WIN32_FIND_DATA pathFound;
      HANDLE hFind;

      strcpy(searchPath, buffer);
      strcat(searchPath, R"(\user_*)");
      if (hFind = FindFirstFileA(searchPath, &pathFound))
      {
        PathAppendA(buffer, pathFound.cFileName);
        FindClose(hFind);
      }
		}
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  return TRUE;
}

__declspec(dllexport) HANDLE __stdcall dotemuCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
  HANDLE ret = INVALID_HANDLE_VALUE;

  if (StrStrIA(lpFileName, R"(SAVE\)") != NULL) // SAVE\SLOTX\saveN or save\chocorpg
	{
		CHAR newPath[MAX_PATH]{ 0 };
		CHAR saveFileName[50]{ 0 };

		// Search for the next character pointer after "SAVE\"
		const char* pos = StrStrIA(lpFileName, R"(SAVE\)") + 5;
		strcpy(saveFileName, pos);
		_strlwr(saveFileName);
		char* posSeparator = strstr(saveFileName, R"(\)");
		if (posSeparator != NULL)
		{
			*posSeparator = '_';
		}
		strcat(saveFileName, R"(.ff8)");

		get_userdata_path(newPath, sizeof(newPath), true);
		PathAppendA(newPath, saveFileName);

		ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
  else
  {
    ret = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
  }

  return ret;
}

__declspec(dllexport) HANDLE __stdcall dotemuCreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName)
{
  return CreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
}
