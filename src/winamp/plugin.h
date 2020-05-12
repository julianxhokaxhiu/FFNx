#pragma once

#include <windows.h>
#include <DSound.h>
#include "../log.h"
#include "winamp.h"
#include "winamp_ext.h"

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
	WinampOutContext* context;
public:
	AbstractOutPlugin() : mod(nullptr), context(nullptr) {}
	virtual ~AbstractOutPlugin() {}
	inline WinampOutModule* getModule() const {
		return mod;
	}
	inline WinampOutContext* getContext() const {
		return context;
	}
	// volume stuff
	virtual void setVolume(int volume);	// from 0 to 255.. usually just call outMod->SetVolume
	void setPan(int pan);	    // from -127 to 127.. usually just call outMod->SetPan
	int getOutputTime() const;
	int getWrittenTime() const;
	void pause() const;
	void unPause() const;
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
	void setVolume(int volume);
	// This method is not part of the winamp plugin
	void setTempo(int tempo);
};

struct CustomOutPluginState {
	IDirectSoundBuffer* sound_buffer;
	DWORD sound_buffer_size;
	DWORD sound_write_pointer;
	DWORD bytes_written;
	DWORD prebuffer_size;
	DWORD start_t;
	DWORD offset_t;
	DWORD paused_t;
	DWORD last_pause_t;
	DWORD last_stop_t;
	WAVEFORMATEX sound_format;
	bool play_started;
	bool clear_done;
};

class CustomOutPlugin : public AbstractOutPlugin {
private:
	static WinampOutContext static_context;
	static CustomOutPluginState state;
	static CustomOutPluginState dup_state;
	static int last_pause;
	static int last_volume;

	static void FakeDialog(HWND hwndParent);
	static void Noop();
	static int Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
	static void Close();
	static bool playHelper();
	static int Write(char* buf, int len);
	static int canWriteHelper(DWORD &current_play_cursor);
	static int CanWrite();
	// More likely: 'in': Hey 'out' I finished my decoding, have you finished playing?
	static int IsPlaying();
	static int Pause(int pause);
	static void SetVolume(int volume);
	static void SetPan(int pan);
	static void Flush(int t);
	static int GetOutputTime();
	static int GetWrittenTime();
	static void Duplicate();
	static int Resume(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
	static int CancelDuplicate();
public:
	CustomOutPlugin();
	virtual ~CustomOutPlugin();
	// This method is not part of the winamp plugin
	void setTempo(int tempo);
};

class InPluginWithFailback;

class AbstractInPlugin {
private:
	int current_saved_time_ms;
	friend class InPluginWithFailback;
protected:
	WinampInModule* mod;
	WinampInContext* context;
	AbstractOutPlugin* outPlugin;
	inline virtual WinampInModule* getMod() const {
		return mod;
	}
	inline virtual WinampInContext* getContext() const {
		return context;
	}
	virtual int beforePlay(char* fna);
	void initModule(HINSTANCE dllInstance);
	void quitModule();
	bool knownExtension(const char* fn) const;
	int isOurFile(const char* fn) const;
	bool accept(const char* fn) const;
	bool canDuplicate() const;
public:
	AbstractInPlugin(AbstractOutPlugin* outPlugin, WinampInModule* mod = nullptr,
		WinampInContext* context = nullptr);
	virtual ~AbstractInPlugin();
	
	int play(char* fn);
	void pause();			// pause stream
	void unPause();			// unpause stream
	int isPaused();			// ispaused? return 1 if paused, 0 if not
	void stop();				// stop (unload) stream

	// time stuff
	int getLength();			// get length in ms
	int getOutputTime();		// returns current output time in ms. (usually returns outMod->GetOutputTime()
	void setOutputTime(int time_in_ms);	// seeks to point in stream (in ms). Usually you signal your thread to seek, which seeks and calls outMod->Flush()..

	// Resuming (not part of standard Winamp plugin)
	void duplicate();
	int resume(char* fn);
	bool cancelDuplicate();
};

class WinampInPlugin : public WinampPlugin, public AbstractInPlugin {
private:
	inline LPCSTR procName() const {
		return "winampGetInModule2";
	}
	bool openModule(FARPROC procAddress);
	void closeModule();
public:
	WinampInPlugin(AbstractOutPlugin* outPlugin);
	virtual ~WinampInPlugin();
};

class VgmstreamInPlugin : public AbstractInPlugin {
public:
	VgmstreamInPlugin(AbstractOutPlugin* outPlugin);
	virtual ~VgmstreamInPlugin();
};

class InPluginWithFailback : public AbstractInPlugin {
private:
	AbstractInPlugin* inPlugin1;
	AbstractInPlugin* inPlugin2;
	AbstractInPlugin* current;
	WinampInModule* getMod() const;
	WinampInContext* getContext() const;
	int beforePlay(char* fna);
public:
	InPluginWithFailback(
		AbstractOutPlugin* outPlugin,
		AbstractInPlugin* inPlugin1,
		AbstractInPlugin* inPlugin2 = nullptr
	);
	virtual ~InPluginWithFailback();
	inline AbstractInPlugin* getPlugin1() const {
		return inPlugin1;
	}
	inline AbstractInPlugin* getPlugin2() const {
		return inPlugin2;
	}
};
