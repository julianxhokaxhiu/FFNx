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

class WinampInPlugin : public WinampPlugin {
private:
	FARPROC getExtendedFileInfoProc;
	inline LPCSTR procName() const {
		return "winampGetInModule2";
	}
	bool openModule(FARPROC procAddress);
	void closeModule();
protected:
	WinampInModule* mod;
	AbstractOutPlugin* outPlugin;
	inline virtual WinampInModule* getModule() const {
		return mod;
	}
	void initModule(HINSTANCE dllInstance);
	void quitModule();
	bool knownExtension(const char* fn) const;
	int isOurFile(const char* fn) const;
	int inPsfGetTag(const char* fn, const char* metadata, char* ret, int retlen);
public:
	WinampInPlugin(AbstractOutPlugin* outPlugin);
	virtual ~WinampInPlugin();

	bool accept(const char* fn) const;

	int play(const char* fn);
	void pause();			// pause stream
	void unPause();			// unpause stream
	int isPaused();			// ispaused? return 1 if paused, 0 if not
	void stop();				// stop (unload) stream

	// time stuff
	int getLength();			// get length in ms
	int getOutputTime();		// returns current output time in ms. (usually returns outMod->GetOutputTime()
	void setOutputTime(int time_in_ms);	// seeks to point in stream (in ms). Usually you signal your thread to seek, which seeks and calls outMod->Flush()..
	
	size_t getTitle(const char* fn, char* ret, size_t max);
	// tags
	int getTag(const char* fn, const char* metadata, char* ret, int retlen);
};
