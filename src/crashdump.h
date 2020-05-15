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
