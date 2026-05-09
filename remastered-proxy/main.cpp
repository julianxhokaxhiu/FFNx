/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2026 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2020 Marcin Gomulak                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include <filesystem>
#include <windows.h>
#include <winuser.h>

const char processes[][32]{
    "FF8.ffnx",
    "FF8.exe"
};
const int numProcesses = sizeof(processes) / sizeof(processes[0]);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    return TRUE;
}

__declspec(dllexport) void __stdcall runGame()
{
    int process_to_start = -1;

    for (int i = 0; i < numProcesses; i++)
    {
        if (std::filesystem::exists(processes[i]))
        {
            process_to_start = i;
        }
    }

    if (process_to_start < 0)
    {
        MessageBoxA(NULL, "FF8.ffnx/FF8.exe not found", "Error", MB_ICONERROR | MB_OK);
    }

    // Initialize the process start information
    STARTUPINFOA si = STARTUPINFOA();
    PROCESS_INFORMATION pi = PROCESS_INFORMATION();
    si.cb = sizeof(si);

    // Start the process
    if (!CreateProcessA(processes[process_to_start], NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
    {
        MessageBoxA(NULL, "Something went wrong while launching the game.", "Error", MB_ICONERROR | MB_OK);
        return;
    }

    // Wait for the process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return;
}
