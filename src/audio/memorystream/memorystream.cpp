/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include "memorystream.h"

namespace SoLoud
{
	MemoryStreamInstance::MemoryStreamInstance(MemoryStream* aParent)
	{
		mParent = aParent;
		mOffset = 0;
		mParent->mReadOffset.store(0, std::memory_order_release);
	}

	uint32_t MemoryStreamInstance::getAudio(float* aBuffer, uint32_t aSamplesToRead, uint32_t aBufferSize)
	{
		uint32_t j, k;
		uint32_t current_offset = mParent->mPushOffset.load(std::memory_order_acquire) / sizeof(float);
		uint32_t available_samples = current_offset > mOffset ? (current_offset - mOffset) / mChannels : 0;
		uint32_t samples_to_read = available_samples < aSamplesToRead ? available_samples : aSamplesToRead;
		if (samples_to_read < aSamplesToRead && current_offset < mParent->mPushLength / sizeof(float))
			mParent->mUnderrunSamples.fetch_add(aSamplesToRead - samples_to_read, std::memory_order_relaxed);

		if (samples_to_read > 0)
		{
			for (j = 0; j < samples_to_read; j++)
			{
				for (k = 0; k < mChannels; k++)
				{
					aBuffer[k * aBufferSize + j] = mParent->mData[mOffset + (j * mChannels) + k];
				}
			}

			mOffset += samples_to_read * mChannels;
			mParent->mReadOffset.store(mOffset, std::memory_order_release);

			return samples_to_read;
		}

		return 0;
	}

	bool MemoryStreamInstance::hasEnded()
	{
		if (mOffset >= mParent->mSampleCount)
		{
			return 1;
		}
		return 0;
	}

	MemoryStream::MemoryStream(float sampleRate, uint32_t sampleCount, uint32_t channels)
	{
		mBaseSamplerate = sampleRate;
		mSampleCount = sampleCount;
		mChannels = channels;

    mData = new float[mSampleCount]{ NULL };
    mPushLength = mSampleCount * sizeof(float);
    mPushOffset = 0;
		mReadOffset = 0;
		mUnderrunSamples = 0;
	}

	MemoryStream::~MemoryStream()
	{
		stop();

		delete[] mData;
	}

  result MemoryStream::push(uint8_t* data, uint32_t size)
  {
	uint32_t pushOffset = mPushOffset.load(std::memory_order_relaxed);
	uint32_t leftSpace = mPushLength - pushOffset;

    if ( size > leftSpace ) size = leftSpace;

		memcpy((uint8_t*)mData + pushOffset, data, size);

		mPushOffset.store(pushOffset + size, std::memory_order_release);

    return SO_NO_ERROR;
  }

	double MemoryStream::getBufferedDuration() const
	{
		if (mBaseSamplerate <= 0 || mChannels == 0) return 0.0;
		uint32_t written_samples = mPushOffset.load(std::memory_order_acquire) / sizeof(float);
		uint32_t read_samples = mReadOffset.load(std::memory_order_acquire);
		return written_samples > read_samples ? double(written_samples - read_samples) / (mBaseSamplerate * mChannels) : 0.0;
	}

	AudioSourceInstance* MemoryStream::createInstance()
	{
		return new MemoryStreamInstance(this);
	}

	double MemoryStream::getLength()
	{
		if (mBaseSamplerate == 0)
			return 0;

		return mSampleCount / mBaseSamplerate;
	}
};
