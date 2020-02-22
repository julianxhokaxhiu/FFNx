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

#include <windows.h>
#include <stdio.h>
#include <dbghelp.h>

#include "crashdump.h"
#include "globals.h"
#include "types.h"
#include "log.h"

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

// Prints stack trace based on context record
// Via https://stackoverflow.com/a/50208684
void printStack(CONTEXT *ctx)
{
	uint result;
	HANDLE process;
	HANDLE thread;
	HMODULE hModule;

	STACKFRAME stack;
	ULONG frame;
	DWORD64 displacement;

	DWORD disp;
	IMAGEHLP_LINE *line;

	char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
	char name[STACK_MAX_NAME_LENGTH];
	char module[STACK_MAX_NAME_LENGTH];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

	memset(&stack, 0, sizeof(STACKFRAME));

	process = GetCurrentProcess();
	thread = GetCurrentThread();
	displacement = 0;
	stack.AddrPC.Offset = (*ctx).Eip;
	stack.AddrPC.Mode = AddrModeFlat;
	stack.AddrStack.Offset = (*ctx).Esp;
	stack.AddrStack.Mode = AddrModeFlat;
	stack.AddrFrame.Offset = (*ctx).Ebp;
	stack.AddrFrame.Mode = AddrModeFlat;

	SymInitialize(process, NULL, TRUE); //load symbols

	for (frame = 0;; frame++)
	{
		//get next call from stack
		result = StackWalk(
				IMAGE_FILE_MACHINE_I386,
				process,
				thread,
				&stack,
				ctx,
				NULL,
				SymFunctionTableAccess,
				SymGetModuleBase,
				NULL);

		if (!result)
			break;

		//get symbol name for address
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;
		SymFromAddr(process, (ULONG64)stack.AddrPC.Offset, &displacement, pSymbol);

		line = (IMAGEHLP_LINE *)driver_malloc(sizeof(IMAGEHLP_LINE));
		line->SizeOfStruct = sizeof(IMAGEHLP_LINE);

		//try to get line
		if (SymGetLineFromAddr(process, stack.AddrPC.Offset, &disp, line))
		{
			trace("\tat %s in %s: line: %lu: address: 0x%I64x\n", pSymbol->Name, line->FileName, line->LineNumber, pSymbol->Address);
		}
		else
		{
			hModule = NULL;
			lstrcpyA(module, "");
			GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
												(LPCTSTR)(stack.AddrPC.Offset), &hModule);

			//at least print module name
			if (hModule != NULL)
				GetModuleFileNameA(hModule, module, STACK_MAX_NAME_LENGTH);

			trace("in \"%s\"\n", module);

			//failed to get line
			trace("\tat %s, address 0x%I64x\n", pSymbol->Name, pSymbol->Address);
		}

		driver_free(line);
		line = NULL;
	}
}

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS *ep)
{
	static uint had_exception = false;
	char filename[4096];
	uint save;

	// give up if we crash again inside the exception handler (this function)
	if(had_exception)
	{
		unexpected("ExceptionHandler: crash while running another ExceptionHandler. Exiting.");
		SetUnhandledExceptionFilter(0);
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	trace("*** Exception 0x%x, address 0x%x ***\n", ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	printStack(ep->ContextRecord);

	had_exception = true;

	// show cursor in case it was hidden
	ShowCursor(true);

	if(!ff8)
	{
		MessageBoxA(0, "Oops! Something very bad happened.\n\nWrote emergency save to save/crash.ff7 dir.\n\n"
			"Please provide a copy of those files along with FFNx.LOG when reporting this error at https://github.com/julianxhokaxhiu/FFNx/issues.\n", "Error", MB_OK);

		save = true;
	}
	else
	{
		MessageBoxA(0, "Oops! Something very bad happened.\n\n"
			"Please provide a copy of FFNx.LOG when reporting this error at https://github.com/julianxhokaxhiu/FFNx/issues.\n", "Error", MB_OK);

		save = false;
	}

	if (create_crash_dump)
	{
		// save crash dump to game directory
		HANDLE file = CreateFile("crash.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
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
		sprintf(filename, "%s/%s", basedir, "save/crash.ff7");

		// try to dump the current savemap from memory
		// the savemap could be old, inconsistent or corrupted at this point
		// avoid playing from an emergency save if at all possible!
		if(save)
		{
			FILE *f = fopen(filename, "wb");
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

	// let OS handle the crash
	SetUnhandledExceptionFilter(0);
	return EXCEPTION_CONTINUE_EXECUTION;
}
