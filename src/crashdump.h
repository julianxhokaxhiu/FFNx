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
 * crashdump.h - crash handler definitons
 */

#pragma once

#include <stdio.h>
#include <StackWalker.h>
#include <dbghelp.h>

#include "crashdump.h"
#include "log.h"

#define STACK_MAX_NAME_LENGTH 256

class FFNxStackWalker : public StackWalker
{
public:
    FFNxStackWalker() : StackWalker() {}
protected:
    virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
    {
        // Silence is golden.
    }

    virtual void OnOutput(LPCSTR szText)
    {
        trace(szText);
    }
};

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS *ep);
