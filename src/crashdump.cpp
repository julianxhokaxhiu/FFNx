/* 
 * FFNx - Complete OpenGL replacement of the Direct3D renderer used in 
 * the original ports of Final Fantasy VII and Final Fantasy VIII for the PC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * crashdump.c - crash dump & emergency save functionality
 */

#include "crashdump.h"

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

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS *ep)
{
	static uint had_exception = false;
	char savefilePath[4096];
	uint save;

	char crashdmpPath[260]{ 0 };

	// give up if we crash again inside the exception handler (this function)
	if(had_exception)
	{
		unexpected("ExceptionHandler: crash while running another ExceptionHandler. Exiting.");
		SetUnhandledExceptionFilter(0);
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	trace("*** Exception 0x%x, address 0x%x ***\n", ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	FFNxStackWalker sw;
	sw.ShowCallstack(
		GetCurrentThread(),
		ep->ContextRecord
	);

	had_exception = true;

	// show cursor in case it was hidden
	ShowCursor(true);

	if (create_crash_dump)
	{
		if (steam_edition) get_userdata_path(crashdmpPath, sizeof(crashdmpPath), false);
		PathAppendA(crashdmpPath, "crash.dmp");

		HANDLE file = CreateFile(crashdmpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
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
			wchar_t buf[256];

			FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
				buf, (sizeof(buf) / sizeof(wchar_t)), NULL);

			trace("MiniDumpWriteDump failed with error: %ls\n", buf);
		}
	}


	if(!ff8)
	{
		if (steam_edition)
		{
			get_userdata_path(savefilePath, sizeof(savefilePath), false);
			PathAppendA(savefilePath, "crash.ff7");
		}
		else
		{
			PathAppendA(savefilePath, "save/crash.ff7");
		}

		// try to dump the current savemap from memory
		// the savemap could be old, inconsistent or corrupted at this point
		// avoid playing from an emergency save if at all possible!
		if(save)
		{
			FILE *f = fopen(savefilePath, "wb");
			uint magic = 0x6277371;
			uint bitmask = 1;
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
	}

	error("Unhandled Exception. See dumped information above.\n");

	if (!ff8)
	{
		char msg[1024]{ 0 };

		sprintf(msg, "Oops! Something very bad happened.\n\nWrote emergency save to %s dir.\n\nPlease provide a copy of those files along with FFNx.LOG when reporting this error at https://github.com/julianxhokaxhiu/FFNx/issues.\n", savefilePath);

		MessageBoxA(0, msg, "Error", MB_OK);

		save = true;
	}
	else
	{
		MessageBoxA(0, "Oops! Something very bad happened.\n\nPlease provide a copy of FFNx.LOG when reporting this error at https://github.com/julianxhokaxhiu/FFNx/issues.\n", "Error", MB_OK);

		save = false;
	}

	// let OS handle the crash
	SetUnhandledExceptionFilter(0);
	return EXCEPTION_CONTINUE_EXECUTION;
}
