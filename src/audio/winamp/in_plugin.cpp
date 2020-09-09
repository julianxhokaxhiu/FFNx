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

const char* default_extension = "ogg";

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

AbstractInPlugin::AbstractInPlugin(AbstractOutPlugin* outPlugin) :
	outPlugin(outPlugin)
{
}

AbstractInPlugin::~AbstractInPlugin()
{
}

WinampInPlugin::WinampInPlugin(AbstractOutPlugin* outPlugin) :
	WinampPlugin(), AbstractInPlugin(outPlugin), current_saved_time_ms(0),
	mod(nullptr), context(nullptr)
{
}

WinampInPlugin::~WinampInPlugin()
{
	close();
	quitModule();
}

bool WinampInPlugin::openModule(FARPROC procAddress)
{
	winampGetInModule2 f = (winampGetInModule2)procAddress;
	this->mod = f();

	if (nullptr == this->mod) {
		error("couldn't call function %s in external library\n", procName());
		return false;
	}

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

		if (nullptr != this->context) {
			this->context->outContext = this->outPlugin->getContext();
		}
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
		if (nullptr != this->context && nullptr != this->context->CancelDuplicate) {
			this->context->CancelDuplicate();
		}

		if (nullptr != this->mod->standard.Quit) {
			this->mod->standard.Quit();
		}
		this->mod = nullptr;
	}
}

bool WinampInPlugin::knownExtension(const char* fn) const
{
	/*
	char* extension_list = getMod()->standard.FileExtensions;
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
	if (getMod()->standard.version & IN_UNICODE == IN_UNICODE) {
		// Convert char* to wchar_t*
		int count = MultiByteToWideChar(CP_ACP, 0, fn, -1, nullptr, 0);
		wchar_t wideFn[MAX_PATH * 2];
		memset(wideFn, 0, MAX_PATH * 2);
		MultiByteToWideChar(CP_ACP, 0, fn, -1, wideFn, count);
		return getMod()->wide.IsOurFile(wideFn);
	}

	return getMod()->standard.IsOurFile(fn);
}

bool WinampInPlugin::accept(const char* fn) const
{
	return isOurFile(fn) != 0 || knownExtension(fn);
}

int WinampInPlugin::play(char* fn)
{
	if (getMod()->standard.version & IN_UNICODE == IN_UNICODE) {
		// Convert char* to wchar_t*
		int count = MultiByteToWideChar(CP_ACP, 0, fn, -1, nullptr, 0);
		wchar_t wideFn[MAX_PATH * 2];
		memset(wideFn, 0, MAX_PATH * 2);
		MultiByteToWideChar(CP_ACP, 0, fn, -1, wideFn, count);
		return getMod()->wide.Play(wideFn);
	}

	return getMod()->standard.Play(fn);
}

void WinampInPlugin::pause()
{
	getMod()->standard.Pause();
}

void WinampInPlugin::unPause()
{
	getMod()->standard.UnPause();
}

int WinampInPlugin::isPaused()
{
	return getMod()->standard.IsPaused();
}

void WinampInPlugin::stop()
{
	getMod()->standard.Stop();
}

int WinampInPlugin::getLength()
{
	return getMod()->standard.GetLength();
}

int WinampInPlugin::getOutputTime()
{
	return getMod()->standard.GetOutputTime();
}

void WinampInPlugin::setOutputTime(int time_in_ms)
{
	getMod()->standard.SetOutputTime(time_in_ms);
}

bool WinampInPlugin::canDuplicate() const
{
	return getContext() && IN_CONTEXT_VER == getContext()->version
		&& nullptr != getContext()->outContext
		&& OUT_CONTEXT_VER == getContext()->outContext->version;
}

void WinampInPlugin::duplicate()
{
	if (canDuplicate()) {
		getContext()->Duplicate();
	}
	else {
		current_saved_time_ms = outPlugin->getOutputTime() % getLength();

		if (current_saved_time_ms < 0) {
			current_saved_time_ms = 0;
		}
	}
}

int WinampInPlugin::resume(char* fn)
{
	if (canDuplicate()) {
		return getContext()->Resume(fn);
	}

	if (current_saved_time_ms > 0) {
		pause();
		outPlugin->pause();
	}

	stop();
	int err = play(fn);

	if (current_saved_time_ms > 0) {
		setOutputTime(current_saved_time_ms); // FIXME: can take a while
		unPause();
		outPlugin->unPause();
		current_saved_time_ms = 0;
	}

	return err;
}

bool WinampInPlugin::cancelDuplicate()
{
	if (canDuplicate()) {
		return getContext()->CancelDuplicate() != 0;
	}

	if (current_saved_time_ms > 0) {
		current_saved_time_ms = 0;
		return true;
	}

	return false;
}

void WinampInPlugin::setLoopingEnabled(bool enabled)
{
	UNUSED_PARAM(enabled);
}
