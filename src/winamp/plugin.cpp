#include "plugin.h"
#include <math.h>
#include "../common.h"
#include "../log.h"

#define AUDIO_BUFFER_SIZE 5

WinampPlugin::WinampPlugin() : handle(nullptr)
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
		this->handle = LoadLibraryExW(libFileNameW, nullptr, LOAD_LIBRARY_SEARCH_APPLICATION_DIR);
	}
	else {
		this->handle = LoadLibraryExA(libFileNameA, nullptr, LOAD_LIBRARY_SEARCH_APPLICATION_DIR);
	}

	if (nullptr != this->handle)
	{
		FARPROC procAddress = GetProcAddress(this->handle, procName());

		closeModule();

		if (nullptr != procAddress && openModule(procAddress))
		{
			return true;
		}

		close();
	}

	return false;
}

bool WinampPlugin::open(LPCWSTR libFileName)
{
	return open(libFileName, nullptr);
}

bool WinampPlugin::open(char* libFileName)
{
	return open(nullptr, libFileName);
}

void WinampPlugin::close()
{
	if (nullptr != this->handle) {
		closeModule();
		FreeLibrary(this->handle);
		this->handle = nullptr;
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
	WinampPlugin(), AbstractOutPlugin()
{
}

WinampOutPlugin::~WinampOutPlugin()
{
	closeModule();
	close();
}

bool WinampOutPlugin::openModule(FARPROC procAddress)
{
	winampGetOutModule f = (winampGetOutModule)procAddress;
	this->mod = f();

	if (nullptr == this->mod) {
		return false;
	}

	// Set fields
	this->mod->hMainWindow = nullptr;
	this->mod->hDllInstance = getHandle();
	// Initialize module
	if (nullptr != this->mod->Init) {
		this->mod->Init();
	}

	return true;
}

void WinampOutPlugin::closeModule()
{
	if (nullptr != this->mod) {
		if (nullptr != this->mod->Quit) {
			this->mod->Quit();
		}
		this->mod = nullptr;
	}
}

int WinampOutPlugin::isPlaying() const
{
	if (nullptr != this->mod) {
		return this->mod->IsPlaying();
	}
	return 0;
}

void WinampOutPlugin::setTempo(int tempo)
{
	error("setTempo not implemented for Winamp out plugin (%i)\n", tempo);
}

IDirectSoundBuffer* CustomOutPlugin::sound_buffer = 0;
uint CustomOutPlugin::sound_buffer_size = 0;
uint CustomOutPlugin::sound_write_pointer = 0;
uint CustomOutPlugin::bytes_written = 0;
DWORD CustomOutPlugin::start_t = 0;
DWORD CustomOutPlugin::last_pause_t = 0;
int CustomOutPlugin::last_pause = 0;
int CustomOutPlugin::volume = -1;
WAVEFORMATEX CustomOutPlugin::sound_format = WAVEFORMATEX();

CustomOutPlugin::CustomOutPlugin() :
	AbstractOutPlugin()
{
	mod = new WinampOutModule();
	mod->version = OUT_VER;
	mod->description = "FFnX Winamp Output Plugin";
	mod->id = 38; // DirectSound id
	mod->hMainWindow = nullptr;
	mod->hDllInstance = nullptr;
	mod->Config = Config;
	mod->About = About;
	mod->Init = Init;
	mod->Quit = Quit;
	mod->Open = Open;
	mod->Close = Close;
	mod->Write = Write;
	mod->CanWrite = CanWrite;
	mod->IsPlaying = IsPlaying;
	mod->Pause = Pause;
	mod->SetVolume = SetVolume;
	mod->SetPan = SetPan;
	mod->Flush = Flush;
	mod->GetOutputTime = GetOutputTime;
	mod->GetWrittenTime = GetWrittenTime;
}

CustomOutPlugin::~CustomOutPlugin()
{
	CustomOutPlugin::Quit();
	delete mod;
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
	
	start_t = GetTickCount();
	
	return 0;
}

void CustomOutPlugin::Close()
{
	if (sound_buffer) {
		sound_buffer->Release();
	}

	sound_buffer = 0;
}

int CustomOutPlugin::Write(char* buffer, int len)
{
	LPVOID ptr1, ptr2;
	DWORD bytes1, bytes2;

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
		return 0;
	}

	DWORD current_play_cursor;

	sound_buffer->GetCurrentPosition(&current_play_cursor, nullptr);

	if (current_play_cursor <= sound_write_pointer) {
		return (sound_buffer_size - sound_write_pointer) + current_play_cursor;
	}
	return current_play_cursor - sound_write_pointer;
}

int CustomOutPlugin::IsPlaying()
{
	if (sound_buffer) {
		DWORD ret;

		if (DS_OK == sound_buffer->GetStatus(&ret)) {
			return ret & DSBSTATUS_PLAYING ? 1 : 0;
		}
	}

	return 0;
}

int CustomOutPlugin::Pause(int pause)
{
	int t = last_pause;

	if (sound_buffer) {
		if (0 == pause) {
			sound_buffer->Play(0, 0, DSBPLAY_LOOPING);
			if (last_pause_t > 0) {
				start_t += (GetTickCount() - last_pause_t); // Remove paused time
				last_pause_t = 0;
			}
		}
		else {
			last_pause_t = GetTickCount();
			sound_buffer->Stop();
		}
		last_pause = pause;
	}

	return t;
}

void CustomOutPlugin::SetVolume(int volume)
{
	if (volume < 0 || volume > 255) {
		return;
	}

	CustomOutPlugin::volume = volume;

	if (sound_buffer) {
		float decibel = 20.0f * log10f((volume * 100 / 255) / 100.0f);
		sound_buffer->SetVolume(volume ? int(decibel * 100.0f) : DSBVOLUME_MIN);
	}
}

void CustomOutPlugin::SetPan(int pan)
{
	if (sound_buffer) {
		LONG dstpan = pan * DSBPAN_LEFT / 128;
		info("Winamp OutPlugin SetPan: %i\n", dstpan);
		sound_buffer->SetPan(dstpan);
	}
}

void CustomOutPlugin::Flush(int t)
{
	info("Winamp OutPlugin Flush: %i\n", t);
	// TODO: implement seek?
	start_t = GetTickCount() - t;
}

int CustomOutPlugin::GetOutputTime()
{
	if (last_pause_t > 0) {
		return last_pause_t - start_t;
	}
	return GetTickCount() - start_t;
}

int CustomOutPlugin::GetWrittenTime()
{
	if (0 == sound_format.nAvgBytesPerSec) {
		return bytes_written;
	}

	int written_time = bytes_written / sound_format.nAvgBytesPerSec * 1000;
	written_time += ((bytes_written % sound_format.nAvgBytesPerSec) * 1000) / sound_format.nAvgBytesPerSec;

	return written_time;
}

int CustomOutPlugin::isPlaying() const
{
	return CustomOutPlugin::IsPlaying();
}

void CustomOutPlugin::setTempo(int tempo)
{
	if (sound_buffer) {
		int dsttempo = (sound_format.nSamplesPerSec * (tempo + 480)) / 512;

		sound_buffer->SetFrequency(dsttempo);
	}
}

WinampInPlugin::WinampInPlugin(AbstractOutPlugin* outPlugin) :
	WinampPlugin(), mod(nullptr), outPlugin(outPlugin)
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
		return false;
	}
	
	// Set fields
	this->mod->standard.hMainWindow = nullptr;
	this->mod->standard.hDllInstance = getHandle();

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

	return true;
}

void WinampInPlugin::closeModule()
{
	if (nullptr != this->mod) {
		if (nullptr != this->mod->standard.Quit) {
			this->mod->standard.Quit();
		}
		this->mod = nullptr;
	}
}

int WinampInPlugin::play(const char* fn)
{
	if (nullptr == this->mod->wide.Play) {
		return -42;
	}

	if (this->mod->standard.version & IN_UNICODE == IN_UNICODE) {
		// Convert char* to wchar_t*
		int count = MultiByteToWideChar(CP_ACP, 0, fn, -1, nullptr, 0);
		wchar_t wideFn[MAX_PATH * 2];
		memset(wideFn, 0, MAX_PATH * 2);
		MultiByteToWideChar(CP_ACP, 0, fn, -1, wideFn, count);
		return this->mod->wide.Play(wideFn);
	}

	return this->mod->standard.Play(fn);
}

int WinampInPlugin::play(const wchar_t* fn)
{
	if (nullptr == this->mod->wide.Play) {
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
	this->mod->standard.SetVolume(volume);
}

void WinampInPlugin::setPan(int pan)
{
	this->mod->standard.SetPan(pan);
}

void WinampInPlugin::setTempo(int tempo)
{
	if (nullptr != this->outPlugin) {
		this->outPlugin->setTempo(tempo);
	}
}
