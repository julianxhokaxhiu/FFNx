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

#include "vgmstream.h"

#include <sys/stat.h>

#define SOLOUD_VGMSTREAM_NUM_SAMPLES 512

namespace SoLoud
{
	VGMStreamInstance::VGMStreamInstance(VGMStream* aParent)
	{
		mParent = aParent;
		mOffset = 0;

		if (mParent->isStreaming)
			mStreamBuffer = new sample_t[SOLOUD_VGMSTREAM_NUM_SAMPLES * aParent->mChannels];
		else
			mStreamBuffer = nullptr;
	}

	VGMStreamInstance::~VGMStreamInstance()
	{
		if (mParent->isStreaming) delete[] mStreamBuffer;
	}

	unsigned int VGMStreamInstance::getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize)
	{
		unsigned int offset = mOffset;

		if (mParent->isStreaming)
		{
			unsigned int i, j, k;

			for (i = 0; i < aSamplesToRead; i += SOLOUD_VGMSTREAM_NUM_SAMPLES)
			{
				memset(mStreamBuffer, 0, sizeof(sample_t) * SOLOUD_VGMSTREAM_NUM_SAMPLES * mChannels);
				unsigned int copylen = (aSamplesToRead - i) > SOLOUD_VGMSTREAM_NUM_SAMPLES ? SOLOUD_VGMSTREAM_NUM_SAMPLES : aSamplesToRead - i;
				offset += (unsigned int)render_vgmstream(mStreamBuffer, copylen, mParent->mStream);

				for (j = 0; j < copylen; j++)
				{
					for (k = 0; k < mChannels; k++)
					{
						aBuffer[k * aSamplesToRead + i + j] = mStreamBuffer[(j * mChannels) + k] / (float)INT16_MAX;
					}
				}
			}
		}
		else
		{
			if (mParent->mData != NULL)
			{
				unsigned int i, j;
				unsigned int dataleft = mParent->mSampleCount - mOffset;
				unsigned int copylen = dataleft;
				if (copylen > aSamplesToRead)
					copylen = aSamplesToRead;

				for (j = 0; j < copylen; j++)
				{
					for (i = 0; i < mChannels; i++)
					{
						aBuffer[i * aBufferSize + j] = mParent->mData[offset + j + i] / (float)INT16_MAX;
					}
				}

				offset += copylen;
			}
			else
				offset = 0;
		}

		mOffset = offset;
		return offset;
	}

	result VGMStreamInstance::rewind()
	{
		if (mParent->isStreaming) reset_vgmstream(mParent->mStream);

		mOffset = 0;
		mStreamPosition = 0.0f;
		return 0;
	}

	bool VGMStreamInstance::hasEnded()
	{
		if (!(mFlags & AudioSourceInstance::LOOPING) && mOffset >= mParent->mSampleCount)
		{
			return 1;
		}
		return 0;
	}

	VGMStream::VGMStream()
	{
		mSampleCount = 0;
		isStreaming = true;
	}

	VGMStream::~VGMStream()
	{
		stop();

		if (isStreaming)
			close_vgmstream(mStream);
		else
			delete[] mData;
	}

	result VGMStream::load(const char* aFilename, bool doStreaming)
	{
		mBaseSamplerate = 0;

		if (aFilename == 0)
			return INVALID_PARAMETER;

		struct stat dummy;
		if (stat(aFilename, &dummy) != 0)
			return FILE_NOT_FOUND;

		stop();

		isStreaming = doStreaming;
		mStream = init_vgmstream(aFilename);

		if (mStream == nullptr) {
			return FILE_LOAD_FAILED;
		}

		mBaseSamplerate = (float)mStream->sample_rate;
		mSampleCount = (unsigned int)mStream->num_samples;
		mChannels = mStream->channels;
		if (mStream->loop_flag) setLooping(true);

		if (!isStreaming)
		{
			int ret = 0;
			mData = new sample_t[mSampleCount * mChannels]();
			while(ret != mSampleCount) ret = render_vgmstream(mData, mSampleCount, mStream);
			close_vgmstream(mStream);
		}

		return SO_NO_ERROR;
	}

	AudioSourceInstance* VGMStream::createInstance()
	{
		return new VGMStreamInstance(this);
	}

	double VGMStream::getLength()
	{
		if (mBaseSamplerate == 0)
			return 0;

		return mSampleCount / mBaseSamplerate;
	}
};
