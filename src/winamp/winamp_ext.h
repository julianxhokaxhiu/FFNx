#pragma once

#define IN_CONTEXT_VER 0x1
#define OUT_CONTEXT_VER 0x1

struct WinampOutContext {
	int version;
	void (*Duplicate)();
	int (*Resume)(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
	int (*CancelDuplicate)();
};

struct WinampInContext {
	int version;
	void (*Duplicate)();
	int (*Resume)(const char* fn);
	int (*CancelDuplicate)();
	WinampOutContext* outContext;
};
