#include "plugin.h"
#include <math.h>
#include "../common.h"
#include "../log.h"

#define AUDIO_BUFFER_SIZE 5

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
/*
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
}*/

IDirectSoundBuffer* CustomOutPlugin::sound_buffer = 0;
uint CustomOutPlugin::sound_buffer_size = 0;
uint CustomOutPlugin::sound_write_pointer = 0;
uint CustomOutPlugin::bytes_written = 0;
int CustomOutPlugin::last_pause = 0;
int CustomOutPlugin::volume = -1;
WAVEFORMATEX CustomOutPlugin::sound_format = WAVEFORMATEX();

CustomOutPlugin::CustomOutPlugin()
{
	mod.version = OUT_VER;
	mod.description = "FFnX Winamp Output Plugin";
	mod.id = 38; // DirectSound id
	mod.hMainWindow = NULL;
	mod.hDllInstance = NULL;
	mod.Config = Config;
	mod.About = About;
	mod.Init = Init;
	mod.Quit = Quit;
	mod.Open = Open;
	mod.Close = Close;
	mod.Write = Write;
	mod.CanWrite = CanWrite;
	mod.IsPlaying = IsPlaying;
	mod.Pause = Pause;
	mod.SetVolume = SetVolume;
	mod.SetPan = SetPan;
	mod.Flush = Flush;
	mod.GetOutputTime = GetOutputTime;
	mod.GetWrittenTime = GetWrittenTime;
}

CustomOutPlugin::~CustomOutPlugin()
{
	CustomOutPlugin::Quit();
}

void CustomOutPlugin::Config(HWND hwndParent)
{
	UNUSED_PARAM(hwndParent);
	// Nothing to do
}

void CustomOutPlugin::About(HWND hwndParent)
{
	UNUSED_PARAM(hwndParent);
	// Nothing to do
}

void CustomOutPlugin::Init()
{
	// Nothing to do
}

void CustomOutPlugin::Quit()
{
	Close();
}

int CustomOutPlugin::Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms)
{
	info("OutPlugin Open\n");

	UNUSED_PARAM(bufferlenms);
	UNUSED_PARAM(prebufferms);

	Close();

	DSBUFFERDESC1 sbdesc;

	sound_format.cbSize = sizeof(sound_format);
	sound_format.wBitsPerSample = bitspersamp;
	sound_format.nChannels = numchannels;
	sound_format.nSamplesPerSec = samplerate;
	sound_format.nBlockAlign = sound_format.nChannels * sound_format.wBitsPerSample / 8;
	sound_format.nAvgBytesPerSec = sound_format.nSamplesPerSec * sound_format.nBlockAlign;
	sound_format.wFormatTag = WAVE_FORMAT_PCM;

	sound_buffer_size = sound_format.nAvgBytesPerSec * AUDIO_BUFFER_SIZE;
	sound_write_pointer = 0;
	bytes_written = 0;

	sbdesc.dwSize = sizeof(sbdesc);
	sbdesc.lpwfxFormat = &sound_format;
	sbdesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	sbdesc.dwReserved = 0;
	sbdesc.dwBufferBytes = sound_buffer_size;

	if ((*common_externals.directsound)->CreateSoundBuffer((LPCDSBUFFERDESC)&sbdesc, &sound_buffer, 0))
	{
		error("couldn't create sound buffer (%i, %i)\n", numchannels, samplerate);
		sound_buffer = 0;

		return -1;
	}

	if (volume >= 0) {
		SetVolume(volume);
	}

	if (sound_buffer->Play(0, 0, DSBPLAY_LOOPING)) {
		error("couldn't play sound buffer\n");
	}
	
	return 0;
}

void CustomOutPlugin::Close()
{
	error("OutPlugin Close\n");

	if (sound_buffer) {
		sound_buffer->Release();
	}

	sound_buffer = 0;
}

int CustomOutPlugin::Write(char* buffer, int len)
{
	LPVOID ptr1, ptr2;
	DWORD bytes1, bytes2;

	error("OutPlugin Write %i\n", len);

	if (sound_buffer->Lock(sound_write_pointer, len, &ptr1, &bytes1, &ptr2, &bytes2, 0)) {
		error("couldn't lock sound buffer\n");
	}

	memcpy(ptr1, buffer, bytes1);
	memcpy(ptr2, buffer + bytes1, bytes2);

	if (sound_buffer->Unlock(ptr1, bytes1, ptr2, bytes2)) {
		error("couldn't unlock sound buffer\n");
	}

	sound_write_pointer = (sound_write_pointer + bytes1 + bytes2) % sound_buffer_size;
	bytes_written += bytes1 + bytes2;

	return 0;
}

int CustomOutPlugin::CanWrite()
{
	if (!sound_buffer || last_pause) {
		error("OutPlugin CanWrite No\n");
		return 0;
	}
	DWORD lpdwCurrentPlayCursor, lpdwCurrentWriteCursor;

	if (DS_OK != sound_buffer->GetCurrentPosition(&lpdwCurrentPlayCursor, &lpdwCurrentWriteCursor)) {
		error("OutPlugin CanWrite Error\n");
		return 0;
	}

	error("OutPlugin CanWrite %i %i %i %i\n", lpdwCurrentPlayCursor, lpdwCurrentWriteCursor, sound_write_pointer, sound_buffer_size);

	if (lpdwCurrentPlayCursor <= sound_write_pointer) {
		return (sound_buffer_size - sound_write_pointer) + lpdwCurrentPlayCursor;
	}
	return lpdwCurrentPlayCursor - sound_write_pointer;
}

int CustomOutPlugin::IsPlaying()
{
	if (sound_buffer) {
		DWORD ret;

		if (DS_OK == sound_buffer->GetStatus(&ret)) {
			error("OutPlugin IsPlaying %i\n", ret & DSBSTATUS_PLAYING ? 1 : 0);
			return ret & DSBSTATUS_PLAYING ? 1 : 0;
		}
	}
	error("OutPlugin IsPlaying no\n");
	return 0;
}

int CustomOutPlugin::Pause(int pause)
{
	error("OutPlugin Pause %i\n", pause);
	int t = last_pause;

	if (sound_buffer) {
		if (0 == pause) {
			sound_buffer->Play(0, 0, DSBPLAY_LOOPING);
		}
		else {
			sound_buffer->Stop();
		}
		last_pause = pause;
	}

	return t;
}

void CustomOutPlugin::SetVolume(int volume)
{
	info("OutPlugin SetVolume: %i/255\n", volume);

	if (volume < 0 || volume > 255) {
		return;
	}

	CustomOutPlugin::volume = volume;

	if (sound_buffer) {
		float decibel = 20.0f * log10f((volume * 100 / 255) / 100.0f);
		info("OutPlugin SetVolume: %f DB\n", decibel);
		sound_buffer->SetVolume(volume ? int(decibel * 100.0f) : DSBVOLUME_MIN);
	}
}

void CustomOutPlugin::SetPan(int pan)
{
	if (sound_buffer) {
		info("OutPlugin SetPan: %i\n", pan * DSBPAN_LEFT / 128);
		sound_buffer->SetPan(pan * DSBPAN_LEFT / 128);
	}
}

void CustomOutPlugin::Flush(int t)
{
	info("OutPlugin Flush: %i\n", t);
	// TODO: implement seek?
}

int CustomOutPlugin::GetOutputTime()
{
	info("OutPlugin GetOutputTime\n");
	return GetWrittenTime();
}

int CustomOutPlugin::GetWrittenTime()
{
	if (0 == sound_format.nAvgBytesPerSec) {
		return bytes_written;
	}

	int written_time = bytes_written / sound_format.nAvgBytesPerSec * 1000;
	written_time += ((bytes_written % sound_format.nAvgBytesPerSec) * 1000) / sound_format.nAvgBytesPerSec;

	info("OutPlugin GetWrittenTime %i\n", written_time);

	return written_time;
}

void CustomOutPlugin::setTempo(int tempo)
{
	if (sound_buffer) {
		int dsttempo = (sound_format.nSamplesPerSec * (tempo + 480)) / 512;

		sound_buffer->SetFrequency(dsttempo);
	}
}

WinampInPlugin::WinampInPlugin(WinampOutModule* outMod) :
	WinampPlugin(), mod(NULL), outMod(outMod)
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

	if (NULL != this->outMod) {
		this->mod->standard.outMod = this->outMod;
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

void WinampInPlugin::setVolume(int volume)
{
	info("InPlugin SetVolume: %i/255\n", volume);
	this->mod->standard.SetVolume(volume);
}

void WinampInPlugin::setPan(int pan)
{
	this->mod->standard.SetPan(pan);
}

void WinampInPlugin::setTempo(int tempo)
{
	CustomOutPlugin::setTempo(tempo);
}
