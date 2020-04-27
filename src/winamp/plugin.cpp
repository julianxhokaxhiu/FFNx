#include "plugin.h"

WinampPlugin::WinampPlugin() : handle(NULL)
{
}

WinampPlugin::~WinampPlugin()
{
	close();
}

bool WinampPlugin::open(LPCWSTR libFileNameW, char* libFileNameA)
{
	close();
	if (libFileNameW) {
		this->handle = LoadLibraryExW(libFileNameW, NULL, LOAD_LIBRARY_SEARCH_APPLICATION_DIR);
	}
	else {
		this->handle = LoadLibraryExA(libFileNameA, NULL, LOAD_LIBRARY_SEARCH_APPLICATION_DIR);
	}

	if (NULL != this->handle)
	{
		FARPROC procAddress = GetProcAddress(this->handle, procName());

		closeModule();

		if (NULL != procAddress && openModule(procAddress))
		{
			return true;
		}

		close();
	}

	return false;
}

bool WinampPlugin::open(LPCWSTR libFileName)
{
	return open(libFileName, NULL);
}

bool WinampPlugin::open(char* libFileName)
{
	return open(NULL, libFileName);
}

void WinampPlugin::close()
{
	if (NULL != this->handle) {
		closeModule();
		FreeLibrary(this->handle);
		this->handle = NULL;
	}
}

#define UNUSED_PARAM(x) (void)x;

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

WinampOutPlugin::WinampOutPlugin() :
	WinampPlugin(), mod(NULL)
{
}

WinampOutPlugin::~WinampOutPlugin()
{
	close();
}

bool WinampOutPlugin::openModule(FARPROC procAddress)
{
	winampGetOutModule f = (winampGetOutModule)procAddress;
	this->mod = f();

	if (NULL == this->mod) {
		return false;
	}

	// Set fields
	this->mod->hMainWindow = NULL;
	this->mod->hDllInstance = getHandle();
	// Initialize module
	if (NULL != this->mod->Init) {
		this->mod->Init();
	}

	return true;
}

void WinampOutPlugin::closeModule()
{
	if (NULL != this->mod) {
		if (NULL != this->mod->Quit) {
			this->mod->Quit();
		}
		this->mod = NULL;
	}
}

WinampInPlugin::WinampInPlugin(WinampOutPlugin *outPlugin) :
	WinampPlugin(), outPlugin(outPlugin), mod(NULL)
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

	if (NULL == this->mod) {
		return false;
	}
	
	// Set fields
	this->mod->standard.hMainWindow = NULL;
	this->mod->standard.hDllInstance = getHandle();

	if (NULL != this->outPlugin) {
		this->mod->standard.outMod = this->outPlugin->getModule();
	}
	else {
		this->mod->standard.outMod = NULL;
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
	if (NULL != this->mod->standard.Init) {
		this->mod->standard.Init();
	}

	return true;
}

void WinampInPlugin::closeModule()
{
	if (NULL != this->mod) {
		if (NULL != this->mod->standard.Quit) {
			this->mod->standard.Quit();
		}
		this->mod = NULL;
	}
}

int WinampInPlugin::play(const char* fn)
{
	if (NULL == this->mod->wide.Play) {
		return -42;
	}

	if (this->mod->standard.version & IN_UNICODE == IN_UNICODE) {
		// Convert char* to wchar_t*
		int count = MultiByteToWideChar(CP_ACP, 0, fn, -1, NULL, 0);
		wchar_t wideFn[MAX_PATH * 2];
		memset(wideFn, 0, MAX_PATH * 2);
		MultiByteToWideChar(CP_ACP, 0, fn, -1, wideFn, count);
		return this->mod->wide.Play(wideFn);
	}

	return this->mod->standard.Play(fn);
}

int WinampInPlugin::play(const wchar_t* fn)
{
	if (NULL == this->mod->wide.Play) {
		return -42;
	}

	if (this->mod->standard.version & IN_UNICODE == IN_UNICODE) {
		return this->mod->wide.Play(fn);
	}

	// Convert wchar_t* to char*
	char mbstring[MAX_PATH];
	size_t size = wcstombs(mbstring, fn, MAX_PATH);
	mbstring[size] = '\0';

	return this->mod->standard.Play(mbstring);
}

void WinampInPlugin::pause()
{
	this->mod->standard.Pause();
}

void WinampInPlugin::unPause()
{
	this->mod->standard.UnPause();
}

int WinampInPlugin::isPaused()
{
	return this->mod->standard.IsPaused();
}

void WinampInPlugin::stop()
{
	this->mod->standard.Stop();
}

int WinampInPlugin::getLength()
{
	return this->mod->standard.GetLength();
}

int WinampInPlugin::getOutputTime()
{
	return this->mod->standard.GetOutputTime();
}

void WinampInPlugin::setOutputTime(int time_in_ms)
{
	this->mod->standard.SetOutputTime(time_in_ms);
}

// volume stuff
void WinampInPlugin::setVolume(int volume)
{
	this->mod->standard.SetVolume(volume);
}

void WinampInPlugin::setPan(int pan)
{
	this->mod->standard.SetPan(pan);
}
