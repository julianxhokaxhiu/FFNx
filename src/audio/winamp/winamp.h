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

#include <soloud/soloud.h>
#include "in_plugin.h"
#include "out_plugin.h"

namespace SoLoud
{
	class Winamp : public AudioSource
	{
	public:
		BufferOutPlugin* outPlugin;
		WinampInPlugin* inPlugin;

		Winamp(WinampInPlugin* inPlugin, BufferOutPlugin* outPlugin);
		virtual ~Winamp();
		result load(const char* aFilename);

		virtual AudioSourceInstance* createInstance();
		time getLength();
	};

	class WinampInstance : public AudioSourceInstance
	{
		Winamp* mParent;
		unsigned int mOffset;
		size_t mAudioBufferSize;
		unsigned int mBytesPerSample;
		char* mAudioBuffer;
	public:
		WinampInstance(Winamp* aParent);
		virtual ~WinampInstance();
		virtual unsigned int getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize);
		virtual result rewind();
		virtual result seek(double aSeconds, float* mScratch, unsigned int mScratchSize);
		virtual bool hasEnded();
		virtual float getInfo(unsigned int aInfoKey);
	};
};
