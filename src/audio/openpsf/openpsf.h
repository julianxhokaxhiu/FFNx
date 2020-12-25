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
#include <stdint.h>
#include <libopenpsf/openpsf.h>
#include "../../log.h"

namespace SoLoud
{
	class OpenPsf : public AudioSource
	{
	public:
		Psf* stream;
		unsigned int mSampleCount;

		OpenPsf();
		virtual ~OpenPsf();
		static bool is_our_path(const char* aFilename);
		result load(const char* aFilename);

		virtual AudioSourceInstance* createInstance();
		time getLength();
	};

	class OpenPsfInstance : public AudioSourceInstance
	{
		size_t mStreamBufferSize;
		int16_t* mStreamBuffer;
		OpenPsf* mParent;
		unsigned int mOffset;
		bool ended;
	public:
		OpenPsfInstance(OpenPsf* aParent);
		virtual ~OpenPsfInstance();
		virtual unsigned int getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize);
		virtual result rewind();
		virtual bool hasEnded();
	};
};
