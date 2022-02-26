/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#include <stdint.h>
#include <soloud.h>

namespace SoLoud
{
	class MemoryStream : public AudioSource
	{
	public:
    uint32_t mPushOffset;
		uint32_t mSampleCount;
		float* mData;

		MemoryStream(float sampleRate, uint32_t sampleCount, uint32_t channels);
		virtual ~MemoryStream();

    result push(uint8_t* data, uint32_t size);

		virtual AudioSourceInstance* createInstance();
		time getLength();
	};

	class MemoryStreamInstance : public AudioSourceInstance
	{
		MemoryStream* mParent;
		uint32_t mOffset;
	public:
		MemoryStreamInstance(MemoryStream* aParent);
		virtual uint32_t getAudio(float* aBuffer, uint32_t aSamplesToRead, uint32_t aBufferSize);
		virtual bool hasEnded();
	};
};
