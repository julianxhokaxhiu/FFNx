/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#include "in_plugin.h"

void SAVSAInit(int maxlatency_in_ms, int srate)
{
	UNUSED_PARAM(maxlatency_in_ms);
	UNUSED_PARAM(srate);
}

void SAVSADeInit()
{
}

void SAAddPCMData(void* PCMData, int nch, int bps, int timestamp)
{
	UNUSED_PARAM(PCMData);
	UNUSED_PARAM(nch);
	UNUSED_PARAM(bps);
	UNUSED_PARAM(timestamp);
}

int SAGetMode()
{
	return 4;
}

int SAAdd(void* data, int timestamp, int csa)
{
	UNUSED_PARAM(data);
	UNUSED_PARAM(timestamp);
	UNUSED_PARAM(csa);
	return 0;
}

void VSAAddPCMData(void* PCMData, int nch, int bps, int timestamp)
{
	UNUSED_PARAM(PCMData);
	UNUSED_PARAM(nch);
	UNUSED_PARAM(bps);
	UNUSED_PARAM(timestamp);
}

int VSAGetMode(int* specNch, int* waveNch)
{
	UNUSED_PARAM(specNch);
	UNUSED_PARAM(waveNch);
	return 0;
}

int VSAAdd(void* data, int timestamp)
{
	UNUSED_PARAM(data);
	UNUSED_PARAM(timestamp);
	return 0;
}

void VSASetInfo(int srate, int nch)
{
	UNUSED_PARAM(srate);
	UNUSED_PARAM(nch);
}

int dsp_isactive()
{
	return 0;
}

int dsp_dosamples(short int* samples, int numsamples, int bps, int nch, int srate)
{
	UNUSED_PARAM(samples);
	UNUSED_PARAM(numsamples);
	UNUSED_PARAM(bps);
	UNUSED_PARAM(nch);
	UNUSED_PARAM(srate);
	return 0;
}

void SetInfo(int bitrate, int srate, int stereo, int synched)
{
	UNUSED_PARAM(bitrate);
	UNUSED_PARAM(srate);
	UNUSED_PARAM(stereo);
	UNUSED_PARAM(synched);
}

WinampInPlugin::WinampInPlugin(AbstractOutPlugin* outPlugin) :
	WinampPlugin(), getExtendedFileInfoProc(nullptr), outPlugin(outPlugin),
	mod(nullptr)
{
}

WinampInPlugin::~WinampInPlugin()
{
	close();
}

bool WinampInPlugin::openModule(FARPROC procAddress)
{
	winampGetInModule2 f = (winampGetInModule2)procAddress;
	this->mod = f();

	if (nullptr == this->mod) {
		error("couldn't call function %s in external library\n", procName());
		return false;
	}

	getExtendedFileInfoProc = GetProcAddress(getHandle(), "winampGetExtendedFileInfo");

	initModule(getHandle());

	return true;
}

void WinampInPlugin::closeModule()
{
	quitModule();
}

void WinampInPlugin::initModule(HINSTANCE dllInstance)
{
	// Set fields
	this->mod->standard.hMainWindow = nullptr;
	this->mod->standard.hDllInstance = dllInstance;

	if (nullptr != this->outPlugin) {
		this->mod->standard.outMod = this->outPlugin->getModule();
	}
	else {
		this->mod->standard.outMod = nullptr;
	}

	// Set Winamp dummy functions
	this->mod->standard.SAVSAInit = SAVSAInit;
	this->mod->standard.SAVSADeInit = SAVSADeInit;
	this->mod->standard.SAAddPCMData = SAAddPCMData;
	this->mod->standard.SAGetMode = SAGetMode;
	this->mod->standard.SAAdd = SAAdd;
	this->mod->standard.VSAAddPCMData = VSAAddPCMData;
	this->mod->standard.VSAGetMode = VSAGetMode;
	this->mod->standard.VSAAdd = VSAAdd;
	this->mod->standard.VSASetInfo = VSASetInfo;
	this->mod->standard.dsp_isactive = dsp_isactive;
	this->mod->standard.dsp_dosamples = dsp_dosamples;
	this->mod->standard.SetInfo = SetInfo;

	// Initialize module
	if (nullptr != this->mod->standard.Init) {
		this->mod->standard.Init();
	}
}

void WinampInPlugin::quitModule()
{
	if (nullptr != this->mod) {
		if (nullptr != this->mod->standard.Quit) {
			this->mod->standard.Quit();
		}
		this->mod = nullptr;
	}
}

bool WinampInPlugin::knownExtension(const char* fn) const
{
	/*
	char* extension_list = getModule()->standard.FileExtensions;
	const char* ext = filename_extension(fn);

	while (true) {
		size_t len_ext = strnlen(extension_list, 64);
		if (len_ext == 0) {
			break;
		}

		if (strcasecmp(extension_list, ext) == 0) {
			return true;
		}

		extension_list += len_ext + 1;

		size_t len_desc = strnlen(extension_list, 64);
		if (len_desc == 0) {
			break;
		}

		extension_list += len_desc + 1;
	}
	*/

	return false;
}

int WinampInPlugin::isOurFile(const char* fn) const
{
	if (getModule()->standard.version & IN_UNICODE == IN_UNICODE) {
		// Convert char* to wchar_t*
		int count = MultiByteToWideChar(CP_ACP, 0, fn, -1, nullptr, 0);
		wchar_t wideFn[MAX_PATH * 2];
		memset(wideFn, 0, MAX_PATH * 2);
		MultiByteToWideChar(CP_ACP, 0, fn, -1, wideFn, count);
		return getModule()->wide.IsOurFile(wideFn);
	}

	return getModule()->standard.IsOurFile(fn);
}

int WinampInPlugin::inPsfGetTag(const char* fn, const char* metadata, char* ret, int retlen)
{
	if (!fn || !metadata || !ret || retlen <= 0) {
		error("inPsfGetTag invalid input\n");
		return 0;
	}

	FILE* f = fopen(fn, "rb");
	if (f == nullptr) {
		error("inPsfGetTag Cannot open file %s\n", fn);
		return 0;
	}

	char magic[4];
	if (fread(magic, 1, 3, f) != 3) {
		fclose(f);
		error("inPsfGetTag Cannot read magic word %s\n", fn);
		return 0;
	}
	magic[3] = '\0';

	if (strncmp(magic, "PSF", 3) != 0) {
		fclose(f);
		return 0;
	}

	if (fseek(f, 8, SEEK_SET) != 0) {
		fclose(f);
		error("inPsfGetTag cannot seek to size %s\n", fn);
		return 0;
	}

	int32_t size = 0;

	if (fread(reinterpret_cast<char*>(&size), sizeof(size), 1, f) != 1) {
		fclose(f);
		error("inPsfGetTag Cannot read file data size %s\n", fn);
		return 0;
	}

	if (fseek(f, size + 21, SEEK_SET) != 0) {
		fclose(f);
		error("inPsfGetTag cannot seek to tags %s\n", fn);
		return 0;
	}

	char data[8192];
	size_t r = fread(data, 1, 8192, f);

	fclose(f);

	data[r] = '\0';

	int keyPos = 0, valuePos = 0;
	bool found = false;

	for (int i = 0; i < r; ++i) {
		if (data[i] == '=') {
			if (strncmp(metadata, data + keyPos, i - keyPos) == 0) {
				found = true;
			}
			valuePos = i + 1;
		}
		else if (valuePos > keyPos && data[i] == '\n') {
			if (found) {
				memcpy(ret, data + valuePos, i - valuePos);
				return 1;
			}
			keyPos = i + 1;
		}
	}

	error("inPsfGetTag tag not found %s %s\n", fn, metadata);

	return 0;
}

bool WinampInPlugin::accept(const char* fn) const
{
	return isOurFile(fn) != 0 || knownExtension(fn);
}

int WinampInPlugin::play(const char* fn)
{
	if (getModule()->standard.version & IN_UNICODE == IN_UNICODE) {
		// Convert char* to wchar_t*
		int count = MultiByteToWideChar(CP_ACP, 0, fn, -1, nullptr, 0);
		wchar_t wideFn[MAX_PATH * 2];
		memset(wideFn, 0, MAX_PATH * 2);
		MultiByteToWideChar(CP_ACP, 0, fn, -1, wideFn, count);
		return getModule()->wide.Play(wideFn);
	}

	return getModule()->standard.Play(fn);
}

void WinampInPlugin::pause()
{
	getModule()->standard.Pause();
}

void WinampInPlugin::unPause()
{
	getModule()->standard.UnPause();
}

int WinampInPlugin::isPaused()
{
	return getModule()->standard.IsPaused();
}

void WinampInPlugin::stop()
{
	getModule()->standard.Stop();
}

int WinampInPlugin::getLength()
{
	return getModule()->standard.GetLength();
}

int WinampInPlugin::getOutputTime()
{
	return getModule()->standard.GetOutputTime();
}

void WinampInPlugin::setOutputTime(int time_in_ms)
{
	getModule()->standard.SetOutputTime(time_in_ms);
}

size_t WinampInPlugin::getTitle(const char* fn, char* ret, size_t max)
{
	char title[GETFILEINFO_TITLE_LENGTH];
	memset(title, 0, GETFILEINFO_TITLE_LENGTH);
	getModule()->standard.GetFileInfo(fn, title, nullptr);
	size_t len = strnlen(title, GETFILEINFO_TITLE_LENGTH);
	if (len == 0) {
		return 0;
	}

	memset(ret, 0, max);
	if (len > max - 1) {
		len = max - 1;
	}
	memcpy(ret, title, len);

	return len;
}

int WinampInPlugin::getTag(const char* fn, const char* metadata, char* ret, int retlen)
{
	return inPsfGetTag(fn, metadata, ret, retlen);

	/* if (getExtendedFileInfoProc != nullptr) {
		winampGetExtendedFileInfo f = winampGetExtendedFileInfo(getExtendedFileInfoProc);
		// FIXME: makes the game crashes with in_psf!
		return f(fn, metadata, ret, retlen);
	}

	return 0; */
}
