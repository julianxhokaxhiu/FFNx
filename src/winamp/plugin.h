#include "windows.h"
#include "winamp.h"

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

class WinampOutPlugin : public WinampPlugin {
private:
	WinampOutModule* mod;
	inline LPCSTR procName() const {
		return "winampGetOutModule";
	}
	bool openModule(FARPROC procAddress);
	void closeModule();
public:
	WinampOutPlugin();
	~WinampOutPlugin();
	inline WinampOutModule* getModule() const {
		return mod;
	}
};

class WinampInPlugin : public WinampPlugin {
private:
	WinampInModule* mod;
	WinampOutPlugin* outPlugin;
	inline LPCSTR procName() const {
		return "winampGetInModule2";
	}
	bool openModule(FARPROC procAddress);
	void closeModule();
public:
	WinampInPlugin(WinampOutPlugin* outPlugin);
	~WinampInPlugin();

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
};
