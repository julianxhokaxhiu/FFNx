#pragma once

#define IN_CONTEXT_VER 0x1

struct WinampInContext {
	int version;
	void (*Duplicate)();
	int (*Resume)(const char* fn);
	bool (*CancelDuplicate)();
};
