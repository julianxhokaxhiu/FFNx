#pragma once

#include <windows.h>
#include <DSound.h>
#include "../log.h"
#include "winamp.h"
#include "winamp_ext.h"

#define UNUSED_PARAM(x) (void)x;

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
