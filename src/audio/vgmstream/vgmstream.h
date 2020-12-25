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

#if defined(__cplusplus)
extern "C" {
#endif

#include <libvgmstream/vgmstream.h>

#if defined(__cplusplus)
}
#endif

namespace SoLoud
{
	class VGMStream : public AudioSource
	{
		static VGMSTREAM* init_vgmstream_with_extension(const char* aFilename, const char* ext);
	public:
		VGMSTREAM* mStream;
		unsigned int mSampleCount;

		sample_t* mData;

		VGMStream();
		virtual ~VGMStream();
		result load(const char* aFilename, const char* ext = nullptr);

		virtual AudioSourceInstance* createInstance();
		time getLength();
	};

	class VGMStreamInstance : public AudioSourceInstance
	{
		sample_t* mStreamBuffer;
		VGMStream* mParent;
		unsigned int mOffset;
	public:
		VGMStreamInstance(VGMStream* aParent);
		virtual ~VGMStreamInstance();
		virtual unsigned int getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize);
		virtual result rewind();
		virtual bool hasEnded();
	};
};
