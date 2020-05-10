#pragma once

extern "C" {
#include "winamp.h"
#include "winamp_ext.h"

	//WinampInModuleA input_module;
	WinampInModule* in_vgmstream_module();
	WinampInContext* in_context_vgmstream();
}
