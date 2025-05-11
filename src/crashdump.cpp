/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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
#include <commctrl.h>

#include "audio.h"

#include "crashdump.h"
#include "utils.h"

// FF7 save file checksum, original by dziugo
int ff7_checksum(void* qw)
{
	int i = 0, t, d;
	long r = 0xFFFF, len = 4336;
	long pbit = 0x8000;
	char* b = (char*)qw;

	while(len--)
	{
		t = b[i++];
		r ^= t << 8;

		for(d = 0; d < 8; d++)
		{
			if(r & pbit) r = (r << 1) ^ 0x1021;
			else r <<= 1;
		}

		r &= (1 << 16) - 1;
	}

	return (r ^ 0xFFFF) & 0xFFFF;
}

static const char save_name[] = "\x25" "MERGENCY" "\x00\x33" "AVE" "\xFF";

HRESULT CALLBACK TaskDialogCallbackProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData) {
	if (msg == TDN_HYPERLINK_CLICKED) {
		LPCWSTR url = (LPCWSTR)lParam;
		ShellExecuteW(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
	}
	return S_OK;
}

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS *ep)
{
	static uint32_t had_exception = false;
	char filePath[260]{ 0 };

	// give up if we crash again inside the exception handler (this function)
	if(had_exception)
	{
		ffnx_unexpected("ExceptionHandler: crash while running another ExceptionHandler. Exiting.");
		SetUnhandledExceptionFilter(0);
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	ffnx_trace("*** Exception 0x%x, address 0x%x ***\n", ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	FFNxStackWalker sw;
	sw.ShowCallstack(
		GetCurrentThread(),
		ep->ContextRecord
	);

	had_exception = true;

	// show cursor in case it was hidden
	while (ShowCursor(true) < 0);

	if (create_crash_dump)
	{
		if (steam_edition) get_userdata_path(filePath, sizeof(filePath), false);
		PathAppendA(filePath, "crash.dmp");

		HANDLE file = CreateFile(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		HANDLE proc = GetCurrentProcess();
		DWORD procid = GetCurrentProcessId();
		MINIDUMP_EXCEPTION_INFORMATION mdei;

		CONTEXT c;
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());;
		memset(&c, 0, sizeof(c));
		c.ContextFlags = CONTEXT_FULL;
		GetThreadContext(hThread, &c);

		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = ep;
		mdei.ExceptionPointers->ContextRecord = &c;
		mdei.ClientPointers = true;

		if (!MiniDumpWriteDump(
			proc,
			procid,
			file,
			(MINIDUMP_TYPE)(MiniDumpWithFullMemory |
				MiniDumpWithFullMemoryInfo |
				MiniDumpWithHandleData |
				MiniDumpWithUnloadedModules |
				MiniDumpWithThreadInfo),
			&mdei, NULL, NULL)) {
			ffnx_trace("MiniDumpWriteDump failed with error: %ls\n", GetErrorMessage(GetLastError()));
		}
	}


	if(!ff8)
	{
		memset(filePath, 0, sizeof(filePath));

		if (steam_edition)
		{
			get_userdata_path(filePath, sizeof(filePath), false);
			PathAppendA(filePath, "crash.ff7");
		}
		else
		{
			PathAppendA(filePath, "save/crash.ff7");
		}

		// try to dump the current savemap from memory
		// the savemap could be old, inconsistent or corrupted at this point
		// avoid playing from an emergency save if at all possible!
		FILE *f = fopen(filePath, "wb");
		uint32_t magic = 0x6277371;
		uint32_t bitmask = 1;
		struct savemap dummy[14];

		memset(dummy, 0, sizeof(dummy));

		memcpy(ff7_externals.savemap->preview_location, save_name, sizeof(save_name));

		ff7_externals.savemap->checksum = ff7_checksum(&(ff7_externals.savemap->preview_level));

		fwrite(&magic, 4, 1, f);
		fwrite("", 1, 1, f);
		fwrite(&bitmask, 4, 1, f);
		fwrite(ff7_externals.savemap, sizeof(*ff7_externals.savemap), 1, f);
		fwrite(dummy, sizeof(dummy), 1, f);
		fclose(f);
	}

	ffnx_error("Unhandled Exception. See dumped information above.\n");

	TASKDIALOGCONFIG config = { sizeof(config) };
	config.hwndParent = gameHwnd;
	config.dwFlags = TDF_ENABLE_HYPERLINKS;
	config.pszWindowTitle = L"Something went wrong";
	config.pszMainInstruction = L"Game crashed :(";
	config.pszContent = L"Something unexpected happened and unfortunately the game crashed.\n\nFeel free to visit <a href=\"https://github.com/julianxhokaxhiu/FFNx/blob/master/docs/faq.md\">this link</a> to know about further next steps you can take.";
	config.pszMainIcon = TD_ERROR_ICON;
	config.pfCallback = TaskDialogCallbackProc;

	TaskDialogIndirect(&config, NULL, NULL, NULL);

	// Cleanup the audio device
	nxAudioEngine.cleanup();

	// let OS handle the crash
	SetUnhandledExceptionFilter(0);
	return EXCEPTION_CONTINUE_EXECUTION;
}
