#include "windows.h"
#include "winamp.h"
#include "../types.h"
#include <DSound.h>

class WinampPlugin {
private:
	HINSTANCE handle;
	bool open(LPCWSTR libFileNameW, char* libFileNameA);
protected:
	virtual LPCSTR procName() const = 0;
	virtual bool openModule(FARPROC procAddress) = 0;
	virtual void closeModule() = 0;
	inline HINSTANCE getHandle() const {
		return this->handle;
	}
public:
	WinampPlugin();
	virtual ~WinampPlugin();
	bool open(LPCWSTR libFileName);
	bool open(char* libFileName);
	void close();
};

class AbstractOutPlugin {
protected:
	WinampOutModule* mod;
public:
	AbstractOutPlugin() : mod(nullptr) {}
	virtual ~AbstractOutPlugin() {}
	inline WinampOutModule* getModule() const {
		return mod;
	}
	virtual int isPlaying() const = 0;
	// This method is not part of the winamp plugin
	virtual void setTempo(int tempo) = 0;
};

class WinampOutPlugin : public WinampPlugin, public AbstractOutPlugin {
private:
	inline LPCSTR procName() const {
		return "winampGetOutModule";
	}
	bool openModule(FARPROC procAddress);
	void closeModule();
public:
	WinampOutPlugin();
	virtual ~WinampOutPlugin();
	int isPlaying() const;
	// This method is not part of the winamp plugin
	void setTempo(int tempo);
};

class CustomOutPlugin : public AbstractOutPlugin {
private:
	static IDirectSoundBuffer* sound_buffer;
	static uint sound_buffer_size;
	static uint sound_write_pointer;
	static uint bytes_written;
	static DWORD start_t;
	static DWORD last_pause_t;
	static int last_pause;
	static LONG volume;
	static LONG pan;
	static DWORD tempo;
	static WAVEFORMATEX sound_format;
	static void Config(HWND hwndParent);
	static void About(HWND hwndParent);
	static void Init();
	static void Quit();
	static int Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
	static void Close();
	static int Write(char* buf, int len);
	static int CanWrite();
	static int IsPlaying();
	static int Pause(int pause);
	static void SetVolume(int volume);
	static void SetPan(int pan);
	static void Flush(int t);
	static int GetOutputTime();
	static int GetWrittenTime();
public:
	CustomOutPlugin();
	virtual ~CustomOutPlugin();
	int isPlaying() const;
	// This method is not part of the winamp plugin
	void setTempo(int tempo);
};

class WinampInPlugin : public WinampPlugin {
private:
	WinampInModule* mod;
	AbstractOutPlugin* outPlugin;
	inline LPCSTR procName() const {
		return "winampGetInModule2";
	}
	bool openModule(FARPROC procAddress);
	void closeModule();
public:
	WinampInPlugin(AbstractOutPlugin* outPlugin);
	virtual ~WinampInPlugin();

	int play(const char* fn);
	int play(const wchar_t* fn);
	void pause();			// pause stream
	void unPause();			// unpause stream
	int isPaused();			// ispaused? return 1 if paused, 0 if not
	void stop();				// stop (unload) stream

	// time stuff
	int getLength();			// get length in ms
	int getOutputTime();		// returns current output time in ms. (usually returns outMod->GetOutputTime()
	void setOutputTime(int time_in_ms);	// seeks to point in stream (in ms). Usually you signal your thread to seek, which seeks and calls outMod->Flush()..

	// volume stuff
	void setVolume(int volume);	// from 0 to 255.. usually just call outMod->SetVolume
	void setPan(int pan);	// from -127 to 127.. usually just call outMod->SetPan
	// This method is not part of the winamp plugin
	void setTempo(int tempo);
};
