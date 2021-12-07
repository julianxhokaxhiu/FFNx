/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
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
		mStreamBuffer = new sample_t[SOLOUD_VGMSTREAM_NUM_SAMPLES * aParent->mChannels];

		rewind();
	}

	VGMStreamInstance::~VGMStreamInstance()
	{
		delete[] mStreamBuffer;
	}

	unsigned int VGMStreamInstance::getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize)
	{
		unsigned int offset = mOffset;
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

		mOffset = offset;
		return offset;
	}

	result VGMStreamInstance::rewind()
	{
		reset_vgmstream(mParent->mStream);

		mOffset = 0;
		mStreamPosition = 0.0f;
		return SO_NO_ERROR;
	}

	result VGMStreamInstance::seek(double aSeconds, float *mScratch, unsigned int mScratchSize)
	{
		int seek_samples = int(floor(mSamplerate * aSeconds));

		seek_vgmstream(mParent->mStream, seek_samples);

		mStreamPosition = aSeconds;
		return SO_NO_ERROR;
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
	}

	VGMStream::~VGMStream()
	{
		stop();

		close_vgmstream(mStream);
	}

	VGMSTREAM* VGMStream::init_vgmstream_with_extension(const char* aFilename, const char* ext)
	{
		STREAMFILE* streamFile = open_stdio_streamfile(aFilename);
		if (streamFile == nullptr) {
			return nullptr;
		}
		// Force extension
		streamFile = open_fakename_streamfile_f(streamFile, nullptr, ext);
		if (streamFile == nullptr) {
			return nullptr;
		}
		VGMSTREAM* stream = init_vgmstream_from_STREAMFILE(streamFile);
		close_streamfile(streamFile);
		return stream;
	}

	result VGMStream::load(const char* aFilename, const char* ext)
	{
		mBaseSamplerate = 0;

		if (aFilename == 0)
			return INVALID_PARAMETER;

		struct stat dummy;
		if (stat(aFilename, &dummy) != 0)
			return FILE_NOT_FOUND;

		stop();

		if (ext && ext[0] != '\0') {
			mStream = init_vgmstream_with_extension(aFilename, ext);
		}
		else {
			mStream = init_vgmstream(aFilename);
		}

		if (mStream == nullptr) {
			return FILE_LOAD_FAILED;
		}

		mBaseSamplerate = (float)mStream->sample_rate;
		mSampleCount = (unsigned int)mStream->num_samples;
		mChannels = mStream->channels;

		// Autodetect looping from the file itself and just inform SoLoud about it
		if (mStream->loop_flag) setLooping(true);
		// If the file has no loop tags, but the users wants to loop, force a basic start to end loop
		else if (mFlags & AudioSourceInstance::LOOPING) vgmstream_force_loop(mStream, true, 0, mStream->num_samples);

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
