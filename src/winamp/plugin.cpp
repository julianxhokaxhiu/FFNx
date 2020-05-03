#include "plugin.h"

#define _USE_MATH_DEFINES
#include <math.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include "in_vgmstream.h"

#if defined(__cplusplus)
}
#endif

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

		error("couldn't load function %s in external library (error %u)\n", procName(), GetLastError());

		close();
	}
	else {
		error("couldn't load external library (error %u)\n", GetLastError());
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

void AbstractOutPlugin::setVolume(int volume)
{
	if (volume == 255) {
		// Force volume for MM (out_wave fix)
		for (int i = 0; i < waveInGetNumDevs(); ++i) {
			waveOutSetVolume(HWAVEOUT(i), 0xFFFFFFFF);
		}
	}

	if (nullptr != this->mod) {
		this->mod->SetVolume(volume);
	}
}

void AbstractOutPlugin::setPan(int pan)
{
	if (nullptr != this->mod) {
		this->mod->SetPan(pan);
	}
}

int AbstractOutPlugin::getOutputTime() const
{
	if (nullptr != this->mod) {
		return this->mod->GetOutputTime();
	}

	return 0;
}

int AbstractOutPlugin::getWrittenTime() const
{
	if (nullptr != this->mod) {
		return this->mod->GetWrittenTime();
	}

	return 0;
}

void AbstractOutPlugin::pause() const
{
	if (nullptr != this->mod) {
		this->mod->Pause(1);
	}
}

void AbstractOutPlugin::unPause() const
{
	if (nullptr != this->mod) {
		this->mod->Pause(0);
	}
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
		error("couldn't call function %s in external library\n", procName());
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

void WinampOutPlugin::setTempo(int tempo)
{
	error("setTempo not implemented for Winamp out plugin (%i)\n", tempo);
}

IDirectSoundBuffer* CustomOutPlugin::sound_buffer = nullptr;
DWORD CustomOutPlugin::sound_buffer_size = 0;
DWORD CustomOutPlugin::sound_write_pointer = 0;
DWORD CustomOutPlugin::bytes_written = 0;
DWORD CustomOutPlugin::prebuffer_size = 0;
DWORD CustomOutPlugin::start_t = 0;
DWORD CustomOutPlugin::offset_t = 0;
DWORD CustomOutPlugin::paused_t = 0;
DWORD CustomOutPlugin::last_pause_t = 0;
DWORD CustomOutPlugin::last_stop_t = 0;
int CustomOutPlugin::last_pause = 0;
WAVEFORMATEX CustomOutPlugin::sound_format = WAVEFORMATEX();
bool CustomOutPlugin::play_started = false;
bool CustomOutPlugin::clear_done = false;
int CustomOutPlugin::last_volume = 255;

CustomOutPlugin::CustomOutPlugin() :
	AbstractOutPlugin()
{
	mod = new WinampOutModule();
	mod->version = OUT_VER;
	mod->description = "FFNx Winamp Output Plugin";
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
	// Nothing to do
}

int CustomOutPlugin::Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms)
{
	UNUSED_PARAM(bufferlenms);
	UNUSED_PARAM(prebufferms);

	Close();
	
	DSBUFFERDESC1 sbdesc = DSBUFFERDESC1();

	sound_format.cbSize = 0;
	sound_format.wBitsPerSample = bitspersamp;
	sound_format.nChannels = numchannels;
	sound_format.nSamplesPerSec = samplerate;
	sound_format.nBlockAlign = sound_format.nChannels * sound_format.wBitsPerSample / 8;
	sound_format.nAvgBytesPerSec = sound_format.nSamplesPerSec * sound_format.nBlockAlign;
	sound_format.wFormatTag = WAVE_FORMAT_PCM;

	sound_buffer_size = sound_format.nAvgBytesPerSec * AUDIO_BUFFER_SIZE;
	prebuffer_size = sound_format.nAvgBytesPerSec / 2; // ~500 ms
	sound_write_pointer = 0;
	bytes_written = 0;

	start_t = 0;
	offset_t = 0;
	paused_t = 0;
	last_stop_t = 0;
	last_pause_t = 0;

	play_started = false;
	clear_done = false;

	sbdesc.dwSize = sizeof(sbdesc);
	sbdesc.lpwfxFormat = &sound_format;
	sbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY
		| DSBCAPS_CTRLPAN | DSBCAPS_GETCURRENTPOSITION2
		| DSBCAPS_TRUEPLAYPOSITION | DSBCAPS_GLOBALFOCUS;
	sbdesc.dwReserved = 0;
	sbdesc.dwBufferBytes = sound_buffer_size;

	if (!*common_externals.directsound)
	{
		error("No directsound device\n");
		
		return -1;
	}
	
	if ((*common_externals.directsound)->CreateSoundBuffer((LPCDSBUFFERDESC)&sbdesc, &sound_buffer, 0))
	{
		error("couldn't create sound buffer (%i, %i)\n", numchannels, samplerate);
		sound_buffer = nullptr;

		return -1;
	}

	SetVolume(-1); // Set last known volume to buffer
	
	return 0;
}

void CustomOutPlugin::Close()
{
	if (sound_buffer) {
		last_stop_t = GetTickCount();
		if (sound_buffer->Stop() || sound_buffer->Release()) {
			error("couldn't stop or release sound buffer\n");
		}
		sound_buffer = nullptr;
	}
}

bool CustomOutPlugin::playHelper()
{
	if (sound_buffer->Play(0, 0, DSBPLAY_LOOPING)) {
		error("couldn't play sound buffer\n");
		return false;
	}

	start_t = GetTickCount();
	play_started = true;

	return true;
}

int CustomOutPlugin::Write(char* buffer, int len)
{
	LPVOID ptr1, ptr2;
	DWORD bytes1, bytes2;
	
	clear_done = false;
	
	if (!sound_buffer || !*common_externals.directsound) {
		return 1;
	}
	
	if (sound_buffer->Lock(sound_write_pointer, len, &ptr1, &bytes1, &ptr2, &bytes2, 0)) {
		error("couldn't lock sound buffer\n");
		return 1;
	}

	memcpy(ptr1, buffer, bytes1);
	memcpy(ptr2, buffer + bytes1, bytes2);

	if (sound_buffer->Unlock(ptr1, bytes1, ptr2, bytes2)) {
		error("couldn't unlock sound buffer\n");
		return 1;
	}

	sound_write_pointer = (sound_write_pointer + bytes1 + bytes2) % sound_buffer_size;
	bytes_written += bytes1 + bytes2;

	//trace("Write bytes_written=%i sound_write_pointer=%i sound_buffer_size=%i\n", bytes_written, sound_write_pointer, sound_buffer_size);

	// Do not play the song until buffering reach 'prebuffer_size' value
	if (!last_pause && !play_started && bytes_written >= prebuffer_size
			&& !playHelper()) {
		return 1;
	}

	return 0;
}

int CustomOutPlugin::canWriteHelper(DWORD &current_play_cursor)
{
	sound_buffer->GetCurrentPosition(&current_play_cursor, nullptr);

	if (current_play_cursor <= sound_write_pointer) {
		return (sound_buffer_size - sound_write_pointer) + current_play_cursor;
	}
	return current_play_cursor - sound_write_pointer;
}

int CustomOutPlugin::CanWrite()
{
	if (!sound_buffer || !*common_externals.directsound) {
		return 0;
	}

	DWORD current_play_cursor;
	return canWriteHelper(current_play_cursor);
}

int CustomOutPlugin::IsPlaying()
{
	if (last_pause || !sound_buffer || !*common_externals.directsound) {
		return 0;
	}
	
	// IsPlaying is called at the very end of decoding,
	// if we never started the song we should do it now
	// and we should deactivate the loop flag
	if (!play_started && !playHelper()) {
		return 0;
	}

	DWORD current_play_cursor;
	const int can_write = canWriteHelper(current_play_cursor);
	const int clear_data_size = sound_buffer_size / AUDIO_BUFFER_SIZE / 2; // ~500 ms

	// It is hard to know the current accurate play time with DirectSound
	// And the buffer is circular, so we make precautions
	if (clear_done) {
		if (current_play_cursor < sound_write_pointer
			|| current_play_cursor > sound_write_pointer + clear_data_size) {
			return 1;
		}
		
		Close();

		return 0;
	}
	
	// Clear some data after the write pointer when it is possible (to remove artifacts)
	if (can_write >= clear_data_size) {
		LPVOID ptr1, ptr2;
		DWORD bytes1, bytes2;

		if (!sound_buffer->Lock(sound_write_pointer, clear_data_size, &ptr1, &bytes1, &ptr2, &bytes2, 0)) {
			memset(ptr1, 0, bytes1);
			memset(ptr2, 0, bytes2);
				
			if (sound_buffer->Unlock(ptr1, bytes1, ptr2, bytes2)) {
				error("couldn't unlock sound buffer\n");
			}
		}
		else {
			error("couldn't lock sound buffer\n");
		}

		clear_done = true;
	}

	return 1;
}

int CustomOutPlugin::Pause(int pause)
{
	int t = last_pause;
	
	if (last_pause != pause) {
		if (pause) { // Pause
			last_pause_t = GetTickCount();
			if (sound_buffer && *common_externals.directsound && play_started) {
				if (sound_buffer->Stop()) {
					error("couldn't stop sound buffer\n");
				}
			}
		}
		else {
			if (last_pause_t > 0) { // UnPause
				paused_t += GetTickCount() - last_pause_t; // Remove paused time
				last_pause_t = 0;
			}

			if (sound_buffer && *common_externals.directsound && play_started) {
				if (sound_buffer->Play(0, 0, DSBPLAY_LOOPING)) {
					error("couldn't play sound buffer\n");
				}
			}
		}
	}
	
	last_pause = pause;

	return t;
}

void CustomOutPlugin::SetVolume(int volume)
{
	if (volume < 0 || volume > 255) {
		volume = last_volume;
	}
	else {
		last_volume = volume;
	}

	if (sound_buffer) {
		float decibel = 20.0f * log10f((volume * 100 / 255) / 100.0f);
		sound_buffer->SetVolume(volume ? int(decibel * 100.0f) : DSBVOLUME_MIN);
	}
}

void CustomOutPlugin::SetPan(int pan)
{
	if (sound_buffer) {
		sound_buffer->SetPan(pan * DSBPAN_LEFT / 128);
	}
}

void CustomOutPlugin::Flush(int t)
{
	if (sound_buffer && *common_externals.directsound) {
		if (!last_pause && play_started) {
			sound_buffer->Stop();
		}

		LPVOID ptr1, ptr2;
		DWORD bytes1, bytes2;

		// Clear the entire buffer
		if (!sound_buffer->Lock(0, 0, &ptr1, &bytes1, &ptr2, &bytes2, DSBLOCK_ENTIREBUFFER)) {
			memset(ptr1, 0, bytes1);
			memset(ptr2, 0, bytes2);
			
			if (sound_buffer->Unlock(ptr1, bytes1, ptr2, bytes2)) {
				error("couldn't unlock sound buffer\n");
			}
		}
		else {
			error("couldn't lock sound buffer\n");
		}
		
		sound_write_pointer = 0;
		bytes_written = 0;
		sound_buffer->SetCurrentPosition(0);
		play_started = false;
	}

	last_pause_t = 0;
	last_stop_t = 0;
	paused_t = 0;
	offset_t = t;
}

int CustomOutPlugin::GetOutputTime()
{
	DWORD t;
	
	if (last_pause_t > 0) {
		t = last_pause_t;
	}
	else if (last_stop_t > 0) {
		t = last_stop_t;
	}
	else if (play_started) {
		t = GetTickCount();
	}
	else {
		return offset_t;
	}

	return offset_t + (t - start_t) - paused_t;
}

int CustomOutPlugin::GetWrittenTime()
{
	if (0 == sound_format.nAvgBytesPerSec) {
		return offset_t + bytes_written;
	}

	int written_time = bytes_written / sound_format.nAvgBytesPerSec * 1000;
	written_time += ((bytes_written % sound_format.nAvgBytesPerSec) * 1000) / sound_format.nAvgBytesPerSec;

	return offset_t + written_time;
}

void CustomOutPlugin::setTempo(int tempo)
{
	if (sound_buffer) {
		sound_buffer->SetFrequency((sound_format.nSamplesPerSec * (tempo + 480)) / 512);
	}
}

AbstractInPlugin::AbstractInPlugin(AbstractOutPlugin* outPlugin, WinampInModule* mod) :
	mod(mod), outPlugin(outPlugin)
{
}

AbstractInPlugin::~AbstractInPlugin()
{
	quitModule();
}

void AbstractInPlugin::initModule(HINSTANCE dllInstance)
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

void AbstractInPlugin::quitModule()
{
	if (nullptr != this->mod) {
		if (nullptr != this->mod->standard.Quit) {
			this->mod->standard.Quit();
		}
		this->mod = nullptr;
	}
}

int AbstractInPlugin::play(const char* fn)
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

int AbstractInPlugin::play(const wchar_t* fn)
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

void AbstractInPlugin::pause()
{
	this->mod->standard.Pause();
}

void AbstractInPlugin::unPause()
{
	this->mod->standard.UnPause();
}

int AbstractInPlugin::isPaused()
{
	return this->mod->standard.IsPaused();
}

void AbstractInPlugin::stop()
{
	this->mod->standard.Stop();
}

int AbstractInPlugin::getLength()
{
	return this->mod->standard.GetLength();
}

int AbstractInPlugin::getOutputTime()
{
	return this->mod->standard.GetOutputTime();
}

void AbstractInPlugin::setOutputTime(int time_in_ms)
{
	this->mod->standard.SetOutputTime(time_in_ms);
}

WinampInPlugin::WinampInPlugin(AbstractOutPlugin* outPlugin) :
	WinampPlugin(), AbstractInPlugin(outPlugin)
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
	
	initModule(getHandle());

	return true;
}

void WinampInPlugin::closeModule()
{
	quitModule();
}

VgmstreamInPlugin::VgmstreamInPlugin(AbstractOutPlugin* outPlugin) :
	AbstractInPlugin(outPlugin, in_vgmstream_module())
{
	initModule(nullptr);
}

VgmstreamInPlugin::~VgmstreamInPlugin()
{
	quitModule();
}
