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

#pragma once

#include "plugin.h"
#include "out_plugin.h"

class InPluginWithFailback;

class AbstractInPlugin {
protected:
	AbstractOutPlugin* outPlugin;
public:
	AbstractInPlugin(AbstractOutPlugin* outPlugin);
	virtual ~AbstractInPlugin();

	virtual bool accept(const char* fn) const = 0;

	virtual int play(char* fn) = 0;
	virtual void pause() = 0;			// pause stream
	virtual void unPause() = 0;			// unpause stream
	virtual int isPaused() = 0;			// ispaused? return 1 if paused, 0 if not
	virtual void stop() = 0;				// stop (unload) stream

	// time stuff
	virtual int getLength() = 0;			// get length in ms
	virtual int getOutputTime() = 0;		// returns current output time in ms. (usually returns outMod->GetOutputTime()
	virtual void setOutputTime(int time_in_ms) = 0;	// seeks to point in stream (in ms). Usually you signal your thread to seek, which seeks and calls outMod->Flush()..

	// Resuming (not part of standard Winamp plugin)
	virtual bool canDuplicate() const = 0;
	virtual void duplicate() = 0;
	virtual int resume(char* fn) = 0;
	virtual bool cancelDuplicate() = 0;

	// Looping (not part of standard Winamp plugin)
	virtual void setLoopingEnabled(bool enabled) = 0;
};

class WinampInPlugin : public WinampPlugin, public AbstractInPlugin {
private:
	int current_saved_time_ms;
	inline LPCSTR procName() const {
		return "winampGetInModule2";
	}
	bool openModule(FARPROC procAddress);
	void closeModule();
protected:
	WinampInModule* mod;
	WinampInContext* context;
	inline virtual WinampInModule* getMod() const {
		return mod;
	}
	inline virtual WinampInContext* getContext() const {
		return context;
	}
	void initModule(HINSTANCE dllInstance);
	void quitModule();
	bool knownExtension(const char* fn) const;
	int isOurFile(const char* fn) const;
public:
	WinampInPlugin(AbstractOutPlugin* outPlugin);
	virtual ~WinampInPlugin();

	bool accept(const char* fn) const;

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
	bool canDuplicate() const;
	void duplicate();
	int resume(char* fn);
	bool cancelDuplicate();

	// Looping (not part of standard Winamp plugin)
	void setLoopingEnabled(bool enabled);
};

class InPluginWithFailback : public AbstractInPlugin {
private:
	AbstractInPlugin* inPlugin1;
	AbstractInPlugin* inPlugin2;
	AbstractInPlugin* current;
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

	bool accept(const char* fn) const;

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
	bool canDuplicate() const;
	void duplicate();
	int resume(char* fn);
	bool cancelDuplicate();

	// Looping (not part of standard Winamp plugin)
	void setLoopingEnabled(bool enabled);
};
