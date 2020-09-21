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

#include "openpsf.h"

#include <sys/stat.h>

constexpr auto SOLOUD_OPENPSF_NUM_SAMPLES = 512;

namespace SoLoud
{
	OpenPsfInstance::OpenPsfInstance(OpenPsf* aParent)
	{
		trace("OpenPsfInstance %i\n", aParent);
		mParent = aParent;
		mOffset = 0;
		mStreamBufferSize = SOLOUD_OPENPSF_NUM_SAMPLES * mChannels;
		mStreamBuffer = new float[mStreamBufferSize];
		trace("/OpenPsfInstance\n");
	}

	OpenPsfInstance::~OpenPsfInstance()
	{
		trace("~OpenPsfInstance\n");
		//delete[] mStreamBuffer;
		trace("/~OpenPsfInstance\n");
	}

	unsigned int OpenPsfInstance::getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize)
	{
		trace("OpenPsfInstance::getAudio %i %i\n", aSamplesToRead, aBufferSize);
		unsigned int offset = 0;
		unsigned int i, j, k;

		for (i = 0; i < aSamplesToRead; i += SOLOUD_OPENPSF_NUM_SAMPLES)
		{
			memset(mStreamBuffer, 0, mStreamBufferSize * sizeof(float));
			unsigned int blockSize = aSamplesToRead - i > SOLOUD_OPENPSF_NUM_SAMPLES ? SOLOUD_OPENPSF_NUM_SAMPLES : aSamplesToRead - i;
			size_t r = mParent->openpsf.decode(mParent->stream, mStreamBuffer, blockSize);

			if (r == 0) {
				error("OpenPsfInstance::%s Decoding error: %s\n%s\n", __func__, mParent->openpsf.get_last_error(mParent->stream), mParent->openpsf.get_last_status(mParent->stream));
				break;
			}

			offset += r;

			for (j = 0; j < blockSize; j++)
			{
				for (k = 0; k < mChannels; k++)
				{
					aBuffer[k * aSamplesToRead + i + j] = mStreamBuffer[j * mChannels + k];
				}
			}
		}

		mOffset += offset;
		trace("/OpenPsfInstance::getAudio\n");
		return offset;
	}

	result OpenPsfInstance::rewind()
	{
		trace("OpenPsfInstance::rewind\n");
		return 1;
		//mParent->openpsf.seek(mParent->stream, 0);

		mOffset = 0;
		mStreamPosition = 0.0f;
		return 0;
	}

	bool OpenPsfInstance::hasEnded()
	{
		trace("OpenPsfInstance::hasEnded %i %i\n", mOffset, mParent->mSampleCount);
		return !(mFlags & AudioSourceInstance::LOOPING) && mOffset >= mParent->mSampleCount;
	}

	OpenPsf::OpenPsf(const OPENPSF& openpsf) :
		openpsf(openpsf), stream(nullptr), mSampleCount(0)
	{
		trace("OpenPsf %i\n", &openpsf);
	}

	OpenPsf::~OpenPsf()
	{
		trace("~OpenPsf\n");
		stop();

		/* if (stream != nullptr) {
			openpsf.destroy(stream);
		} */
		trace("/~OpenPsf\n");
	}

	result OpenPsf::load(const char* aFilename)
	{
		trace("OpenPsf::load %s\n", aFilename);
		mBaseSamplerate = 0;

		if (aFilename == nullptr) {
			trace("/OpenPsf::load invalid\n");
			return INVALID_PARAMETER;
		}

		struct stat dummy;
		if (stat(aFilename, &dummy) != 0) {
			trace("/OpenPsf::load %s not found\n", aFilename);
			return FILE_NOT_FOUND;
		}

		/* if (!openpsf.is_our_path(aFilename, external_music_ext)) {
			error("Incompatible file extension %s\n", external_music_ext);
			return FILE_LOAD_FAILED;
		} */

		stop();

		stream = openpsf.create();
		if (stream == nullptr || !openpsf.open(stream, aFilename, true)) {
			error("Cannot open file %s: %s\n%s\n", aFilename, openpsf.get_last_error(stream), openpsf.get_last_status(stream));
			return FILE_LOAD_FAILED;
		}

		mBaseSamplerate = 44100; // float(openpsf.get_sample_rate(stream));
		mSampleCount = openpsf.get_length(stream) / 1000.0 * mBaseSamplerate;
		mChannels = 2; // openpsf.get_channel_count(stream);
		setLooping(true);

		/* info("Opening file %s with openPSF (samplerate: %i, length: %i, channels: %i):\n%s\n",
			aFilename, int(mBaseSamplerate), openpsf.get_length(stream), mChannels, openpsf.get_last_status(stream)); */

		trace("/OpenPsf::load %s OK\n", aFilename);
		return SO_NO_ERROR;
	}

	AudioSourceInstance* OpenPsf::createInstance()
	{
		trace("OpenPsf::createInstance\n");
		return new OpenPsfInstance(this);
	}

	double OpenPsf::getLength()
	{
		trace("OpenPsf::getLength\n");
		if (mBaseSamplerate == 0) {
			return 0;
		}

		return mSampleCount / mBaseSamplerate;
	}
};
